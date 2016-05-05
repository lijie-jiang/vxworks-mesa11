/* gfxFboDemo.inl - OpenGL Fbo Demo */

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
01mar16,yat  Add gfxGbmHelper (US76256)
24feb16,yat  Fix static analysis defects (US75033)
13jan16,yat  Written (US24710)
*/

/*

DESCRIPTION

This program runs the fbo demo.

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
#include <string.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <EGL/egl.h>
#if defined(GFX_USE_GBM)
#include "gfxGbmHelper.inl"
#endif /* GFX_USE_GBM */
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
    GFX_GBM              gbm;
#endif /* GFX_USE_GBM */
#if defined(GFX_USE_EVDEV)
    int                  evDevFd;
#endif
    long                 tRate0;
    long                 frames;
    GLuint               texture[1];
    GLuint               fbo[1];
    GLuint               rbo[1];
    } FBO_SCENE;

/*******************************************************************************
 *
 * fboCheckGL0 - check GL status
 *
 * This routine checks the GL status.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void fboCheckGL0
    (
    const char *file,
    int line
    )
    {
    GLenum status = glGetError();
    const char *str;

    if (status == GL_NO_ERROR)
        {
        return;
        }

    switch (status)
        {
        case GL_INVALID_ENUM:
            str = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            str = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            str = "GL_INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            str = "GL_OUT_OF_MEMORY";
            break;
        default:
            str = "Unknown";
            break;
        }

    (void)fprintf (stderr, "%s:%d:%s\n", file, line, str);
    }

#define fboCheckGL() fboCheckGL0(__FILE__, __LINE__)

/*******************************************************************************
 *
 * fboCheckFramebufferStatus0 - check framebuffer status
 *
 * This routine checks the framebuffer status.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void fboCheckFramebufferStatus0
    (
    const char *file,
    int line
    )
    {
    GLenum status = glCheckFramebufferStatus (GL_FRAMEBUFFER);
    const char *str;

    if (status == GL_FRAMEBUFFER_COMPLETE)
        {
        return;
        }

    switch (status)
        {
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            str = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            str = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            str = "GL_FRAMEBUFFER_UNSUPPORTED";
            break;
        default:
            str = "Unknown";
            break;
        }

    (void)fprintf (stderr, "%s:%d:%s\n", file, line, str);
    }

#define fboCheckFramebufferStatus() \
        fboCheckFramebufferStatus0(__FILE__, __LINE__)

/*******************************************************************************
 *
 * fboPrint - print the configuration of the graphics library
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
static void fboPrint
    (
    FBO_SCENE* pDemo,
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
 * fboCleanup - perform final cleanup
 *
 * This routine performs final cleanup
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fboEglDeinit
 *
 */
static void fboCleanup
    (
    FBO_SCENE* pDemo
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
 * fboEglDeinit - perform necessary steps to deinitialize
 *
 * This routine takes care of all the steps involved in cleaning-up EGL
 * resources. The EGL resources must be provided by the FBO_SCENE parameter
 * <pDemo>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fboEglInit
 *
 */
static void fboEglDeinit
    (
    FBO_SCENE* pDemo
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
        gfxGbmRmFB (&(pDemo->gbm));
        gfxGbmDestroySurface (&(pDemo->gbm));
#endif /* GFX_USE_GBM */

        /* Close the display */
        (void)eglMakeCurrent (pDemo->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        (void)eglTerminate (pDemo->display);
        }
    (void)eglReleaseThread ();

#if defined(GFX_USE_GBM)
    gfxGbmDestroyDevice (&(pDemo->gbm));
#endif /* GFX_USE_GBM */

    fboCleanup (pDemo);
    }

/*******************************************************************************
 *
 * fboEglInit - perform necessary steps to initialize
 *
 * This routine takes care of all the steps involved in initializing EGL,
 * creating a surface, and setting up the program for rendering.
 *
 * RETURNS: pointer to FBO_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fboEglDeinit
 *
 */
static FBO_SCENE* fboEglInit
    (
    unsigned int device,
    unsigned int eglDriver
    )
    {
    FBO_SCENE* pDemo;
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
    pDemo = (FBO_SCENE*)calloc (1, sizeof (FBO_SCENE));
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "calloc failed\n");
        return (NULL);
        }

    pDemo->tRate0 = -1;
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
        /* Mesa EGL DRI driver */
        if (gfxGbmCreateDevice (&(pDemo->gbm)))
            {
            fboEglDeinit (pDemo);
            return (NULL);
            }

        pDemo->display = eglGetDisplay ((EGLNativeDisplayType)pDemo->gbm.gbm_dev);
        }
#else
    pDemo->display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
#endif /* GFX_USE_GBM */
    if (pDemo->display == EGL_NO_DISPLAY)
        {
        (void)fprintf (stderr, "eglGetDisplay failed - possibly driver is not compatible\n");
        fboEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglInitialize (pDemo->display, &eglMajor, &eglMinor))
        {
        (void)fprintf (stderr, "eglInitialize failed\n");
        fboEglDeinit (pDemo);
        return (NULL);
        }
#if defined(GFX_USE_GBM)
    if (pDemo->gbm.gbm_dev)
        {
        if (gfxGbmFindDrmFb (&(pDemo->gbm)))
            {
            fboEglDeinit (pDemo);
            return (NULL);
            }
        }
#endif /* GFX_USE_GBM */
    if (!eglBindAPI (EGL_OPENGL_API))
        {
        (void)fprintf (stderr, "eglBindAPI failed\n");
        fboEglDeinit (pDemo);
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
        fboEglDeinit (pDemo);
        return (NULL);
        }

    /*
     * EGL Initialization Step 3: Create the back buffer
     * Create a window surface using the selected configuration and query its
     * dimensions
     */
#if defined(GFX_USE_GBM)
    if (pDemo->gbm.gbm_dev)
        {
        if (gfxGbmCreateSurface (&(pDemo->gbm)))
            {
            fboEglDeinit (pDemo);
            return (NULL);
            }
        }
    else
        {
        pDemo->gbm.gbm_surf = NULL;
        }
    pDemo->surface = eglCreateWindowSurface (pDemo->display, eglConfig,
                        (EGLNativeWindowType)pDemo->gbm.gbm_surf, NULL);
#else
    pDemo->surface = eglCreateWindowSurface (pDemo->display, eglConfig,
                                             NULL, NULL);
#endif /* GFX_USE_GBM */
    if (pDemo->surface == EGL_NO_SURFACE)
        {
        (void)fprintf (stderr, "eglCreateWindowSurface failed\n");
        fboEglDeinit (pDemo);
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
        fboEglDeinit (pDemo);
        return (NULL);
        }

    /* Set the context as current */
    if (!eglMakeCurrent (pDemo->display, pDemo->surface, pDemo->surface, pDemo->context))
        {
        (void)fprintf (stderr, "eglMakeCurrent failed\n");
        fboEglDeinit (pDemo);
        return (NULL);
        }

    /* Print configuration info, if requested */
    if (printInfo)
        {
        fboPrint (pDemo, eglMajor, eglMinor);
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
 * fboDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fboInit
 *
 */
static void fboDeinit
    (
    FBO_SCENE* pDemo
    )
    {
    GLuint fbo;

    glGetIntegerv (GL_FRAMEBUFFER_BINDING, (GLint *)&fbo);
    glBindFramebuffer (GL_FRAMEBUFFER, pDemo->fbo[0]);

    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_RENDERBUFFER, 0);

    if (fbo != pDemo->fbo[0])
        {
        glBindFramebuffer (GL_FRAMEBUFFER, fbo);
        }
    else
        {
        glBindFramebuffer (GL_FRAMEBUFFER, 0);
        }

    glDeleteRenderbuffers (1, pDemo->rbo);
    glDeleteFramebuffers (1, pDemo->fbo);
    }

/*******************************************************************************
 *
 * fboInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: 0, or 1 if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fboDeinit
 *
 */
static int fboInit
    (
    FBO_SCENE* pDemo
    )
    {
    /* Create texture */
    glGenTextures (1, pDemo->texture);

    /* Bind texture */
    glBindTexture (GL_TEXTURE_2D, pDemo->texture[0]);
    fboCheckGL ();

    /* Generate texture */
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
                  256, 256,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    fboCheckGL ();

    /* Nearest filtering */
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    /* Create framebuffer */
    glGenFramebuffers (1, pDemo->fbo);
    fboCheckGL ();

    /* Bind framebuffer */
    glBindFramebuffer (GL_FRAMEBUFFER, pDemo->fbo[0]);
    fboCheckGL ();

    /* Attach texture */
    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D, pDemo->texture[0], 0);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glClear (GL_COLOR_BUFFER_BIT);
    fboCheckGL ();
    fboCheckFramebufferStatus ();

    /* Create renderbuffer */
    glGenRenderbuffers (1, pDemo->rbo);
    fboCheckGL ();

    /* Bind renderbuffer */
    glBindRenderbuffer (GL_RENDERBUFFER, pDemo->rbo[0]);
    fboCheckGL ();

    glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 256, 256);

    glBindRenderbuffer (GL_RENDERBUFFER, 0);

    /* Attach renderbuffer */
    glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_RENDERBUFFER, pDemo->rbo[0]);
    fboCheckGL ();
    fboCheckFramebufferStatus ();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport (0, 0, (GLint) pDemo->surfWidth, (GLint) pDemo->surfHeight);

    return 0;
    }

/*******************************************************************************
 *
 * fboRender - render the application
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
static int fboRender
    (
    FBO_SCENE* pDemo
    )
    {
    GLfloat color = (GLfloat)(pDemo->frames % 2);

    glClearColor (color, 0.0f, 1.0f - color, 0.0f);
    glClear (GL_COLOR_BUFFER_BIT);

    return 0;
    }

/*******************************************************************************
 *
 * fboPerfCount - calculate and show performance
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
static void fboPerfCount
    (
    FBO_SCENE* pDemo
    )
    {
    long t;
    struct timeval curtime;

    (void)gettimeofday (&curtime, NULL);
    t = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000);

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
 * fboGetChar - gets an ASCII character from keyboard
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
static unsigned short fboGetChar
    (
    int evDevFd
    )
    {
    fd_set readFds;
    struct timeval timeout;
    static int m_x = 0;
    static int m_y = 0;
    static int m_slot = 0;
    static int m_id = 0;

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
            switch (evDevEvent.type)
                {
                case EV_DEV_SYN:
                    if (m_x < 0) m_x = 0;
                    if (m_y < 0) m_y = 0;
                    (void)printf ("Syn x:%d y:%d slot:%d id:%d\n", m_x, m_y, m_slot, m_id);
                    break;
                case EV_DEV_KEY:
                    switch (evDevEvent.code)
                        {
                        case EV_DEV_PTR_BTN_LEFT:
                            (void)printf ("Pointer button left [%08x]\n", evDevEvent.value);
                            break;
                        case EV_DEV_PTR_BTN_RIGHT:
                            (void)printf ("Pointer button right [%08x]\n", evDevEvent.value);
                            break;
                        case EV_DEV_PTR_BTN_MIDDLE:
                            (void)printf ("Pointer button middle [%08x]\n", evDevEvent.value);
                            break;
                        case EV_DEV_PTR_BTN_TOUCH:
                            (void)printf ("Pointer button touch [%08x]\n", evDevEvent.value);
                            break;
                        default:
                            (void)printf ("Key [%04x] [%08x]\n", evDevEvent.code, evDevEvent.value);
                            return (unsigned short)((evDevEvent.value)? evDevEvent.code:0);
                        }
                    break;
                case EV_DEV_REL:
                    switch (evDevEvent.code)
                        {
                        case EV_DEV_PTR_REL_X:
                            m_x += evDevEvent.value;
                            break;
                        case EV_DEV_PTR_REL_Y:
                            m_y += evDevEvent.value;
                            break;
                        }
                    break;
                case EV_DEV_ABS:
                    switch (evDevEvent.code)
                        {
                        case EV_DEV_PTR_ABS_X:
                            m_x = evDevEvent.value;
                            break;
                        case EV_DEV_PTR_ABS_Y:
                            m_y = evDevEvent.value;
                            break;
                        case EV_DEV_PTR_ABS_MT_SLOT:
                            m_slot = evDevEvent.value;
                            break;
                        case EV_DEV_PTR_ABS_MT_POSITION_X:
                            break;
                        case EV_DEV_PTR_ABS_MT_POSITION_Y:
                            break;
                        case EV_DEV_PTR_ABS_MT_TRACKING_ID:
                            m_id = evDevEvent.value;
                            break;
                        }
                    break;
                }
            }
        }

    return 0;
    }
#endif

/*******************************************************************************
 *
 * gfxFboDemo - core functionality of the program
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
static int gfxFboDemo
    (
    unsigned int device,
    int runTime,
    unsigned int eglDriver
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
    FBO_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                                /* terminate flag */
#endif
    /* Initialize demo */
    pDemo = fboEglInit (device, eglDriver);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "fboEglInit failed\n");
        return (EXIT_FAILURE);
        }

    /* Initialize demo */
    if (fboInit (pDemo))
        {
        (void)fprintf (stderr, "fboInit failed\n");
        fboDeinit (pDemo);
        fboEglDeinit (pDemo);
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
            unsigned short character = fboGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        fboPerfCount (pDemo);

        if (fboRender (pDemo))
            {
            (void)fprintf (stderr, "fboRender failed\n");
            break;
            }

        if (eglSwapBuffers (pDemo->display, pDemo->surface) != EGL_TRUE)
            {
            (void)fprintf (stderr, "eglSwapBuffers failed\n");
            break;
            }
#if defined(GFX_USE_GBM)
        if (pDemo->gbm.gbm_surf)
            {
            if (gfxGbmPageFlip (&(pDemo->gbm)))
                {
                break;
                }
            }
#endif /* GFX_USE_GBM */
        if (runTime != 0)
            {
            (void)gettimeofday (&curtime, NULL);
            diffTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000) - startTime;
            }
        }

    /* Deinitialize demo */
    fboDeinit (pDemo);
    fboEglDeinit (pDemo);
    (void)fprintf (stdout, "DEMO COMPLETED\n");
    return (EXIT_SUCCESS);
    }
