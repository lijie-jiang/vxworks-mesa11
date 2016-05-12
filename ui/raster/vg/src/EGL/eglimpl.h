/* eglimpl.h - Wind River EGL Implementation Specific Header */

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

#ifndef __eglimpl_h
#define __eglimpl_h

/* includes */

#include <EGL/egl.h>
#include <EGL/eglwrs.h>
#include <gfxVgRaster.h>

/* defines */

/* Macros */
#define API_INDEX(_api)  ((_api) - 0x30A0)

/* - Error handling */
#define EGL_FAIL(_err)  do                                  \
                        {                                   \
                            pGc->egl.error = _err;          \
                            if ((_err) != EGL_BAD_DISPLAY)  \
                                goto zz;                    \
                        } while (0)

/* - Sizes */
#define DEV_NAME_LEN    29

/* typedefs */

/* Types */
typedef struct context      context_t;
typedef struct display      display_t;
typedef struct rectangle    rectangle_t;
typedef struct surface      surface_t;
typedef __eglMustCastToProperFunctionPointerType    EGLFuncPtr;

/* - Context */
struct context
    {
    struct context *    pNext;           /* linked list node */
    display_t *         pDisplay;        /* display */
    unsigned int        refCount;        /* reference counter */
    char                deletePending;   /* TRUE if deletion is pending */
    int                 api;             /* client API */
    FB_CONFIG*          pConfig;         /* configuration */
    surface_t*          pDrawSurface;    /* draw surface */
    surface_t*          pReadSurface;    /* read surface */
    unsigned long       threadId;        /* bound thread (0 if none) */
    };

/* - Display */
struct display
    {
    int                 fd;     /* frame buffer device file descriptor */
    FB_IOCTL_ARG        arg;    /* ioctl() request argument */
    SEM_ID              lock;   /* lock */
    char                initialized;             /* TRUE if initialized */
    char                name[DEV_NAME_LEN];      /* device name */
    FB_CONFIG           configs[FB_MAX_CONFIGS]; /* frame buffer configurations */
    surface_t*          pSurfaces;               /* linked list of surfaces */
    context_t*          pContexts;               /* linked list of contexts */
    };

/* - Rectangle */
struct rectangle
    {
    struct rectangle *  pNext;     /* linked list node */
    float               x0, y0;    /* top-left corner */
    float               x1, y1;    /* lower-right corner */
    };

/* - Surface */
#define    SURFACE_TYPE_PIXMAP  1
#define    SURFACE_TYPE_PBUFFER 2
#define    SURFACE_TYPE_WINDOW  3

struct surface
    {
    struct surface *    pNext;           /* linked list node */
    display_t *         pDisplay;        /* display */
    unsigned int        refCount;        /* reference counter */
    char                deletePending;   /* TRUE if deletion is pending */
    char                type;            /* type */
    FB_CONFIG *         pConfig;         /* configuration */
    unsigned int        buffers;         /* number of buffers */
    void *              pFrontBuf;       /* front buffer */
    void *              pBackBuf;        /* back buffer */
    void *              pThirdBuf;       /* third buffer */
    int                 width, stride;   /* width in pixels, stride in bytes */
    int                 height;          /* height in lines */
    rectangle_t         rect;            /* rectangle */
    unsigned long       threadId;        /* bound thread (0 if none) */
    int                 swapInterval;    /* swap interval (windows only) */
    };

#endif /* #ifndef __eglimpl_h */
