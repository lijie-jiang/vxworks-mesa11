/* vgset.c - Wind River VG Set Functionality */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
22dec14,yat  Fix build warning for LP64 (US50456)
14aug13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
21may09,m_c  Written
*/

/*
DESCRIPTION
These routines provide set functionality that support the OpenVG
implementation.
*/

/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/* defines */

/* A small value */
#define EPSILON                     0.000001

/* Rectangle macros */
#define RECT_ADD(_pHead, _pRect)    do                                      \
                                        {                                   \
                                        if ((_pHead) == NULL)               \
                                            (_pHead) = (_pRect);            \
                                        else                                \
                                            {                               \
                                            rectangle_t* pNode = (_pHead);  \
                                            while (pNode->pNext != NULL)    \
                                                pNode = pNode->pNext;       \
                                            pNode->pNext = (_pRect);        \
                                            }                               \
                                        } while (0)

#define RECT_FREE(_pRect)           (_pRect)->x0 = (float)MAX_INT

#define RECT_POOL_OFFSET            (MAX_SCISSOR_RECTS * sizeof(int))

#define RECT_REMOVE(_pHead, _pRect) do                                              \
                                        {                                           \
                                        if ((_pHead) == (_pRect))                   \
                                            (_pHead) = (_pRect)->pNext;             \
                                        else                                        \
                                            {                                       \
                                            rectangle_t* pNode = (_pHead);          \
                                            while (pNode != NULL)                   \
                                                {                                   \
                                                if (pNode->pNext == (_pRect))       \
                                                    {                               \
                                                    pNode->pNext = (_pRect)->pNext; \
                                                    break;                          \
                                                    }                               \
                                                pNode = pNode->pNext;               \
                                                }                                   \
                                            }                                       \
                                        } while (0)

/* forward declarations */

LOCAL rectangle_t * generateClippingRects(_VGRectangle*, int, rectangle_t*, size_t, surface_t*);
LOCAL void initLinearGradient(paint_t* pPaint);
LOCAL void initRadialGradient(paint_t* pPaint);
LOCAL void set(VGParamType, int, const _VGType*, int);
LOCAL void setParameter(void*, VGParamType, int, _VGType*, int);

/*******************************************************************************
 *
 *  RECT_CALLOC
 */
LOCAL rectangle_t * RECT_CALLOC
    (
    rectangle_t* pPool,
    size_t poolSize
    )
    {
    while (poolSize-- > 0)
        {
        if (pPool->x0 == MAX_INT)
            {
            memset(pPool, 0, sizeof(rectangle_t));
            return (pPool);
            }
        pPool++;
        }
    die("Fatal error");
    return (NULL);
    }

/*******************************************************************************
 *
 * generateClippingRects - generate a list of non-overlapping clipping rectangles
 *
 * RETURNS: A pointer to a list of non-overlapping rectangles if any,
 *          otherwise NULL
 *
 */
LOCAL rectangle_t * generateClippingRects
    (
    _VGRectangle* pRects,
    int numRects,
    rectangle_t* pRectPool,
    size_t rectPoolSize,
    surface_t* pSurface
    )
    {
    int             i;
    rectangle_t *   pHeadRect = NULL;   /* head of list of rectangles */
    rectangle_t *   pRectA;             /* first rectangle */
    rectangle_t *   pRectB;             /* second rectangle */
    rectangle_t *   pRectX;             /* intermediate rectangle */

    if (pSurface == NULL)
        return (NULL);

    /* Initialize pool */
    for (i = 0; i < rectPoolSize; i++)
        RECT_FREE(&pRectPool[i]);

    /* Create the initial list of rectangles */
    for (i = 0; i < numRects; i++)
        {
        _VGRectangle rect = pRects[i];
        /* Ignore invalid rectangles */
        if ((rect.width <= 0) || (rect.height <= 0))
            continue;
        /* Clip against surface */
        if ((rect.x0 >= pSurface->width) || (rect.y0 >= pSurface->height))
            continue;
        if (rect.x0 < 0)
            rect.width += rect.x0, rect.x0 = 0;
        if (rect.y0 < 0)
            rect.height += rect.y0, rect.y0 = 0;
        if ((rect.width <= 0) || (rect.height <= 0))
            continue;
        if ((rect.x0 + rect.width) > pSurface->width)
            rect.width = pSurface->width - rect.x0;
        if ((rect.y0 + rect.height) > pSurface->height)
            rect.height = pSurface->height - rect.y0;
        /* Create a new rectangle */
        pRectA = RECT_CALLOC(pRectPool, rectPoolSize);
        pRectA->x0 = (float)rect.x0;
        pRectA->y0 = (float)(pSurface->height - rect.y0 - rect.height);
        pRectA->x1 = (float)(rect.x0 + rect.width);
        pRectA->y1 = (float)(pSurface->height - rect.y0);
        RECT_ADD(pHeadRect, pRectA);
        }

    /*
     * Check each rectangle against all the others. This double loop, as it is,
     * is not optimal mostly because when a rectangle is split, the chunks are
     * added to the tail of the linked list which causes unnecessary tests.
     * Also, it is crucial that the "remove" and "free" operations do NOT affect
     * linked list nodes
     */
    for (pRectA = pHeadRect; pRectA != NULL; pRectA = pRectA->pNext)
        {
        for (pRectB = pHeadRect; pRectB != NULL; pRectB = pRectB->pNext)
            {
            if (pRectA != pRectB)
                {
                /* Case 0
                 *
                 *  A---+
                 *  |   |    B------+
                 *  +---+    |      |
                 *           +------+
                 */
                if ((pRectA->x1 <= pRectB->x0) || (pRectA->x0 >= pRectB->x1) ||
                    (pRectA->y1 <= pRectB->y0) || (pRectA->y0 >= pRectB->y1))
                    {
                    }

                /* Case 1
                 *
                 *  B---------+
                 *  |         |
                 *  | A----+  |
                 *  | |    |  |
                 *  | +----+  |
                 *  +---------+
                 */
                else if ((pRectA->x0 >= pRectB->x0) && (pRectA->x1 <= pRectB->x1) &&
                         (pRectA->y0 >= pRectB->y0) && (pRectA->y1 <= pRectB->y1))
                    {
                    RECT_REMOVE(pHeadRect, pRectA);
                    RECT_FREE(pRectA);
                    break;
                    }

                /* Case 2
                 *
                 *      B----+
                 *      |    |
                 *  A-------------+
                 *  |   :    :    |
                 *  |   :    :    |
                 *  +-------------+
                 *      +----+
                 */
                else if ((pRectA->x0 < pRectB->x0) && (pRectA->x1 > pRectB->x1) &&
                         (pRectA->y0 >= pRectB->y0) && (pRectA->y1 <= pRectB->y1))
                    {
                    RECT_REMOVE(pHeadRect, pRectB);
                    if (pRectA->y0 > pRectB->y0)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectB->x0;
                        pRectX->y0 = pRectB->y0;
                        pRectX->x1 = pRectB->x1;
                        pRectX->y1 = pRectA->y0;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    if (pRectA->y1 < pRectB->y1)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectB->x0;
                        pRectX->y0 = pRectA->y1;
                        pRectX->x1 = pRectB->x1;
                        pRectX->y1 = pRectB->y1;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    RECT_FREE(pRectB);
                    }

                /* Case 3
                 *
                 *      A----+
                 *      |    |
                 *  +---|....|----+
                 *  |   |    |    |
                 *  |   |    |    |
                 *  +---|....|----+
                 *      +----+
                 */
                else if ((pRectA->y0 < pRectB->y0) && (pRectA->y1 > pRectB->y1) &&
                         (pRectA->x0 >= pRectB->x0) && (pRectA->x1 <= pRectB->x1))
                    {
                    RECT_REMOVE(pHeadRect, pRectB);
                    if (pRectA->x0 > pRectB->x0)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectB->x0;
                        pRectX->y0 = pRectB->y0;
                        pRectX->x1 = pRectA->x0;
                        pRectX->y1 = pRectB->y1;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    if (pRectA->x1 < pRectB->x1)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectA->x1;
                        pRectX->y0 = pRectB->y0;
                        pRectX->x1 = pRectB->x1;
                        pRectX->y1 = pRectB->y1;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    RECT_FREE(pRectB);
                    }

                /* Case 4
                 *
                 *       A-----+
                 *  B----|..   |
                 *  |    | :   |
                 *  |    | :   |
                 *  +----|..   |
                 *       |     |
                 *       +-----+
                 */
                else if ((pRectA->y0 < pRectB->y0) && (pRectA->y1 > pRectB->y1) &&
                         (pRectA->x1 > pRectB->x1) && (pRectA->x0 >= pRectB->x0)&&
                         (pRectA->x0 < pRectB->x1))
                    {
                    if (pRectA->x0 > pRectB->x0)
                        pRectB->x1 = pRectA->x0;
                    else
                        {
                        RECT_REMOVE(pHeadRect, pRectB);
                        RECT_FREE(pRectB);
                        }
                    }

                /* Case 5
                 *
                 *  A-----+
                 *  | B...|---+
                 *  | :   |   |
                 *  | :   |   |
                 *  | ....|---+
                 *  |     |
                 *  +-----+
                 */
                else if ((pRectA->y0 < pRectB->y0) && (pRectA->y1 > pRectB->y1) &&
                         (pRectA->x0 < pRectB->x0) && (pRectA->x1 > pRectB->x0) &&
                         (pRectA->x1 <= pRectB->x1))
                    {
                    if (pRectA->x1 < pRectB->x1)
                        pRectB->x0 = pRectA->x1;
                    else
                        {
                        RECT_REMOVE(pHeadRect, pRectB);
                        RECT_FREE(pRectB);
                        }
                    }

                /* Case 6
                 *
                 *  A-------------+
                 *  |   B......   |
                 *  |   :     :   |
                 *  +-------------+
                 *      |     |
                 *      +-----+
                 */
                else if ((pRectA->x0 < pRectB->x0) && (pRectA->x1 > pRectB->x1) &&
                         (pRectA->y0 < pRectB->y0) && (pRectA->y1 > pRectB->y0) &&
                         (pRectA->y1 <= pRectB->y1))
                    {
                    if (pRectA->y1 < pRectB->y1)
                        pRectB->y0 = pRectA->y1;
                    else
                        {
                        RECT_REMOVE(pHeadRect, pRectB);
                        RECT_FREE(pRectB);
                        }
                    }

                /* Case 7
                 *
                 *      B----+
                 *      |    |
                 *  A------------+
                 *  |   :    :   |
                 *  |   ......   |
                 *  +------------+
                 */
                else if ((pRectA->x0 < pRectB->x0) && (pRectA->x1 > pRectB->x1) &&
                         (pRectA->y1 > pRectB->y1) && (pRectA->y0 >= pRectB->y0)&&
                         (pRectA->y0 < pRectB->y1))
                    {
                    if (pRectA->y0 > pRectB->y0)
                        pRectB->y1 = pRectA->y0;
                    else
                        {
                        RECT_REMOVE(pHeadRect, pRectB);
                        RECT_FREE(pRectB);
                        }
                    }

                /* Case 8
                 *
                 *  B---------+
                 *  |  A---+  |
                 *  |  |   |  |
                 *  +--|...|--+
                 *     |   |
                 *     +---+
                 */
                else if ((pRectA->x0 >= pRectB->x0) && (pRectA->x1 <= pRectB->x1) &&
                         (pRectA->y1 > pRectB->y1) && (pRectA->y0 < pRectB->y1) &&
                         (pRectA->y0 >= pRectB->y0))
                    {
                    pRectA->y0 = pRectB->y1;
                    }

                /* Case 9
                 *
                 *      A----+
                 *      |    |
                 *  B---|....|---+
                 *  |   |    |   |
                 *  |   +----+   |
                 *  +------------+
                 */
                else if ((pRectA->x0 >= pRectB->x0) && (pRectA->x1 <= pRectB->x1) &&
                         (pRectA->y0 < pRectB->y0) && (pRectA->y1 > pRectB->y0) &&
                         (pRectA->y1 <= pRectB->y1))
                    {
                    pRectA->y1 = pRectB->y0;
                    }

                /* Case 10
                 *
                 *  B------+
                 *  |      |
                 *  |   A------+
                 *  |   |  :   |
                 *  |   +------+
                 *  +------+
                 */
                else if ((pRectA->y0 >= pRectB->y0) && (pRectA->y1 <= pRectB->y1) &&
                         (pRectA->x1 > pRectB->x1) && (pRectA->x0 < pRectB->x1) &&
                         (pRectA->x0 >= pRectB->x0))
                    {
                    pRectA->x0 = pRectB->x1;
                    }

                /* Case 11
                 *
                 *     B------+
                 *     |      |
                 *  A------+  |
                 *  |  :   |  |
                 *  +------+  |
                 *     +------+
                 */
                else if ((pRectA->y0 >= pRectB->y0) && (pRectA->y1 <= pRectB->y1) &&
                         (pRectA->x0 < pRectB->x0) && (pRectA->x1 > pRectB->x0) &&
                         (pRectA->x1 <= pRectB->x1))
                    {
                    pRectA->x1 = pRectB->x0;
                    }

                /* Case 12
                 *
                 *     A------+
                 *     |      |
                 *  B--|....  |
                 *  |  |   :  |
                 *  |  +------+
                 *  +------+
                 */
                else if ((pRectA->x1 > pRectB->x1) && (pRectA->x0 < pRectB->x1) &&
                         (pRectA->x0 >= pRectB->x0) && (pRectA->y0 < pRectB->y0) &&
                         (pRectA->y1 > pRectB->y0) && (pRectA->y1 <= pRectB->y1))
                    {
                    RECT_REMOVE(pHeadRect, pRectB);
                    if (pRectA->y1 < pRectB->y1)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectB->x0;
                        pRectX->y0 = pRectA->y1;
                        pRectX->x1 = pRectB->x1;
                        pRectX->y1 = pRectB->y1;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    if (pRectA->x0 > pRectB->x0)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectB->x0;
                        pRectX->y0 = pRectB->y0;
                        pRectX->x1 = pRectA->x0;
                        pRectX->y1 = pRectA->y1;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    RECT_FREE(pRectB);
                    }

                /* Case 13
                 *
                 *  B-----+
                 *  |  A-----+
                 *  |  |  :  |
                 *  +--|...  |
                 *     +-----+
                 */
                else if ((pRectA->x1 > pRectB->x1) && (pRectA->x0 < pRectB->x1) &&
                         (pRectA->x0 >= pRectB->x0) && (pRectA->y1 > pRectB->y1) &&
                         (pRectA->y0 < pRectB->y1) && (pRectA->y0 >= pRectB->y0))
                    {
                    RECT_REMOVE(pHeadRect, pRectB);
                    if (pRectA->y0 > pRectB->y0)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectB->x0;
                        pRectX->y0 = pRectB->y0;
                        pRectX->x1 = pRectB->x1;
                        pRectX->y1 = pRectA->y0;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    if (pRectA->x0 > pRectB->x0)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectB->x0;
                        pRectX->y0 = pRectA->y0;
                        pRectX->x1 = pRectA->x0;
                        pRectX->y1 = pRectB->y1;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    RECT_FREE(pRectB);
                    }

                /* Case 14
                 *
                 *  A-----+
                 *  |  B..|--+
                 *  |  :  |  |
                 *  +-----+  |
                 *     +-----+
                 */
                else if ((pRectA->x0 < pRectB->x0) && (pRectA->x1 > pRectB->x0) &&
                         (pRectA->x1 <= pRectB->x1) && (pRectA->y0 < pRectB->y0) &&
                         (pRectA->y1 > pRectB->y0) && (pRectA->y1 <= pRectB->y1))
                    {
                    RECT_REMOVE(pHeadRect, pRectB);
                    if (pRectA->y1 < pRectB->y1)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectB->x0;
                        pRectX->y0 = pRectA->y1;
                        pRectX->x1 = pRectB->x1;
                        pRectX->y1 = pRectB->y1;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    if (pRectA->x1 < pRectB->x1)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectA->x1;
                        pRectX->y0 = pRectB->y0;
                        pRectX->x1 = pRectB->x1;
                        pRectX->y1 = pRectA->y1;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    RECT_FREE(pRectB);
                    }

                /* Case 15
                 *
                 *     B------+
                 *     |      |
                 *  A------+  |
                 *  |  :   |  |
                 *  |  ....|--+
                 *  +------+
                 */
                else if ((pRectA->x0 < pRectB->x0) && (pRectA->x1 > pRectB->x0) &&
                         (pRectA->x1 <= pRectB->x1) && (pRectA->y1 > pRectB->y1) &&
                         (pRectA->y0 < pRectB->y1) && (pRectA->y0 >= pRectB->y0))
                    {
                    RECT_REMOVE(pHeadRect, pRectB);
                    if (pRectA->y0 > pRectB->y0)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectB->x0;
                        pRectX->y0 = pRectB->y0;
                        pRectX->x1 = pRectB->x1;
                        pRectX->y1 = pRectA->y0;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    if (pRectA->x1 < pRectB->x1)
                        {
                        pRectX = RECT_CALLOC(pRectPool, rectPoolSize);
                        pRectX->x0 = pRectA->x1;
                        pRectX->y0 = pRectA->y0;
                        pRectX->x1 = pRectB->x1;
                        pRectX->y1 = pRectB->y1;
                        RECT_ADD(pHeadRect, pRectX);
                        }
                    RECT_FREE(pRectB);
                    }

                /* Case 16
                 *
                 *  A---------+
                 *  | B.....  |
                 *  | :    :  |
                 *  | ......  |
                 *  |         |
                 *  +---------+
                 */
                else if ((pRectA->x1 > pRectB->x1) && (pRectA->x0 < pRectB->x0) &&
                         (pRectA->y1 > pRectB->y1) && (pRectA->y0 < pRectB->y0))
                    {
                    RECT_REMOVE(pHeadRect, pRectB);
                    RECT_FREE(pRectB);
                    }

                /* ??? */
                else
                    die("Fatal error");
                }
            }
        }

    return (pHeadRect);
    }

/*******************************************************************************
 *
 * initLinearGradient - initialize the linear gradient internals for the
 *                      specified paint
 *
 */
LOCAL void initLinearGradient
    (
    paint_t* pPaint
    )
    {
    float                           dx, dy;
    float                           length2;

    /* Precalculate constants in the gradient function defined as
     *
     *             (dx * (x - x0)) + (dy * (y - y0))
     *   G(x, y) = ---------------------------------
     *                         dx� + dy�
     */
    dx = pPaint->linearGradient_x1 - pPaint->linearGradient_x0;
    dy = pPaint->linearGradient_y1 - pPaint->linearGradient_y0;
    length2 = SQUARED(dx) + SQUARED(dy);

    pPaint->linearGradient_dx = dx;
    pPaint->linearGradient_dy = dy;
    pPaint->linearGradient_q = (length2 == 0.0f) ? (0.0f) : (COLOR_RAMP_MAX_COORD / length2);
    }

/*******************************************************************************
 *
 * initRadialGradient - initialize the radial gradient internals for the
 *                      specified paint
 *
 */
LOCAL void initRadialGradient
    (
    paint_t* pPaint
    )
    {
    float   dist;   /* distance from center to focus point */
    float   cx, cy; /* center */
    float   fx, fy; /* focus point */
    float   kx, ky; /* fx - cx, fy - cy */
    float   q;      /* r� - kx� - ky� */
    float   r, r2;  /* radius */

    /* Get data */
    cx = pPaint->radialGradient_cx, cy = pPaint->radialGradient_cy;
    fx = pPaint->radialGradient_fx, fy = pPaint->radialGradient_fy;
    r = pPaint->radialGradient_r, r2 = SQUARED(r);

    /*
     * Per specifications, if the radius is not strictly positive, the gradient
     * function is given the value 1 everywhere
     */
    if (r <= 0.0)
        {
        pPaint->radialGradient_a = 0.0;
        pPaint->radialGradient_b = 0.0;
        pPaint->radialGradient_c = 0.0;
        pPaint->radialGradient_d = 0.0;
        pPaint->radialGradient_e = 0.0;
        }
    else
        {
        /* Force the focus point to lie inside the circumference of the circle */
        dist = SQUARED(fx - cx) + SQUARED(fy - cy);
        if (dist > r2)
            {
            dist = 0.99f * (r / (float)sqrt((double)dist));
            pPaint->radialGradient_fx = fx = cx + (fx * dist);
            pPaint->radialGradient_fy = fy = cy + (fy * dist);
            }

        /*
         * Precalculate constants in the gradient function defined as
         *               _______________________________________________
         *           \/ (r� * (dx� + dy�)) - ((dx * ky) - (dy * kx))�  + ((dx * kx) + (dy * ky))
         * G(x, y) = ---------------------------------------------------------------------------
         *             _________                  r� - (kx� + ky�)
         *         = \/ S(x, y)  + T(x, y)
         *
         * with S(x, y) = (a * x�) + (b * y�) + (c * x * y) and
         *      T(x, y) = (d * x) + (e * y)
         *
         * In order to minimize the number of operations in the scanline
         * rasterizer, the form of S(x, y) and T(x, y) can be simplified as
         * follows
         *                                                                    ___             ___                 _______
         *   S(x, y) = (x' * x') + (y' * y') + (c' * x' * y') with x' = x * \/ a , y' = y * \/ c , and c' = c / \/ a * b
         *           = ((x' + (c' * y')) * x') + (y' * y')
         */
        kx = fx - cx, ky = fy - cy;
        q = r2 - SQUARED(kx) - SQUARED(ky);

        pPaint->radialGradient_a = (float)sqrt((double)(r2 - SQUARED(ky))) / q;
        pPaint->radialGradient_b = (float)sqrt((double)(r2 - SQUARED(kx))) / q;
        pPaint->radialGradient_c = (2 * kx * ky) / (q * q * pPaint->radialGradient_a * pPaint->radialGradient_b);
        pPaint->radialGradient_d = kx / (q * pPaint->radialGradient_a);
        pPaint->radialGradient_e = ky / (q * pPaint->radialGradient_b);

        /*
         * Premultiply to avoid an extra operation on the coordinates
         * during rasterization
         */
        pPaint->radialGradient_a *= COLOR_RAMP_MAX_COORD;
        pPaint->radialGradient_b *= COLOR_RAMP_MAX_COORD;
        }
    }


/*******************************************************************************
 *
 * set - set the value of a paramater on the current context
 *
 */
LOCAL void set
    (
    VGParamType param,
    int count,
    const _VGType* values,
    int type
    )
    {
    int                             i;
    int                             x;  /* parameter value */

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if ((count < 0) || ((count > 0) &&
            (isInvalidPtr(values, sizeof(_VGType)))) ||
            ((count == 0) && (values != NULL)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Set parameter */
        x = (values != NULL) ? (TO_INT(values[0], type)) : (0);
        switch (param)
            {
            /* Scalars */
            case VG_FILL_RULE:
                if ((count != 1) || (x < VG_EVEN_ODD) || (x > VG_NON_ZERO))
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.fillRule = x;
                break;

            case VG_FILTER_CHANNEL_MASK:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.filterChannelMask = x;
                break;

            case VG_FILTER_FORMAT_LINEAR:
                if ((count != 1) || ((x != VG_TRUE) && (x != VG_FALSE)))
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.filterLinear = x;
                break;

            case VG_FILTER_FORMAT_PREMULTIPLIED:
                if ((count != 1) || ((x != VG_TRUE) && (x != VG_FALSE)))
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.filterPremultiplied = x;
                break;

            case VG_IMAGE_MODE:
                if ((count != 1) || (x < VG_DRAW_IMAGE_NORMAL) ||
                    (x > VG_DRAW_IMAGE_STENCIL))
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.imageMode = x;
                break;

            case VG_IMAGE_QUALITY:
                if ((count != 1) || ((x & ~VG_IMAGE_QUALITY_MASK) != 0) ||
                    ((x & (x - 1)) != 0))
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.imageQuality = x;
                break;

            case VG_PIXEL_LAYOUT:
                if ((count != 1) || (x < VG_PIXEL_LAYOUT_UNKNOWN) ||
                    (x > VG_PIXEL_LAYOUT_BGR_HORIZONTAL))
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.pixelLayout = x;
                break;

            case VG_SCISSORING:
                if ((count != 1) || ((x != VG_TRUE) && (x != VG_FALSE)))
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.enableScissoring = x;
                break;

            case VG_MATRIX_MODE:
                if ((count != 1) || (x < VG_MATRIX_PATH_USER_TO_SURFACE) ||
                    (x > VG_MATRIX_GLYPH_USER_TO_SURFACE))
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.matrixMode = x;

                pGc->vg.pCurrentMatrix = pGc->vg.matrix[MATRIX_INDEX(x)];
                break;

            /* Vectors */
            case VG_CLEAR_COLOR:
                if (count != 4)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.clearColor[VG_R] = clamp1(TO_FLOAT(values[0], type));
                pGc->vg.clearColor[VG_G] = clamp1(TO_FLOAT(values[1], type));
                pGc->vg.clearColor[VG_B] = clamp1(TO_FLOAT(values[2], type));
                pGc->vg.clearColor[VG_A] = clamp1(TO_FLOAT(values[3], type));
                break;

            case VG_GLYPH_ORIGIN:
                if (count != 2)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.glyphOrigin[0] = TO_FLOAT(values[0], type);
                pGc->vg.glyphOrigin[1] = TO_FLOAT(values[1], type);
                pGc->vg.glyphOrigin[2] = 1.0;
                break;

            case VG_SCISSOR_RECTS:
                if (values == NULL)
                    pGc->vg.pScissorRects = NULL;
                else
                    {
                    if ((count % 4) != 0)
                        VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                    if (count > MAX_SCISSOR_RECTS)
                        count = MAX_SCISSOR_RECTS;

                    if (pGc->vg.scissoringData == NULL)
                        VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);
                    for (i = 0; i < count; i++)
                        ((int*)pGc->vg.scissoringData)[i] = TO_INT(values[i], type);
                    pGc->vg.numScissorRects = count;
                    pGc->vg.pScissorRects = generateClippingRects(pGc->vg.scissoringData,
                                                                  count / 4,
                                                                  (void *)((long)pGc->vg.scissoringData + RECT_POOL_OFFSET),
                                                                  RECT_POOL_SIZE,
                                                                  pGc->vg.pDrawSurface);
                    }
                break;

            case VG_TILE_FILL_COLOR:
                if (count != 4)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                pGc->vg.tileFillColor[VG_R] = clamp1(TO_FLOAT(values[0], type));
                pGc->vg.tileFillColor[VG_G] = clamp1(TO_FLOAT(values[1], type));
                pGc->vg.tileFillColor[VG_B] = clamp1(TO_FLOAT(values[2], type));
                pGc->vg.tileFillColor[VG_A] = clamp1(TO_FLOAT(values[3], type));
                break;

            /* ??? */
            default:
                VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                break;
            }
        }
    }

/*******************************************************************************
 *
 * setParameter - set the value of a parameter on the specified object
 *
 */
LOCAL void setParameter
    (
    void* pObject,
    VGParamType param,
    int count,
    _VGType* values,
    int type
    )
    {
    int                             i;
    int                             objType;    /* type of object */
    paint_t*                        pPaint;     /* paint */
    int                             x;          /* parameter value */

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        objType = getObjectType(pObject, pGc);
        if (objType < 0)
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((count < 0) || (((count > 0) &&
            isInvalidPtr(values, sizeof(_VGType)))) ||
            ((count == 0) && (values != NULL)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Set parameter */
        x = (values != NULL) ? (TO_INT(values[0], type)) : (0);
        switch (objType)
            {
            case OBJECT_TYPE_PATH:
                VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                break;

            case OBJECT_TYPE_IMAGE:
                VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                break;

            case OBJECT_TYPE_MASK_LAYER:
                /* :TODO: */
                break;

            case OBJECT_TYPE_FONT:
                VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                break;

            case OBJECT_TYPE_PAINT:
                pPaint = pObject;
                switch ((VGPaintParamType)param)
                    {
                    case VG_PAINT_COLOR:
                        if (count != 4)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        pPaint->color[0] = clamp1(TO_FLOAT(values[0], type));
                        pPaint->color[1] = clamp1(TO_FLOAT(values[1], type));
                        pPaint->color[2] = clamp1(TO_FLOAT(values[2], type));
                        pPaint->color[3] = clamp1(TO_FLOAT(values[3], type));

                        pPaint->dirty = TRUE;
                        break;

                    case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
                        if ((count != 1) || (x < VG_COLOR_RAMP_SPREAD_PAD) ||
                            (x > VG_COLOR_RAMP_SPREAD_REFLECT))
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        pPaint->spreadMode = x;
                        break;

                    case VG_PAINT_COLOR_RAMP_STOPS:
                        if ((count % 5) != 0)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                        if (count > MAX_STOPS)
                            count = MAX_STOPS;

                        for (i = 0; i < count; i += 5)
                            {
                            pPaint->stops[i] = TO_FLOAT(values[i], type);
                            pPaint->stops[i + 1] = clamp1(TO_FLOAT(values[i + 1], type));
                            pPaint->stops[i + 2] = clamp1(TO_FLOAT(values[i + 2], type));
                            pPaint->stops[i + 3] = clamp1(TO_FLOAT(values[i + 3], type));
                            pPaint->stops[i + 4] = clamp1(TO_FLOAT(values[i + 4], type));
                            }
                        pPaint->numStops = count;

                        pPaint->dirty = TRUE;
                        break;

                    case VG_PAINT_TYPE:
                        if ((count != 1) || (x < VG_PAINT_TYPE_COLOR) ||
                            (x > VG_PAINT_TYPE_PATTERN))
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        pPaint->type = x;

                        pPaint->dirty = TRUE;
                        break;

                    case VG_PAINT_LINEAR_GRADIENT:
                        if (count != 4)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        for (i = 0; i < count; i++)
                            pPaint->linearGradient[i] = TO_FLOAT(values[i], type);

                        initLinearGradient(pPaint);
                        break;

                    case VG_PAINT_RADIAL_GRADIENT:
                        if (count != 5)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        for (i = 0; i < count; i++)
                            pPaint->radialGradient[i] = TO_FLOAT(values[i], type);

                        initRadialGradient(pPaint);
                        break;

                    case VG_PAINT_PATTERN_TILING_MODE:
                        if ((count != 1) || (x < VG_TILE_FILL) || (x > VG_TILE_REFLECT))
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        pPaint->pattern.tilingMode = x;

                        pPaint->dirty = TRUE;
                        break;

                    case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
                        if ((count != 1) || ((x != VG_TRUE) && (x != VG_FALSE)))
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        pPaint->premultiplyColorRamp = x;
                        break;

                    default:
                        VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                        break;
                    }
                break;
            }
        }
    }

/*******************************************************************************
 *
 * vgSetf
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetf
    (
    VGParamType param,
    VGfloat value
    )
    {
    set(param, 1, (_VGType*)&value, VG_FLOAT);
    }

/*******************************************************************************
 *
 * vgSetfv
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetfv
    (
    VGParamType param,
    VGint count,
    const VGfloat* values
    )
    {
    set(param, count, (_VGType*)values, VG_FLOAT);
    }

/*******************************************************************************
 *
 * vgSeti
 *
 */
VG_API_CALL void VG_API_ENTRY vgSeti
    (
    VGParamType param,
    VGint value
    )
    {
    set(param, 1, (_VGType*)&value, VG_INT);
    }

/*******************************************************************************
 *
 * vgSetiv
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetiv
    (
    VGParamType param,
    VGint count,
    const VGint* values
    )
    {
    set(param, count, (_VGType*)values, VG_INT);
    }

/*******************************************************************************
 *
 * vgSetParameterf
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetParameterf
    (
    VGHandle object,
    VGint param,
    VGfloat value
    ) VG_API_EXIT
    {
    setParameter((void*)object, param, 1, (_VGType*)&value, VG_FLOAT);
    }

/*******************************************************************************
 *
 * vgSetParameterfv
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetParameterfv
    (
    VGHandle object,
    VGint param,
    VGint count,
    const VGfloat* values
    ) VG_API_EXIT
    {
    setParameter((void*)object, param, count, (_VGType*)values, VG_FLOAT);
    }

/*******************************************************************************
 *
 * vgSetParameteri
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetParameteri
    (
    VGHandle object,
    VGint param,
    VGint value
    ) VG_API_EXIT
    {
    setParameter((void*)object, param, 1, (_VGType*)&value, VG_INT);
    }

/*******************************************************************************
 *
 * vgSetParameteriv
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetParameteriv
    (
    VGHandle object,
    VGint param,
    VGint count,
    const VGint* values
    ) VG_API_EXIT
    {
    setParameter((void*)object, param, count, (_VGType*)values, VG_INT);
    }
