/* gfxGearDemo.inl - Standard OpenGL Gears Demo */

/*
 * Copyright (C) 2009 Chia-I Wu <olv@0xlab.org>
 *
 * Based on eglgears by
 * Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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
22dec14,yat  Create OpenGL demo for Mesa (US24712)
*/

/*

DESCRIPTION

This example program provides the standard OpenGL gear program.

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
#include "gfxKmsHelper.inl"
#endif /* GFX_USE_GBM */
#if defined(GFX_USE_EVDEV)
#include <evdevLib.h>
#endif

/* defines */

#ifndef M_PI
#define M_PI 3.14159265358979323846
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
    long                 tRate0;
    long                 frames;
    long                 tRot0;
    GLfloat              angle;
    GLfloat              view_rotx, view_roty, view_rotz;
    GLint                gear[3];
    } GEAR_SCENE;

/**************************************************************************
***************************************************************************
*
*  Support Routines code
*
***************************************************************************
**************************************************************************/


/**************************************************************************
***************************************************************************
*
*  Initialization code
*
***************************************************************************
**************************************************************************/

/*******************************************************************************
 *
 * gearInitOne - draw a gear wheel
 *
 *  Input:  inner_radius - radius of hole at center
 *          outer_radius - radius at center of teeth
 *          width - width of gear
 *          teeth - number of teeth
 *          tooth_depth - depth of tooth
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gearInit
 *
 */
static void gearInitOne
    (
    GLfloat inner_radius,
    GLfloat outer_radius,
    GLfloat width,
    GLint teeth,
    GLfloat tooth_depth
    )
    {
    GLint i;
    GLfloat r0, r1, r2;
    GLfloat angle, a0, da;
    GLfloat u, v, len;

    r0 = inner_radius;
    r1 = outer_radius - tooth_depth / 2.0f;
    r2 = outer_radius + tooth_depth / 2.0f;

    a0 = (GLfloat)(2.0 * M_PI / teeth);
    da = a0 / 4.0f;

    glShadeModel (GL_FLAT);

    glNormal3f (0.0, 0.0, 1.0);

    /* draw front face */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++)
          {
          angle = (GLfloat)(i * 2.0 * M_PI / teeth);
          glVertex3f (r0 * (float)cos ((double)angle), r0 * (float)sin ((double)angle), width * 0.5f);
          glVertex3f (r1 * (float)cos ((double)angle), r1 * (float)sin ((double)angle), width * 0.5f);
          if (i < teeth)
              {
              glVertex3f (r0 * (float)cos ((double)angle),               r0 * (float)sin ((double)angle),               width * 0.5f);
              glVertex3f (r1 * (float)cos ((double)(angle + 3.0f * da)), r1 * (float)sin ((double)(angle + 3.0f * da)), width * 0.5f);
              }
          }
    glEnd();

    /* draw front sides of teeth */
    glBegin(GL_QUADS);
    for (i = 0; i < teeth; i++)
          {
          angle = (GLfloat)(i * 2.0 * M_PI / teeth);
          glVertex3f (r1 * (float)cos ((double)angle),               r1 * (float)sin ((double)angle),               width * 0.5f);
          glVertex3f (r2 * (float)cos ((double)(angle + da)),        r2 * (float)sin ((double)(angle + da)),        width * 0.5f);
          glVertex3f (r2 * (float)cos ((double)(angle + 2.0f * da)), r2 * (float)sin ((double)(angle + 2.0f * da)), width * 0.5f);
          glVertex3f (r1 * (float)cos ((double)(angle + 3.0f * da)), r1 * (float)sin ((double)(angle + 3.0f * da)), width * 0.5f);
          }
    glEnd();

    glNormal3f (0.0f, 0.0f, -1.0f);

    /* draw back face */
    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= teeth; i++)
          {
          angle = (GLfloat)(i * 2.0 * M_PI / teeth);
          glVertex3f (r1 * (float)cos ((double)angle), r1 * (float)sin ((double)angle), -width * 0.5f);
          glVertex3f (r0 * (float)cos ((double)angle), r0 * (float)sin ((double)angle), -width * 0.5f);
          if (i < teeth)
              {
              glVertex3f (r1 * (float)cos ((double)(angle + 3.0f * da)), r1 * (float)sin ((double)(angle + 3.0f * da)), -width * 0.5f);
              glVertex3f (r0 * (float)cos ((double)angle),               r0 * (float)sin ((double)angle),               -width * 0.5f);
              }
          }
    glEnd();

    /* draw back sides of teeth */
    glBegin(GL_QUADS);
    for (i = 0; i < teeth; i++)
          {
          angle = (GLfloat)(i * 2.0 * M_PI / teeth);
          glVertex3f (r1 * (float)cos ((double)(angle + 3.0f * da)), r1 * (float)sin ((double)(angle + 3.0f * da)), -width * 0.5f);
          glVertex3f (r2 * (float)cos ((double)(angle + 2.0f * da)), r2 * (float)sin ((double)(angle + 2.0f * da)), -width * 0.5f);
          glVertex3f (r2 * (float)cos ((double)(angle + da)),        r2 * (float)sin ((double)(angle + da)),        -width * 0.5f);
          glVertex3f (r1 * (float)cos ((double)angle),               r1 * (float)sin ((double)angle),               -width * 0.5f);
          }
    glEnd();

    /* draw outward faces of teeth */
    glBegin(GL_QUAD_STRIP);
      for (i = 0; i < teeth; i++)
          {
          angle = (GLfloat)(i * 2.0 * M_PI / teeth);
          glVertex3f (r1 * (float)cos ((double)angle), r1 * (float)sin ((double)angle), width * 0.5f);
          glVertex3f (r1 * (float)cos ((double)angle), r1 * (float)sin ((double)angle), -width * 0.5f);
          u = r2 * (float)cos ((double)(angle + da)) - r1 * (float)cos ((double)angle);
          v = r2 * (float)sin ((double)(angle + da)) - r1 * (float)sin ((double)angle);
          len = (float)sqrt ((double)(u * u + v * v));
          u /= len;
          v /= len;
          glNormal3f (v, -u, 0.0f);
          glVertex3f (r2 * (float)cos ((double)(angle + da)), r2 * (float)sin ((double)(angle + da)), width * 0.5f);
          glVertex3f (r2 * (float)cos ((double)(angle + da)), r2 * (float)sin ((double)(angle + da)), -width * 0.5f);
          glNormal3f ((float)cos ((double)angle), (float)sin ((double)angle), 0.0f);
          glVertex3f (r2 * (float)cos ((double)(angle + 2.0f * da)), r2 * (float)sin ((double)(angle + 2.0f * da)), width * 0.5f);
          glVertex3f (r2 * (float)cos ((double)(angle + 2.0f * da)), r2 * (float)sin ((double)(angle + 2.0f * da)), -width * 0.5f);
          u = r1 * (float)cos ((double)(angle + 3.0f * da)) - r2 * (float)cos ((double)(angle + 2.0f * da));
          v = r1 * (float)sin ((double)(angle + 3.0f * da)) - r2 * (float)sin ((double)(angle + 2.0f * da));
          glNormal3f (v, -u, 0.0f);
          glVertex3f (r1 * (float)cos ((double)(angle + 3.0f * da)), r1 * (float)sin ((double)(angle + 3.0f * da)), width * 0.5f);
          glVertex3f (r1 * (float)cos ((double)(angle + 3.0f * da)), r1 * (float)sin ((double)(angle + 3.0f * da)), -width * 0.5f);
          glNormal3f ((float)cos ((double)angle), (float)sin ((double)angle), 0.0f);
          }

      glVertex3f (r1 * (float)cos (0.0), r1 * (float)sin (0.0), width * 0.5f);
      glVertex3f (r1 * (float)cos (0.0), r1 * (float)sin (0.0), -width * 0.5f);

    glEnd();

    glShadeModel (GL_SMOOTH);

    /* draw inside radius cylinder */
    glBegin(GL_QUAD_STRIP);

      for (i = 0; i <= teeth; i++)
          {
          angle = (GLfloat)(i * 2.0 * M_PI / teeth);
          glNormal3f (-(float)cos ((double)angle), -(float)sin ((double)angle), 0.0f);
          glVertex3f (r0 * (float)cos ((double)angle), r0 * (float)sin ((double)angle), -width * 0.5f);
          glVertex3f (r0 * (float)cos ((double)angle), r0 * (float)sin ((double)angle), width * 0.5f);
          }
    glEnd();
    }

/*******************************************************************************
 *
 * gearPrint - print the configuration of the graphics library
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
static void gearPrint
    (
    GEAR_SCENE* pDemo,
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
 * gearCleanup - perform final cleanup
 *
 * This routine performs final cleanup
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gearEglDeinit
 *
 */
static void gearCleanup
    (
    GEAR_SCENE* pDemo
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
 * gearEglDeinit - perform necessary steps to deinitialize
 *
 * This routine takes care of all the steps involved in cleaning-up EGL
 * resources. The EGL resources must be provided by the GEAR_SCENE parameter
 * <pDemo>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gearEglInit
 *
 */
static void gearEglDeinit
    (
    GEAR_SCENE* pDemo
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

    gearCleanup (pDemo);
    }

#if defined(GFX_USE_GBM)
/*******************************************************************************
*
* gearFindDrmFb - find a DRM framebuffer
*
* This routine finds a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static int gearFindDrmFb
    (
    GEAR_SCENE* pDemo
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
 * gearEglInit - perform necessary steps to initialize
 *
 * This routine takes care of all the steps involved in initializing EGL,
 * creating a surface, and setting up the program for rendering.
 *
 * RETURNS: pointer to GEAR_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gearEglDeinit
 *
 */
static GEAR_SCENE* gearEglInit
    (
    unsigned int device,
    unsigned int eglDriver
    )
    {
    GEAR_SCENE* pDemo;
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
    pDemo = (GEAR_SCENE*)calloc (1, sizeof (GEAR_SCENE));
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
            gearEglDeinit (pDemo);
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
        gearEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglInitialize (pDemo->display, &eglMajor, &eglMinor))
        {
        (void)fprintf (stderr, "eglInitialize failed\n");
        gearEglDeinit (pDemo);
        return (NULL);
        }
#if defined(GFX_USE_GBM)
    if (pDemo->gbm_dev)
        {
        if (gearFindDrmFb (pDemo))
            {
            gearEglDeinit (pDemo);
            return (NULL);
            }
        }
#endif /* GFX_USE_GBM */
    if (!eglBindAPI (EGL_OPENGL_API))
        {
        (void)fprintf (stderr, "eglBindAPI failed\n");
        gearEglDeinit (pDemo);
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
        gearEglDeinit (pDemo);
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
            gearEglDeinit (pDemo);
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
        gearEglDeinit (pDemo);
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
        gearEglDeinit (pDemo);
        return (NULL);
        }

    /* Set the context as current */
    if (!eglMakeCurrent (pDemo->display, pDemo->surface, pDemo->surface, pDemo->context))
        {
        (void)fprintf (stderr, "eglMakeCurrent failed\n");
        gearEglDeinit (pDemo);
        return (NULL);
        }

    /* Print configuration info, if requested */
    if (printInfo)
        {
        gearPrint (pDemo, eglMajor, eglMinor);
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
            gearEglDeinit (pDemo);
            return (NULL);
            }

        handle = gbm_bo_get_handle (gbm_bo).u32;
        if (handle == 0)
            {
            (void)fprintf (stderr, "Bad handle\n");
            gearEglDeinit (pDemo);
            return (NULL);
            }

        /* Add FB. Note: pitch can be at least 512 byte aligned in DRM/I915 */
        if (drmModeAddFB (pDemo->drmDevFd,
                          pDemo->mode.hdisplay, pDemo->mode.vdisplay,
                          24, 32, (((pDemo->mode.hdisplay << 2) + 511) & ~511),
                          handle, &(pDemo->fbId)))
            {
            (void)fprintf (stderr, "drmModeAddFB failed\n");
            gearEglDeinit (pDemo);
            return (NULL);
            }

        if (drmModeSetCrtc (pDemo->drmDevFd, pDemo->crtcId, pDemo->fbId,
                            0, 0, &(pDemo->connId), 1, &(pDemo->mode)))
            {
            gearEglDeinit (pDemo);
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
 * gearDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gearInit
 *
 */
static void gearDeinit
    (
    GEAR_SCENE* pDemo
    )
    {
    GLint i;

    for (i = 0; i < sizeof (pDemo->gear) / sizeof (pDemo->gear[0]); i++)
        {
        glDeleteLists (pDemo->gear[i], 1);
        }
    }

/*******************************************************************************
 *
 * gearInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gearDeinit
 *
 */
static int gearInit
    (
    GEAR_SCENE* pDemo
    )
    {
    const GLfloat pos[4] = { 5.0f, 5.0f, 10.0f, 0.0f };
    const GLfloat red[4] = { 0.8f, 0.1f, 0.0f, 1.0f };
    const GLfloat green[4] = { 0.0f, 0.8f, 0.2f, 1.0f };
    const GLfloat blue[4] = { 0.2f, 0.2f, 1.0f, 1.0f };

    /* Initialize the gears */
    pDemo->view_rotx = 20.0f;
    pDemo->view_roty = 30.0f;
    pDemo->view_rotz = 0.0f;
    pDemo->angle = 0.0f;

    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);

    glLightfv (GL_LIGHT0, GL_POSITION, pos);
    glEnable (GL_CULL_FACE);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_NORMALIZE);

    pDemo->gear[0] = glGenLists (1);
    glNewList (pDemo->gear[0], GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
    gearInitOne (1.0f, 4.0f, 1.0f, 20, 0.7f);
    glEndList ();

    pDemo->gear[1] = glGenLists (1);
    glNewList (pDemo->gear[1], GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
    gearInitOne (0.5f, 1.5f, 0.8f, 10, 0.6f);
    glEndList ();

    pDemo->gear[2] = glGenLists (1);
    glNewList (pDemo->gear[2], GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
    gearInitOne (1.3f, 2.0f, 0.5f, 10, 0.7f);
    glEndList ();

    glViewport (0, 0, (GLint) pDemo->surfWidth, (GLint) pDemo->surfHeight);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glFrustum (-1.0f, 1.0f, -1.0f, 1.0f, 5.0f, 100.0f);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glTranslatef (0.0f, 0.0f, -5.0f);

    return 0;
    }

/*******************************************************************************
 *
 * gearRender - render the application
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
static int gearRender
    (
    GEAR_SCENE* pDemo
    )
    {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix ();
    glTranslatef (0.0f, 0.0f, -40.0f);
    glRotatef (pDemo->view_rotx, 1.0f, 0.0f, 0.0f);
    glRotatef (pDemo->view_roty, 0.0f, 1.0f, 0.0f);
    glRotatef (pDemo->view_rotz, 0.0f, 0.0f, 1.0f);

    glPushMatrix ();
    glTranslatef (-3.0f, -2.0f, 0.0f);
    glRotatef (pDemo->angle, 0.0f, 0.0f, 1.0f);
    glCallList (pDemo->gear[0]);
    glPopMatrix ();

    glPushMatrix ();
    glTranslatef (3.1f, -2.0f, 0.0f);
    glRotatef (-2.0f * pDemo->angle - 9.0f, 0.0f, 0.0f, 1.0f);
    glCallList (pDemo->gear[1]);
    glPopMatrix ();

    glPushMatrix ();
    glTranslatef (-3.1f, 4.2f, 0.0f);
    glRotatef (-2.0f * pDemo->angle - 25.0f, 0.0f, 0.0f, 1.0f);
    glCallList (pDemo->gear[2]);
    glPopMatrix ();

    glPopMatrix ();

    return 0;
    }

/*******************************************************************************
 *
 * gearPerfCount - calculate and show performance
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
static void gearPerfCount
    (
    GEAR_SCENE* pDemo
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

    pDemo->angle += 70.0f * (GLfloat)dt / 1000.0f;  /* 70 degrees per second */
    if (pDemo->angle > 360.0f)
        {
        pDemo->angle = (GLfloat)fmod ((double)pDemo->angle, 360.0);
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
 * gearGetChar - gets an ASCII character from keyboard
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
static unsigned short gearGetChar
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
 * gfxGearDemo - core functionality of the program
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
static int gfxGearDemo
    (
    unsigned int device,
    int runTime,
    unsigned int eglDriver
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
    GEAR_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                                /* terminate flag */
#endif
    /* Initialize demo */
    pDemo = gearEglInit (device, eglDriver);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "gearEglInit failed\n");
        return (EXIT_FAILURE);
        }

    /* Initialize demo */
    if (gearInit (pDemo))
        {
        (void)fprintf (stderr, "gearInit failed\n");
        gearDeinit (pDemo);
        gearEglDeinit (pDemo);
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
            unsigned short character = gearGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        gearPerfCount (pDemo);

        if (gearRender (pDemo))
            {
            (void)fprintf (stderr, "gearRender failed\n");
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
    gearDeinit (pDemo);
    gearEglDeinit (pDemo);
    (void)fprintf (stdout, "DEMO COMPLETED\n");
    return (EXIT_SUCCESS);
    }
