/* egl.c - Wind River EGL Implementation */

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
08oct15,yat  Add GFX_FIXED_FBMODE_OFFSET (US67554)
15sep15,jxy  Update for fixing coverity issue
12jun15,yat  Add offset for xlnxlcvc (US58560)
22dec14,yat  Fix build warning for LP64 (US50456)
25jun13,mgc  Modified for VxWorks 7 release
03mar13,m_c  Updated for SMP
04apr09,m_c  Written
*/

/*
DESCRIPTION
These routines provide an EGL implementation for the Wind River software VG
API.
*/

/* includes */

#include    <fcntl.h>
#include    <ioLib.h>
#if defined(_WRS_KERNEL)
#include    <spinLockLib.h>
#else
#include    <pthread.h>
#endif
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <unistd.h>
#include    "EGL/eglimpl.h"
#include    "VG/vgimpl.h"

/* defines */

/* Limits */
#define MAX_DISPLAYS            2

/* Miscellaneous */
#define NUM_ELEMENTS(_array)    (sizeof(_array) / sizeof((_array)[0]))

/* typedefs */

typedef struct
    {
    const char*                 name;
    EGLFuncPtr                  address;
    } ext_proc_addr_t;

/* locals */

/* Forward declerations */
LOCAL void         bindContext (context_t*, unsigned long);
LOCAL void         bindSurface (surface_t*, unsigned long);
LOCAL STATUS       controlDisplay (display_t*, int);
LOCAL void         deleteContext (context_t*);
LOCAL void         deleteSurface (surface_t*);
LOCAL EGLint       eglGetAttrib (const EGLint*, EGLint);
LOCAL EGLBoolean   eglGetAttribEx (const EGLint*, EGLint, EGLint*);
LOCAL void         eglRemoveAttrib (EGLint*, EGLint);
LOCAL EGLBoolean   eglValidateAttribList (const EGLint*);
LOCAL EGLint       fbGetAttrib (FB_CONFIG*, EGLint);
LOCAL int          fbGetAttribEx (FB_CONFIG*, EGLint, EGLint*);
LOCAL int          fbGetBufferTypeOrder (FB_CONFIG*);
LOCAL int          fbGetCaveatOrder (FB_CONFIG*);
LOCAL int          fbGetColorBitsOrder (FB_CONFIG*, EGLint*);
LOCAL int          fbIsValidConfig (FB_CONFIG*, display_t*);
LOCAL int          fbMatchConfig (FB_CONFIG*, EGLint*);
LOCAL int          isCompatibleSurface (surface_t*, context_t*);
LOCAL int          isValidContext (context_t*, display_t*);
LOCAL int          isValidSurface (surface_t*, display_t*);
LOCAL int          lockEgl (void);
LOCAL int          lockDisplay (display_t*);
LOCAL int          qcompare (const void *, const void *);
LOCAL void         unbindContext (context_t*);
LOCAL void         unbindSurface (surface_t*);
LOCAL void         unlockEgl (void);
LOCAL void         unlockDisplay (display_t*);

/* Displays */
LOCAL display_t    displays[MAX_DISPLAYS];

/* EGL base configuration as per specifications */
LOCAL const EGLint eglBaseConfig[] =
    {
    EGL_BUFFER_SIZE,            0,
    EGL_RED_SIZE,               0,
    EGL_GREEN_SIZE,             0,
    EGL_BLUE_SIZE,              0,
    EGL_LUMINANCE_SIZE,         0,
    EGL_ALPHA_SIZE,             0,
    EGL_ALPHA_MASK_SIZE,        0,
    EGL_BIND_TO_TEXTURE_RGB,    EGL_DONT_CARE,
    EGL_BIND_TO_TEXTURE_RGBA,   EGL_DONT_CARE,
    EGL_COLOR_BUFFER_TYPE,      EGL_RGB_BUFFER,
    EGL_CONFIG_CAVEAT,          EGL_DONT_CARE,
    EGL_CONFIG_ID,              EGL_DONT_CARE,
    EGL_CONFORMANT,             0,
    EGL_DEPTH_SIZE,             0,
    EGL_LEVEL,                  0,
    EGL_MAX_PBUFFER_WIDTH,      EGL_DONT_CARE,
    EGL_MAX_PBUFFER_HEIGHT,     EGL_DONT_CARE,
    EGL_MAX_PBUFFER_PIXELS,     EGL_DONT_CARE,
    EGL_MATCH_NATIVE_PIXMAP,    EGL_NONE,
    EGL_MAX_SWAP_INTERVAL,      EGL_DONT_CARE,
    EGL_MIN_SWAP_INTERVAL,      EGL_DONT_CARE,
    EGL_NATIVE_RENDERABLE,      EGL_DONT_CARE,
    EGL_NATIVE_VISUAL_ID,       EGL_DONT_CARE,
    EGL_NATIVE_VISUAL_TYPE,     EGL_DONT_CARE,
    EGL_RENDERABLE_TYPE,        EGL_OPENVG_BIT,
    EGL_SAMPLE_BUFFERS,         0,
    EGL_SAMPLES,                0,
    EGL_STENCIL_SIZE,           0,
    EGL_SURFACE_TYPE,           EGL_WINDOW_BIT,
    EGL_TRANSPARENT_TYPE,       EGL_NONE,
    EGL_TRANSPARENT_RED_VALUE,  EGL_DONT_CARE,
    EGL_TRANSPARENT_GREEN_VALUE,EGL_DONT_CARE,
    EGL_TRANSPARENT_BLUE_VALUE, EGL_DONT_CARE,
    EGL_NONE
    };

/* EGL implementation information */
LOCAL const char    eglClientAPIs[] = "OpenVG";
LOCAL const char    eglVendor[]     = VENDOR_STRING;
LOCAL const char    eglVersion[]    = "1.4";
LOCAL const char    eglExtensions[] = "EGL_WRS_surface";

/* Extensions */
LOCAL const ext_proc_addr_t extensions[] = {
    {"eglDestroySurfaceWRS", (EGLFuncPtr)eglDestroySurfaceWRS},
    {"eglCreatePbufferSurfaceWRS", (EGLFuncPtr)eglCreatePbufferSurfaceWRS},
#ifdef SUPPORT_OPENVG
    {"vgConvertColorWRS", (EGLFuncPtr)vgConvertColorWRS},
    {"vgGetBitDepthWRS", (EGLFuncPtr)vgGetBitDepthWRS},
    {"vgGetSurfaceFormatWRS", (EGLFuncPtr)vgGetSurfaceFormatWRS},
    {"vgTransformWRS", (EGLFuncPtr)vgTransformWRS},
#endif /* SUPPORT_OPENVG */
    {NULL, NULL}};

/* Spinlock */
#if defined(_WRS_KERNEL)
LOCAL SPIN_LOCK_TASK_DECL (spinlock, 0);
#else
LOCAL int eglMutexInit = FALSE;
LOCAL pthread_mutex_t eglMutex;
#endif

/*******************************************************************************
 *
 * bindContext - bind a context to the specified thread
 *
 */
LOCAL void bindContext
    (
    context_t* pContext,
    unsigned long threadId
    )
    {
    pContext->refCount++;
    pContext->threadId = threadId;
    }

/*******************************************************************************
 *
 * bindSurface - bind a surface to the specified thread
 *
 */
LOCAL void bindSurface
    (
    surface_t* pSurface,
    unsigned long threadId
    )
    {
    pSurface->refCount++;
    pSurface->threadId = threadId;
    }

/*******************************************************************************
 *
 * controlDisplay - execute a control call on the specified display
 *
 * RETURNS: OK if successful, otherwise ERROR
 */
LOCAL STATUS controlDisplay
    (
    display_t* pDisplay,
    int request
    )
    {
    return (ioctl (pDisplay->fd, request, &pDisplay->arg));
    }

/*******************************************************************************
 *
 * deleteContext - unlink and delete a context
 *
 * SEE ALSO: unbindContext
 */
LOCAL void deleteContext
    (
    context_t* pContext
    )
    {
    /* Unlink */
    LL_REMOVE(pContext->pDisplay->pContexts, pContext);

    /* Free allocated memory */
    free(pContext);
    }

/*******************************************************************************
 *
 * deleteSurface - unlink and delete a surface
 *
 * SEE ALSO: unbindSurface
 */
LOCAL void deleteSurface
    (
    surface_t* pSurface
    )
    {
    /* Unlink */
    LL_REMOVE(pSurface->pDisplay->pSurfaces, pSurface);

    if (pSurface->type == SURFACE_TYPE_PBUFFER)
        {
        free(pSurface->pBackBuf);
        }

    /* Free allocated memory */
    free(pSurface);
    }

/*******************************************************************************
 *
 * eglBindAPI
 */
EGLAPI EGLBoolean EGLAPIENTRY eglBindAPI
    (
    EGLenum api
    )
    {
    EGLBoolean retVal = EGL_FALSE;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (api != EGL_OPENVG_API)
            EGL_FAIL(EGL_BAD_PARAMETER);

        /* Set the API for the current thread */
        pGc->egl.api = api - 0x30a0;
        retVal = EGL_TRUE;
        }

zz: return (retVal);
    }

/*******************************************************************************
 *
 * eglBindTexImage
 */
EGLAPI EGLBoolean EGLAPIENTRY eglBindTexImage
    (
    EGLDisplay display,
    EGLSurface surface,
    EGLint buffer
    )
    {
    EGLBoolean retVal = EGL_FALSE;

    GET_GC();
    if (pGc != NULL)
        {
        /* Always generate an error */
        EGL_FAIL(EGL_BAD_SURFACE);
        }

zz: return (retVal);
    }

/*******************************************************************************
 *
 * eglChooseConfig
 */
EGLAPI EGLBoolean EGLAPIENTRY eglChooseConfig
    (
    EGLDisplay display,
    const EGLint* attribList,
    EGLConfig* configs,
    EGLint configSize,
    EGLint* pNumConfigs
    )
    {
    EGLint          criteria[NUM_ELEMENTS(eglBaseConfig)] = {0};
    int             i = 0;
    EGLint          numConfigs = 0;
    EGLBoolean      retVal = EGL_FALSE;
    display_t *     pDisplay = (display_t*)display;
    FB_CONFIG **    pConfigs = (FB_CONFIG**)configs;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if ((attribList != NULL) && (eglValidateAttribList(attribList) != EGL_TRUE))
                EGL_FAIL(EGL_BAD_ATTRIBUTE);

            if (pNumConfigs == NULL)
                EGL_FAIL(EGL_BAD_PARAMETER);

            /* If the selection criteria is empty, use the default one */
            if ((attribList == NULL) || (attribList[0] == EGL_NONE))
                attribList = eglBaseConfig;

            /* Clone selection criteria... */
            for (i = 0; (criteria[i] = attribList[i]) != EGL_NONE; i++);
            /* ...and fix it */
            eglRemoveAttrib(criteria, EGL_MAX_PBUFFER_WIDTH);
            eglRemoveAttrib(criteria, EGL_MAX_PBUFFER_HEIGHT);
            eglRemoveAttrib(criteria, EGL_MAX_PBUFFER_PIXELS);
            eglRemoveAttrib(criteria, EGL_NATIVE_VISUAL_ID);
            if (eglGetAttrib(criteria, EGL_TRANSPARENT_TYPE) == EGL_NONE)
                {
                eglRemoveAttrib(criteria, EGL_TRANSPARENT_TYPE);
                eglRemoveAttrib(criteria, EGL_TRANSPARENT_RED_VALUE);
                eglRemoveAttrib(criteria, EGL_TRANSPARENT_GREEN_VALUE);
                eglRemoveAttrib(criteria, EGL_TRANSPARENT_BLUE_VALUE);
                }

            /* Find matching configurations */
            numConfigs = 0;
            for (i = 0; i < FB_MAX_CONFIGS; i++)
                {
                if (pDisplay->configs[i].id == 0)
                    break;
                else if (fbMatchConfig(&pDisplay->configs[i], criteria))
                    {
                    if (pConfigs != NULL)
                        {
                        if (--configSize < 0)
                            break;
                        pConfigs[numConfigs] = &pDisplay->configs[i];
                        }
                    numConfigs++;
                    }
                }

            /* Sort configurations */
            if ((numConfigs > 1) && (pConfigs != NULL))
                {
                pGc->egl.criteria = criteria;
                qsort(pConfigs, numConfigs, sizeof(FB_CONFIG*), qcompare);
                }

            /* Return the number of matching configurations */
            *pNumConfigs = numConfigs;
            retVal = EGL_TRUE;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglCreateContext
 */
EGLAPI EGLContext EGLAPIENTRY eglCreateContext
    (
    EGLDisplay display,
    EGLConfig config,
    EGLContext sharedContext,
    const EGLint* attribList
    )
    {
    int         api = 0;            /* current API */
    context_t * pContext = NULL;    /* new context */
    display_t * pDisplay = (display_t*)display;
    context_t * pSharedContext = (context_t*)sharedContext;
    FB_CONFIG * pConfig = (FB_CONFIG*)config;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (pGc->egl.api == API_NONE)
                EGL_FAIL(EGL_BAD_MATCH);

            if (!fbIsValidConfig(pConfig, pDisplay))
                EGL_FAIL(EGL_BAD_CONFIG);

            /* Check if the specified configuration and the current rendering API are compatible */
            switch (pGc->egl.api)
                {
                case API_OPENGL_ES:
                    api = EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT;
                    break;

                case API_OPENVG:
                    api = EGL_OPENVG_BIT;
                    break;

                case API_OPENGL:
                    api = EGL_OPENGL_BIT;
                    break;
                }
            if ((fbGetAttrib(pConfig, EGL_RENDERABLE_TYPE) & api) == 0)
                EGL_FAIL(EGL_BAD_CONFIG);

            /* Process list of attributes */
            if (attribList != NULL)
                {
                while (attribList[0] != EGL_NONE)
                    {
                    switch (attribList[0])
                        {
                        case EGL_CONTEXT_CLIENT_VERSION:
                            /* Fail (OpenGL|ES only) */
                            EGL_FAIL(EGL_BAD_CONFIG);
                            break;

                        default:
                            EGL_FAIL(EGL_BAD_ATTRIBUTE);
                            break;
                        }
                    attribList += 2;
                    }
                }

            /* :TODO: Add support for shared contexts */
            if (pSharedContext != NULL)
                EGL_FAIL(EGL_BAD_CONTEXT);

            /* Allocate memory */
            pContext = malloc(sizeof(context_t));
            if (pContext == NULL)
                EGL_FAIL(EGL_BAD_ALLOC);

            /* Initialize the context */
            LL_ADD_HEAD(pDisplay->pContexts, pContext);
            pContext->pDisplay = pDisplay;
            pContext->refCount = 0;
            pContext->deletePending = FALSE;
            pContext->api = pGc->egl.api;
            pContext->pConfig = pConfig;
            pContext->pDrawSurface = NULL;
            pContext->pReadSurface = NULL;
            pContext->threadId = INVALID_THREAD_ID;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (pContext);
    }

/*******************************************************************************
 *
 * eglCreatePbufferSurface
 */
EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferSurface
    (
    EGLDisplay display,
    EGLConfig config,
    const EGLint* attribList
    )
    {
    return (eglCreatePbufferSurfaceWRS(display, config, attribList, EGL_FALSE));
    }

/*******************************************************************************
 *
 * eglCreatePbufferSurfaceWRS - create a single pbuffer surface
 *
 * RETURNS: A valid handle if successful, otherwise EGL_NO_SURFACE
 */
EGLAPI EGLSurface EGLAPIENTRY eglCreatePbufferSurfaceWRS
    (
    EGLDisplay display,
    EGLConfig config,
    const EGLint* attribList,
    EGLBoolean lock
    )
    {
    EGLint          attrib, value;
    surface_t *     pSurface = NULL;
    display_t *     pDisplay = (display_t*)display;
    FB_CONFIG *     pConfig = (FB_CONFIG*)config;
    unsigned int *  pBufferAlloc = NULL;
    int             width, stride, height;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (!fbIsValidConfig(pConfig, pDisplay))
                EGL_FAIL(EGL_BAD_CONFIG);

            if ((fbGetAttrib(pConfig, EGL_SURFACE_TYPE) & EGL_PBUFFER_BIT) == 0)
                EGL_FAIL(EGL_BAD_MATCH);

            /* Set required attributes to their default value */
            width = height = 0;

            /* Validate the list of attributes for the window */
            if (attribList != NULL)
                {
                while (attribList[0] != EGL_NONE)
                    {
                    attrib = attribList[0];
                    value = attribList[1];
                    switch (attrib)
                        {
                        case EGL_WIDTH:
                            if (value < 0)
                                EGL_FAIL(EGL_BAD_ATTRIBUTE);
                            width = value;
                            break;

                        case EGL_HEIGHT:
                            if (value < 0)
                                EGL_FAIL(EGL_BAD_ATTRIBUTE);
                            height = value;
                            break;

                        case EGL_LARGEST_PBUFFER:
                            /* :TODO: */
                            break;

                        case EGL_TEXTURE_FORMAT:
                            /* :TODO: */
                            break;

                        case EGL_TEXTURE_TARGET:
                            /* :TODO: */
                            break;

                        case EGL_MIPMAP_TEXTURE:
                            /* :TODO: */
                            break;

                        case EGL_VG_COLORSPACE:
                            if (value != EGL_VG_COLORSPACE_sRGB)
                                EGL_FAIL(EGL_BAD_ATTRIBUTE);
                            break;

                        case EGL_VG_ALPHA_FORMAT:
                            if (value != EGL_VG_ALPHA_FORMAT_PRE)
                                EGL_FAIL(EGL_BAD_ATTRIBUTE);
                            break;

                        default:
                            EGL_FAIL(EGL_BAD_ATTRIBUTE);
                            break;
                        }
                    attribList += 2;
                    }
                }

            /* Determine the stride */
            stride = width * fbGetAttrib(pConfig, EGL_BUFFER_SIZE) / 8;

            /* Allocate memory */
            pSurface = malloc(sizeof(surface_t));
            if (pSurface == NULL)
                EGL_FAIL(EGL_BAD_ALLOC);

            pBufferAlloc = (unsigned int *)calloc(1, stride *height);
            if (pBufferAlloc == NULL)
                {
                free(pSurface);
                pSurface = NULL;
                EGL_FAIL(EGL_BAD_ALLOC);
                }

            /* Initialize the surface */
            LL_ADD_HEAD(pDisplay->pSurfaces, pSurface);
            pSurface->pDisplay = pDisplay;
            pSurface->refCount = (lock == EGL_TRUE) ? (1) : (0);
            pSurface->deletePending = FALSE;
            pSurface->type = SURFACE_TYPE_PBUFFER;
            pSurface->pConfig = pConfig;
            pSurface->pFrontBuf = (void *)pBufferAlloc;
            pSurface->pBackBuf = (void *)pBufferAlloc;
            pSurface->width = width, pSurface->stride = stride;
            pSurface->height = height;
            pSurface->rect.pNext = NULL;
            pSurface->rect.x0 = pSurface->rect.y0 = 0.0f;
            pSurface->rect.x1 = (float)width, pSurface->rect.y1 = (float)height;
            pSurface->threadId = INVALID_THREAD_ID;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (pSurface);
    }

/*******************************************************************************
 *
 * eglCreateWindowSurface
 */
EGLAPI EGLSurface EGLAPIENTRY eglCreateWindowSurface
    (
    EGLDisplay display,
    EGLConfig config,
    EGLNativeWindowType win,
    const EGLint* attribList
    )
    {
    EGLint      attrib, value;
    surface_t * pSurface = NULL;
    display_t * pDisplay = (display_t*)display;
    FB_CONFIG * pConfig = (FB_CONFIG*)config;
    int         width, stride, height, offset;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (!fbIsValidConfig(pConfig, pDisplay))
                EGL_FAIL(EGL_BAD_CONFIG);

            if ((fbGetAttrib(pConfig, EGL_SURFACE_TYPE) & EGL_WINDOW_BIT) == 0)
                EGL_FAIL(EGL_BAD_MATCH);

            if (win != 0)
                EGL_FAIL(EGL_BAD_NATIVE_WINDOW);

            /* Validate the list of attributes for the window */
            if (attribList != NULL)
                {
                while (attribList[0] != EGL_NONE)
                    {
                    attrib = attribList[0];
                    value = attribList[1];
                    switch (attrib)
                        {
                        case EGL_RENDER_BUFFER:
                            if ((value != EGL_BACK_BUFFER) && (value != EGL_SINGLE_BUFFER))
                                EGL_FAIL(EGL_BAD_ATTRIBUTE);
                            /* Ignore (single-buffering is not supported) */
                            break;

                        case EGL_VG_COLORSPACE:
                            if (value != EGL_VG_COLORSPACE_sRGB)
                                EGL_FAIL(EGL_BAD_ATTRIBUTE);
                            break;

                        case EGL_VG_ALPHA_FORMAT:
                            if ((value != EGL_VG_ALPHA_FORMAT_NONPRE) && (value != EGL_VG_ALPHA_FORMAT_PRE))
                                EGL_FAIL(EGL_BAD_ATTRIBUTE);
                            /* Ignore (alpha values on a frame buffer surface are not used) */
                            break;

                        default:
                            EGL_FAIL(EGL_BAD_ATTRIBUTE);
                            break;
                        }
                    attribList += 2;
                    }
                }

            /* Get window dimensions */
            assert(controlDisplay(pDisplay, FB_IOCTL_GET_FB_INFO) == OK);
            width = pDisplay->arg.getFbInfo.width;
            stride = pDisplay->arg.getFbInfo.stride;
            height = pDisplay->arg.getFbInfo.height;
#if defined(GFX_FIXED_FBMODE_OFFSET)
            offset = pDisplay->arg.getFbInfo.offset;
#else
            offset = stride * height;
#endif
            /* Allocate memory */
            pSurface = malloc(sizeof(surface_t));
            if (pSurface == NULL)
                EGL_FAIL(EGL_BAD_ALLOC);

            /* Initialize the window surface */
            LL_ADD_HEAD(pDisplay->pSurfaces, pSurface);
            pSurface->pDisplay = pDisplay;
            pSurface->refCount = 0;
            pSurface->deletePending = FALSE;
            pSurface->type = SURFACE_TYPE_WINDOW;
            pSurface->pConfig = pConfig;
            pSurface->buffers = pDisplay->arg.getFbInfo.buffers;
            pSurface->pFrontBuf = pDisplay->arg.getFbInfo.pFirstFb;
            if (pSurface->buffers < 2)
                {
                pSurface->pBackBuf = pSurface->pFrontBuf;
                pSurface->pThirdBuf = pSurface->pBackBuf;
                }
            else if (pSurface->buffers == 2)
                {
                pSurface->pBackBuf = (void *)((char*)(pSurface->pFrontBuf) + offset);
                pSurface->pThirdBuf = pSurface->pBackBuf;
                }
            else
                {
                pSurface->pBackBuf = (void *)((char*)(pSurface->pFrontBuf) + offset);
                pSurface->pThirdBuf = (void *)((char*)(pSurface->pBackBuf) + offset);
                }
            pSurface->width = width, pSurface->stride = stride;
            pSurface->height = height;
            pSurface->rect.pNext = NULL;
            pSurface->rect.x0 = pSurface->rect.y0 = 0.0f;
            pSurface->rect.x1 = (float)width, pSurface->rect.y1 = (float)height;
            pSurface->threadId = INVALID_THREAD_ID;
            pSurface->swapInterval = 1;

            pDisplay->arg.setFb.pFb = pSurface->pFrontBuf;
            pDisplay->arg.setFb.when = pSurface->swapInterval;
            assert(controlDisplay(pDisplay, FB_IOCTL_SET_FB) == OK);

zz:         unlockDisplay(pDisplay);
            }
        }

    return (pSurface);
    }

/*******************************************************************************
 *
 * eglCreatePixmapSurface
 */
EGLAPI EGLSurface EGLAPIENTRY eglCreatePixmapSurface
    (
    EGLDisplay display,
    EGLConfig config,
    EGLNativePixmapType pixmap,
    const EGLint* attribList
    )
    {
    /* :TODO: */
    return (NULL);
    }

/*******************************************************************************
 *
 * eglDestroyContext
 */
EGLAPI EGLBoolean EGLAPIENTRY eglDestroyContext
    (
    EGLDisplay display,
    EGLContext context
    )
    {
    EGLBoolean  retVal = EGL_FALSE;
    display_t * pDisplay = (display_t*)display;
    context_t * pContext = (context_t*)context;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (!isValidContext(pContext, pDisplay))
                EGL_FAIL(EGL_BAD_CONTEXT);

            /* Delete the context */
            if (pContext->refCount > 0)
                pContext->deletePending = TRUE;
            else
                deleteContext(pContext);
            retVal = EGL_TRUE;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglDestroySurface
 */
EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurface
    (
    EGLDisplay pDisplay,
    EGLSurface pSurface
    )
    {
    return (eglDestroySurfaceWRS(pDisplay, pSurface, EGL_FALSE));
    }

/*******************************************************************************
 *
 * eglDestroySurfaceWRS - mark all resources associated with the specified
 *                        surface for deletion as soon as possible
 *
 * RETURNS: EGL_TRUE if successful, otherwise EGL_FALSE
 */
EGLAPI EGLBoolean EGLAPIENTRY eglDestroySurfaceWRS
    (
    EGLDisplay display,
    EGLSurface surface,
    EGLBoolean force
    )
    {
    EGLBoolean  retVal = EGL_FALSE;
    display_t * pDisplay = (display_t*)display;
    surface_t * pSurface = (surface_t*)surface;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (!isValidSurface(pSurface, pDisplay))
                EGL_FAIL(EGL_BAD_SURFACE);

            /* Destroy the surface */
            if ((pSurface->refCount > 0) && (force == EGL_FALSE))
                pSurface->deletePending = TRUE;
            else
                deleteSurface(pSurface);
            retVal = EGL_TRUE;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglGetAttrib - return the value of an attribute from the specified list
 *
 * RETURNS: The attribute value if set, otherwise EGL_DONT_CARE
 *
 * SEE ALSO: eglGetAttribEx
 */
LOCAL EGLint eglGetAttrib
    (
    const EGLint* attribList,
    EGLint attrib
    )
    {
    EGLint value;

    if (eglGetAttribEx(attribList, attrib, &value) != EGL_TRUE)
        value = EGL_DONT_CARE;

    return (value);
    }

/*******************************************************************************
 *
 * eglGetAttribEx - return the value of an attribute from the specified list
 *
 * RETURNS: EGL_TRUE if the requested attribute has a value, otherwise EGL_FALSE
 *
 * SEE ALSO: eglGetAttrib
 */
LOCAL EGLBoolean eglGetAttribEx
    (
    const EGLint* attribList,
    EGLint attrib,
    EGLint* pValue
    )
    {
    EGLBoolean retVal = EGL_FALSE;

    while (attribList[0] != EGL_NONE)
        {
        if (attribList[0] == attrib)
            {
            *pValue = attribList[1];
            retVal = EGL_TRUE;
            break;
            }
        attribList += 2;
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglGetConfigAttrib
 */
EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigAttrib
    (
    EGLDisplay display,
    EGLConfig config,
    EGLint attrib,
    EGLint* pValue
    )
    {
    EGLBoolean  retVal = EGL_FALSE;
    display_t * pDisplay = (display_t*)display;
    FB_CONFIG * pConfig = (FB_CONFIG*)config;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (!fbIsValidConfig(pConfig, pDisplay))
                EGL_FAIL(EGL_BAD_CONFIG);

            if (pValue == NULL)
                EGL_FAIL(EGL_BAD_PARAMETER);

            /* Return the requested attribute */
            switch (attrib)
                {
                case EGL_BUFFER_SIZE:
                case EGL_RED_SIZE:
                case EGL_GREEN_SIZE:
                case EGL_BLUE_SIZE:
                case EGL_LUMINANCE_SIZE:
                case EGL_ALPHA_SIZE:
                case EGL_ALPHA_MASK_SIZE:
                case EGL_BIND_TO_TEXTURE_RGB:
                case EGL_BIND_TO_TEXTURE_RGBA:
                case EGL_COLOR_BUFFER_TYPE:
                case EGL_CONFIG_CAVEAT:
                case EGL_CONFIG_ID:
                case EGL_CONFORMANT:
                case EGL_DEPTH_SIZE:
                case EGL_LEVEL:
                case EGL_MAX_PBUFFER_WIDTH:
                case EGL_MAX_PBUFFER_HEIGHT:
                case EGL_MAX_PBUFFER_PIXELS:
                case EGL_MAX_SWAP_INTERVAL:
                case EGL_MIN_SWAP_INTERVAL:
                case EGL_NATIVE_RENDERABLE:
                case EGL_NATIVE_VISUAL_ID:
                case EGL_NATIVE_VISUAL_TYPE:
                case EGL_RENDERABLE_TYPE:
                case EGL_SAMPLE_BUFFERS:
                case EGL_SAMPLES:
                case EGL_STENCIL_SIZE:
                case EGL_SURFACE_TYPE:
                case EGL_TRANSPARENT_TYPE:
                case EGL_TRANSPARENT_RED_VALUE:
                case EGL_TRANSPARENT_GREEN_VALUE:
                case EGL_TRANSPARENT_BLUE_VALUE:
                    *pValue = fbGetAttrib(pConfig, attrib);
                    retVal = EGL_TRUE;
                    break;

                default:
                    EGL_FAIL(EGL_BAD_ATTRIBUTE);
                    break;
                }

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglGetConfigs
 */
EGLAPI EGLBoolean EGLAPIENTRY eglGetConfigs
    (
    EGLDisplay display,
    EGLConfig* configs,
    EGLint configSize,
    EGLint* pNumConfigs
    )
    {
    int             i = 0;
    EGLint          numConfigs = 0;
    EGLBoolean      retVal = EGL_FALSE;
    display_t *     pDisplay = (display_t*)display;
    FB_CONFIG **    pConfigs = (FB_CONFIG**)configs;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (pNumConfigs == NULL)
                EGL_FAIL(EGL_BAD_PARAMETER);

            /* Enumerate configurations */
            numConfigs = 0;
            for (i = 0; i < FB_MAX_CONFIGS; i++)
                {
                if (pDisplay->configs[i].id == 0)
                    break;
                else
                    {
                    if (pConfigs != NULL)
                        {
                        if (--configSize < 0)
                            break;
                        pConfigs[numConfigs] = &pDisplay->configs[i];
                        }
                    numConfigs++;
                    }
                }

            /* Return the number of configurations */
            *pNumConfigs = numConfigs;
            retVal = EGL_TRUE;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglGetCurrentContext
 */
EGLAPI EGLContext EGLAPIENTRY eglGetCurrentContext
    (
    )
    {
    EGLContext pContext = NULL;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockEgl())
            {
            if (pGc->egl.api != API_NONE)
                pContext = pGc->egl.pContext[pGc->egl.api];

            unlockEgl();
            }
        }

    return (pContext);
    }

/*******************************************************************************
 *
 * eglGetCurrentDisplay
 */
EGLAPI EGLDisplay EGLAPIENTRY eglGetCurrentDisplay
    (
    )
    {
    context_t* pCurrentContext;
    EGLDisplay pDisplay = NULL;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockEgl())
            {
            if (pGc->egl.api != API_NONE)
                {
                pCurrentContext = pGc->egl.pContext[pGc->egl.api];
                if (pCurrentContext != NULL)
                    pDisplay = pCurrentContext->pDisplay;
                }

            unlockEgl();
            }
        }

    return (pDisplay);
    }

/*******************************************************************************
 *
 * eglGetCurrentSurface
 */
EGLAPI EGLSurface EGLAPIENTRY eglGetCurrentSurface
    (
    EGLint type
    )
    {
    context_t* pCurrentContext;
    EGLSurface pSurface = NULL;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockEgl())
            {
            /* Check arguments */
            if ((type != EGL_READ) && (type != EGL_DRAW))
                EGL_FAIL(EGL_BAD_PARAMETER);

            /* Return the current surface */
            if (pGc->egl.api != API_NONE)
                {
                pCurrentContext = pGc->egl.pContext[pGc->egl.api];
                if (pCurrentContext != NULL)
                    {
                    if (type == EGL_DRAW)
                        pSurface = pCurrentContext->pDrawSurface;
                    else
                        pSurface = pCurrentContext->pReadSurface;
                    }
                }

zz:         unlockEgl();
            }
        }

    return (pSurface);
    }

/*******************************************************************************
 *
 * eglGetDisplay
 */
EGLAPI EGLDisplay EGLAPIENTRY eglGetDisplay
    (
    EGLNativeDisplayType devName
    )
    {
    int        i, j;
    display_t* pDisplay = NULL;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockEgl())
            {
            /* If no specific device is requested, use the default one */
            if (devName == EGL_DEFAULT_DISPLAY)
                devName = "/dev/fb0"; /*FB_DEFAULT_DEVICE;*/

            /* Check if a display was already initialized for the requested device */
            for (i = 0, j = -1; i < MAX_DISPLAYS; i++)
                {
                if (displays[i].fd > 0)
                    {
                    if (strncmp(displays[i].name, devName, DEV_NAME_LEN - 1) == 0)
                        {
                        pDisplay = &displays[i];
                        unlockEgl();
                        goto zz;
                        }
                    }
                else if (j < 0)
                    {
                    j = i;
                    }
                }

            /* If all displays are used, stop here... */
            if (j < 0)
                {
                unlockEgl();
                }
            /* ...otherwise initialize one */
            else
                {
                /* Open underlying I/O device... */
                displays[j].fd = open(devName, O_RDWR, 0666);
                unlockEgl();
                /* ...but make sure it went ok... */
                if (displays[j].fd > 0)
                    {
                    /* ...before committing */
                    pDisplay = &displays[j];
                    pDisplay->lock = semMCreate(SEM_Q_PRIORITY | SEM_DELETE_SAFE | SEM_INVERSION_SAFE);
                    strncpy(pDisplay->name, devName, DEV_NAME_LEN - 1);
                    pDisplay->pSurfaces = NULL;
                    pDisplay->pContexts = NULL;
                    }
                }
            }
        }

zz: return (pDisplay);
    }

/*******************************************************************************
 *
 * eglGetError
 */
EGLAPI EGLint EGLAPIENTRY eglGetError
    (
    )
    {
    EGLint lastError = EGL_SUCCESS;

    GET_GC();
    if (pGc != NULL)
        {
        lastError = pGc->egl.error;
        pGc->egl.error = EGL_SUCCESS;
        }

    return (lastError);
    }

/*******************************************************************************
 *
 * eglGetProcAddress
 */
EGLAPI EGLFuncPtr EGLAPIENTRY eglGetProcAddress
    (
    const char* funcName
    )
    {
    int        i;
    EGLFuncPtr procAddress = NULL;

    i = 0;
    while (extensions[i].name != NULL)
        {
        if (strcmp(funcName, extensions[i].name) == 0)
            {
            procAddress = extensions[i].address;
            break;
            }
        i++;
        }

    return (procAddress);
    }

/*******************************************************************************
 *
 * eglInitialize
 */
EGLAPI EGLBoolean EGLAPIENTRY eglInitialize
    (
    EGLDisplay display,
    EGLint* pMajor,
    EGLint* pMinor
    )
    {
    EGLBoolean retVal = EGL_FALSE;
    display_t* pDisplay = (display_t*)display;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            if (!pDisplay->initialized)
                {
                /* Get information about the current video mode */
                if (controlDisplay(pDisplay, FB_IOCTL_GET_FB_INFO) != OK)
                    {
                    /* If the display has not been initialized yet, do it now */
                    pDisplay->arg.setVideoMode.videoMode = FB_DEFAULT_VIDEO_MODE;
                    if (controlDisplay(pDisplay, FB_IOCTL_SET_VIDEO_MODE) != OK)
                        EGL_FAIL(EGL_NOT_INITIALIZED);
                    }

                /* Retrieve all supported configurations */
                pDisplay->arg.getConfigs.pConfigs = pDisplay->configs;
                if (controlDisplay(pDisplay, FB_IOCTL_GET_CONFIGS) != OK)
                    EGL_FAIL(EGL_NOT_INITIALIZED);

                /* Finish initialization */
                pDisplay->initialized = TRUE;
                pDisplay->pSurfaces = NULL;
                pDisplay->pContexts = NULL;
                }

            /* Return version numbers */
            if (pMajor != NULL)
                *pMajor = eglVersion[0] - '0';
            if (pMinor != NULL)
                *pMinor = eglVersion[2] - '0';
            retVal = EGL_TRUE;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglMakeCurrent
 */
EGLAPI EGLBoolean EGLAPIENTRY eglMakeCurrent
    (
    EGLDisplay display,
    EGLSurface drawSurface,
    EGLSurface readSurface,
    EGLContext context
    )
    {
    int         api = 0;
    context_t * pCurrentContext = NULL;
    EGLBoolean  retVal = EGL_FALSE;
    display_t * pDisplay = (display_t*)display;
    surface_t * pDrawSurface = (surface_t*)drawSurface;
    surface_t * pReadSurface = (surface_t*)readSurface;
    context_t * pContext = (context_t*)context;
    unsigned long threadId = 0;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (pGc->egl.api == API_NONE)
                EGL_FAIL(EGL_BAD_MATCH);

            if (pContext == NULL)
                {
                if ((pDrawSurface != NULL) || (pReadSurface != NULL))
                    EGL_FAIL(EGL_BAD_MATCH);
                }
            else
                {
                if (!isValidContext(pContext, pDisplay))
                    EGL_FAIL(EGL_BAD_CONTEXT);

                if ((pContext->threadId != INVALID_THREAD_ID) &&
                    (pContext->threadId != pGc->threadId))
                    EGL_FAIL(EGL_BAD_ACCESS);

                if (!isValidSurface(pDrawSurface, pDisplay))
                    EGL_FAIL(EGL_BAD_SURFACE);

                if (!isValidSurface(pReadSurface, pDisplay))
                    EGL_FAIL(EGL_BAD_SURFACE);

                if ((pGc->egl.api == API_OPENVG) &&
                    (pDrawSurface != pReadSurface))
                    EGL_FAIL(EGL_BAD_SURFACE);

                if (!isCompatibleSurface(pDrawSurface, pContext))
                    EGL_FAIL(EGL_BAD_MATCH);

                if ((pDrawSurface->threadId != INVALID_THREAD_ID) &&
                    (pDrawSurface->threadId != pGc->threadId))
                    EGL_FAIL(EGL_BAD_ACCESS);

                if (!isCompatibleSurface(pReadSurface, pContext))
                    EGL_FAIL(EGL_BAD_MATCH);

                if ((pReadSurface->threadId != INVALID_THREAD_ID) &&
                    (pReadSurface->threadId != pGc->threadId))
                    EGL_FAIL(EGL_BAD_ACCESS);
                }

            /* Determine the client API to bind to */
            api = (pContext == NULL) ? (pGc->egl.api) : (pContext->api);
            threadId = pGc->threadId;

            /* Unbind the current context */
            pCurrentContext = pGc->egl.pContext[api];
            if (pCurrentContext != NULL)
                {
                unbindSurface(pCurrentContext->pDrawSurface);
                pCurrentContext->pDrawSurface = NULL;
                unbindSurface(pCurrentContext->pReadSurface);
                pCurrentContext->pReadSurface = NULL;
                unbindContext(pCurrentContext);
                }

            /* Bind the new context */
            pGc->egl.pContext[api] = pContext;
            if (pContext != NULL)
                {
                bindSurface(pDrawSurface, threadId);
                pContext->pDrawSurface = pDrawSurface;
                bindSurface(pReadSurface, threadId);
                pContext->pReadSurface = pReadSurface;
                bindContext(pContext, threadId);
                }

            /* Call the API specific context initialization */
            if (pGc->egl.makeCurrent[api] != NULL)
                pGc->egl.makeCurrent[api](pGc, pDrawSurface, pReadSurface, pContext);

            retVal = EGL_TRUE;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglQueryAPI
 */
EGLAPI EGLenum EGLAPIENTRY eglQueryAPI(void)
    {
    EGLenum api = EGL_NONE;

    GET_GC();
    if (pGc != NULL)
        {
        if (pGc->egl.api != API_NONE)
            api = pGc->egl.api + 0x30a0;
        }

    return (api);
    }

/*******************************************************************************
 *
 * eglQueryContext
 */
EGLAPI EGLBoolean EGLAPIENTRY eglQueryContext
    (
    EGLDisplay display,
    EGLContext context,
    EGLint attrib,
    EGLint* pValue
    )
    {
    EGLBoolean  retVal = EGL_FALSE;
    EGLint      value = 0;
    display_t * pDisplay = (display_t*)display;
    context_t * pContext = (context_t*)context;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (!isValidContext(pContext, pDisplay))
                EGL_FAIL(EGL_BAD_CONTEXT);

            if (pValue == NULL)
                EGL_FAIL(EGL_BAD_PARAMETER);

            /* Return the requested attribute */
            switch (attrib)
                {
                case EGL_CONFIG_ID:
                    value = pContext->pConfig->id;
                    break;

                case EGL_CONTEXT_CLIENT_TYPE:
                    value = pContext->api + 0x30a0;
                    break;

                case EGL_CONTEXT_CLIENT_VERSION:
                    value = 1;
                    break;

                case EGL_RENDER_BUFFER:
                    if (pContext->pDrawSurface == NULL)
                        value = EGL_NONE;
                    else if (pContext->pDrawSurface->type == SURFACE_TYPE_PIXMAP)
                        value = EGL_SINGLE_BUFFER;
                    else
                        value = EGL_BACK_BUFFER;
                    break;

                default:
                    EGL_FAIL(EGL_BAD_ATTRIBUTE);
                    break;
                }
            *pValue = value;
            retVal = EGL_TRUE;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglQueryString
 */
EGLAPI const char* EGLAPIENTRY eglQueryString
    (
    EGLDisplay display,
    EGLint name
    )
    {
    const char * string = NULL;
    display_t *  pDisplay = (display_t*)display;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            /* Return the requested string */
            switch (name)
                {
                case EGL_CLIENT_APIS:
                    string = eglClientAPIs;
                    break;

                case EGL_EXTENSIONS:
                    string = eglExtensions;
                    break;

                case EGL_VENDOR:
                    string = eglVendor;
                    break;

                case EGL_VERSION:
                    string = eglVersion;
                    break;

                default:
                    EGL_FAIL(EGL_BAD_PARAMETER);
                    break;
                }

zz:         unlockDisplay(pDisplay);
            }
        }

    return (string);
    }

/*******************************************************************************
 *
 * eglQuerySurface
 */
EGLAPI EGLBoolean EGLAPIENTRY eglQuerySurface
    (
    EGLDisplay display,
    EGLSurface surface,
    EGLint attrib,
    EGLint* pValue
    )
    {
    EGLBoolean  retVal = EGL_FALSE;
    display_t * pDisplay = (display_t*)display;
    surface_t * pSurface = (surface_t*)surface;
    EGLint      value = 0;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (!isValidSurface(pSurface, pDisplay))
                EGL_FAIL(EGL_BAD_SURFACE);

            if (pValue == NULL)
                EGL_FAIL(EGL_BAD_PARAMETER);

            /* Return the requested attribute */
            value = EGL_DONT_CARE;
            switch (attrib)
                {
                case EGL_VG_ALPHA_FORMAT:
                    value = EGL_VG_ALPHA_FORMAT_PRE;
                    break;

                case EGL_VG_COLORSPACE:
                    value = EGL_VG_COLORSPACE_sRGB;
                    break;

                case EGL_CONFIG_ID:
                    value = pSurface->pConfig->id;
                    break;

                case EGL_HEIGHT:
                    value = pSurface->height;
                    break;

                case EGL_HORIZONTAL_RESOLUTION:
                    value = EGL_UNKNOWN;
                    break;

                case EGL_LARGEST_PBUFFER:
                    if (pSurface->type == SURFACE_TYPE_PBUFFER)
                    {
                        /* :TODO: */
                    }
                    break;

                case EGL_MIPMAP_TEXTURE:
                    if (pSurface->type == SURFACE_TYPE_PBUFFER)
                        {
                        /* :TODO: */
                        }
                    break;

                case EGL_MIPMAP_LEVEL:
                    if (pSurface->type == SURFACE_TYPE_PBUFFER)
                        {
                        /* :TODO: */
                        }
                    break;

                case EGL_MULTISAMPLE_RESOLVE:
                    value = EGL_MULTISAMPLE_RESOLVE_DEFAULT;
                    break;

                case EGL_PIXEL_ASPECT_RATIO:
                    /* Assume a 1:1 aspect ratio */
                    value = 1 * EGL_DISPLAY_SCALING;
                    break;

                case EGL_RENDER_BUFFER:
                    if (pSurface->type == SURFACE_TYPE_PIXMAP)
                        value = EGL_SINGLE_BUFFER;
                    else
                        value = EGL_BACK_BUFFER;
                    break;

                case EGL_SWAP_BEHAVIOR:
                    value = EGL_BUFFER_DESTROYED;
                    break;

                case EGL_TEXTURE_FORMAT:
                    if (pSurface->type == SURFACE_TYPE_PBUFFER)
                        {
                        /* :TODO: */
                        }
                    break;

                case EGL_TEXTURE_TARGET:
                    if (pSurface->type == SURFACE_TYPE_PBUFFER)
                        {
                        /* :TODO: */
                        }
                    break;

                case EGL_VERTICAL_RESOLUTION:
                    value = EGL_UNKNOWN;
                    break;

                case EGL_WIDTH:
                    value = pSurface->width;
                    break;

                default:
                    EGL_FAIL(EGL_BAD_ATTRIBUTE);
                    break;
                }
            *pValue = value;
            retVal = EGL_TRUE;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglReleaseTexImage
 */
EGLAPI EGLBoolean EGLAPIENTRY eglReleaseTexImage
    (
    EGLDisplay display,
    EGLSurface surface,
    EGLint buffer
    )
    {
    EGLBoolean  retVal = EGL_FALSE;

    GET_GC();
    if (pGc != NULL)
        {
        /* Always generate an error */
        EGL_FAIL(EGL_BAD_SURFACE);
        }

zz: return (retVal);
    }

/*******************************************************************************
 *
 * eglReleaseThread
 */
EGLAPI EGLBoolean EGLAPIENTRY eglReleaseThread
    (
    )
    {
    int         api = 0;
    context_t * pContext = NULL;
    EGLBoolean  retVal = EGL_FALSE;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockEgl())
            {
            /* Unbind contexts and API specific data */
            for (api = API_OPENGL_ES; api <= API_OPENGL; api++)
                {
                pContext = pGc->egl.pContext[api];
                if (pContext != NULL)
                    (void)eglMakeCurrent(pContext->pDisplay, NULL, NULL, NULL);

                if (pGc->egl.releaseThread[api] != NULL)
                    pGc->egl.releaseThread[api](pGc);
                }

            /* Discard the graphics context */
            DELETE_GC();
            retVal = EGL_TRUE;

            unlockEgl();
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglRemoveAttrib - remove the specified attribute from a list
 */
LOCAL void eglRemoveAttrib
    (
    EGLint* attribList,
    EGLint attrib
    )
    {
    while ((attribList[0] != EGL_NONE) && (attribList[0] != attrib))
        attribList += 2;
    while (attribList[0] != EGL_NONE)
        {
        attribList[0] = attribList[2];
        if (attribList[0] == EGL_NONE)
            break;
        attribList[1] = attribList[3];
        attribList += 2;
        }
    }

/*******************************************************************************
 *
 * eglSurfaceAttrib
 */
EGLAPI EGLBoolean EGLAPIENTRY eglSurfaceAttrib
    (
    EGLDisplay display,
    EGLSurface surface,
    EGLint attrib,
    EGLint value
    )
    {
    EGLBoolean  retVal = EGL_FALSE;
    display_t * pDisplay = (display_t*)display;
    surface_t * pSurface = (surface_t*)surface;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (!isValidSurface(pSurface, pDisplay))
                EGL_FAIL(EGL_BAD_SURFACE);

            /* Set the specified attribute */
            switch (attrib)
                {
                case EGL_MIPMAP_LEVEL:
                    if ((fbGetAttrib(pSurface->pConfig, EGL_RENDERABLE_TYPE) & EGL_OPENGL_ES_BIT) == 0)
                        EGL_FAIL(EGL_BAD_PARAMETER);
                    /* Ignore */
                    break;

                case EGL_MULTISAMPLE_RESOLVE:
                    if ((value != EGL_MULTISAMPLE_RESOLVE_DEFAULT) || (value != EGL_MULTISAMPLE_RESOLVE_BOX))
                        EGL_FAIL(EGL_BAD_ATTRIBUTE);
                    if (value == EGL_MULTISAMPLE_RESOLVE_BOX)
                        EGL_FAIL(EGL_BAD_MATCH);
                    /* Ignore */
                    break;

                case EGL_SWAP_BEHAVIOR:
                    if ((value != EGL_BUFFER_PRESERVED) || (value != EGL_BUFFER_DESTROYED))
                        EGL_FAIL(EGL_BAD_ATTRIBUTE);
                    if (value == EGL_BUFFER_PRESERVED)
                        EGL_FAIL(EGL_BAD_MATCH);
                    /* Ignore */
                    break;

                default:
                    EGL_FAIL(EGL_BAD_ATTRIBUTE);
                    break;
                }
            retVal = EGL_TRUE;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglSwapBuffers
 */
EGLAPI EGLBoolean EGLAPIENTRY eglSwapBuffers
    (
    EGLDisplay display,
    EGLSurface surface
    )
    {
    EGLBoolean  retVal = EGL_FALSE;
    display_t * pDisplay = (display_t*)display;
    surface_t * pSurface = (surface_t*)surface;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (pGc->egl.api == API_NONE)
                EGL_FAIL(EGL_BAD_MATCH);

            if (!isValidSurface(pSurface, pDisplay))
                EGL_FAIL(EGL_BAD_SURFACE);

            if (pGc->egl.pContext[pGc->egl.api] == NULL)
                EGL_FAIL(EGL_BAD_CONTEXT);

            if (pGc->egl.pContext[pGc->egl.api]->pDrawSurface != pSurface)
                EGL_FAIL(EGL_BAD_SURFACE);

            /* Swap the buffers */
            if (pSurface->type == SURFACE_TYPE_WINDOW)
                {
                pDisplay->arg.setFb.pFb = pSurface->pBackBuf;
                pDisplay->arg.setFb.when = pSurface->swapInterval;
                assert(controlDisplay(pDisplay, FB_IOCTL_SET_FB) == OK);
                if (pSurface->buffers == 2)
                    EXCHANGE(pSurface->pFrontBuf, pSurface->pBackBuf);
                else if (pSurface->buffers > 2)
                    EXCHANGE3(pSurface->pFrontBuf, pSurface->pBackBuf,
                              pSurface->pThirdBuf);
                }
            retVal = EGL_TRUE;

zz:            unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglSwapInterval
 */
EGLAPI EGLBoolean EGLAPIENTRY eglSwapInterval
    (
    EGLDisplay display,
    EGLint interval
    )
    {
    int         value = 0;
    surface_t * pDrawSurface = NULL;
    display_t * pDisplay = (display_t*)display;
    EGLBoolean  retVal = EGL_FALSE;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            /* Check arguments */
            if (!pDisplay->initialized)
                EGL_FAIL(EGL_NOT_INITIALIZED);

            if (pGc->egl.api == API_NONE)
                EGL_FAIL(EGL_BAD_MATCH);

            if (pGc->egl.pContext[pGc->egl.api] == NULL)
                EGL_FAIL(EGL_BAD_CONTEXT);

            if (pGc->egl.pContext[pGc->egl.api]->pDrawSurface == NULL)
                EGL_FAIL(EGL_BAD_SURFACE);

            /* Check if the surface has the right type */
            pDrawSurface = pGc->egl.pContext[pGc->egl.api]->pDrawSurface;
            if (pDrawSurface->type != SURFACE_TYPE_WINDOW)
                EGL_FAIL(EGL_BAD_SURFACE);

            /* Clamp to the min and max of the configuration */
            value = fbGetAttrib(pDrawSurface->pConfig, EGL_MIN_SWAP_INTERVAL);
            if (value != EGL_DONT_CARE)
                {
                if (interval < value)
                    interval = value;
                }
            value = fbGetAttrib(pDrawSurface->pConfig, EGL_MAX_SWAP_INTERVAL);
            if (value != EGL_DONT_CARE)
                {
                if (interval > value)
                    interval = value;
                }

            /* Set the new interval */
            pDrawSurface->swapInterval = interval;
            retVal = EGL_TRUE;

zz:         unlockDisplay(pDisplay);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglTerminate
 */
EGLAPI EGLBoolean EGLAPIENTRY eglTerminate
    (
    EGLDisplay display
    )
    {
    int        i = 0;
    EGLBoolean retVal = EGL_FALSE;
    display_t* pDisplay = (display_t*)display;

    GET_GC();
    if (pGc != NULL)
        {
        if (lockDisplay(pDisplay) != OK)
            EGL_FAIL(EGL_BAD_DISPLAY);
        else
            {
            if (pDisplay->initialized)
                {
                /* Invalidate all configurations */
                for (i = 0; i < FB_MAX_CONFIGS; i++)
                    pDisplay->configs[i].id = 0;

                /* Delete all surfaces */
                LL_FOREACH(pDisplay->pSurfaces, if (pObj->refCount > 0)
                                                    pObj->deletePending = TRUE;
                                                else
                                                    deleteSurface(pObj));

                /* Delete all contexts */
                LL_FOREACH(pDisplay->pContexts, if (pObj->refCount > 0)
                                                    pObj->deletePending = TRUE;
                                                else
                                                    deleteContext(pObj));

                /* Detach from the underlying device */
                (void)close(pDisplay->fd);

                /* Uninitialize the display */
                pDisplay->initialized = FALSE;
                strncpy(pDisplay->name, "", DEV_NAME_LEN - 1);
                pDisplay->fd = 0;
                }

zz:             unlockDisplay(pDisplay);
                (void)semDelete(pDisplay->lock);
            }
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglWaitClient
 */
EGLAPI EGLBoolean EGLAPIENTRY eglWaitClient
    (
    )
    {
    int        api = 0;             /* client API */
    context_t* pContext = NULL;     /* context */
    EGLBoolean retVal = EGL_FALSE;

    GET_GC();
    if (pGc != NULL)
        {
        /* If there's no context, the function has no effect */
        api = pGc->egl.api;
        if (api == API_NONE)
            retVal = EGL_TRUE;
        else
            {
            pContext = pGc->egl.pContext[api];

            /* Validate the drawing surface */
            if ((pContext->pDrawSurface == NULL) || pContext->pDrawSurface->deletePending)
                EGL_FAIL(EGL_BAD_CURRENT_SURFACE);

            /* Call the client API callback, if any */
            retVal = (pGc->egl.wait[api] == NULL) ? (EGL_TRUE) : (pGc->egl.wait[api]());
            }
        }

zz: return (retVal);
    }

/*******************************************************************************
 *
 * eglWaitGL
 */
EGLAPI EGLBoolean EGLAPIENTRY eglWaitGL
    (
    )
    {
    return (EGL_FALSE);
    }

/*******************************************************************************
 *
 * eglWaitNative
 */
EGLAPI EGLBoolean EGLAPIENTRY eglWaitNative
    (
    EGLint engine
    )
    {
    EGLBoolean retVal = EGL_FALSE;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (engine != EGL_CORE_NATIVE_ENGINE)
            EGL_FAIL(EGL_BAD_PARAMETER);

        /* Return OK as there's no support for native rendering */
        retVal = EGL_TRUE;
        }

zz: return (retVal);
    }

/*******************************************************************************
 *
 * isCompatibleSurface - determine if a surface is compatible the specified
 *                       context
 *
 * RETURNS: TRUE or FALSE
 */
LOCAL int isCompatibleSurface
    (
    surface_t * pSurface,
    context_t * pContext
    )
    {
    int retVal = FALSE;

    /* :TODO: Improve this compatibility check (see page 3, Section 2.2) */
    if (pSurface->pConfig == pContext->pConfig)
        retVal = TRUE;

    return (retVal);
    }

/*******************************************************************************
 *
 * isValidContext - determine if a context is valid for the specified display
 *
 * RETURNS: TRUE or FALSE
 */
LOCAL int isValidContext
    (
    context_t * pContext,
    display_t * pDisplay
    )
    {
    int retVal = FALSE;


    if ((pContext != NULL) && (!pContext->deletePending))
        {
        LL_FOREACH(pDisplay->pContexts, if (pObj == pContext)
                                            {
                                            retVal = TRUE;
                                            break;
                                            }
                                        else);
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * isValidSurface - determine if a surface is valid for the specified display
 *
 * RETURNS: TRUE or FALSE
 */
LOCAL int isValidSurface
    (
    surface_t * pSurface,
    display_t * pDisplay
    )
    {
    int retVal = FALSE;

    if ((pSurface != NULL) && (!pSurface->deletePending))
        {
        LL_FOREACH(pDisplay->pSurfaces, if (pObj == pSurface)
                                            {
                                            retVal = TRUE;
                                            break;
                                            }
                                        else);
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * eglValidateAttribList - make sure that the specified list of attributes does
 *                         not contain undefined attributes or an attribute
 *                         value that is unrecognized or out of range
 *
 * RETURNS: EGL_TRUE or EGL_FALSE
 */
LOCAL EGLBoolean eglValidateAttribList(const EGLint* attribList)
    {
    EGLint     attrib, value;
    EGLBoolean retVal = EGL_TRUE;

    while (attribList[0] != EGL_NONE)
        {
        attrib = attribList[0];
        value = attribList[1];
        switch (attrib)
            {
            /* Integers */
            case EGL_BUFFER_SIZE:
            case EGL_RED_SIZE:
            case EGL_GREEN_SIZE:
            case EGL_BLUE_SIZE:
            case EGL_LUMINANCE_SIZE:
            case EGL_ALPHA_SIZE:
            case EGL_ALPHA_MASK_SIZE:
            case EGL_CONFIG_ID:
            case EGL_DEPTH_SIZE:
            case EGL_MAX_PBUFFER_WIDTH:
            case EGL_MAX_PBUFFER_HEIGHT:
            case EGL_MAX_PBUFFER_PIXELS:
            case EGL_MAX_SWAP_INTERVAL:
            case EGL_MIN_SWAP_INTERVAL:
            case EGL_SAMPLE_BUFFERS:
            case EGL_SAMPLES:
            case EGL_STENCIL_SIZE:
            case EGL_TRANSPARENT_RED_VALUE:
            case EGL_TRANSPARENT_GREEN_VALUE:
            case EGL_TRANSPARENT_BLUE_VALUE:
                if ((value != EGL_DONT_CARE) && (value < 0))
                    retVal = EGL_FALSE;
                break;

            case EGL_LEVEL:
            case EGL_NATIVE_VISUAL_ID:
            case EGL_NATIVE_VISUAL_TYPE:
                break;

            /* Booleans */
            case EGL_BIND_TO_TEXTURE_RGB:
            case EGL_BIND_TO_TEXTURE_RGBA:
            case EGL_NATIVE_RENDERABLE:
                if ((value != EGL_DONT_CARE) && (value != EGL_TRUE) &&
                                                (value != EGL_FALSE))
                    retVal = EGL_FALSE;
                break;

            /* Enums */
            case EGL_COLOR_BUFFER_TYPE:
                if ((value != EGL_DONT_CARE) && (value != EGL_RGB_BUFFER) &&
                                                (value != EGL_LUMINANCE_BUFFER))
                    retVal = EGL_FALSE;
                break;

            case EGL_CONFIG_CAVEAT:
                if ((value != EGL_DONT_CARE) && (value != EGL_NONE) &&
                                                (value != EGL_SLOW_CONFIG) &&
                                                (value != EGL_NON_CONFORMANT_CONFIG))
                    retVal = EGL_FALSE;
                break;

            case EGL_TRANSPARENT_TYPE:
                if ((value != EGL_DONT_CARE) && (value != EGL_NONE) &&
                                                (value != EGL_TRANSPARENT_RGB))
                    retVal = EGL_FALSE;
                break;

            /* Bitmasks */
            case EGL_CONFORMANT:
            case EGL_RENDERABLE_TYPE:
                if ((value != EGL_DONT_CARE) && ((value & ~(EGL_OPENGL_BIT |
                                                            EGL_OPENGL_ES_BIT |
                                                            EGL_OPENGL_ES2_BIT |
                                                            EGL_OPENVG_BIT)) != 0))
                    retVal = EGL_FALSE;
                break;

            case EGL_SURFACE_TYPE:
                if ((value != EGL_DONT_CARE) && ((value & ~(EGL_WINDOW_BIT |
                                                            EGL_PIXMAP_BIT |
                                                            EGL_PBUFFER_BIT |
                                                            EGL_MULTISAMPLE_RESOLVE_BOX_BIT |
                                                            EGL_SWAP_BEHAVIOR_PRESERVED_BIT |
                                                            EGL_VG_COLORSPACE_LINEAR_BIT |
                                                            EGL_VG_ALPHA_FORMAT_PRE_BIT)) != 0))
                    retVal = EGL_FALSE;
                break;

            /* ??? */
            default:
                retVal = EGL_FALSE;
                break;
            }
        if (retVal != EGL_TRUE)
            break;
        attribList += 2;
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * fbGetAttrib - return the value of a configuration attribute
 *
 * RETURNS: The value for the specified attribute if set, otherwise its default
 *          value
 *
 * SEE ALSO: fbGetAttribEx
 */
LOCAL EGLint fbGetAttrib
    (
    FB_CONFIG* pConfig,
    EGLint attrib
    )
    {
    EGLint value;

    if (fbGetAttribEx(pConfig, attrib, &value) != TRUE)
        value = eglGetAttrib(eglBaseConfig, attrib);

    return (value);
    }

/*******************************************************************************
 *
 * fbGetAttrib - return the value of a configuration attribute
 *
 * RETURNS: TRUE if the requested attribute has a value, otherwise FALSE
 *
 * SEE ALSO: fbGetAttrib
 */
LOCAL int fbGetAttribEx
    (
    FB_CONFIG* pConfig,
    EGLint attrib,
    EGLint* pValue
    )
    {
    int retVal = TRUE;

    switch (attrib)
        {
        /* Depth of the color buffer */
        case EGL_BUFFER_SIZE:
            *pValue = pConfig->pixelFormat.redBits + pConfig->pixelFormat.greenBits +
                                                     pConfig->pixelFormat.blueBits +
                                                     pConfig->pixelFormat.alphaBits;
            break;

        /* Bits of Red in the color buffer */
        case EGL_RED_SIZE:
            *pValue = pConfig->pixelFormat.redBits;
            break;

        /* Bits of Green in the color buffer */
        case EGL_GREEN_SIZE:
            *pValue = pConfig->pixelFormat.greenBits;
            break;

        /* Bits of Blue in the color buffer */
        case EGL_BLUE_SIZE:
            *pValue = pConfig->pixelFormat.blueBits;
            break;

        /* Bits of Alpha in the color buffer */
        case EGL_ALPHA_SIZE:
            *pValue = (pConfig->pixelFormat.flags & FB_PIX_ALPHA_IS_PAD) ? (0) : (pConfig->pixelFormat.alphaBits);
            break;

        /* Unique identifier */
        case EGL_CONFIG_ID:
            *pValue = pConfig->id;
            break;

        /* Color buffer type */
        case EGL_COLOR_BUFFER_TYPE:
            *pValue = EGL_RGB_BUFFER;
            break;

        /* Native visual id */
        case EGL_NATIVE_VISUAL_ID:
            *pValue = 0;
            break;

        /* Native visual type */
        case EGL_NATIVE_VISUAL_TYPE:
            *pValue = EGL_NONE;
            break;

        /* Supported client APIs */
        case EGL_RENDERABLE_TYPE:
            *pValue = EGL_OPENVG_BIT;
            break;

        /* Types of supported EGL surfaces */
        case EGL_SURFACE_TYPE:
            *pValue = EGL_PBUFFER_BIT;
            if (pConfig->pixelFormat.flags & FB_PIX_NATIVE)
                *pValue |= EGL_WINDOW_BIT;
            if (~pConfig->pixelFormat.flags & FB_PIX_ALPHA_IS_PAD)
                *pValue |= EGL_VG_ALPHA_FORMAT_PRE_BIT;
            break;

        /* ??? */
        default:
            retVal = FALSE;
            break;
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * fbGetBufferTypeOrder
 *
 * RETURNS: The sort order value of EGL_COLOR_BUFFER_TYPE for the specified
 *          configuration
 */
LOCAL int fbGetBufferTypeOrder
    (
    FB_CONFIG* pConfig
    )
    {
    int order = 0;

    switch (fbGetAttrib(pConfig, EGL_COLOR_BUFFER_TYPE))
        {
        case EGL_RGB_BUFFER:
            order = 0;
            break;

        case EGL_LUMINANCE_BUFFER:
            order = 1;
            break;
        }

    return (order);
    }

/*******************************************************************************
 *
 * fbGetCaveatOrder
 *
 * RETURNS: The sort order value of EGL_CONFIG_CAVEAT for the specified
 *          configuration
 */
LOCAL int fbGetCaveatOrder
    (
    FB_CONFIG* pConfig
    )
    {
    int order = 0;

    switch (fbGetAttrib(pConfig, EGL_CONFIG_CAVEAT))
        {
        case EGL_NONE:
            order = 0;
            break;

        case EGL_SLOW_CONFIG:
            order = 1;
            break;

        case EGL_NON_CONFORMANT_CONFIG:
            order = 2;
            break;
        }

    return (order);
    }

/*******************************************************************************
 *
 * fbGetColorBitsOrder
 *
 * RETURNS: The sort order value of the color attributes for the specified
 *          configuration
 */
LOCAL int fbGetColorBitsOrder
    (
    FB_CONFIG* pConfig,
    EGLint* criteria
    )
    {
    EGLint              attrib, value;
    LOCAL const EGLint  colorAttribs[] = {/* RGB color buffer */
                                         EGL_RED_SIZE,
                                         EGL_GREEN_SIZE,
                                         EGL_BLUE_SIZE,
                                         EGL_ALPHA_SIZE,
                                         EGL_NONE,
                                         /* luminance color buffer */
                                         EGL_LUMINANCE_SIZE,
                                         EGL_ALPHA_SIZE,
                                         EGL_NONE};
    int                 i = 0;
    int                 order = 0;

    /* Determine which attributes to deal with based on buffer type */
    i = (fbGetAttrib(pConfig, EGL_COLOR_BUFFER_TYPE) == EGL_RGB_BUFFER) ? (0) : (5);

    /* Sum all color components that are set in the selection criteria */
    while (colorAttribs[i] != EGL_NONE)
        {
        attrib = colorAttribs[i++];
        value = eglGetAttrib(criteria, attrib);
        if ((value != EGL_DONT_CARE) && (value != 0))
            order += fbGetAttrib(pConfig, attrib);
        }

    return (order);
    }

/*******************************************************************************
 *
 * fbIsValidConfig - determine if a configuration is valid for the specified
 *                   display
 *
 * RETURNS: TRUE or FALSE
 */
LOCAL int fbIsValidConfig
    (
    FB_CONFIG* pConfig,
    display_t* pDisplay
    )
    {
    int retVal = FALSE;

    if (pConfig != NULL)
        {
        if (pConfig->id > 0)
            retVal = TRUE;
        }

    return (retVal);
    }

/*******************************************************************************
 *
 * fbMatchConfig - determine if a configuration matches the specified selection
 *                 criteria
 *
 * RETURNS: TRUE or FALSE
 */
LOCAL int fbMatchConfig
    (
    FB_CONFIG* pConfig,
    EGLint* criteria
    )
    {
    EGLint attrib, criterion, value;
    int    retVal = TRUE;

    /* When EGL_CONFIG_ID is set in the selection criteria, all other attributes are ignored */
    criterion = eglGetAttrib(criteria, EGL_CONFIG_ID);
    if (criterion != EGL_DONT_CARE)
        {
        if (fbGetAttrib(pConfig, EGL_CONFIG_ID) != criterion)
            retVal = FALSE;
        goto zz;
        }

    /* Match attributes against selection criteria */
    while (criteria[0] != EGL_NONE)
        {
        attrib = criteria[0];
        criterion = criteria[1];
        if (criterion != EGL_DONT_CARE)
            {
            value = fbGetAttrib(pConfig, attrib);
            switch (attrib)
                {
                /* At least */
                case EGL_BUFFER_SIZE:
                case EGL_RED_SIZE:
                case EGL_GREEN_SIZE:
                case EGL_BLUE_SIZE:
                case EGL_LUMINANCE_SIZE:
                case EGL_ALPHA_SIZE:
                case EGL_ALPHA_MASK_SIZE:
                case EGL_DEPTH_SIZE:
                case EGL_SAMPLE_BUFFERS:
                case EGL_SAMPLES:
                case EGL_STENCIL_SIZE:
                    if (value < criterion)
                        retVal = FALSE;
                    break;

                /* Exact */
                case EGL_BIND_TO_TEXTURE_RGB:
                case EGL_BIND_TO_TEXTURE_RGBA:
                case EGL_COLOR_BUFFER_TYPE:
                case EGL_CONFIG_CAVEAT:
                case EGL_CONFIG_ID:
                case EGL_LEVEL:
                case EGL_MAX_SWAP_INTERVAL:
                case EGL_MIN_SWAP_INTERVAL:
                case EGL_NATIVE_RENDERABLE:
                case EGL_NATIVE_VISUAL_TYPE:
                case EGL_TRANSPARENT_TYPE:
                case EGL_TRANSPARENT_RED_VALUE:
                case EGL_TRANSPARENT_GREEN_VALUE:
                case EGL_TRANSPARENT_BLUE_VALUE:
                    if (value != criterion)
                        retVal = FALSE;
                    break;

                /* Mask */
                case EGL_CONFORMANT:
                case EGL_RENDERABLE_TYPE:
                case EGL_SURFACE_TYPE:
                    if ((value & criterion) != criterion)
                        retVal = FALSE;
                    break;

                /* Special */
                case EGL_MATCH_NATIVE_PIXMAP:
                    break;

                /* Ignore */
                case EGL_MAX_PBUFFER_WIDTH:
                case EGL_MAX_PBUFFER_HEIGHT:
                case EGL_MAX_PBUFFER_PIXELS:
                case EGL_NATIVE_VISUAL_ID:
                    break;

                /* ??? */
                default:
                    retVal = FALSE;
                    break;
                }
            }
        if (retVal != TRUE)
            break;
        criteria += 2;
        }

zz: return (retVal);
    }

/*******************************************************************************
 *
 * lockEgl - Disable thread preemption
 *
 * RETURNS: A non-zero value if successful
 */
LOCAL int lockEgl
    (
    )
    {
#if defined(_WRS_KERNEL)
    SPIN_LOCK_TASK_TAKE(&spinlock);
#else
    if (!eglMutexInit)
        {
        pthread_mutex_init(&eglMutex, 0);
        eglMutexInit = TRUE;
        }
    pthread_mutex_lock(&eglMutex);
#endif
    return (1);
    }

/*******************************************************************************
 *
 * lockDisplay - lock the specified display
 *
 * RETURNS: OK if successful, otherwise ERROR
 */
LOCAL int lockDisplay(display_t* pDisplay)
    {
    int status = ERROR;

    if ((pDisplay != NULL) && (pDisplay->fd > 0))
        status = semTake(pDisplay->lock, WAIT_FOREVER);

    return (status);
    }

/*******************************************************************************
 *
 * qcompare - compare the 2 configurations
 *
 * RETURNS: An integral value that is negative, null or positive if the first
 *          configuration is, respectively, less than, equal to or greater than
 *          the second one
 */
LOCAL int qcompare
    (
    const void * pValue1,
    const void * pValue2
    )
    {
    int value1, value2;
    EGLint * criteria = NULL;
    FB_CONFIG ** ppConfig1 = NULL;
    FB_CONFIG ** ppConfig2 = NULL;

    ppConfig1 = (FB_CONFIG **)pValue1;
    ppConfig2 = (FB_CONFIG **)pValue2;

    GET_GC();
    if (pGc == NULL)
        {
        /* error */
        return 0;
        }

    criteria = pGc->egl.criteria;

    /* Sort according to the "Sort Priority" */
    /* - 1 */
    value1 = fbGetCaveatOrder(*ppConfig1);
    value2 = fbGetCaveatOrder(*ppConfig2);
    if (value1 != value2)
        goto zz;
    /* - 2 */
    value1 = fbGetBufferTypeOrder(*ppConfig1);
    value2 = fbGetBufferTypeOrder(*ppConfig2);
    if (value1 != value2)
        goto zz;
    /* - 3 */
    value1 = fbGetColorBitsOrder(*ppConfig1, criteria);
    value2 = fbGetColorBitsOrder(*ppConfig2, criteria);
    if (value1 != value2)
        goto zz;
    /* - 4 */
    value1 = fbGetAttrib(*ppConfig1, EGL_BUFFER_SIZE);
    value2 = fbGetAttrib(*ppConfig2, EGL_BUFFER_SIZE);
    if (value1 != value2)
        goto zz;
    /* - 5 */
    value1 = fbGetAttrib(*ppConfig1, EGL_SAMPLE_BUFFERS);
    value2 = fbGetAttrib(*ppConfig2, EGL_SAMPLE_BUFFERS);
    if (value1 != value2)
        goto zz;
    /* - 6 */
    value1 = fbGetAttrib(*ppConfig1, EGL_SAMPLES);
    value2 = fbGetAttrib(*ppConfig2, EGL_SAMPLES);
    if (value1 != value2)
        goto zz;
    /* 7 */
    value1 = fbGetAttrib(*ppConfig1, EGL_DEPTH_SIZE);
    value2 = fbGetAttrib(*ppConfig2, EGL_DEPTH_SIZE);
    if (value1 != value2)
        goto zz;
    /* - 8 */
    value1 = fbGetAttrib(*ppConfig1, EGL_STENCIL_SIZE);
    value2 = fbGetAttrib(*ppConfig2, EGL_STENCIL_SIZE);
    if (value1 != value2)
        goto zz;
    /* - 9 */
    value1 = fbGetAttrib(*ppConfig1, EGL_ALPHA_MASK_SIZE);
    value2 = fbGetAttrib(*ppConfig2, EGL_ALPHA_MASK_SIZE);
    if (value1 != value2)
        goto zz;
    /* - 10 */
    /* :TODO: Figure out a way to order native visual types */
    /* - 11 */
    value1 = fbGetAttrib(*ppConfig1, EGL_CONFIG_ID);
    value2 = fbGetAttrib(*ppConfig2, EGL_CONFIG_ID);

zz: return (value1 - value2);
    }

/*******************************************************************************
 *
 * unbindContext - unbind the specified context
 */
LOCAL void unbindContext
    (
    context_t* pContext
    )
    {
    pContext->refCount--;
    if (pContext->refCount == 0)
        {
        pContext->threadId = INVALID_THREAD_ID;
        if (pContext->deletePending)
            deleteContext(pContext);
        }
    }

/*******************************************************************************
 *
 * unbindSurface - unbind the specified surface
 */
LOCAL void unbindSurface
    (
    surface_t* pSurface
    )
    {
    if (pSurface != NULL)
        {
        pSurface->refCount--;
        if (pSurface->refCount == 0)
            {
            pSurface->threadId = INVALID_THREAD_ID;
            if (pSurface->deletePending)
                deleteSurface(pSurface);
            }
        }
    }

/*******************************************************************************
 *
 * unlockEgl - enable thread preemption
 */
LOCAL void unlockEgl(void)
    {
#if defined(_WRS_KERNEL)
    SPIN_LOCK_TASK_GIVE(&spinlock);
#else
    pthread_mutex_unlock(&eglMutex);
#endif
    }

/*******************************************************************************
 *
 * unlockDisplay - unlock the specified display
 */
LOCAL void unlockDisplay(display_t* pDisplay)
    {
    assert(semGive(pDisplay->lock) == OK);
    }
