/* gfxPolygonTest.inl - OpenGL Polygon Test */

/*
 * This code was created by Jeff Molofee '99
 * (ported to Linux/SDL by Ti Leggett '01)
 *
 * If you've found this code useful, please let me know.
 *
 * Visit Jeff at http://nehe.gamedev.net/
 *
 * or for port-specific comments, questions, bugreports etc.
 * email to leggett@eecs.tulane.edu
 */

/*
 * Copyright (c) 2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24feb16,yat  Fix static analysis defects (US75033)
05feb16,yat  Create OpenGL demo for Mesa (US24712)
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
#if defined(_WRS_KERNEL)
#include <envLib.h>
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
#include <GL/gl.h>
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
    int                  countPolygon;
    long                 tRate0;
    long                 frames;
    long                 tRot0;
    GLfloat              angle;
    } POLYGON_SCENE;

/*******************************************************************************
 *
 * polygonPrint - print the configuration of the graphics library
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
static void polygonPrint
    (
    POLYGON_SCENE* pDemo,
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
 * polygonCleanup - perform final cleanup
 *
 * This routine performs final cleanup
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: polygonEglDeinit
 *
 */
static void polygonCleanup
    (
    POLYGON_SCENE* pDemo
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
 * polygonEglDeinit - perform necessary steps to deinitialize
 *
 * This routine takes care of all the steps involved in cleaning-up EGL
 * resources. The EGL resources must be provided by the POLYGON_SCENE parameter
 * <pDemo>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: polygonEglInit
 *
 */
static void polygonEglDeinit
    (
    POLYGON_SCENE* pDemo
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

#if defined(GFX_USE_GBM)
    gfxKmsCloseDrmDev (pDemo->drmDevFd);
#endif /* GFX_USE_GBM */

    polygonCleanup (pDemo);
    }

#if defined(GFX_USE_GBM)
/*******************************************************************************
*
* polygonFindDrmFb - find a DRM framebuffer
*
* This routine finds a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int polygonFindDrmFb
    (
    POLYGON_SCENE* pDemo
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
 * polygonEglInit - perform necessary steps to initialize
 *
 * This routine takes care of all the steps involved in initializing EGL,
 * creating a surface, and setting up the program for rendering.
 *
 * RETURNS: pointer to POLYGON_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: polygonEglDeinit
 *
 */
static POLYGON_SCENE* polygonEglInit
    (
    unsigned int device,
    unsigned int eglDriver
    )
    {
    POLYGON_SCENE* pDemo;
    GLboolean   printInfo = GL_TRUE;
    EGLint      eglMajor, eglMinor;
    EGLint      configAttribs[] =
        {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_BUFFER_SIZE, 32,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
        };
    EGLConfig eglConfig;                        /* EGL configuration */
    EGLint numConfigs = 0;                      /* Number of EGL configurations */
#if defined(GFX_USE_EVDEV)
    char evDevName[EV_DEV_NAME_LEN + 1];
    unsigned int e;
#endif
    /* Allocate basic structure to carry demo data */
    pDemo = (POLYGON_SCENE*)calloc (1, sizeof (POLYGON_SCENE));
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
            polygonEglDeinit (pDemo);
            return (NULL);
            }

        pDemo->display = eglGetDisplay ((EGLNativeDisplayType)pDemo->gbm_dev);
        }
#else
    pDemo->display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
#endif /* GFX_USE_GBM */
    if (pDemo->display == EGL_NO_DISPLAY)
        {
        (void)fprintf (stderr, "eglGetDisplay failed - possibly driver is not compatible\n");
        polygonEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglInitialize (pDemo->display, &eglMajor, &eglMinor))
        {
        (void)fprintf (stderr, "eglInitialize failed\n");
        polygonEglDeinit (pDemo);
        return (NULL);
        }
#if defined(GFX_USE_GBM)
    if (pDemo->gbm_dev)
        {
        if (polygonFindDrmFb (pDemo))
            {
            polygonEglDeinit (pDemo);
            return (NULL);
            }
        }
#endif /* GFX_USE_GBM */
    if (!eglBindAPI (EGL_OPENGL_API))
        {
        (void)fprintf (stderr, "eglBindAPI failed\n");
        polygonEglDeinit (pDemo);
        return (NULL);
        }

    /*
     * EGL Initialization Step 2: Choose an EGL configuration
     * Query the OpenGL library (and fbdev driver) to find a compatible
     * configuration.
     */
    if (!eglChooseConfig (pDemo->display, configAttribs, &eglConfig, 1, &numConfigs) || (numConfigs <= 0))
        {
        (void)fprintf (stderr, "Couldn't get an EGL visual config\n");
        polygonEglDeinit (pDemo);
        return (NULL);
        }

    /*
     * EGL Initialization Step 3: Create the back buffer
     * Create a window surface using the selected configuration and query its
     * dimensions
     */
#if defined(GFX_USE_GBM)
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
            polygonEglDeinit (pDemo);
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
#endif /* GFX_USE_GBM */
    if (pDemo->surface == EGL_NO_SURFACE)
        {
        (void)fprintf (stderr, "eglCreateWindowSurface failed\n");
        polygonEglDeinit (pDemo);
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
                                       EGL_NO_CONTEXT, NULL);
    if (pDemo->context == EGL_NO_CONTEXT)
        {
        (void)fprintf (stderr, "eglCreateContext failed\n");
        polygonEglDeinit (pDemo);
        return (NULL);
        }

    /* Set the context as current */
    if (!eglMakeCurrent (pDemo->display, pDemo->surface, pDemo->surface, pDemo->context))
        {
        (void)fprintf (stderr, "eglMakeCurrent failed\n");
        polygonEglDeinit (pDemo);
        return (NULL);
        }

    /* Print configuration info, if requested */
    if (printInfo)
        {
        polygonPrint (pDemo, eglMajor, eglMinor);
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
            polygonEglDeinit (pDemo);
            return (NULL);
            }

        handle = gbm_bo_get_handle (gbm_bo).u32;
        if (handle == 0)
            {
            (void)fprintf (stderr, "Bad handle\n");
            polygonEglDeinit (pDemo);
            return (NULL);
            }

        /* Add FB. Note: pitch can be at least 512 byte aligned in DRM/I915 */
        if (drmModeAddFB (pDemo->drmDevFd,
                          pDemo->mode.hdisplay, pDemo->mode.vdisplay,
                          24, 32, (((pDemo->mode.hdisplay << 2) + 511) & ~511),
                          handle, &(pDemo->fbId)))
            {
            (void)fprintf (stderr, "drmModeAddFB failed\n");
            polygonEglDeinit (pDemo);
            return (NULL);
            }

        if (drmModeSetCrtc (pDemo->drmDevFd, pDemo->crtcId, pDemo->fbId,
                            0, 0, &(pDemo->connId), 1, &(pDemo->mode)))
            {
            polygonEglDeinit (pDemo);
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
 * polygonDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: polygonInit
 *
 */
static void polygonDeinit
    (
    POLYGON_SCENE* pDemo
    )
    {
    }

/*******************************************************************************
 *
 * polygonInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: polygonDeinit
 *
 */
static int polygonInit
    (
    POLYGON_SCENE* pDemo
    )
    {
    pDemo->angle = 360.0f;

    /* Set the background color */
    glClearColor (0.4f, 0.6f, 0.9f, 0.0f);

    /* Depth buffer setup */
    glClearDepth (1.0f);

    /* Enables Depth Testing */
    glEnable (GL_DEPTH_TEST);

    /* The Type Of Depth Test To Do */
    glDepthFunc (GL_LESS);

    glViewport (0, 0, (GLint) pDemo->surfWidth, (GLint) pDemo->surfHeight);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glFrustum (-1.0f, 1.0f, -1.0f, 1.0f, 2.0f, 20.0f);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glTranslatef (0.0f, 0.0f, -5.0f);

    return 0;
    }

/*******************************************************************************
 *
 * polygonRender - render the application
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
static int polygonRender
    (
    POLYGON_SCENE* pDemo
    )
    {
    int i;

    /* Clear The Screen And The Depth Buffer */
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Move Into The Screen 5 Units */
    glLoadIdentity ();
    glTranslatef (0.0f, 0.0f, -5.0f);

    glRotatef (pDemo->angle, 0.0f, 1.0f, 0.0f);
#if 0
    glBegin(GL_QUADS);

    /* Front Face (note that the texture's corners have to match the quad's corners) */
    glVertex3f (-1.0f, -1.0f,  1.0f); /* Bottom Left Of The Texture and Quad */
    glVertex3f ( 1.0f, -1.0f,  1.0f); /* Bottom Right Of The Texture and Quad */
    glVertex3f ( 1.0f,  1.0f,  1.0f); /* Top Right Of The Texture and Quad */
    glVertex3f (-1.0f,  1.0f,  1.0f); /* Top Left Of The Texture and Quad */

    /* Back Face */
    glVertex3f (-1.0f, -1.0f, -1.0f); /* Bottom Right Of The Texture and Quad */
    glVertex3f (-1.0f,  1.0f, -1.0f); /* Top Right Of The Texture and Quad */
    glVertex3f ( 1.0f,  1.0f, -1.0f); /* Top Left Of The Texture and Quad */
    glVertex3f ( 1.0f, -1.0f, -1.0f); /* Bottom Left Of The Texture and Quad */

    /* Top Face */
    glVertex3f (-1.0f,  1.0f, -1.0f); /* Top Left Of The Texture and Quad */
    glVertex3f (-1.0f,  1.0f,  1.0f); /* Bottom Left Of The Texture and Quad */
    glVertex3f ( 1.0f,  1.0f,  1.0f); /* Bottom Right Of The Texture and Quad */
    glVertex3f ( 1.0f,  1.0f, -1.0f); /* Top Right Of The Texture and Quad */

    /* Bottom Face */
    glVertex3f (-1.0f, -1.0f, -1.0f); /* Top Right Of The Texture and Quad */
    glVertex3f ( 1.0f, -1.0f, -1.0f); /* Top Left Of The Texture and Quad */
    glVertex3f ( 1.0f, -1.0f,  1.0f); /* Bottom Left Of The Texture and Quad */
    glVertex3f (-1.0f, -1.0f,  1.0f); /* Bottom Right Of The Texture and Quad */

    /* Right face */
    glVertex3f ( 1.0f, -1.0f, -1.0f); /* Bottom Right Of The Texture and Quad */
    glVertex3f ( 1.0f,  1.0f, -1.0f); /* Top Right Of The Texture and Quad */
    glVertex3f ( 1.0f,  1.0f,  1.0f); /* Top Left Of The Texture and Quad */
    glVertex3f ( 1.0f, -1.0f,  1.0f); /* Bottom Left Of The Texture and Quad */

    /* Left Face */
    glVertex3f (-1.0f, -1.0f, -1.0f); /* Bottom Left Of The Texture and Quad */
    glVertex3f (-1.0f, -1.0f,  1.0f); /* Bottom Right Of The Texture and Quad */
    glVertex3f (-1.0f,  1.0f,  1.0f); /* Top Right Of The Texture and Quad */
    glVertex3f (-1.0f,  1.0f, -1.0f); /* Top Left Of The Texture and Quad */

    glEnd ();
#else
    glBegin (GL_TRIANGLES);

    for (i = 0; i < pDemo->countPolygon; i += 3)
        {
        glVertex3f (g_verticesPolygon[i], g_verticesPolygon[i+1], g_verticesPolygon[i+2]);
        }

    glEnd ();
#endif
    return 0;
    }

/*******************************************************************************
 *
 * polygonPerfCount - calculate and show performance
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
static void polygonPerfCount
    (
    POLYGON_SCENE* pDemo
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

    pDemo->angle -= 10.0f * (GLfloat)dt / 1000.0f;  /* 10 degrees per second */
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
        }
    }

#if defined(GFX_USE_EVDEV)
/*******************************************************************************
 *
 * polygonGetChar - gets an ASCII character from keyboard
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
static unsigned short polygonGetChar
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
 * gfxPolygonTest - core functionality of the program
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
static int gfxPolygonTest
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
    POLYGON_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                                /* terminate flag */
#endif
    /* Initialize demo */
    pDemo = polygonEglInit (device, eglDriver);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "polygonEglInit failed\n");
        return (EXIT_FAILURE);
        }

    pDemo->countPolygon = countPolygon;
    if ((pDemo->countPolygon <= 0) || pDemo->countPolygon > 6000)
        {
        pDemo->countPolygon = 6000;
        }
    (void)printf ("Polygon count %d\n", pDemo->countPolygon);

    /* Initialize demo */
    if (polygonInit (pDemo))
        {
        (void)fprintf (stderr, "polygonInit failed\n");
        polygonDeinit (pDemo);
        polygonEglDeinit (pDemo);
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
            unsigned short character = polygonGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        polygonPerfCount (pDemo);

        if (polygonRender (pDemo))
            {
            (void)fprintf (stderr, "polygonRender failed\n");
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
    polygonDeinit (pDemo);
    polygonEglDeinit (pDemo);
    (void)fprintf (stdout, "TEST COMPLETED\n");
    return (EXIT_SUCCESS);
    }
