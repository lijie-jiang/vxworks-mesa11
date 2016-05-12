/* vgdraw.c - Wind River VG Draw Functionality */

/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
31mar15,rpc  Static analysis fixes (US50633)
22dec14,yat  Fix build warning for LP64 (US50456)
14aug13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
21may09,m_c  Written
*/

/*
DESCRIPTION
These routines provide the bulk of the draw capabilities that support the OpenVG
implementation.
*/

/* includes */

#include <stdlib.h>
#include <string.h>
#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/* defines */
#define DEBUG_LEVEL 0

/* Clipping macros */
#define CLIP_END_BOTTOM()   do                          \
                                {                       \
                                t = (yMax - y2) / dy;   \
                                x2 += t * dx;           \
                                y2 = yMax;              \
                                } while (0)
#define CLIP_END_LEFT()     do                          \
                                {                       \
                                t = (xMin - x2) / dx;   \
                                x2 = xMin;              \
                                y2 += t * dy;           \
                                } while (0)
#define CLIP_END_RIGHT()    do                          \
                                {                       \
                                t = (xMax - x2) / dx;   \
                                x2 = xMax;              \
                                y2 += t * dy;           \
                                } while (0)
#define CLIP_END_TOP()      do                          \
                                {                       \
                                t = (yMin - y2) / dy;   \
                                x2 += t * dx;           \
                                y2 = yMin;              \
                                } while (0)
#define CLIP_START_BOTTOM() do                          \
                                {                       \
                                t = (yMax - y1) / dy;   \
                                x1 += t * dx;           \
                                y1 = yMax;              \
                                } while (0)
#define CLIP_START_LEFT()   do                          \
                                {                       \
                                t = (xMin - x1) / dx;   \
                                x1 = xMin;              \
                                y1 += t * dy;           \
                                } while (0)
#define CLIP_START_RIGHT()  do                          \
                                {                       \
                                t = (xMax - x1) / dx;   \
                                x1 = xMax;              \
                                y1 += t * dy;           \
                                } while (0)
#define CLIP_START_TOP()    do                          \
                                {                       \
                                t = (yMin - y1) / dy;   \
                                x1 += t * dx;           \
                                y1 = yMin;              \
                                } while (0)
#define SAVE_END()          SAVE_VERTEX(x2, y2)
#define SAVE_START()        SAVE_VERTEX(x1, y1)
#define SAVE_VERTEX(_x, _y) do                          \
                                {                       \
                                V->x = _x;              \
                                V->y = _y;              \
                                V++;                    \
                                } while (0)

/* Draw flags */
#define DRAW_FLAG_FORCE_ALPHA   (1 << 0)
#define DRAW_FLAG_IGNORE_ALPHA  (1 << 1)

/* Draw types */
#define DRAW_TYPE_FILL          0    /* color fill */
#define DRAW_TYPE_TEXMAP        1    /* affine texture mapping */
#define DRAW_TYPE_TEXMAP_3D     2    /* projective texture mapping */
#define DRAW_TYPE_LINEAR_GRAD   3    /* linear gradient */
#define DRAW_TYPE_RADIAL_GRAD   4    /* radial gradient */
#define DRAW_TYPE_PATTERN       5    /* pattern */

/* Pattern lookup table */
#define U_OFFSET                0
#define V_OFFSET                PATTERN_DENORM_LUT_SIZE

/* typedefs */

/* Brush */
typedef union
    {
    uint        color;      /* solid color */
    surface_t * pBitmap;    /* bitmap */
    paint_t *   pPaint;     /* paint */
    pattern_t * pPattern;   /* pattern */
    } brush_t;

/* Polygon edge */
typedef struct
    {
    vertex_ptr  V1;     /* first vertex */
    int         height; /* number of scanlines */
    float       x, ix;  /* x */
    float       w, iw;  /* w */
    float       u, iu;  /* mapping coordinates */
    float       v, iv;
    } edge_t;

/* Triangle gradients */
typedef struct
    {
    char    dirty;      /* TRUE if need to be updated */
    float   dwdx, dwdy; /* gradients */
    float   dudx, dudy;
    float   dvdx, dvdy;
    } gradients_t;

/* forward declarations */

LOCAL void calcGradients(const vertex_ptr*, gradients_t*);
LOCAL VOIDFUNCPTR chooseRasterizer(VGImageFormat, VGImageFormat, int, int);
LOCAL long clipTriangle(const vertex_ptr*, float, float, float, float, vertex_t*);
LOCAL __inline__ int denormClamp(float x);
LOCAL __inline__ int denormWrap(float x);
LOCAL void drawScanlineFill32A(int, int, uint*, brush_t);
LOCAL void drawScanlineFill32X(int, int, uint*, brush_t);
LOCAL void drawScanlineLinear32A(int, int, uint*, brush_t, const edge_t*, const gradients_t*);
LOCAL void drawScanlineMapping32A(int, int, uint*, brush_t, const edge_t*, const gradients_t*);
LOCAL void drawScanlineMapping32X(int, int, uint*, brush_t, const edge_t*, const gradients_t*);
LOCAL void drawScanlinePattern32A(int, int, uint*, brush_t, const edge_t*, const gradients_t*);
LOCAL void drawScanlinePattern32X(int, int, uint*, brush_t, const edge_t*, const gradients_t*);
LOCAL void drawScanlineProjMapping32A(int, int, uint*, brush_t, const edge_t*, const gradients_t*);
LOCAL void drawScanlineProjMapping32X(int, int, uint*, brush_t, const edge_t*, const gradients_t*);
LOCAL void drawScanlineRadial32A(int, int, uint*, brush_t, const edge_t*, const gradients_t*);
LOCAL void drawTriangle(const vertex_ptr*, gradients_t*, brush_t, surface_t*, rectangle_t*, VOIDFUNCPTR);
LOCAL int generateColorRamp(paint_t*);
LOCAL glyph_t * getGlyph(const font_t*, int);
LOCAL void initEdge(edge_t*, int, const vertex_ptr, const vertex_ptr, const gradients_t*);
LOCAL void initPatternLookUpTable(paint_t* pPaint, int offset, int dimension, int stride);
LOCAL int isAffine(const float*);
LOCAL char isProjective(const float*);
LOCAL void stepEdge(edge_t*);
LOCAL int isClockwise(const vertex_ptr, const vertex_ptr, const vertex_ptr);

/* locals */

/* Default paint (fill and stroke) */
LOCAL paint_t defaultPaint = {.dirty = TRUE,
                              .type = VG_PAINT_TYPE_COLOR,
                              .color = {0.0, 0.0, 0.0, 1.0},
                              .numStops = 2,
                              .stops = {0.0, 0.0, 0.0, 0.0, 1.0,
                                       1.0, 1.0, 1.0, 1.0, 1.0}};

/* Draw functions (see DRAW_xxx macros) */
LOCAL const VOIDFUNCPTR drawScanlineFnTable32[] = {/* Fill */
                                                   drawScanlineFill32X,
                                                   drawScanlineFill32A,
                                                   /* Mapping */
                                                   drawScanlineMapping32X,
                                                   drawScanlineMapping32A,
                                                   /* ProjMapping */
                                                   drawScanlineProjMapping32X,
                                                   drawScanlineProjMapping32A,
                                                   /* Linear */
                                                   NULL,
                                                   drawScanlineLinear32A,
                                                   /* Radial */
                                                   NULL,
                                                   drawScanlineRadial32A,
                                                   /* Pattern */
                                                   drawScanlinePattern32X,
                                                   drawScanlinePattern32A};

/*******************************************************************************
 *
 * calcGradients - calculate the gradients for the specified triangle
 *
 */
LOCAL void calcGradients
    (
    const vertex_ptr* pTriangle,
    gradients_t* pGradients
    )
    {
    float   dt[2];
    float   dx[2], dy[2];
    float   q;

    dx[0] = pTriangle[0]->x - pTriangle[2]->x, dx[1] = pTriangle[1]->x - pTriangle[2]->x;
    dy[0] = pTriangle[0]->y - pTriangle[2]->y, dy[1] = pTriangle[1]->y - pTriangle[2]->y;
    q = (dx[1] * dy[0]) - (dx[0] * dy[1]);
    if (q != 0.0f)
        {
        q = 1.0f / q;

        dt[0] = pTriangle[0]->w - pTriangle[2]->w, dt[1] = pTriangle[1]->w - pTriangle[2]->w;
        pGradients->dwdx = ((dt[1] * dy[0]) - (dt[0] * dy[1])) * q;
        pGradients->dwdy = ((dt[0] * dx[1]) - (dt[1] * dx[0])) * q;
        dt[0] = pTriangle[0]->u - pTriangle[2]->u, dt[1] = pTriangle[1]->u - pTriangle[2]->u;
        pGradients->dudx = ((dt[1] * dy[0]) - (dt[0] * dy[1])) * q;
        pGradients->dudy = ((dt[0] * dx[1]) - (dt[1] * dx[0])) * q;
        dt[0] = pTriangle[0]->v - pTriangle[2]->v, dt[1] = pTriangle[1]->v - pTriangle[2]->v;
        pGradients->dvdx = ((dt[1] * dy[0]) - (dt[0] * dy[1])) * q;
        pGradients->dvdy = ((dt[0] * dx[1]) - (dt[1] * dx[0])) * q;

        pGradients->dirty = FALSE;
        }
    }

/*******************************************************************************
 *
 * chooseRasterizer - determine the rasterizer matching the specified parameters
 *
 */
LOCAL VOIDFUNCPTR chooseRasterizer
    (
    VGImageFormat dstFormat,
    VGImageFormat srcFormat,
    int type,
    int flags
    )
    {
    VOIDFUNCPTR drawScanlineFn = NULL;
    int         index;

    switch (dstFormat)
        {
        case VG_sXRGB_8888:
        case VG_sARGB_8888_PRE:
            index = type * 2;
            switch (srcFormat)
                {
                case VG_sXRGB_8888:
                    if (flags & DRAW_FLAG_FORCE_ALPHA)
                        index++;
                    drawScanlineFn = drawScanlineFnTable32[index + 0];
                    break;

                case VG_sARGB_8888_PRE:
                    if (flags & DRAW_FLAG_IGNORE_ALPHA)
                        index--;
                    drawScanlineFn = drawScanlineFnTable32[index + 1];
                    break;
                default:
                    break;
                }
            break;
        default:
            break;
        }
    assert(drawScanlineFn != NULL);

    return (drawScanlineFn);
    }

/*******************************************************************************
 *
 * clipTriangle - clip a triangle against the specified rectangle
 *
 * The algorithm is a derivative of Sutherland-Hodgman. However, it has beed
 * improved by removing the edge clipping loop and intermediate storage;
 * instead a big switch statement deals with all possible combinations of each
 * start and end vertices position with regards to each other.
 *
 *  Each vertex is assigned an outcode depending on its location vis-a-vis the
 *  clipping rectangle:
 *
 *        |  top   |           1010 | 1000 | 1100     a | 8 | c
 *   -----+--------+------     -----+------+-----     --+---+--
 *   left | center | right  >  0010 | 0000 | 0100  >  2 | 0 | 4
 *   -----+--------+------     -----+------+-----     --+---+--
 *        | bottom |           0011 | 0001 | 0101     3 | 1 | 5
 *
 *  Each edge has a "combined" outcode that can take one of 81 possible values.
 *  If the end vertex is in a corner region, then a turning point must be added
 *  to maintain polygon continuity. A turning point must also be added in some
 *  cases where a discarded edge is going "over" a corner, e.g. case 0x21.
 *
 *  The tests performed show a 14% speed increase over two-pass axis boundary
 *  clipping when the triangle is cut into an heptagon (and 9.5% if it is
 *  entirely in the clipping window)
 *
 *  RETURNS: The number of vertices in the resulting polygon
 */
LOCAL long clipTriangle
    (
    const vertex_ptr* pTriangle,
    float xMin,
    float xMax,
    float yMin,
    float yMax,
    vertex_t* polygon
    )
    {
    float       dx, dy;             /* current edge's deltas */
    int         i, j;
    static char i1[3] = {1, 2, 0};  /* lookup table for i + 1 */
    char        outcode[3];         /* outcodes */
    char        tmp;
    float       t;
    float       x1, y1;             /* start vertex coordinates */
    float       x2, y2;             /* end vertex coordinates */
    vertex_t *  V;                  /* current vertex (in polygon) */

    /* Determine the outcodes */
    for (i = 0; i < 3; i++)
        {
        tmp = 0;
        x1 = pTriangle[i]->x, y1 = pTriangle[i]->y;
        if (x1 < xMin)
            tmp |= 0x02;
        else if (x1 > xMax)
            tmp |= 0x04;
        if (y1 < yMin)
            tmp |= 0x08;
        else if (y1 > yMax)
            tmp |= 0x01;
        outcode[i] = tmp;
        }

    /* Clip */
    V = polygon;
    for (i = 0; i < 3; i++)
        {
        j = i1[i];
        x1 = pTriangle[i]->x, y1 = pTriangle[i]->y;
        x2 = pTriangle[j]->x, y2 = pTriangle[j]->y;
        dx = x1 - x2;
        dy = y1 - y2;
        tmp = outcode[j];
        /* Clip edge */
        switch ((outcode[i] << 4) | tmp)
            {
            case 0x00:
                SAVE_START();
                break;

            case 0x01:
                CLIP_END_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0x02:
                CLIP_END_LEFT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x03:
                CLIP_END_LEFT();
                if (y2 > yMax)
                    CLIP_END_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0x04:
                CLIP_END_RIGHT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x05:
                CLIP_END_RIGHT();
                if (y2 > yMax)
                    CLIP_END_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0x08:
                CLIP_END_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0x0a:
                CLIP_END_LEFT();
                if (y2 < yMin)
                    CLIP_END_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0x0c:
                CLIP_END_RIGHT();
                if (y2 < yMin)
                    CLIP_END_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0x10:
                CLIP_START_BOTTOM();
                SAVE_START();
                break;

            case 0x12:
                CLIP_START_BOTTOM();
                if (x1 < xMin)
                    return (0);
                CLIP_END_LEFT();
                if (y2 > yMax)
                    CLIP_END_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0x14:
                CLIP_START_BOTTOM();
                if (x1 > xMax)
                    {
                    SAVE_VERTEX(xMax, yMax);
                    break;
                    }
                CLIP_END_RIGHT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x18:
                CLIP_START_BOTTOM();
                CLIP_END_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0x1a:
                CLIP_START_BOTTOM();
                if (x1 < xMin)
                    return (0);
                CLIP_END_LEFT();
                if (y2 < yMin)
                    CLIP_END_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0x1c:
                CLIP_START_BOTTOM();
                if (x1 > xMax)
                    {
                    SAVE_VERTEX(xMax, yMax);
                    break;
                    }
                CLIP_END_RIGHT();
                if (y2 < yMin)
                    CLIP_END_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0x20:
                CLIP_START_LEFT();
                SAVE_START();
                break;

            case 0x21:
                CLIP_START_LEFT();
                if (y1 > yMax)
                    {
                    SAVE_VERTEX(xMin, yMax);
                    break;
                    }
                CLIP_END_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0x24:
                CLIP_START_LEFT();
                CLIP_END_RIGHT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x25:
                CLIP_START_LEFT();
                if (y1 > yMax)
                    {
                    SAVE_VERTEX(xMin, yMax);
                    break;
                    }
                CLIP_END_BOTTOM();
                if (x2 > xMax)
                    CLIP_END_RIGHT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x28:
                CLIP_START_LEFT();
                if (y1 < yMin)
                    return (0);
                CLIP_END_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0x2c:
                CLIP_START_LEFT();
                if (y1 < yMin)
                    return (0);
                CLIP_END_TOP();
                if (x2 > xMax)
                    CLIP_END_RIGHT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x30:
                CLIP_START_LEFT();
                if (y1 > yMax)
                    CLIP_START_BOTTOM();
                SAVE_START();
                break;

            case 0x34:
                CLIP_END_RIGHT();
                if (y2 > yMax)
                    {
                    SAVE_VERTEX(xMax, yMax);
                    break;
                    }
                CLIP_START_BOTTOM();
                if (x1 < xMin)
                    CLIP_START_LEFT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x38:
                CLIP_END_TOP();
                if (x2 < xMin)
                    {
                    SAVE_VERTEX(xMin, yMin);
                    break;
                    }
                CLIP_START_BOTTOM();
                if (x1 < xMin)
                    CLIP_START_LEFT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x3c:
                CLIP_START_LEFT();
                if (y1 < yMin)
                    return (0);
                CLIP_END_RIGHT();
                if (y2 > yMax)
                    {
                    SAVE_VERTEX(xMax, yMax);
                    break;
                    }
                if (y1 > yMax)
                    CLIP_START_BOTTOM();
                if (y2 < yMin)
                    CLIP_END_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0x40:
                CLIP_START_RIGHT();
                SAVE_START();
                break;

            case 0x41:
                CLIP_START_RIGHT();
                if (y1 > yMax)
                    return (0);
                CLIP_END_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0x42:
                CLIP_START_RIGHT();
                CLIP_END_LEFT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x43:
                CLIP_START_RIGHT();
                if (y1 > yMax)
                    return (0);
                CLIP_END_BOTTOM();
                if (x2 < xMin)
                    CLIP_END_LEFT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x48:
                CLIP_START_RIGHT();
                if (y1 < yMin)
                    {
                    SAVE_VERTEX(xMax, yMin);
                    break;
                    }
                CLIP_END_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0x4a:
                CLIP_START_RIGHT();
                if (y1 < yMin)
                    {
                    SAVE_VERTEX(xMax, yMin);
                    break;
                    }
                CLIP_END_TOP();
                if (x2 < xMin)
                    CLIP_END_LEFT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x50:
                CLIP_START_RIGHT();
                if (y1 > yMax)
                    CLIP_START_BOTTOM();
                SAVE_START();
                break;

            case 0x52:
                CLIP_END_LEFT();
                if (y2 > yMax)
                    return (0);
                CLIP_START_BOTTOM();
                if (x1 > xMax)
                    CLIP_START_RIGHT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x58:
                CLIP_END_TOP();
                if (x2 > xMax)
                    {
                    SAVE_VERTEX(xMax, yMin);
                    break;
                    }
                CLIP_START_RIGHT();
                if (y1 > yMax)
                    CLIP_START_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0x5a:
                CLIP_END_LEFT();
                if (y2 > yMax)
                    return (0);
                CLIP_START_RIGHT();
                if (y1 < yMin)
                    {
                    SAVE_VERTEX(xMax, yMin);
                    break;
                    }
                if (y2 < yMin)
                    CLIP_END_TOP();
                if (y1 > yMax)
                    CLIP_START_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0x80:
                CLIP_START_TOP();
                SAVE_START();
                break;

            case 0x81:
                CLIP_START_TOP();
                CLIP_END_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0x82:
                CLIP_START_TOP();
                if (x1 < xMin)
                    {
                    SAVE_VERTEX(xMin, yMin);
                    break;
                    }
                CLIP_END_LEFT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x83:
                CLIP_START_TOP();
                if (x1 < xMin)
                    {
                    SAVE_VERTEX(xMin, yMin);
                    break;
                    }
                CLIP_END_LEFT();
                if (y2 > yMax)
                    CLIP_END_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0x84:
                CLIP_START_TOP();
                if (x1 > xMax)
                    return (0);
                CLIP_END_RIGHT();
                SAVE_START();
                SAVE_END();
                break;

            case 0x85:
                CLIP_START_TOP();
                if (x1 > xMax)
                    return (0);
                CLIP_END_RIGHT();
                if (y2 > yMax)
                    CLIP_END_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0xa0:
                CLIP_START_LEFT();
                if (y1 < yMin)
                    CLIP_START_TOP();
                SAVE_START();
                break;

            case 0xa1:
                CLIP_END_BOTTOM();
                if (x2 < xMin)
                    {
                    SAVE_VERTEX(xMin, yMax);
                    break;
                    }
                CLIP_START_LEFT();
                if (y1 < yMin)
                    CLIP_START_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0xa4:
                CLIP_END_RIGHT();
                if (y2 < yMin)
                    return (0);
                CLIP_START_TOP();
                if (x1 < xMin)
                    CLIP_START_LEFT();
                SAVE_START();
                SAVE_END();
                break;

            case 0xa5:
                CLIP_START_LEFT();
                if (y1 > yMax)
                    {
                    SAVE_VERTEX(xMin, yMax);
                    break;
                    }
                CLIP_END_RIGHT();
                if (y2 < yMin)
                    return (0);
                if (y1 < yMin)
                    CLIP_START_TOP();
                if (y2 > yMax)
                    CLIP_END_BOTTOM();
                SAVE_START();
                SAVE_END();
                break;

            case 0xc0:
                CLIP_START_RIGHT();
                if (y1 < yMin)
                    CLIP_START_TOP();
                SAVE_START();
                break;

            case 0xc1:
                CLIP_END_BOTTOM();
                if (x2 > xMax)
                    return (0);
                CLIP_START_RIGHT();
                if (y1 < yMin)
                    CLIP_START_TOP();
                SAVE_START();
                SAVE_END();
                break;

            case 0xc2:
                CLIP_END_LEFT();
                if (y2 < yMin)
                    {
                    SAVE_VERTEX(xMin, yMin);
                    break;
                    }
                CLIP_START_TOP();
                if (x1 > xMax)
                    CLIP_START_RIGHT();
                SAVE_START();
                SAVE_END();
                break;

            case 0xc3:
                CLIP_END_LEFT();
                if (y2 < yMin)
                    {
                    SAVE_VERTEX(xMin, yMin);
                    break;
                    }
                CLIP_START_RIGHT();
                if (y1 > yMax)
                    return (0);
                if (y2 > yMax)
                    CLIP_END_BOTTOM();
                if (y1 < yMin)
                    CLIP_START_TOP();
                SAVE_START();
                SAVE_END();
                break;
            }
        /* Add turning point */
        switch (tmp)
            {
            case 0x03:
                SAVE_VERTEX(xMin, yMax);
                break;

            case 0x05:
                SAVE_VERTEX(xMax, yMax);
                break;

            case 0x0a:
                SAVE_VERTEX(xMin, yMin);
                break;

            case 0x0c:
                SAVE_VERTEX(xMax, yMin);
                break;
            }
        }

    /* Return the number of vertices in the clipped triangle */
    return (V - polygon);
    }

/*******************************************************************************
 *
 * denormClamp - clamp the specified value within [0 .. PATTERN_NORM_SIZE]
 *
 * RETURNS: Clamped value
 *
 */
LOCAL __inline__ int denormClamp
    (
    float x
    )
    {
    int             y;
    unsigned int    q;

    y = ifloor(x);
    q = y >> PATTERN_NORM_SIZE2;
    if (q != 0)
        y = (~q) >> (32 - PATTERN_NORM_SIZE2);

    return (y);
    }

/*******************************************************************************
 *
 * denormWrap - wrap the specified value within [0 .. PATTERN_DENORM_SIZE]
 *
 * RETURNS: Wrapped value
 *
 */
LOCAL __inline__ int denormWrap
    (
    float x
    )
    {
    return (ifloor(x) & (PATTERN_DENORM_LUT_SIZE - 1));
    }

/*******************************************************************************
 *
 * drawScanlineFill32A - rasterize a scanline using single color filling
 *                       (with alpha-blending)
 *
 */
LOCAL void drawScanlineFill32A
    (
    int x,
    int width,
    uint * pBuf,
    brush_t brush
    )
    {
    const uint * pEnd;

    pBuf += x;
    pEnd = pBuf + width;
    while (pBuf < pEnd)
        *pBuf = blend32(brush.color, *pBuf), pBuf++;
    }

/*******************************************************************************
 *
 * drawScanlineFill32X - rasterize a scanline using single color filling
 *
 */
LOCAL void drawScanlineFill32X
    (
    int x,
    int width,
    uint* pBuf,
    brush_t brush
    )
    {
    const uint * pEnd;

    pBuf += x;
    pEnd = pBuf + width;
    while (pBuf < pEnd)
        *pBuf++ = brush.color;
    }

/*******************************************************************************
 *
 * drawScanlineLinear32A - rasterize a scanline using a linear gradient
 *                         (with alpha-blending)
 *
 */
LOCAL void drawScanlineLinear32A
    (
    int x,
    int width,
    uint* pBuf,
    brush_t brush,
    const edge_t* pEdge,
    const gradients_t* pGradients
    )
    {
    const uint *    colorRamp = brush.pPaint->colorRamp;
    const float     ha = (float)x - pEdge->x;
    int             i;
    float           u;

    pBuf += x;
    u = pEdge->u + (ha * pGradients->dudx);
    if (brush.pPaint->spreadMode == VG_COLOR_RAMP_SPREAD_PAD)
        {
        /* Clamp */
        while (width-- > 0)
            {
            i = (u < 0.0) ? (0) : ((u > COLOR_RAMP_MAX_COORD) ? (COLOR_RAMP_MAX_COORD) : (ifloor(u)));
            *pBuf = blend32(colorRamp[i], *pBuf), pBuf++;
            u += pGradients->dudx;
            }
        }
    else if (brush.pPaint->spreadMode == VG_COLOR_RAMP_SPREAD_REPEAT)
        {
        /* Wrap */
        while (width-- > 0)
            {
            i = ifloor(u) & COLOR_RAMP_MASK_REPEAT;
            *pBuf = blend32(colorRamp[i], *pBuf), pBuf++;
            u += pGradients->dudx;
            }
        }
    else
        {
        /* Mirror (i.e. VG_COLOR_RAMP_SPREAD_REFLECT) */
        while (width-- > 0)
            {
            i = ifloor(u) & COLOR_RAMP_MASK_REFLECT;
            *pBuf = blend32(colorRamp[i], *pBuf), pBuf++;
            u += pGradients->dudx;
            }
        }
    }

/*******************************************************************************
 *
 * drawScanlineMapping32A - rasterize a scanline using texture mapping
 *                         (with alpha-blending)
 *
 */
LOCAL void drawScanlineMapping32A
    (
    int x,
    int width,
    uint* pBuf,
    brush_t brush,
    const edge_t* pEdge,
    const gradients_t* pGradients
    )
    {
    const float ha = (float)x - pEdge->x;
    uint *      pBits = brush.pBitmap->pBackBuf;
    const int   stride = brush.pBitmap->stride / 4;
    float       u, v;

    pBuf += x;
    u = pEdge->u + (ha * pGradients->dudx);
    v = pEdge->v + (ha * pGradients->dvdx);
    while (width-- > 0)
        {
        *pBuf = blend32(pBits[ifloor(u) + (ifloor(v) * stride)], *pBuf), pBuf++;
        u += pGradients->dudx;
        v += pGradients->dvdx;
        }
    }

/*******************************************************************************
 *
 * drawScanlineMapping32X - rasterize a scanline using texture mapping
 *
 */
LOCAL void drawScanlineMapping32X
    (
    int x,
    int width,
    uint* pBuf,
    brush_t brush,
    const edge_t* pEdge,
    const gradients_t* pGradients
    )
    {
    const float ha = (float)x - pEdge->x;
    uint *      pBits = brush.pBitmap->pBackBuf;
    const int   stride = brush.pBitmap->stride / 4;
    float       u, v;

    pBuf += x;
    u = pEdge->u + (ha * pGradients->dudx);
    v = pEdge->v + (ha * pGradients->dvdx);
    while (width-- > 0)
        {
        *pBuf = pBits[ifloor(u) + (ifloor(v) * stride)], pBuf++;
        u += pGradients->dudx;
        v += pGradients->dvdx;
        }
    }

/*******************************************************************************
 *
 * drawScanlinePattern32A - rasterize a scanline using a pattern
 *                          (with alpha-blending)
 *
 */
LOCAL void drawScanlinePattern32A
    (
    int x,
    int width,
    uint* pBuf,
    brush_t brush,
    const edge_t* pEdge,
    const gradients_t* pGradients
    )
    {
    uint        color;
    int         i, j;
    const float ha = (float)x - pEdge->x;
    uint *      pBits = brush.pPattern->pImage->pSurface->pBackBuf;
    float       u, v;
    const int * uLut = brush.pPattern->denormLut + U_OFFSET;
    const int * vLut = brush.pPattern->denormLut + V_OFFSET;

    pBuf += x;
    u = pEdge->u + (ha * pGradients->dudx);
    v = pEdge->v + (ha * pGradients->dvdx);
    if (brush.pPattern->tilingMode == VG_TILE_FILL)
        {
        /* Fill */
        color = brush.pPattern->fillColor;
        while (width-- > 0)
            {
            i = ifloor(u), j = ifloor(v);
            *pBuf = blend32((((i >> PATTERN_NORM_SIZE2) != 0) || ((j >> PATTERN_NORM_SIZE2) != 0)) ? (color) : (pBits[uLut[i] + vLut[j]]), *pBuf), pBuf++;
            u += pGradients->dudx;
            v += pGradients->dvdx;
            }
        }
    else  if (brush.pPattern->tilingMode == VG_TILE_PAD)
        {
        /* Pad */
        while (width-- > 0)
            {
            *pBuf = blend32(pBits[uLut[denormClamp(u)] + vLut[denormClamp(v)]], *pBuf), pBuf++;
            u += pGradients->dudx;
            v += pGradients->dvdx;
            }
        }
    else
        {
        /* Repeat (VG_TILE_REPEAT) and reflect (VG_TILE_REFLECT) */
        while (width-- > 0)
            {
            *pBuf = blend32(pBits[uLut[denormWrap(u)] + vLut[denormWrap(v)]], *pBuf), pBuf++;
            u += pGradients->dudx;
            v += pGradients->dvdx;
            }
        }
    }

/*******************************************************************************
 *
 * drawScanlinePattern32X - rasterize a scanline using a pattern
 *
 */
LOCAL void drawScanlinePattern32X
    (
    int x,
    int width,
    uint* pBuf,
    brush_t brush,
    const edge_t* pEdge,
    const gradients_t* pGradients
    )
    {
    uint        color;
    int         i, j;
    const float ha = (float)x - pEdge->x;
    uint *      pBits = brush.pPattern->pImage->pSurface->pBackBuf;
    float       u, v;
    const int * uLut = brush.pPattern->denormLut + U_OFFSET;
    const int * vLut = brush.pPattern->denormLut + V_OFFSET;

    pBuf += x;
    u = pEdge->u + (ha * pGradients->dudx);
    v = pEdge->v + (ha * pGradients->dvdx);
    if (brush.pPattern->tilingMode == VG_TILE_FILL)
        {
        /* Fill */
        color = brush.pPattern->fillColor;
        while (width-- > 0)
            {
            i = ifloor(u), j = ifloor(v);
            *pBuf = (((i >> PATTERN_NORM_SIZE2) != 0) || ((j >> PATTERN_NORM_SIZE2) != 0)) ? (color) : (pBits[uLut[i] + vLut[j]]), pBuf++;
            u += pGradients->dudx;
            v += pGradients->dvdx;
            }
        }
    else  if (brush.pPattern->tilingMode == VG_TILE_PAD)
        {
        /* Pad */
        while (width-- > 0)
            {
            *pBuf = pBits[uLut[denormClamp(u)] + vLut[denormClamp(v)]], pBuf++;
            u += pGradients->dudx;
            v += pGradients->dvdx;
            }
        }
    else
        {
        /* Repeat (VG_TILE_REPEAT) and reflect (VG_TILE_REFLECT) */
        while (width-- > 0)
            {
            *pBuf = pBits[uLut[ifloor(u) & (PATTERN_DENORM_LUT_SIZE - 1)] + vLut[ifloor(v) & (PATTERN_DENORM_LUT_SIZE - 1)]], pBuf++;
            u += pGradients->dudx;
            v += pGradients->dvdx;
            }
        }
    }

/*******************************************************************************
 *
 * drawScanlineProjMapping32A - rasterize a scanline using projective texture
 *                              mapping (with alpha-blending)
 *
 */
LOCAL void drawScanlineProjMapping32A
    (
    int x,
    int width,
    uint* pBuf,
    brush_t brush,
    const edge_t* pEdge,
    const gradients_t* pGradients
    )
    {
    const float ha = (float)x - pEdge->x;
    uint *      pBits = brush.pBitmap->pBackBuf;
    const int   stride = brush.pBitmap->stride / 4;
    float       w, u, v;

    pBuf += x;
    w = pEdge->w + (ha * pGradients->dwdx);
    u = pEdge->u + (ha * pGradients->dudx);
    v = pEdge->v + (ha * pGradients->dvdx);
    while (width-- > 0)
        {
        const float w1 = 1.0f / w;
        *pBuf = blend32(pBits[ifloor(u * w1) + (ifloor(v * w1) * stride)], *pBuf), pBuf++;
        w += pGradients->dwdx;
        u += pGradients->dudx;
        v += pGradients->dvdx;
        }
    }

/*******************************************************************************
 *
 * drawScanlineProjMapping32X - rasterize a scanline using projective texture
 *                              mapping
 *
 */
LOCAL void drawScanlineProjMapping32X
    (
    int x,
    int width,
    uint* pBuf,
    brush_t brush,
    const edge_t* pEdge,
    const gradients_t* pGradients
    )
    {
    const float ha = (float)x - pEdge->x;
    uint *      pBits = brush.pBitmap->pBackBuf;
    const int   stride = brush.pBitmap->stride / 4;
    float       w, u, v;

    pBuf += x;
    w = pEdge->w + (ha * pGradients->dwdx);
    u = pEdge->u + (ha * pGradients->dudx);
    v = pEdge->v + (ha * pGradients->dvdx);
    while (width-- > 0)
        {
        const float w1 = 1.0f / w;
        *pBuf = pBits[ifloor(u * w1) + (ifloor(v * w1) * stride)], pBuf++;
        w += pGradients->dwdx;
        u += pGradients->dudx;
        v += pGradients->dvdx;
        }
    }

/*******************************************************************************
 *
 * drawScanlineRadial32A - rasterize a scanline using a radial gradient
 *                         (with alpha-blending)
 *
 */
LOCAL void drawScanlineRadial32A
    (
    const int x,
    int width,
    uint* pBuf,
    brush_t brush,
    const edge_t* pEdge,
    const gradients_t* pGradients
    )
    {
    const float     c = brush.pPaint->radialGradient_c;
    const uint *    colorRamp = brush.pPaint->colorRamp;
    const float     d = brush.pPaint->radialGradient_d;
    const float     e = brush.pPaint->radialGradient_e;
    float           g;
    const float     ha = (float)x - pEdge->x;
    int             i;
    float           u, v;

    pBuf += x;
    u = pEdge->u + (ha * pGradients->dudx);
    v = pEdge->v + (ha * pGradients->dvdx);
    if (brush.pPaint->spreadMode == VG_COLOR_RAMP_SPREAD_PAD)
        {
        /* Clamp */
        while (width-- > 0)
            {
            g = fast_sqrt((u * (u + (v * c))) + (v * v)) + (u * d) + (v * e);
            i = (g < 0.0) ? (0) : ((g > COLOR_RAMP_MAX_COORD) ? (COLOR_RAMP_MAX_COORD) : (ifloor(g)));
            *pBuf = blend32(colorRamp[i], *pBuf), pBuf++;
            u += pGradients->dudx;
            v += pGradients->dvdx;
            }
        }
    else if (brush.pPaint->spreadMode == VG_COLOR_RAMP_SPREAD_REPEAT)
        {
        /* Wrap */
        while (width-- > 0)
            {
            g = fast_sqrt((u * (u + (v * c))) + (v * v)) + (u * d) + (v * e);
            i = ifloor(g) & COLOR_RAMP_MASK_REPEAT;
            *pBuf = blend32(colorRamp[i], *pBuf), pBuf++;
            u += pGradients->dudx;
            v += pGradients->dvdx;
            }
        }
    else
        {
        /* Mirror (i.e. VG_COLOR_RAMP_SPREAD_REFLECT) */
        while (width-- > 0)
            {
            g = fast_sqrt((u * (u + (v * c))) + (v * v)) + (u * d) + (v * e);
            i = ifloor(g) & COLOR_RAMP_MASK_REFLECT;
            *pBuf = blend32(colorRamp[i], *pBuf), pBuf++;
            u += pGradients->dudx;
            v += pGradients->dvdx;
            }
        }
    }

/*******************************************************************************
 *
 * drawTriangle - draw a triangle on the specified surface
 *
 */
LOCAL void drawTriangle
    (
    const vertex_ptr* triangle,
    gradients_t* pGradients,
    brush_t brush,
    surface_t* pSurface,
    rectangle_t* pRect,
    VOIDFUNCPTR drawScanline
    )
    {
    vertex_t *  B;          /* bottom vertex */
    float       b1, b2;     /* barycentric coordinates */
    int         i;
    edge_t      leftEdge;   /* left edge */
    long        n;          /* number of vertices after clipping */
    void *      pBuf;       /* current scanline's address */
    vertex_t    polygon[7]; /* clipped triangle */
    edge_t      rightEdge;  /* right edge */
    vertex_t *  T;          /* top vertex */
    vertex_t *  V2;         /* intermediate vertex */
    int         width;      /* current scanline's width */
    int         x, y;       /* current pixel on the left edge */
    float       yMin, yMax; /* clipped triangle min and max Y */

    /* Clip */
    n = clipTriangle(triangle, pRect->x0, pRect->x1, pRect->y0, pRect->y1, polygon) - 1;
    if (n < 2)
        return;

    /* Update gradients */
    if (pGradients->dirty)
        calcGradients(triangle, pGradients);

    /* Find the top and bottom vertices and calculate the vertices' interpolated values */
    T = B = &polygon[0];
    yMin = yMax = polygon[0].y;
    for (i = 0; i <= n; i++)
        {
        if (polygon[i].y < yMin)
            {
            yMin = polygon[i].y;
            T = &polygon[i];
            }
        else if (polygon[i].y > yMax)
            {
            yMax = polygon[i].y;
            B = &polygon[i];
            }
        b1 = polygon[i].x - triangle[0]->x;
        b2 = polygon[i].y - triangle[0]->y;
        polygon[i].w = triangle[0]->w + (b1 * pGradients->dwdx) + (b2 * pGradients->dwdy);
        polygon[i].u = triangle[0]->u + (b1 * pGradients->dudx) + (b2 * pGradients->dudy);
        polygon[i].v = triangle[0]->v + (b1 * pGradients->dvdx) + (b2 * pGradients->dvdy);
        }

    /* Rasterize */
    leftEdge.V1 = rightEdge.V1 = T;
    leftEdge.height = rightEdge.height = 0;
    pBuf = pSurface->pBackBuf + (iceil(yMin) * pSurface->stride);
    while (1)
        {
        /* Initialize edges if needed */
        if (leftEdge.height <= 0)
            {
            do
                {
                if (leftEdge.V1 == B)
                    return;
                y = iceil(leftEdge.V1->y);
                V2 = (leftEdge.V1 < &polygon[n]) ? (leftEdge.V1 + 1) : (&polygon[0]);
                leftEdge.height = iceil(V2->y) - y;
                if (leftEdge.height > 0)
                    initEdge(&leftEdge, y, leftEdge.V1, V2, pGradients);
                leftEdge.V1 = V2;
                } while (leftEdge.height <= 0);
            }
        if (rightEdge.height <= 0)
            {
            do
                {
                if (rightEdge.V1 == B)
                    return;
                y = iceil(rightEdge.V1->y);
                V2 = (rightEdge.V1 > &polygon[0]) ? (rightEdge.V1 - 1) : (&polygon[n]);
                rightEdge.height = iceil(V2->y) - y;
                if (rightEdge.height > 0)
                    initEdge(&rightEdge, y, rightEdge.V1, V2, pGradients);
                rightEdge.V1 = V2;
                } while (rightEdge.height <= 0);
            }

        /* Rasterize current scanline */
        x = iceil(leftEdge.x);
        width = iceil(rightEdge.x) - x;
        if (width > 0)
            drawScanline(x, width, pBuf, brush, &leftEdge, pGradients);
        pBuf += pSurface->stride;

        /* Update edges */
        stepEdge(&leftEdge);
        stepEdge(&rightEdge);
        }
    }

/*******************************************************************************
 *
 * generateColorRamp - generate the color ramp for the specified paint
 *
 * RETURNS: Zero if successful
 *
 */
LOCAL int generateColorRamp
    (
    paint_t* pPaint
    )
    {
    float       c[4], ic[4];    /* color components and increments */
    VGuint *    colorRamp;      /* color ramp */
    int         i, j, k;
    int         numStops;       /* total number of stops */
    float *     stops;          /* array of valid stops */
    int         width;          /* width of the current span */
    VGuint      x;              /* surface compatible color */

    /* Get user-supplied parameters */
    colorRamp = pPaint->colorRamp;
    if (colorRamp == NULL)
        {
        /* Allocate memory (bit-depth is ignored) */
        colorRamp = pPaint->colorRamp = malloc((2 * COLOR_RAMP_SIZE) * sizeof(VGuint));
        if (colorRamp == NULL)
            return (1);
        }
    numStops = pPaint->numStops / 5;
    stops = pPaint->stops;

    /* Use the default stops if none are defined */
    if (numStops == 0)
        numStops = defaultPaint.numStops, stops = defaultPaint.stops;

    /* Make sure that all stops are in increasing order */
    numStops--;
    for (i = 0; i < numStops; i++)
        {
        if (stops[VG_STOP_OFFSET(i)] > stops[VG_STOP_OFFSET(i + 1)])
            {
            numStops = defaultPaint.numStops - 1, stops = defaultPaint.stops;
            break;
            }
        }

    /* Find the first valid stop and the number of stops to process */
    i = j = 0;
    while ((i <= numStops) && (stops[VG_STOP_OFFSET(0)] < 0.0))
        i++, stops += 5;
    while ((i <= numStops) && (stops[VG_STOP_OFFSET(j)] <= 1.0))
        i++, j++;

    /* Use default stops if no valid ones are defined */
    if (j == 0)
        numStops = defaultPaint.numStops, stops = defaultPaint.stops;
    else
        numStops = j;

    /* Set the initial color index */
    j = 0;

    /* Start with the color of the first stop */
    c[VG_R] = stops[VG_STOP_R(0)];
    c[VG_G] = stops[VG_STOP_G(0)];
    c[VG_B] = stops[VG_STOP_B(0)];
    c[VG_A] = stops[VG_STOP_A(0)];

    /* Pad when the first stop is not at offset 0 */
    if (stops[VG_STOP_OFFSET(0)] != 0.0)
        {
        /* Calculate the width of the span */
        width = iround((COLOR_RAMP_SIZE - 1) * stops[VG_STOP_OFFSET(0)]);

        /* Pad */
        x = vgConvertColorWRS(c, pPaint->format);
        for (i = 0; (i < width) && (j < COLOR_RAMP_SIZE); i++, j++)
            colorRamp[j] = colorRamp[(2 * COLOR_RAMP_SIZE) - 1 - j] = x;
        }

    /* Generate the spans */
    for (k = 1; k < numStops; k++)
        {
        /* Calculate the width of the span */
        width = iround((COLOR_RAMP_SIZE - 1) * stops[VG_STOP_OFFSET(k)]) - j;

        if (width > 0)
            {
            /* Calculate the increments */
            ic[VG_R] = (stops[VG_STOP_R(k)] - c[VG_R]) / (float)width;
            ic[VG_G] = (stops[VG_STOP_G(k)] - c[VG_G]) / (float)width;
            ic[VG_B] = (stops[VG_STOP_B(k)] - c[VG_B]) / (float)width;
            ic[VG_A] = (stops[VG_STOP_A(k)] - c[VG_A]) / (float)width;

            /* Interpolate */
            for (i = 0; (i < width) && (j < COLOR_RAMP_SIZE); i++, j++)
                {
                x = vgConvertColorWRS(c, pPaint->format);
                colorRamp[j] = colorRamp[(2 * COLOR_RAMP_SIZE) - 1 - j] = x;
                c[VG_R] += ic[VG_R];
                c[VG_G] += ic[VG_G];
                c[VG_B] += ic[VG_B];
                c[VG_A] += ic[VG_A];
                }
            }

        /* Reload the color of the current stop */
        c[VG_R] = stops[VG_STOP_R(k)];
        c[VG_G] = stops[VG_STOP_G(k)];
        c[VG_B] = stops[VG_STOP_B(k)];
        c[VG_A] = stops[VG_STOP_A(k)];
        }

    /* Pad with the last known color */
    if (j < COLOR_RAMP_SIZE)
        {
        x = vgConvertColorWRS(c, pPaint->format);
        while (j < COLOR_RAMP_SIZE)
            {
            colorRamp[j] = colorRamp[(2 * COLOR_RAMP_SIZE) - 1 - j] = x;
            j++;
            }
        }

    /* Indicate success */
    return (0);
    }

/*******************************************************************************
 *
 * getGlyph - find the glyph associated with the specified index
 *
 * RETURNS: A pointer to a glyph if successful, otherwise NULL
 *
 */
LOCAL glyph_t * getGlyph
    (
    const font_t* pFont,
    int index
    )
    {
    glyph_t *   pGlyph = pFont->pRootGlyph;

    while (pGlyph != NULL)
        {
        if (index < pGlyph->index)
            pGlyph = pGlyph->pLeftChild;
        else if (index == pGlyph->index)
            break;
        else
            pGlyph = pGlyph->pRightChild;
        }

    return (pGlyph);
    }

/*******************************************************************************
 *
 * initEdge - initialize the specified edge
 *
 */
LOCAL void initEdge
    (
    edge_t* pEdge,
    int y,
    const vertex_ptr V1,
    const vertex_ptr V2,
    const gradients_t* pGradients
    )
    {
    float   ha, va; /* horizontal and vertical adjustments */

    va = (float)y - V1->y;
    pEdge->ix = (V2->x - V1->x) / (V2->y - V1->y);
    ha = va * pEdge->ix;
    pEdge->x = V1->x + (ha);
    pEdge->w = V1->w + (ha * pGradients->dwdx) + (va * pGradients->dwdy);
    pEdge->u = V1->u + (ha * pGradients->dudx) + (va * pGradients->dudy);
    pEdge->v = V1->v + (ha * pGradients->dvdx) + (va * pGradients->dvdy);
    pEdge->iw = (pEdge->ix * pGradients->dwdx) + pGradients->dwdy;
    pEdge->iu = (pEdge->ix * pGradients->dudx) + pGradients->dudy;
    pEdge->iv = (pEdge->ix * pGradients->dvdx) + pGradients->dvdy;
    }

/*******************************************************************************
 *
 * initPatternLookUpTable - initialize a specific portion of the pattern
 *                          denormalization look-up table
 *
 */
LOCAL void initPatternLookUpTable
    (
    paint_t* pPaint,
    int offset,
    int dimension,
    int stride
    )
    {
    int     d;      /* denormalized coordinate */
    int     i;
    int *   lut;    /* pointer to look-up table */

    /* Adjust stride */
    if (stride == 0)
        stride = 1;
    else
        stride >>= pPaint->pattern.pImage->bpp >> 4;

    /* Figure out where to store things */
    lut = pPaint->pattern.denormLut + offset;

    switch (pPaint->pattern.tilingMode)
        {
        case VG_TILE_FILL:
        case VG_TILE_PAD:
            for (i = 0; i < PATTERN_NORM_SIZE; i++)
                {
                d = iround((i * dimension) * (1.0 / (PATTERN_NORM_SIZE - 1)));
                lut[i] = d * stride;
                lut[i + PATTERN_NORM_SIZE] = 0;
                }
            break;

        case VG_TILE_REPEAT:
            for (i = 0; i < PATTERN_NORM_SIZE; i++)
                {
                d = iround((i * dimension) * (1.0 / (PATTERN_NORM_SIZE - 1)));
                lut[i] = d * stride;
                lut[i + PATTERN_NORM_SIZE] = d * stride;
                }
            break;

        case VG_TILE_REFLECT:
            for (i = 0;  i < PATTERN_NORM_SIZE; i++)
                {
                d = iround((i * dimension) * (1.0 / (PATTERN_NORM_SIZE - 1)));
                lut[i] = d * stride;
                lut[i + PATTERN_NORM_SIZE] =  (dimension - d) * stride;
                }
            break;
        default:
            /* Do nothing */
            break;
        }
    }

/*******************************************************************************
 *
 * isAffine - determine if the specified matrix represents an affine
 *            transformation
 *
 * RETURNS: TRUE or FALSE
 *
 */
LOCAL int isAffine
    (
    const float* M
    )
    {
    return ((M[2] == 0.0) && (M[5] == 0.0) && (M[8] == 1.0));
    }

/*******************************************************************************
 *
 * isProjective - determine if the specified matrix represents a projective
 *                transformation
 *
 * RETURNS: TRUE or FALSE
 *
 */
LOCAL char isProjective
    (
    const float* M
    )
    {
    return (char) ((M[2] != 0.0) || (M[5] != 0.0) || (M[8] != 1.0));
    }

/*******************************************************************************
 *
 * stepEdge - increment the interpolated values on the specified edge
 *
 */
LOCAL void stepEdge
    (
    edge_t* pEdge
    )
    {
    pEdge->height--;
    pEdge->x += pEdge->ix;
    pEdge->w += pEdge->iw;
    pEdge->u += pEdge->iu;
    pEdge->v += pEdge->iv;
    }

/*******************************************************************************
 *
 * isClockwise - determine if the angle formed by the specified vertices is
 *               clockwise or counter-clockwise
 *
 */
LOCAL int isClockwise(const vertex_ptr A, const vertex_ptr B, const vertex_ptr C)
    {
    return (((B->x - A->x) * (C->y - A->y)) > ((C->x - A->x) * (B->y - A->y)));
    }

/*******************************************************************************
 *
 * vgClear
 *
 */
VG_API_CALL void VG_API_ENTRY vgClear
    (
    VGint x,
    VGint y,
    VGint width,
    VGint height
    ) VG_API_EXIT
    {
    brush_t         brush;          /* brush */
    VOIDFUNCPTR     drawScanline;   /* scanline rasterizer */
    VGImageFormat   format;         /* surface format */
    void *          pBuf;           /* clear address */
    rectangle_t *   pClipRect;      /* current clipping rectangle */
    surface_t *     pDrawSurface;   /* EGL draw surface */
    VGint           tmp[4];

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if ((width <= 0) || (height <= 0))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Get the draw surface and its format */
        pDrawSurface = pGc->vg.pDrawSurface;
        if (pDrawSurface == NULL)
            VG_FAIL_VOID(VG_NO_CONTEXT_ERROR);
        format = vgGetSurfaceFormatWRS(pDrawSurface);

        /* Flip Y */
        y = pDrawSurface->height - y - height;

        /* Setup the rasterizer */
        brush.color = vgConvertColorWRS(pGc->vg.clearColor, format);
        drawScanline = chooseRasterizer(format, format, DRAW_TYPE_FILL,
                                        DRAW_FLAG_IGNORE_ALPHA);

        /* Set pixels */
        pClipRect = (pGc->vg.enableScissoring) ? (pGc->vg.pScissorRects) :
                                                 (&pDrawSurface->rect);
        while (pClipRect != NULL)
            {
            tmp[0] = x, tmp[1] = y, tmp[2] = width, tmp[3] = height;

            do
                {
                /* Clip */
                if ((x >= (VGint)pClipRect->x1) || (y >= (VGint)pClipRect->y1))
                    break;
                if (x < (VGint)pClipRect->x0)
                    width -= ((VGint)pClipRect->x0 - x), x = (VGint)pClipRect->x0;
                if (y < (VGint)pClipRect->y0)
                    height -= ((VGint)pClipRect->y0 - y), y = (VGint)pClipRect->y0;
                if ((width <= 0) || (height <= 0))
                    break;
                if ((x + width) > (VGint)pClipRect->x1)
                    width = (VGint)pClipRect->x1 - x;
                if ((y + height) > (VGint)pClipRect->y1)
                    height = (VGint)pClipRect->y1 - y;

                /* Fill */
                pBuf = pDrawSurface->pBackBuf + (y * pDrawSurface->stride);
                while (height-- > 0)
                    {
                    drawScanline(x, width, pBuf, brush);
                    pBuf += pDrawSurface->stride;
                    }
                } while (0);

            x = tmp[0], y = tmp[1], width = tmp[2], height = tmp[3];

            pClipRect = pClipRect->pNext;
            }
        }
    }

/*******************************************************************************
 *
 * vgDrawGlyph
 *
 */
VG_API_CALL void VG_API_ENTRY vgDrawGlyph
    (
    VGFont font,
    VGuint index,
    VGbitfield paintModes,
    VGboolean allowAutoHinting
    ) VG_API_EXIT
    {
    float       advance[3]; /* advance */
    glyph_t *   pGlyph;     /* glyph */
    float       Tu[9];      /* user-to-surface transformation matrix */
    float       w;          /* third coordinate of a 2D point (for projection) */
    font_t *    pFont = (font_t *)font;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidFont(pFont, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((paintModes & ~VG_PAINT_MODE_MASK) != 0)
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /*
         * Get the glyph corresponding to the specified index
         * (generally, but not necessarely, a UTF-32 code)
         */
        pGlyph = getGlyph(pFont, index);
        if (pGlyph == NULL)
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Force context to text mode */
        pGc->vg.pCurrentMatrix = Tu;
        pGc->vg.textMode = TRUE;

        /*
         * Setup the transformation matrix (the user-defined transformation is
         * combined with the glyph origin and then translated to the current
         * text position)
         */
        memcpy(Tu, pGc->vg.matrix[MATRIX_INDEX(VG_MATRIX_GLYPH_USER_TO_SURFACE)], 9 * sizeof(float));
        Tu[6] += pGc->vg.glyphOrigin[0] - (Tu[0] * pGlyph->origin[0]) - (Tu[3] * pGlyph->origin[1]);
        Tu[7] += pGc->vg.glyphOrigin[1] - (Tu[1] * pGlyph->origin[0]) - (Tu[4] * pGlyph->origin[1]);
        Tu[8] = 1.0, Tu[2] = Tu[5] = 0.0;

        /* Draw glyph */
        if (paintModes != 0)
            {
            if (pGlyph->type == OBJECT_TYPE_PATH)
                vgDrawPath(pGlyph->handle, paintModes);
            else if (pGlyph->type == OBJECT_TYPE_IMAGE)
                vgDrawImage(pGlyph->handle);
            }

        /* Calculate the origin for the next glyph */
        advance[0] = pGlyph->escapement[0];
        advance[1] = pGlyph->escapement[1];
        advance[2] = 1.0;
        vgTransformWRS(advance, Tu, pGc->vg.glyphOrigin);
        w = pGc->vg.glyphOrigin[2];
        if (w != 0.0)
            {
            pGc->vg.glyphOrigin[0] /= w;
            pGc->vg.glyphOrigin[1] /= w;
            pGc->vg.glyphOrigin[2] = 1.0;
            }

        /* Restore context */
        pGc->vg.pCurrentMatrix = pGc->vg.matrix[MATRIX_INDEX(pGc->vg.matrixMode)];
        pGc->vg.textMode = FALSE;
        }
    }

/*******************************************************************************
 *
 * vgDrawGlyphs
 *
 */
VG_API_CALL void VG_API_ENTRY vgDrawGlyphs
    (
    VGFont font,
    VGint glyphCount,
    const VGuint* glyphIndices,
    const VGfloat* adjustmentsX,
    const VGfloat* adjustmentsY,
    VGbitfield paintModes,
    VGboolean allowAutoHinting
    ) VG_API_EXIT
    {
    float       advance[3];   /* advance */
    int         i;
    glyph_t *   pGlyph;       /* current glyph */
    float       Tu[9];        /* user-to-surface transformation matrix */
    float       w;            /* third coordinate of a 2D point (for projection) */
    font_t *    pFont = (font_t *)font;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidFont(pFont, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (glyphCount <= 0)
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(glyphIndices, sizeof(VGuint)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((adjustmentsX != NULL) && (isInvalidPtr(adjustmentsX, sizeof(VGfloat))))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((adjustmentsY != NULL) && (isInvalidPtr(adjustmentsY, sizeof(VGfloat))))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((paintModes & ~VG_PAINT_MODE_MASK) != 0)
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Validate the list of glyph indices */
        for (i = 0; i < glyphCount; i++)
            {
            if (getGlyph(pFont, glyphIndices[i]) == NULL)
                VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
            }

        /* Force context to text mode */
        pGc->vg.pCurrentMatrix = Tu;
        pGc->vg.textMode = TRUE;

        /* Draw glyphs (see vgDrawGlyph() for details) */
        for (i = 0; i < glyphCount; i++)
            {
            pGlyph = getGlyph(pFont, glyphIndices[i]);
            if (pGlyph == NULL)
                continue;

            memcpy(Tu, pGc->vg.matrix[MATRIX_INDEX(VG_MATRIX_GLYPH_USER_TO_SURFACE)], 9 * sizeof(float));
            Tu[6] += pGc->vg.glyphOrigin[0] - (Tu[0] * pGlyph->origin[0]) - (Tu[3] * pGlyph->origin[1]);
            Tu[7] += pGc->vg.glyphOrigin[1] - (Tu[1] * pGlyph->origin[0]) - (Tu[4] * pGlyph->origin[1]);
            Tu[8] = 1.0, Tu[2] = Tu[5] = 0.0;

            if (paintModes != 0)
                {
                if (pGlyph->type == OBJECT_TYPE_PATH)
                    vgDrawPath(pGlyph->handle, paintModes);
                else if (pGlyph->type == OBJECT_TYPE_IMAGE)
                    vgDrawImage(pGlyph->handle);
                }

            advance[0] = pGlyph->escapement[0] + ((adjustmentsX != NULL) ? (adjustmentsX[i]) : (0.0f));
            advance[1] = pGlyph->escapement[1] + ((adjustmentsY != NULL) ? (adjustmentsY[i]) : (0.0f));
            advance[2] = 1.0;
            vgTransformWRS(advance, Tu, pGc->vg.glyphOrigin);
            w = pGc->vg.glyphOrigin[2];
            if (w != 0.0f)
                {
                pGc->vg.glyphOrigin[0] /= w;
                pGc->vg.glyphOrigin[1] /= w;
                pGc->vg.glyphOrigin[2] = 1.0f;
                }
            }

        /* Restore context */
        pGc->vg.pCurrentMatrix = pGc->vg.matrix[MATRIX_INDEX(pGc->vg.matrixMode)];
        pGc->vg.textMode = FALSE;
        }
    }

/*******************************************************************************
 *
 * vgDrawImage
 *
 */
VG_API_CALL void VG_API_ENTRY vgDrawImage
    (
    VGImage image
    ) VG_API_EXIT
        {
    brush_t         brush;              /* brush */
    VOIDFUNCPTR     drawScanlineFn;     /* scanline rasterizer */
    VGImageFormat   format;             /* surface format */
    gradients_t     gradients;          /* gradients */
    int             i;
    rectangle_t *   pClipRect;          /* current clipping rectangle */
    surface_t *     pDrawSurface;       /* EGL draw surface */
    vertex_ptr      triangles[2][3];    /* triangles */
    float *         Tu;                 /* user-to-surface transformation matrix */
    vertex_t        vertices[4];        /* image corners */
    image_t *       pImage = (image_t *)image;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Check if the image is a rendering target */
        if (pImage->inUse)
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Get the draw surface and its format */
        pDrawSurface = pGc->vg.pDrawSurface;
        if (pDrawSurface == NULL)
            VG_FAIL_VOID(VG_NO_CONTEXT_ERROR);
        format = vgGetSurfaceFormatWRS(pDrawSurface);

        /* Get the user-to-surface transformation matrix */
        Tu = (pGc->vg.textMode) ? (pGc->vg.pCurrentMatrix) :
                                  (pGc->vg.matrix[MATRIX_INDEX(VG_MATRIX_IMAGE_USER_TO_SURFACE)]);

        /* Initialize the vertices (clockwise order starting from the upper-left corner) */
        vertices[0].x = vertices[0].y = vertices[1].y = vertices[3].x = 0.0f;
        vertices[0].w = vertices[1].w = vertices[2].w = vertices[3].w = 1.0f;
        vertices[0].u = vertices[3].u = (float)pImage->x0;
        vertices[0].v = vertices[1].v = (float)(pImage->y0 + pImage->height);
        vertices[1].x = vertices[2].x = (float)pImage->width;
        vertices[1].u = vertices[2].u = (float)(pImage->x0 + pImage->width);
        vertices[2].y = vertices[3].y = (float)pImage->height;
        vertices[2].v = vertices[3].v = (float)pImage->y0;
        for (i = 0; i < 4; i++)
            vgTransformWRS(&vertices[i].x, Tu, &vertices[i].x);

        /*
         * Adjust the coordinates. Here's what the specifications have to say
         * about that:
         *
         * " When a projective transformation is used (i.e., the bottom row of
         *   the image-user-to-surface transformation contains values
         *   [ w0 w1 w2 ] different from [ 0 0 1 ]), each corner point (x, y) of
         *   the image must result in a positive value of d = (x*w0 + y*w1 + w2),
         *   or else nothing is drawn. This rule prevents degeneracies due to
         *   transformed image points passing through infinity, which occurs
         *   when d passes through 0. By requiring d to be positive at the
         *   corners, it is guaranteed to be positive at all interior points as
         *   well. "
         *
         * Note that the original vertices' order is clockwise and it gets
         * reversed when Y is flipped
         */
        if (isProjective(Tu))
            {
            for (i = 0; i < 4; i++)
                {
                if (vertices[i].w < 0.0f)
                    return;
                vertices[i].x /= vertices[i].w;
                vertices[i].y = (float)pDrawSurface->height - (vertices[i].y / vertices[i].w);
                vertices[i].w = 1.0f / vertices[i].w;
                vertices[i].u *= vertices[i].w;
                vertices[i].v *= vertices[i].w;
                }
            }
        else
            {
            for (i = 0; i < 4; i++)
                vertices[i].y = (float)pDrawSurface->height - vertices[i].y;
            }

        /* Initialize the triangles (2-0-1 and 3-0-2)
         *
         *   3-----2
         *   |   / |
         *   |  /  |
         *   | /   |
         *   0-----1
         */
        triangles[0][0] = triangles[1][2] = &vertices[2];
        triangles[0][1] = triangles[1][1] = &vertices[0];
        triangles[0][2] = &vertices[1];
        triangles[1][0] = &vertices[3];
        if (isClockwise(&vertices[2], &vertices[0], &vertices[1]))
            {
            EXCHANGE(triangles[0][1], triangles[0][2]);
            EXCHANGE(triangles[1][1], triangles[1][2]);
            }

        /* Setup the rasterizer */
        brush.pBitmap = pImage->pSurface;
        drawScanlineFn = chooseRasterizer(format, pImage->format, isAffine(Tu) ? (DRAW_TYPE_TEXMAP) :
                                                                                 (DRAW_TYPE_TEXMAP_3D), 0);

        /* Draw triangles */
        pClipRect = (pGc->vg.enableScissoring) ? (pGc->vg.pScissorRects) : (&pDrawSurface->rect);
        while (pClipRect != NULL)
            {
            gradients.dirty = TRUE;
            drawTriangle(triangles[0], &gradients, brush, pDrawSurface, pClipRect, drawScanlineFn);
            gradients.dirty = isProjective(Tu);
            drawTriangle(triangles[1], &gradients, brush, pDrawSurface, pClipRect, drawScanlineFn);

            pClipRect = pClipRect->pNext;
            }
        }
        }

/*******************************************************************************
 *
 * vgDrawPath
 *
 */
VG_API_CALL void VG_API_ENTRY vgDrawPath
    (
    VGPath path,
    VGbitfield paintModes
    ) VG_API_EXIT
    {
    brush_t         brush;                  /* brush */
    int             checkOrientation;       /* TRUE if the triangle's orientation needs to be checked */
    VGuint          color;                  /* surface compatible color */
    VOIDFUNCPTR     drawScanlineFn = NULL;  /* scanline rasterizer */
    float           dx = 0.0, dy = 0.0;     /* gradient function parameters */
    int             flags;                  /* rasterizer flags */
    VGImageFormat   format;                 /* surface format */
    gradients_t     gradients;              /* triangle gradients */
    float           h = 0.0, w = 0.0;       /* height and width of the paint pattern */
    int             i, j, k;
    VGPaintType     paintType;              /* paint type */
    rectangle_t*    pClipRect;              /* current clipping rectangle */
    surface_t*      pDrawSurface;           /* EGL draw surface */
    paint_t*        pPaint;                 /* fill/stroke paint */
    float           Tp[9];                  /* paint-to-user transformation matrix */
    vertex_ptr      triangle[3];            /* inner loop triangle (for rasterization) */
    float*          Tu;                     /* user-to-surface transformation matrix */
    vertex_t        vertices[3];            /* vertices (for rasterization) */
    float           x = 0.0, y = 0.0;       /* point in the paint coordinate system */
    float           x0 = 0.0, y0 = 0.0;     /* gradient function parameters */
    path_t *        pPath = (path_t *)path;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPath(pPath, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((paintModes & ~VG_PAINT_MODE_MASK) != 0)
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Get the draw surface and its format */
        pDrawSurface = pPath->pGc->vg.pDrawSurface;
        if (pDrawSurface == NULL)
            VG_FAIL_VOID(VG_NO_CONTEXT_ERROR);
        format = vgGetSurfaceFormatWRS((EGLSurface)pDrawSurface);

        /* Avoid unnecessary work */
        if (pPath->numSegs == 0)
            return;

        /* Update the path geometry */
        if (vgPathLength((VGPath)pPath, 0, 1) == -1.0)
            return;

        /* Get the user-to-surface transformation matrix */
        Tu = (pGc->vg.textMode) ? (pGc->vg.pCurrentMatrix) :
                                  (pGc->vg.matrix[MATRIX_INDEX(VG_MATRIX_PATH_USER_TO_SURFACE)]);

        /* Setup the inner loop triangle */
        for (i = 0; i < 3; i++)
            triangle[i] = &vertices[i];

        /* Fill */
        if (paintModes & VG_FILL_PATH)
            {
            /* Get fill paint... */
            pPaint = pPath->pGc->vg.pFillPaint;
            /* ...or use default one */
            if (pPaint == NULL)
                pPaint = &defaultPaint;

            /* Check if the paint type is consistent */
            paintType = pPaint->type;
            if ((paintType == VG_PAINT_TYPE_PATTERN) && (pPaint->pattern.pImage == NULL))
                {
                /*
                 * Per specifications:
                 *
                 * " If the current paint object has its VG_PAINT_TYPE
                 *   parameter set to VG_PAINT_TYPE_PATTERN, but no pattern
                 *   image is set, the paint object behaves as if VG_PAINT_TYPE
                 *   were set to VG_PAINT_TYPE_COLOR. "
                 */
                paintType = VG_PAINT_TYPE_COLOR;
                }

            /* Setup the rasterizer */
            color = 0x00000000;
            flags = 0;
            switch (paintType)
                {
                case VG_PAINT_TYPE_COLOR:
                    if (pPaint->dirty)
                        {
                        switch (format)
                            {
                            case VG_sXRGB_8888:
                            case VG_sARGB_8888_PRE:
                                pPaint->format = (pPaint->color[VG_A] == 1.0) ? (VG_sXRGB_8888) : (VG_sARGB_8888_PRE);
                                color = vgConvertColorWRS(pPaint->color, pPaint->format);
                                break;
                            default:
                                break;
                            }
                        pPaint->color2 = color;
                        pPaint->dirty = (char)(pPaint == &defaultPaint);
                        }
                    /**/
                    brush.color = pPaint->color2;
                    drawScanlineFn = chooseRasterizer(format, pPaint->format, DRAW_TYPE_FILL, flags);
                    break;

                case VG_PAINT_TYPE_LINEAR_GRADIENT:
                    if (pPaint->dirty)
                        {
                        switch (format)
                            {
                            case VG_sXRGB_8888:
                            case VG_sARGB_8888_PRE:
                                pPaint->format = VG_sARGB_8888_PRE;
                                if (generateColorRamp(pPaint))
                                    VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);
                                pPaint->dirty = FALSE;
                                break;
                            default:
                                break;
                            }
                        }
                    /**/
                    (void) invert3x3(pPath->pGc->vg.matrix[MATRIX_INDEX(VG_MATRIX_FILL_PAINT_TO_USER)], Tp);
                    dx = pPaint->linearGradient_dx, dy = pPaint->linearGradient_dy;
                    x0 = pPaint->linearGradient_x0 - Tp[6];
                                        y0 = pPaint->linearGradient_y0 - Tp[7];
                    /**/
                    brush.pPaint = pPaint;
                    drawScanlineFn = chooseRasterizer(format, pPaint->format, DRAW_TYPE_LINEAR_GRAD, flags);
                    break;

                case VG_PAINT_TYPE_RADIAL_GRADIENT:
                    if (pPaint->dirty)
                        {
                        switch (format)
                            {
                            case VG_sXRGB_8888:
                            case VG_sARGB_8888_PRE:
                                pPaint->format = VG_sARGB_8888_PRE;
                                if (generateColorRamp(pPaint))
                                    VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);
                                pPaint->dirty = FALSE;
                                break;
                            default:
                                break;
                            }
                        }
                    /**/
                    (void) invert3x3(pPath->pGc->vg.matrix[MATRIX_INDEX(VG_MATRIX_FILL_PAINT_TO_USER)], Tp);
                    /**/
                    brush.pPaint = pPaint;
                    drawScanlineFn = chooseRasterizer(format, pPaint->format, DRAW_TYPE_RADIAL_GRAD, flags);
                    break;

                case VG_PAINT_TYPE_PATTERN:
                    if (pPaint->dirty)
                        {
                        initPatternLookUpTable(pPaint, U_OFFSET, pPaint->pattern.pImage->width - 1, 0);
                        initPatternLookUpTable(pPaint, V_OFFSET, pPaint->pattern.pImage->height - 1,
                                               pPaint->pattern.pImage->pSurface->stride);
                        pPaint->dirty = FALSE;
                        }
                    if (pPaint->pattern.tilingMode == VG_TILE_FILL)
                        {
                        switch (format)
                            {
                            case VG_sXRGB_8888:
                            case VG_sARGB_8888_PRE:
                                if (pGc->vg.tileFillColor[VG_A] != 1.0)
                                    flags |= DRAW_FLAG_FORCE_ALPHA;
                                color = vgConvertColorWRS(pGc->vg.tileFillColor, (flags == 0) ?
                                                          (VG_sXRGB_8888) : (VG_sARGB_8888_PRE));
                                break;
                            default:
                                break;
                            }
                        pPaint->pattern.fillColor = color;
                        }
                    /**/
                    invert3x3(pPath->pGc->vg.matrix[MATRIX_INDEX(VG_MATRIX_FILL_PAINT_TO_USER)], Tp);
                    h = (float)(pPaint->pattern.pImage->height - 1);
                    w = (float)(pPaint->pattern.pImage->width - 1);
                    /**/
                    brush.pPattern = &pPaint->pattern;
                    drawScanlineFn = chooseRasterizer(format, pPaint->pattern.pImage->format, DRAW_TYPE_PATTERN, flags);
                    break;

                default:
                    /* Do nothing */
                    break;
                }

            /* Process all triangles */
            checkOrientation = TRUE;
            gradients.dirty = TRUE;
            for (i = pPath->numFillTriangles, j = 0; i > 0; i--)
                {
                /* Transform, flip Y and calculate the mapping coordinates */
                for (k = 0; k < 3; j++, k++)
                    {
                    vgTransformWRS(&pPath->triangles[j]->x, Tu, &vertices[k].x);
                    vertices[k].y = (float)pDrawSurface->height - vertices[k].y;
                    switch (paintType)
                        {
                        case VG_PAINT_TYPE_COLOR:
                            break;

                        case VG_PAINT_TYPE_LINEAR_GRADIENT:
                            /* Evaluate the gradient function expressed as
                             *
                             *             (dx * (x - x0)) + (dy * (y - y0))
                             *   G(x, y) = ---------------------------------
                             *                         dx� + dy�
                             *
                             * where dx = (x1 - x0) and dy = (y1 - y0). If the points (x0, y0) and (x1, y1) are coincident
                             * (and thus dx� + dy� = 0), the function is given the value 1 everywhere
                             */
                            if (pPaint->linearGradient_q == 0.0)
                                vertices[k].u = 1.0;
                            else
                                {
                                /* Transform the coordinates... */
                                x = (pPath->triangles[j]->x * Tp[0]) + (pPath->triangles[j]->y * Tp[3]);
                                y = (pPath->triangles[j]->x * Tp[1]) + (pPath->triangles[j]->y * Tp[4]);
                                /* ...and calculate the value of the gradient function */
                                vertices[k].u = (dx * (x - x0) + dy * (y - y0)) * pPaint->linearGradient_q;
                                }
                            break;

                        case VG_PAINT_TYPE_RADIAL_GRADIENT:
                            /*
                             * Because the gradient function is quadratic in nature, it's not possible to do linear
                             * interpolation and avoid the per-pixel calculations. Interestingly enough, the specifications
                             * recommend to rewrite G(x, y) as Gy(x) along a given scanline. Unfortunately, this can only work
                             * if the gradient Y coordinate remains constant, i.e. the "fill paint to user" transformation is
                             * the identity
                             */
                            if (pPaint->radialGradient_r <= 0.0)
                                {
                                vertices[k].u = 1.0;
                                vertices[k].v = 0.0;
                                }
                            else
                                {
                                /* Transform the coordinates... */
                                x = (pPath->triangles[j]->x * Tp[0]) + (pPath->triangles[j]->y * Tp[3]) + Tp[6];
                                y = (pPath->triangles[j]->x * Tp[1]) + (pPath->triangles[j]->y * Tp[4]) + Tp[7];
                                /* ...and adjust them (see initRadialGradient() in vgset.c for details) */
                                vertices[k].u = (x - pPaint->radialGradient_fx) * pPaint->radialGradient_a;
                                vertices[k].v = (y - pPaint->radialGradient_fy) * pPaint->radialGradient_b;
                                }
                            break;

                        case VG_PAINT_TYPE_PATTERN:
                            /* Transform the coordinates... */
                            x = (pPath->triangles[j]->x * Tp[0]) + (pPath->triangles[j]->y * Tp[3]) + Tp[6];
                            y = (pPath->triangles[j]->x * Tp[1]) + (pPath->triangles[j]->y * Tp[4]) + Tp[7];
                            /* ...and normalize them */
                            vertices[k].u = ((x * (PATTERN_NORM_SIZE - 1)) / w);
                            vertices[k].v = (((h + 1 - y) * (PATTERN_NORM_SIZE - 1)) / h);
                            break;

                        default:
                            /* Do nothing */
                            break;
                        }
                    }

                /* Make sure that the triangles have the correct orientation as it may have been changed by the transformation
                 * (this needs to be done only once)
                 */
                if (checkOrientation)
                    {
                    if (isClockwise(triangle[0], triangle[1], triangle[2]))
                        EXCHANGE(triangle[1], triangle[2]);
                    checkOrientation = FALSE;
                    }

                /* Draw */
                pClipRect = (pPath->pGc->vg.enableScissoring) ? (pPath->pGc->vg.pScissorRects) : (&pDrawSurface->rect);
                while (pClipRect != NULL)
                    {
                    drawTriangle(triangle, &gradients, brush, pDrawSurface, pClipRect, drawScanlineFn);
                    pClipRect = pClipRect->pNext;
                    }
                }
            }

        /* Stroke */
        if (paintModes & VG_STROKE_PATH)
            {
            /* :TODO: */
            }
        }
    }
