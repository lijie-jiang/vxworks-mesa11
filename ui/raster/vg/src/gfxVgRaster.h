/* gfxVgRaster.h - Wind River VG Raster Header */

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
25jun13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
*/
#ifndef __gfxVgRaster_h
#define __gfxVgRaster_h

/* includes */

#include <fbdev.h>
#include <semLib.h>
#include <setjmp.h>
#include <stdio.h>
#include <gfxVgConfig.h>

/* defines */

#define VENDOR_STRING "Wind River Systems"

/* extern declarations */

extern int fdprintf (int, const char *, ...);

/* macros */

#undef  assert
#define assert(_expr)   do                              \
                            {                           \
                            if (!(_expr))               \
                            die("Assertion failed");    \
                            }                           \
                        while (0)

#define C_ASSERT(_expr) extern char __C_ASSERT__[(_expr) ? (1) : (-1)]

#define die(_msg)   do                                                                                 \
                        {                                                                              \
                        fdprintf(2, "**** %s in file \""__FILE__"\", line %d ****\n", _msg, __LINE__); \
                        abort();                                                                       \
                        }                                                                              \
                    while (0)

#define sizeofx(_element, _type)    sizeof(((_type*)0)->_element)

/* Linked list functionality */
#define LL_ADD_HEAD(_pHead, _pObj)  do                              \
                                        {                           \
                                        (_pObj)->pNext = (_pHead);  \
                                        (_pHead) = (_pObj);         \
                                        }                           \
                                    while (0)
#define LL_ADD_TAIL(_pHead, _pObj)  do                                          \
                                        {                                       \
                                        if ((_pHead) == NULL)                   \
                                            (_pHead) = (_pObj);                 \
                                        else                                    \
                                            {                                   \
                                            __typeof__(_pObj) pNode = (_pHead); \
                                            while (pNode->pNext != NULL)        \
                                                pNode = pNode->pNext;           \
                                            pNode->pNext = (_pObj);             \
                                            }                                   \
                                        (_pObj)->pNext = NULL;                  \
                                        }                                       \
                                    while (0)
#define LL_FOREACH(_pHead, _action) do                                      \
                                        {                                   \
                                        __typeof__(_pHead) pObj = (_pHead); \
                                        while (pObj != NULL)                \
                                            {                               \
                                            void* pNext = pObj->pNext;      \
                                            _action;                        \
                                            pObj = pNext;                   \
                                            }                               \
                                        }                                   \
                                    while (0)
#define LL_REMOVE(_pHead, _pObj)    do                                      \
                                        {                                   \
                                        __typeof__(_pObj) pNode = (_pHead); \
                                        if (pNode == (_pObj))               \
                                            (_pHead) = (_pObj)->pNext;      \
                                        else                                \
                                            {                               \
                                            while (pNode->pNext != (_pObj)) \
                                            pNode = pNode->pNext;           \
                                            pNode->pNext = (_pObj)->pNext;  \
                                            }                               \
                                        }                                   \
                                    while (0)

/* enums */

enum
    {
    API_NONE        = -1,    /* DO NOT CHANGE! */
    API_OPENGL_ES   = 0,
    API_OPENVG      = 1,
    API_OPENGL      = 2
    };

/* typedefs */

typedef struct
    {
    int                         valid;               /* 0 if invalid */
    jmp_buf                     env;                 /* recovery environment */
    unsigned long               threadId;            /* id of current thread */
    int                         error;               /* copy of errno */
    struct
        {
        int                     api;                 /* bound client API */
        int                     error;               /* last error */
        int *                   criteria;            /* criteria used to select config */
        struct context *        pContext[3];         /* contexts */
        VOIDFUNCPTR             makeCurrent[3];      /* eglMakeCurrent() callbacks */
        VOIDFUNCPTR             releaseThread[3];    /* eglReleaseThread() callbacks */
        FUNCPTR                 wait[3];             /* eglWaitClient() callbacks */
        } egl;

    #ifdef    SUPPORT_OPENVG
    struct
    {
        int                     error;               /* last error */
        struct surface*         pDrawSurface;        /* EGL draw surface */
        /* General */
        float                   clearColor[4];       /* clear color */
        int                     pixelLayout;         /* pixel layout (ignored) */
        float                   tileFillColor[4];    /* tile fill color */
        /* Scissoring */
        int                     enableScissoring;    /* VG_TRUE if scissoring is enabled */
        int                     numScissorRects;     /* number of scissor rectangles * 4 */
        struct rectangle*       pScissorRects;       /* linked list of rectangles */
        void*                   scissoringData;      /* raw scissoring data */
        /* Transformations */
        int                     matrixMode;          /* matrix mode */
        float                   matrix[5][9];        /* matrixes */
        float*                  pCurrentMatrix;      /* current matrix */
        /* Paths */
        int                     fillRule;            /* fill rule */
        float                   flatness;            /* flatness tolerance */
        struct path*            pPaths;              /* linked list of paths */
        struct GLUtesselator *  pTess;               /* GLU tesselator */
        /* Images */
        int                     imageMode;           /* image mode (ignored) */
        int                     imageQuality;        /* image quality (ignored) */
        struct image*           pImages;             /* linked list of images */
        /* Fonts */
        int                     textMode;            /* set to TRUE when drawing glyphs (used to override behavior) */
        float                   glyphOrigin[3];      /* current glyph's origin (x, y, 1) */
        struct font*            pFonts;              /* linked list of fonts */
        /* Paints */
        struct paint*           pFillPaint;          /* fill paint */
        struct paint*           pStrokePaint;        /* stroke paint */
        struct paint*           pPaints;             /* linked list of paints */
        /* Filters */
        int                     filterChannelMask;   /* channel mask */
        int                     filterLinear;        /* VG_TRUE if the internal filter format should be linear */
        int                     filterPremultiplied; /* VG_TRUE if the color channels should be premultiplied */
    } vg;
    #endif    /* SUPPORT_OPENVG */
    } gc_t;

typedef unsigned char   uchar;
typedef unsigned int    uint;
typedef unsigned short  ushort;

/* forward declerations */

void deleteGc (void);
gc_t * initGc (void);

#endif    /* __gfxVgRaster_h */
