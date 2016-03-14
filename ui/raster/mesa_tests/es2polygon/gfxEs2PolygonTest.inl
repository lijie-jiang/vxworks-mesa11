/* gfxEs2PolygonTest.inl - OpenGL ES2 Polygon Test */

/*
 * Copyright (c) 2014-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24feb16,yat  Fix static analysis defects (US75033)
14sep15,yat  Add support for Mesa GPU DRI (US24710)
10sep15,yat  Add missing numConfigs check after eglChooseConfig (V7GFX-279)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
18sep14,yat  Add support for dynamic RTP demos (US40486)
04apr14,yat  Modified for VxWorks 7
*/

/*

DESCRIPTION

This program tests polygon.

*/

/* includes */

#if defined(__vxworks)
#include <vxWorks.h>
#include <ioLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <rtpLib.h>
#include <fbdev.h>
#if defined(_WRS_KERNEL)
#include <envLib.h>
#endif
#if defined(_FSLVIV)
#include <gc_hal_base.h>
#endif
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#if defined(GFX_USE_GBM)
#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#define GFX_KMS_GET_CONN_STR
#define GFX_KMS_FIND_CONN_CRTC
#define GFX_KMS_FIND_CRTC_FB
#include <gfxKmsHelper.inl>
#endif /* GFX_USE_GBM */
#include "subcube.h"
#if defined(GFX_USE_EVDEV)
#include <evdevLib.h>
#endif

/* defines */

#if defined(_FSLVIV)
/*
 * Non-WR EGL implementations may allow additional flexibility in
 * choosing a window size. If these are non-zero, the render surface is clamped.
 */
#define WIN_WIDTH       0
#define WIN_HEIGHT      0
#endif

#if !defined(GFX_USE_EGL_FBDEV)
#define GFX_USE_EGL_FBDEV 0
#define GFX_USE_EGL_DRI   1
#endif

/* typedefs */

/* Data structure that is attached to the window, provides all
 * data required to display and manage the program.
 */

typedef struct
    {
    EGLContext context;     /* EGL context */
    EGLDisplay display;     /* EGL display */
    EGLSurface surface;     /* EGL draw surface */
    EGLint surfWidth;       /* EGL surface width */
    EGLint surfHeight;      /* EGL surface height */
    EGLint surfBuffer;      /* EGL surface render buffer */
#if defined(_FSLVIV)
    EGLNativeDisplayType eglNativeDisplay;  /* Used for non-WR EGL */
    EGLNativeWindowType  eglNativeWindow;   /* Used for non-WR EGL */
#endif
#if defined(GFX_USE_GBM)
    int                  drmDevFd;
    struct gbm_device*   gbm_dev;
    struct gbm_surface*  gbm_surf;
    drmModeModeInfo      mode;
    uint32_t             connId;
    uint32_t             crtcId;
    uint32_t             fbId;
#endif /* GFX_USE_GBM */
#if defined(GFX_USE_EVDEV)
    int                  evDevFd;
#endif
    GLuint               vertexShader;
    GLuint               fragmentShader;
    GLuint               shaderProgram;
    GLuint               positionLoc;
    GLuint               texCoordLoc;
    GLuint               modelViewLoc;
    GLuint               projLoc;
    int                  countPolygon;
    long                 tRate0;
    long                 frames;
    long                 tRot0;
    GLfloat              angle;
    } ES2_POLYGON_SCENE;
#if 0
static const GLfloat g_verticesPolygon[] =
    {
    /* Top Right Of The Quad (Top) */
    1.0f, 1.0f,-1.0f,
    /* Top Left Of The Quad (Top) */
    -1.0f, 1.0f, -1.0f,
    /* Bottom Right Of The Quad (Top) */
    1.0f,1.0f,1.0f,
    /* Bottom Left Of The Quad (Top) */
    -1.0f,1.0f,1.0f,
    /* Top Right Of The Quad (Bottom) */
    1.0f,-1.0f,1.0f,
    /* Top Left Of The Quad (Bottom) */
    -1.0f, -1.0f,1.0f,
    /* Bottom Right Of The Quad (Bottom) */
    1.0f,-1.0f,-1.0f,
    /* Bottom Left Of The Quad (Bottom) */
    -1.0f,-1.0f,-1.0f,
    /* Top Right Of The Quad (Front) */
    1.0f,1.0f,1.0f,
    /* Top Left Of The Quad (Front) */
    -1.0f,1.0f,1.0f,
    /* Bottom Right Of The Quad (Front) */
    1.0f,-1.0f,1.0f,
    /* Bottom Left Of The Quad (Front) */
    -1.0f,-1.0f,1.0f,
    /* Top Right Of The Quad (Back) */
    1.0f,-1.0f,-1.0f,
    /* Top Left Of The Quad (Back) */
    -1.0f,-1.0f,-1.0f,
    /* Bottom Right Of The Quad (Back) */
    1.0f, 1.0f, -1.0f,
    /* Bottom Left Of The Quad (Back) */
    -1.0f,1.0f,-1.0f,
    /* Top Right Of The Quad (Left) */
    -1.0f,1.0f,1.0f,
    /* Top Left Of The Quad (Left) */
    -1.0f,1.0f, -1.0f,
    /* Bottom Right Of The Quad (Left) */
    -1.0f,-1.0f,1.0f,
    /* Bottom Left Of The Quad (Left) */
    -1.0f,-1.0f,-1.0f,
    /* Top Right Of The Quad (Right) */
    1.0f,1.0f,-1.0f,
    /* Top Left Of The Quad (Right) */
    1.0f,1.0f,1.0f,
    /* Bottom Right Of The Quad (Right) */
    1.0f,-1.0f,-1.0f,
    /* Bottom Left Of The Quad (Right) */
    1.0f,-1.0f,1.0f
    };
#endif
static const GLfloat g_texturesPolygon[] =
    {
    /* Top Face */
    0.0f,0.0f,
    1.0f,0.0f,
    0.0f,1.0f,
    1.0f,1.0f,
    /* Bottom Face */
    1.0f,1.0f,
    0.0f,1.0f,
    0.0f,0.0f,
    1.0f,0.0f,
    /* Front Face */
    1.0f,1.0f,
    0.0f,1.0f,
    1.0f,0.0f,
    0.0f,0.0f,
    /* Back Face */
    0.0f,0.0f,
    1.0f,0.0f,
    0.0f,1.0f,
    1.0f,1.0f,
    /*left face*/
    1.0f,1.0f,
    0.0f,1.0f,
    1.0f,0.0f,
    0.0f,0.0f,
    /* Right face */
    1.0f,1.0f,
    0.0f,1.0f,
    1.0f,0.0f,
    0.0f,0.0f,
    };

/**************************************************************************
 *
 * es2PolygonVertexShader - vector shader program
 */
static const char *es2PolygonVertexShader =
    "attribute vec4 position;\n"
    "attribute vec2 texcoord;\n"
    "varying   vec2 tmp;\n"
    "uniform   mat4 modelview;\n"
    "uniform   mat4 proj;\n"
    "void main ()\n"
    "{\n"
    "    vec4 vPositionES = modelview * position;\n"
    "    gl_Position  = proj * vPositionES;\n"
    "    tmp = texcoord;\n"
    "}\n";

/**************************************************************************
 *
 * es2PolygonFragmentShader - fragment shader program
 */
static const char *es2PolygonFragmentShader =
    "precision highp float;\n"
    "uniform sampler2D texture;\n"
    "varying vec2 tmp;\n"
    "void main ()\n"
    "{\n"
    "    gl_FragColor = texture2D(texture,tmp);\n"
    "}\n";

/*******************************************************************************
 *
 * es2PolygonPrint - print the configuration of the graphics library
 *
 * This routine prints the configuration for the EGL and OpenGL libraries.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void es2PolygonPrint
    (
    ES2_POLYGON_SCENE* pDemo,
    EGLint eglMajor,
    EGLint eglMinor
    )
    {
#if defined(__vxworks)
#if defined(_MESA)
    (void)printf ("EGL_DRIVER:%s\n", getenv("EGL_DRIVER"));
#endif
#endif /* __vxworks */
    (void)printf ("EGL API version: %d.%d\n", eglMajor, eglMinor);
    (void)printf ("EGL_VERSION: %s\n", eglQueryString (pDemo->display, EGL_VERSION));
    (void)printf ("EGL_VENDOR: %s\n", eglQueryString (pDemo->display, EGL_VENDOR));
    (void)printf ("EGL_CLIENT_APIS: %s\n", eglQueryString (pDemo->display, EGL_CLIENT_APIS));
    (void)printf ("EGL_EXTENSIONS: %s\n", eglQueryString (pDemo->display, EGL_EXTENSIONS));
    (void)printf ("EGL_WIDTH: %d\n", pDemo->surfWidth);
    (void)printf ("EGL_HEIGHT: %d\n", pDemo->surfHeight);
    (void)printf ("EGL_RENDER_BUFFER: 0x%x(%s)\n", pDemo->surfBuffer,
                  (pDemo->surfBuffer == EGL_BACK_BUFFER) ?
                  "EGL_BACK_BUFFER" : "EGL_SINGLE_BUFFER");
    (void)printf ("GL_RENDERER: %s\n", (char *) glGetString (GL_RENDERER));
    (void)printf ("GL_VERSION: %s\n", (char *) glGetString (GL_VERSION));
    (void)printf ("GL_VENDOR: %s\n", (char *) glGetString (GL_VENDOR));
    (void)printf ("GL_EXTENSIONS: %s\n", (char *) glGetString (GL_EXTENSIONS));
    }

/*******************************************************************************
 *
 * es2PolygonCleanup - perform final cleanup
 *
 * This routine performs final cleanup
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2PolygonEglDeinit
 *
 */
static void es2PolygonCleanup
    (
    ES2_POLYGON_SCENE* pDemo
    )
    {
#if defined(GFX_USE_EVDEV)
    if (pDemo->evDevFd > 0)
        {
        (void)close (pDemo->evDevFd);
        }
#endif
    free (pDemo);
    }

/*******************************************************************************
 *
 * es2PolygonEglDeinit - perform necessary steps to deinitialize
 *
 * This routine takes care of all the steps involved in cleaning-up EGL
 * resources. The EGL resources must be provided by the ES2_POLYGON_SCENE parameter
 * <pDemo>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2PolygonEglInit
 *
 */
static void es2PolygonEglDeinit
    (
    ES2_POLYGON_SCENE* pDemo
    )
    {
    glFinish ();
    if (pDemo->display != EGL_NO_DISPLAY)
        {
        /* Release the rendering context */
        if (pDemo->context != EGL_NO_CONTEXT)
            {
            (void)eglDestroyContext (pDemo->display, pDemo->context);
            }

        /* Release the surface */
        if (pDemo->surface != EGL_NO_SURFACE)
            {
            (void)eglDestroySurface (pDemo->display, pDemo->surface);
            }

#if defined(GFX_USE_GBM)
        if (pDemo->gbm_surf)
            {
            gbm_surface_destroy (pDemo->gbm_surf);
            }
#endif /* GFX_USE_GBM */

        /* Close the display */
        (void)eglMakeCurrent (pDemo->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        (void)eglTerminate (pDemo->display);
        }
    (void)eglReleaseThread ();

#if defined(_FSLVIV)
    /* Release non-WR EGL resources */
    fbDestroyWindow (pDemo->eglNativeWindow);
    fbDestroyDisplay (pDemo->eglNativeDisplay);
#endif
#if defined(GFX_USE_GBM)
    gfxKmsCloseDrmDev (pDemo->drmDevFd);
#endif /* GFX_USE_GBM */

    es2PolygonCleanup (pDemo);
    }

#if defined(GFX_USE_GBM)
/*******************************************************************************
*
* es2PolygonFindDrmFb - find a DRM framebuffer
*
* This routine finds a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int es2PolygonFindDrmFb
    (
    ES2_POLYGON_SCENE* pDemo
    )
    {
    drmModeCrtc*         crtc;
    drmModeFB*           fb;
    drmModeConnector*    conn;

    if (gfxKmsFindCrtcFb (pDemo->drmDevFd, &crtc, &fb))
        {
        (void)fprintf (stderr, "No crtc found\n");
        return 1;
        }

    if (gfxKmsFindConnCrtc (pDemo->drmDevFd, crtc, &conn))
        {
        (void)fprintf (stderr, "No connector found\n");
        drmModeFreeCrtc (crtc);
        return 1;
        }

    if (!fb)
        {
        /* Use the first available mode */
        crtc->mode = conn->modes[0];
        }

    (void)fprintf (stderr, "Found %s %s %dx%d\n",
                   gfxKmsGetConnModeStr(DRM_MODE_CONNECTED),
                   gfxKmsGetConnTypeStr (conn->connector_type),
                   crtc->mode.hdisplay, crtc->mode.vdisplay);

    pDemo->mode = crtc->mode;
    pDemo->connId = conn->connector_id;
    pDemo->crtcId = crtc->crtc_id;
    drmModeFreeCrtc (crtc);
    drmModeFreeConnector (conn);

    return 0;
    }
#endif /* GFX_USE_GBM */

/*******************************************************************************
 *
 * es2PolygonEglInit - perform necessary steps to initialize
 *
 * This routine takes care of all the steps involved in initializing EGL,
 * creating a surface, and setting up the program for rendering.
 *
 * RETURNS: pointer to ES2_POLYGON_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2PolygonEglDeinit
 *
 */
static ES2_POLYGON_SCENE* es2PolygonEglInit
    (
    unsigned int device,
    unsigned int eglDriver
    )
    {
    ES2_POLYGON_SCENE* pDemo;
    GLboolean   printInfo = GL_TRUE;
    EGLint      eglMajor, eglMinor;
    EGLint      configAttribs[] =
        {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BUFFER_SIZE, 32,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
        };
    EGLint      contextAttribs[] =
        {
        EGL_CONTEXT_CLIENT_VERSION,
        2,
        EGL_NONE
        };
    EGLConfig eglConfig;                        /* EGL configuration */
    EGLint numConfigs = 0;                      /* Number of EGL configurations */
#if defined(_FSLVIV)
    EGLint displayWidth = 0;                    /* Display width */
    EGLint displayHeight = 0;                   /* Display height */
    EGLint reservedWidth = 0;                   /* Limit for surface width */
    EGLint reservedHeight = 0;                  /* Limit for surface height */
    unsigned int i;
#elif defined(_TIIMGT)
#elif defined(_MESA)
#else
    int fd;
    char deviceName[FB_MAX_STR_CHARS];
    unsigned int i;
#endif
#if defined(GFX_USE_EVDEV)
    char evDevName[EV_DEV_NAME_LEN + 1];
    unsigned int e;
#endif
    /* Allocate basic structure to carry demo data */
    pDemo = (ES2_POLYGON_SCENE*)calloc (1, sizeof (ES2_POLYGON_SCENE));
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "calloc failed\n");
        return (NULL);
        }

    pDemo->tRate0 = -1;
    pDemo->tRot0 = -1;
#if defined(__vxworks)
#if defined(_MESA)
    switch (eglDriver)
        {
        case GFX_USE_EGL_DRI:
            putenv ("EGL_DRIVER=egl_dri2");
            break;
        default:
            putenv ("EGL_DRIVER=egl_fbdev");
            break;
        }
#endif
#endif /* __vxworks */
    /* EGL Initialization */

    /*
     * EGL Initialization Step 1:  Initialize display
     *
     * The following lines take care of opening the default display
     * (typically /dev/fb0) and setting up the OpenGL library.
     */
#if defined(_FSLVIV)
    /* Use vendor-specific display routine for non-WindRiver EGL */
    for (i = device; i < FB_MAX_DEVICE; i++)
        {
        pDemo->eglNativeDisplay = (EGLNativeDisplayType) fbGetDisplayByIndex (i);
        if (pDemo->eglNativeDisplay)
            {
            break;
            }
        }
    if (i >= FB_MAX_DEVICE)
        {
        free (pDemo);
        (void)fprintf (stderr, "fbGetDisplayByIndex failed\n");
        return (NULL);
        }
    fbGetDisplayGeometry (pDemo->eglNativeDisplay, &displayWidth, &displayHeight);

    reservedWidth = WIN_WIDTH ? WIN_WIDTH : displayWidth;
    reservedHeight = WIN_HEIGHT ? WIN_HEIGHT : displayHeight;
    pDemo->display = eglGetDisplay ((EGLNativeDisplayType) pDemo->eglNativeDisplay);
#elif defined(_TIIMGT)
    pDemo->display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
#elif defined(_MESA)
#if defined(GFX_USE_GBM)
    if (eglDriver == GFX_USE_EGL_FBDEV)
        {
        /* Mesa EGL FBDEV driver */
        pDemo->display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
        }
    else
        {
        /* Open a DRM device to use Mesa GPU DRI driver */
        if (gfxKmsOpenDrmDev (&(pDemo->drmDevFd)))
            {
            free (pDemo);
            return (NULL);
            }

        pDemo->gbm_dev = gbm_create_device (pDemo->drmDevFd);
        if (pDemo->gbm_dev == NULL)
            {
            (void)fprintf (stderr, "gbm_create_device failed\n");
            es2PolygonEglDeinit (pDemo);
            return (NULL);
            }

        pDemo->display = eglGetDisplay ((EGLNativeDisplayType)pDemo->gbm_dev);
        }
#else
    pDemo->display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
#endif /* GFX_USE_GBM */
#else
    for (i = device; i < FB_MAX_DEVICE; i++)
        {
        (void)snprintf (deviceName, FB_MAX_STR_CHARS, "%s%d", FB_DEVICE_PREFIX, i);
        if ((fd = open (deviceName, O_RDWR, 0666)) != -1)
            {
            break;
            }
        }
    if (i >= FB_MAX_DEVICE)
        {
        (void)fprintf (stderr, "Open device %s errno:%08x\n", deviceName, errno);
        free (pDemo);
        return (NULL);
        }
    (void)close (fd);
    pDemo->display = eglGetDisplay ((EGLNativeDisplayType) deviceName);
#endif
    if (pDemo->display == EGL_NO_DISPLAY)
        {
        (void)fprintf (stderr, "eglGetDisplay failed - possibly driver is not compatible\n");
        es2PolygonEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglInitialize (pDemo->display, &eglMajor, &eglMinor))
        {
        (void)fprintf (stderr, "eglInitialize failed\n");
        es2PolygonEglDeinit (pDemo);
        return (NULL);
        }
#if defined(GFX_USE_GBM)
    if (pDemo->gbm_dev)
        {
        if (es2PolygonFindDrmFb (pDemo))
            {
            es2PolygonEglDeinit (pDemo);
            return (NULL);
            }
        }
#endif /* GFX_USE_GBM */
    if (!eglBindAPI (EGL_OPENGL_ES_API))
        {
        (void)fprintf (stderr, "eglBindAPI failed\n");
        es2PolygonEglDeinit (pDemo);
        return (NULL);
        }

    /*
     * EGL Initialization Step 2: Choose an EGL configuration
     * Query the OpenGL ES library (and fbdev driver) to find a compatible
     * configuration.
     */
    if (!eglChooseConfig (pDemo->display, configAttribs, &eglConfig, 1, &numConfigs) || (numConfigs <= 0))
        {
        (void)fprintf (stderr, "Couldn't get an EGL visual config\n");
        es2PolygonEglDeinit (pDemo);
        return (NULL);
        }

    /*
     * EGL Initialization Step 3: Create the back buffer
     * Create a window surface using the selected configuration and query its
     * dimensions
     */
#if defined(_FSLVIV)
    /* Use vendor-specific window creation routine for non-WindRiver EGL */
    pDemo->eglNativeWindow = (EGLNativeWindowType) fbCreateWindow (pDemo->eglNativeDisplay,
                                                  (displayWidth - reservedWidth) / 2,
                                                  (displayHeight - reservedHeight) / 2,
                                                  reservedWidth, reservedHeight);

    pDemo->surface = eglCreateWindowSurface (pDemo->display, eglConfig,
                                             pDemo->eglNativeWindow, NULL);
#elif defined(GFX_USE_GBM)
    if (pDemo->gbm_dev)
        {
        pDemo->gbm_surf = gbm_surface_create (pDemo->gbm_dev,
                                              pDemo->mode.hdisplay,
                                              pDemo->mode.vdisplay,
                                              GBM_BO_FORMAT_XRGB8888,
                          GBM_BO_USE_RENDERING|GBM_BO_USE_SCANOUT);
        if (pDemo->gbm_surf == NULL)
            {
            (void)fprintf (stderr, "gbm_surface_create failed\n");
            es2PolygonEglDeinit (pDemo);
            return (NULL);
            }
        }
    else
        {
        pDemo->gbm_surf = NULL;
        }
    pDemo->surface = eglCreateWindowSurface (pDemo->display, eglConfig,
                        (EGLNativeWindowType)pDemo->gbm_surf, NULL);
#else
    pDemo->surface = eglCreateWindowSurface (pDemo->display, eglConfig,
                                             NULL, NULL);
#endif
    if (pDemo->surface == EGL_NO_SURFACE)
        {
        (void)fprintf (stderr, "eglCreateWindowSurface failed\n");
        es2PolygonEglDeinit (pDemo);
        return (NULL);
        }
    (void)eglQuerySurface (pDemo->display, pDemo->surface,
                           EGL_WIDTH, &(pDemo->surfWidth));
    (void)eglQuerySurface (pDemo->display, pDemo->surface,
                           EGL_HEIGHT, &(pDemo->surfHeight));
    (void)eglQuerySurface (pDemo->display, pDemo->surface,
                           EGL_RENDER_BUFFER, &(pDemo->surfBuffer));

    /*
     * EGL Initialization Step 4: Setup the context
     * Create a rendering context and set it as the current context being used
     */
    pDemo->context = eglCreateContext (pDemo->display, eglConfig,
                                       EGL_NO_CONTEXT, contextAttribs);
    if (pDemo->context == EGL_NO_CONTEXT)
        {
        (void)fprintf (stderr, "eglCreateContext failed\n");
        es2PolygonEglDeinit (pDemo);
        return (NULL);
        }

    /* Set the context as current */
    if (!eglMakeCurrent (pDemo->display, pDemo->surface, pDemo->surface, pDemo->context))
        {
        (void)fprintf (stderr, "eglMakeCurrent failed\n");
        es2PolygonEglDeinit (pDemo);
        return (NULL);
        }

    /* Print configuration info, if requested */
    if (printInfo)
        {
        es2PolygonPrint (pDemo, eglMajor, eglMinor);
        }

    (void)eglSwapInterval (pDemo->display, 1);

    /*
     * EGL Initialization Step 5: Clear screen
     * Clear the front, back, third buffers to begin with a clean slate
     */
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glClear (GL_COLOR_BUFFER_BIT);
    (void)eglSwapBuffers (pDemo->display, pDemo->surface);
    glClear (GL_COLOR_BUFFER_BIT);
    (void)eglSwapBuffers (pDemo->display, pDemo->surface);
    glClear (GL_COLOR_BUFFER_BIT);
    (void)eglSwapBuffers (pDemo->display, pDemo->surface);
#if defined(GFX_USE_GBM)
    if (pDemo->gbm_dev)
        {
        struct gbm_bo* gbm_bo;
        uint32_t handle;

        gbm_bo = gbm_surface_lock_front_buffer (pDemo->gbm_surf);
        if (gbm_bo == NULL)
            {
            (void)fprintf (stderr, "gbm_surface_lock_front_buffer failed\n");
            es2PolygonEglDeinit (pDemo);
            return (NULL);
            }

        handle = gbm_bo_get_handle (gbm_bo).u32;
        if (handle == 0)
            {
            (void)fprintf (stderr, "Bad handle\n");
            es2PolygonEglDeinit (pDemo);
            return (NULL);
            }

        /* Add FB. Note: pitch can be at least 512 byte aligned in DRM/I915 */
        if (drmModeAddFB (pDemo->drmDevFd,
                          pDemo->mode.hdisplay, pDemo->mode.vdisplay,
                          24, 32, (((pDemo->mode.hdisplay << 2) + 511) & ~511),
                          handle, &(pDemo->fbId)))
            {
            (void)fprintf (stderr, "drmModeAddFB failed\n");
            es2PolygonEglDeinit (pDemo);
            return (NULL);
            }

        if (drmModeSetCrtc (pDemo->drmDevFd, pDemo->crtcId, pDemo->fbId,
                            0, 0, &(pDemo->connId), 1, &(pDemo->mode)))
            {
            es2PolygonEglDeinit (pDemo);
            return (NULL);
            }

        gbm_surface_release_buffer (pDemo->gbm_surf, gbm_bo);
        }
#endif /* GFX_USE_GBM */
#if defined(GFX_USE_EVDEV)
    /* Open evdev device */
    for (e = 0; e < EV_DEV_DEVICE_MAX; e++)
        {
        (void)snprintf (evDevName, EV_DEV_NAME_LEN, "%s%d", EV_DEV_NAME_PREFIX, e);
        if ((pDemo->evDevFd = open (evDevName, O_RDONLY|O_NONBLOCK, 0)) != -1)
            {
            break;
            }
        }
    if (e >= EV_DEV_DEVICE_MAX)
        {
        (void)fprintf (stderr, "Open device %s errno:%08x\n", evDevName, errno);
        }
    else
        {
        int evMsgCount = 0;
        EV_DEV_EVENT evDevEvent;

        /* Clear event */
        (void) ioctl (pDemo->evDevFd, FIONREAD, &evMsgCount);
        while (evMsgCount >= sizeof (EV_DEV_EVENT))
            {
            if (read (pDemo->evDevFd, (char *)&evDevEvent, sizeof (EV_DEV_EVENT)) < sizeof (EV_DEV_EVENT))
                {
                break;
                }
            (void) ioctl (pDemo->evDevFd, FIONREAD, &evMsgCount);
            }
        }
#endif
    return (pDemo);
    }

/*******************************************************************************
 *
 * es2PolygonDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2PolygonInit
 *
 */
static void es2PolygonDeinit
    (
    ES2_POLYGON_SCENE* pDemo
    )
    {
    glDisableVertexAttribArray (pDemo->positionLoc);
    glDisableVertexAttribArray (pDemo->texCoordLoc);

    if (pDemo->shaderProgram)
        {
        glDeleteShader (pDemo->vertexShader);
        glDeleteShader (pDemo->fragmentShader);
        glDeleteProgram (pDemo->shaderProgram);
        glUseProgram (0);
        }
    }

/*******************************************************************************
 *
 * es2PolygonInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: es2PolygonDeinit
 *
 */
static int es2PolygonInit
    (
    ES2_POLYGON_SCENE* pDemo
    )
    {
    (void)printf ("Compiling vertex shader\n");

    /* Compile the shaders*/
    pDemo->vertexShader = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (pDemo->vertexShader, 1, &es2PolygonVertexShader, NULL);
    glCompileShader (pDemo->vertexShader);

    /* Check for compile success*/
    GLint nCompileResult = 0;
    glGetShaderiv (pDemo->vertexShader, GL_COMPILE_STATUS, &nCompileResult);
    if (0 == nCompileResult)
        {
        char  strLog[1024];
        GLint nLength;
        glGetShaderInfoLog (pDemo->vertexShader, 1023, &nLength, strLog);
        strLog[nLength] = '\0';
        (void)fprintf (stderr, "%s\n", strLog);
        return 1;
        }

    (void)printf ("Compiling fragment shader\n");

    pDemo->fragmentShader = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (pDemo->fragmentShader, 1, &es2PolygonFragmentShader, NULL);
    glCompileShader (pDemo->fragmentShader);

    /* Check for compile success*/
    glGetShaderiv (pDemo->fragmentShader, GL_COMPILE_STATUS, &nCompileResult);
    if (0 == nCompileResult)
        {
        char  strLog[1024];
        GLint nLength;
        glGetShaderInfoLog (pDemo->fragmentShader, 1023, &nLength, strLog);
        strLog[nLength] = '\0';
        (void)fprintf (stderr, "%s\n", strLog);
        return 1;
        }

    /* Attach the individual shaders to the common shader program*/
    pDemo->shaderProgram = glCreateProgram ();
    glAttachShader (pDemo->shaderProgram, pDemo->vertexShader);
    glAttachShader (pDemo->shaderProgram, pDemo->fragmentShader);

    (void)printf ("Linking shader\n");

    /* Link the vertex shader and fragment shader together*/
    glLinkProgram (pDemo->shaderProgram);

    /* Check for link success*/
    GLint nLinkResult = 0;
    glGetProgramiv (pDemo->shaderProgram, GL_LINK_STATUS, &nLinkResult);
    if (0 == nLinkResult)
        {
        char strLog[1024];
        GLint nLength;
        glGetProgramInfoLog (pDemo->shaderProgram, 1023, &nLength, strLog);
        strLog[nLength] = '\0';
        (void)fprintf (stderr, "%s\n", strLog);
        return 1;
        }

    /* Set the shader program*/
    glUseProgram (pDemo->shaderProgram);

    /* Grab attributes locations */
    pDemo->positionLoc = glGetAttribLocation (pDemo->shaderProgram, "position");
    pDemo->texCoordLoc = glGetAttribLocation (pDemo->shaderProgram, "texcoord");

    /* Get uniform locations */
    pDemo->modelViewLoc = glGetUniformLocation (pDemo->shaderProgram, "modelview");
    pDemo->projLoc = glGetUniformLocation (pDemo->shaderProgram, "proj");

    /* Set data in arrays */
    glVertexAttribPointer (pDemo->positionLoc, 3, GL_FLOAT, GL_FALSE,
        0, g_verticesPolygon);
    glVertexAttribPointer (pDemo->texCoordLoc, 2, GL_FLOAT, GL_FALSE,
        0, g_texturesPolygon);

    /* Enable vertex arrays */
    glEnableVertexAttribArray (pDemo->positionLoc);
    glEnableVertexAttribArray (pDemo->texCoordLoc);

    pDemo->angle = 360.0f;

    /* Set the background color */
    glClearColor (0.4f, 0.6f, 0.9f, 0.0f);

    /* Enables Depth Testing */
    glEnable (GL_DEPTH_TEST);

    /* The Type Of Depth Test To Do */
    glDepthFunc (GL_LESS);

    glViewport (0, 0, (GLint) pDemo->surfWidth, (GLint) pDemo->surfHeight);

    return 0;
    }

/*******************************************************************************
 *
 * es2PolygonRender - render the application
 *
 * This routine renders the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static int es2PolygonRender
    (
    ES2_POLYGON_SCENE* pDemo
    )
    {
    GLfloat matModelView[16];
    GLfloat matProj[16];

    /* Rotate and translate the model view matrix*/
    matModelView[ 0] = +(GLfloat)cos ((double)pDemo->angle);
    matModelView[ 1] = 0.0f;
    matModelView[ 2] = +(GLfloat)sin ((double)pDemo->angle);
    matModelView[ 3] = 0.0f;
    matModelView[ 4] = 0.0f;
    matModelView[ 5] = 1.0f;
    matModelView[ 6] = 0.0f;
    matModelView[ 7] = 0.0f;
    matModelView[ 8] = -(GLfloat)sin ((double)pDemo->angle);
    matModelView[ 9] = 0.0f;
    matModelView[10] = +(GLfloat)cos ((double)pDemo->angle);
    matModelView[11] = 0.0f;
    matModelView[12] = 0.0f; /*X*/
    matModelView[13] = 0.0f;
    matModelView[14] = -5.0f; /*z*/
    matModelView[15] = 1.0f;

    /* Build a perspective projection matrix*/
    matProj[ 0] = (GLfloat)cos ((double)0.5f) / (GLfloat)sin ((double)0.5f);
    matProj[ 1] = 0.0f;
    matProj[ 2] = 0.0f;
    matProj[ 3] = 0.0f;
    matProj[ 4] = 0.0f;
    matProj[ 5] = matProj[0] * ((GLfloat)pDemo->surfWidth/(GLfloat)pDemo->surfHeight) ;
    matProj[ 6] = 0.0f;
    matProj[ 7] = 0.0f;
    matProj[ 8] = 0.0f;
    matProj[ 9] = 0.0f;
    matProj[10] = -(10.0f) / (9.0f);
    matProj[11] = -1.0f;
    matProj[12] = 0.0f;
    matProj[13] = 0.0f;
    matProj[14] = -(10.0f) / (9.0f);
    matProj[15] = 0.0f;

    /* Clear The Screen And The Depth Buffer */
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Set data in arrays */
    glVertexAttribPointer (pDemo->positionLoc, 3, GL_FLOAT, GL_FALSE,
        0, g_verticesPolygon);
    glVertexAttribPointer (pDemo->texCoordLoc, 2, GL_FLOAT, GL_FALSE,
        0, g_texturesPolygon);
    glUniformMatrix4fv (pDemo->modelViewLoc, 1, GL_FALSE, matModelView);
    glUniformMatrix4fv (pDemo->projLoc, 1, GL_FALSE, matProj);
#if 0
    /* Drawing Using Triangle strips, draw triangle strips using 4 vertices */
    glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays (GL_TRIANGLE_STRIP, 4, 4);
    glDrawArrays (GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays (GL_TRIANGLE_STRIP, 12, 4);
    glDrawArrays (GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays (GL_TRIANGLE_STRIP, 20, 4);
#else
    glDrawArrays (GL_TRIANGLE_STRIP, 0, pDemo->countPolygon);
#endif
    return 0;
    }

/*******************************************************************************
 *
 * es2PolygonPerfCount - calculate and show performance
 *
 * This routine calculates and displays the number of frames rendered every
 * 5 seconds.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void es2PolygonPerfCount
    (
    ES2_POLYGON_SCENE* pDemo
    )
    {
    long dt;
    long t;
    struct timeval curtime;

    (void)gettimeofday (&curtime, NULL);
    t = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000);

    if (pDemo->tRot0 < 0)
        {
        pDemo->tRot0 = t;
        }
    dt = t - pDemo->tRot0;
    pDemo->tRot0 = t;

    pDemo->angle -= 1.0f * (GLfloat)dt / 1000.0f;  /* 1 degrees per second */
    if (pDemo->angle < 0.0f)
        {
        pDemo->angle += 360.0f;
        }

    pDemo->frames++;
    if (pDemo->tRate0 < 0)
        {
        pDemo->tRate0 = t;
        pDemo->frames = 0;
        }
    if ((t - pDemo->tRate0) > 5000)
        {
        long seconds = (t - pDemo->tRate0) / 1000;
        long fps = pDemo->frames / seconds;
        (void)fprintf (stdout, "%ld frames in %ld seconds = %ld FPS\n",
                         pDemo->frames, seconds, fps);
        pDemo->tRate0 = t;
        pDemo->frames = 0;
#if defined(_FSLVIV)
        gctSIZE_T contiguousSize = 0;
        gctPHYS_ADDR contiguousAddress = NULL;
        if (gcvSTATUS_OK == gcoHAL_QueryVideoMemory(
                                gcvNULL, gcvNULL, gcvNULL, gcvNULL, gcvNULL,
                                &contiguousAddress, &contiguousSize))
            {
            (void)fprintf (stdout, "contiguousAddress 0x%08x contiguousSize 0x%08x:%d\n",
                           contiguousAddress, contiguousSize, contiguousSize);
            }
#endif
        }
    }

#if defined(GFX_USE_EVDEV)
/*******************************************************************************
 *
 * es2PolygonGetChar - gets an ASCII character from keyboard
 *
 * This routine gets an ASCII character from keyboard
 *
 * RETURNS: Ascii character, or NULL
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static unsigned short es2PolygonGetChar
    (
    int evDevFd
    )
    {
    fd_set readFds;
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    FD_ZERO (&readFds);
    FD_SET (evDevFd, &readFds);
    while ((select (evDevFd+1, &readFds, NULL, NULL, &timeout) > 0) &&
           FD_ISSET (evDevFd, &readFds))
        {
        EV_DEV_EVENT evDevEvent;
        ssize_t evDevCount;
        evDevCount = read (evDevFd, (char *)&evDevEvent, sizeof (EV_DEV_EVENT));
        if (evDevCount == sizeof (EV_DEV_EVENT))
            {
            if (evDevEvent.type == EV_DEV_KEY)
                {
                (void)printf ("Key [%04x] [%08x]\n", evDevEvent.code, evDevEvent.value);
                return (unsigned short)((evDevEvent.value)? evDevEvent.code:0);
                }
            }
        }

    return 0;
    }
#endif

/*******************************************************************************
 *
 * gfxEs2PolygonTest - core functionality of the program
 *
 * This routine contains the core functionality for the program.
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static int gfxEs2PolygonTest
    (
    unsigned int device,
    int runTime,
    unsigned int eglDriver,
    int countPolygon
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
    ES2_POLYGON_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                                /* terminate flag */
#endif
    /* Initialize demo */
    pDemo = es2PolygonEglInit (device, eglDriver);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "es2PolygonEglInit failed\n");
        return (EXIT_FAILURE);
        }

    pDemo->countPolygon = countPolygon;
    if ((pDemo->countPolygon <= 0) || pDemo->countPolygon > 6000)
        {
        pDemo->countPolygon = 6000;
        }
    (void)printf ("Polygon count %d\n", pDemo->countPolygon);

    /* Initialize demo */
    if (es2PolygonInit (pDemo))
        {
        (void)fprintf (stderr, "es2PolygonInit failed\n");
        es2PolygonDeinit (pDemo);
        es2PolygonEglDeinit (pDemo);
        return (EXIT_FAILURE);
        }

    runTime = runTime * 1000;
    (void)gettimeofday (&curtime, NULL);
    startTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000);
    diffTime = 0;
    /* Run the program so long as it hasn't reached its end time */
    while (((diffTime < runTime)||(runTime == 0))
#if defined(GFX_USE_EVDEV)
        && (kbdQuit == 0)
#endif
        )
        {
#if defined(GFX_USE_EVDEV)
        /* Grab a character from keyboard */
        if (pDemo->evDevFd > 0)
            {
            unsigned short character = es2PolygonGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        es2PolygonPerfCount (pDemo);

        if (es2PolygonRender (pDemo))
            {
            (void)fprintf (stderr, "es2PolygonRender failed\n");
            break;
            }

        if (eglSwapBuffers (pDemo->display, pDemo->surface) != EGL_TRUE)
            {
            (void)fprintf (stderr, "eglSwapBuffers failed\n");
            break;
            }
#if defined(GFX_USE_GBM)
        if (pDemo->gbm_surf)
            {
            struct gbm_bo* gbm_bo;

            gbm_bo = gbm_surface_lock_front_buffer (pDemo->gbm_surf);
            if (gbm_bo == NULL)
                {
                (void)fprintf (stderr, "gbm_surface_lock_front_buffer failed\n");
                break;
                }

            (void)gfxKmsPageFlip (pDemo->drmDevFd, pDemo->crtcId, pDemo->fbId,
                                  &(pDemo->connId), &(pDemo->mode), 20, 1);

            gbm_surface_release_buffer (pDemo->gbm_surf, gbm_bo);
            }
#endif /* GFX_USE_GBM */
        if (runTime != 0)
            {
            (void)gettimeofday (&curtime, NULL);
            diffTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000) - startTime;
            }
        }

    /* Deinitialize demo */
    es2PolygonDeinit (pDemo);
    es2PolygonEglDeinit (pDemo);
    (void)fprintf (stdout, "TEST COMPLETED\n");
    return (EXIT_SUCCESS);
    }
