/* gfxVgImageDemo.inl - Vector Graphics Image Load Demo */

/*
 * Copyright (c) 2013-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24feb16,yat  Fix static analysis defects (US75033)
10sep15,yat  Add missing numConfigs check after eglChooseConfig (V7GFX-279)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
18sep14,yat  Add support for dynamic RTP demos (US40486)
18jun14,yat  Fix build for tiimgt (US40486)
25jun13,mgc  Modified for VxWorks 7 release
15apr13,af   Written (based on Image Demo written by m_c)
*/

/*

DESCRIPTION

This example program demonstrates the concepts in loading and rendering PNG and
JPEG images using Vector Graphics.

*/

/* includes */

#if defined(__vxworks)
#include <vxWorks.h>
#include <ioLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <rtpLib.h>
#include <fbdev.h>
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
#include <strings.h>
#include <VG/openvg.h>
#include <EGL/egl.h>
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

#define VG_IMAGE_ERROR_EGL           0
#define VG_IMAGE_ERROR_OPENVG        1

#define JPEG_IMAGE                  "image/data/01.jpg"
#define PNG_IMAGE                   "image/data/02.png"

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
#if defined(_FSLVIV)
    EGLNativeDisplayType eglNativeDisplay;  /* Used for non-WR EGL */
    EGLNativeWindowType  eglNativeWindow;   /* Used for non-WR EGL */
#endif
#if defined(GFX_USE_EVDEV)
    int                  evDevFd;
#endif
    long                 tRate0;
    long                 frames;
    VGImage              jpgImage;
    VGImage              pngImage;
    } VG_IMAGE_SCENE;

/* forward declarations */

IMPORT VGImage png_load_vg (const char*, int);
IMPORT VGImage jpeg_load_vg (const char*, int);

/*******************************************************************************
 *
 * vgImageReportError - reports on an EGL or Vector Graphics error
 *
 * This routine will print to stderr a formated error string based on the type
 * error reported by <type>.  The string will also contain the function name of
 * the culprit routine given by <pErrorStr>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void vgImageReportError
    (
    const char * pErrorStr,
    int type
    )
    {
    static const char * eglErrStrings[] = {"EGL_SUCCESS",
                                           "EGL_NOT_INITIALIZED",
                                           "EGL_BAD_ACCESS",
                                           "EGL_BAD_ALLOC",
                                           "EGL_BAD_ATTRIBUTE",
                                           "EGL_BAD_CONFIG",
                                           "EGL_BAD_CONTEXT",
                                           "EGL_BAD_CURRENT_SURFACE",
                                           "EGL_BAD_DISPLAY",
                                           "EGL_BAD_MATCH",
                                           "EGL_BAD_NATIVE_PIXMAP",
                                           "EGL_BAD_NATIVE_WINDOW",
                                           "EGL_BAD_PARAMETER",
                                           "EGL_BAD_SURFACE",
                                           "EGL_CONTEXT_LOST"};
    static const char * vgErrStrings[] = {"VG_BAD_HANDLE_ERROR",
                                          "VG_ILLEGAL_ARGUMENT_ERROR",
                                          "VG_OUT_OF_MEMORY_ERROR",
                                          "VG_PATH_CAPABILITY_ERROR",
                                          "VG_UNSUPPORTED_IMAGE_FORMAT_ERROR",
                                          "VG_UNSUPPORTED_PATH_FORMAT_ERROR",
                                          "VG_IMAGE_IN_USE_ERROR",
                                          "VG_NO_CONTEXT_ERROR"};
    int errNum = -1;

    (void)fprintf (stderr, "%s () returned ", pErrorStr);
    /* Report EGL error */
    if (type == VG_IMAGE_ERROR_EGL)
        {
        errNum = eglGetError ();
        if ((errNum < EGL_SUCCESS) || (errNum > EGL_CONTEXT_LOST))
            {
            (void)fprintf (stderr, "an unknown error");
            }
        else
            {
            (void)fprintf (stderr, "%s", eglErrStrings[errNum - EGL_SUCCESS]);
            }
        }
    /* Report Vector Graphics error */
    else if (type == VG_IMAGE_ERROR_OPENVG)
        {
        errNum = vgGetError ();
        if (errNum == VG_NO_ERROR)
            {
            (void)fprintf (stderr, "VG_NO_ERROR");
            }
        else if ((errNum < VG_BAD_HANDLE_ERROR) || (errNum > VG_NO_CONTEXT_ERROR))
            {
            (void)fprintf (stderr, "an unknown error");
            }
        else
            {
            (void)fprintf (stderr, "%s", vgErrStrings[errNum - VG_BAD_HANDLE_ERROR]);
            }
        }
    else
        {
        errNum = -1;
        }

    (void)fprintf (stderr, " (0x%x)\n", errNum);
    }

/*******************************************************************************
 *
 * vgImageEglDeinit - perform necessary steps to deinitialize
 *
 * This routine takes care of all the steps involved in cleaning-up EGL
 * resources. The EGL resources must be provided by the VG_IMAGE_SCENE parameter
 * <pDemo>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgImageEglInit
 *
 */
static void vgImageEglDeinit
    (
    VG_IMAGE_SCENE* pDemo
    )
    {
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
 * vgImageEglInit - perform necessary steps to initialize
 *
 * This routine takes care of all the steps involved in initializing EGL,
 * creating a surface, and setting up the program for rendering.
 *
 * RETURNS: pointer to VG_IMAGE_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgImageEglDeinit
 *
 */
static VG_IMAGE_SCENE* vgImageEglInit
    (
    unsigned int device
    )
    {
    VG_IMAGE_SCENE* pDemo;
    EGLint      eglMajor, eglMinor;
    EGLint      configAttribs[] =
        {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
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
    const float black[4] = {0.0, 0.0, 0.0, 0.0};

    /* Allocate basic structure to carry demo data */
    pDemo = (VG_IMAGE_SCENE*)calloc (1, sizeof(VG_IMAGE_SCENE));
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "calloc failed\n");
        return (NULL);
        }

    pDemo->tRate0 = -1;

    /* EGL Initialization */

    /*
     * EGL Initialization Step 1:  Initialize display
     *
     * The following lines take care of opening the default display
     * (typically /dev/fb0) and setting up the Vector Graphics library.
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
    pDemo->display = eglGetDisplay (EGL_DEFAULT_DISPLAY);
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
        /* May occur if the driver is not correctly configured */
        vgImageReportError ("eglGetDisplay", VG_IMAGE_ERROR_EGL);
        vgImageEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglInitialize (pDemo->display, &eglMajor, &eglMinor))
        {
        vgImageReportError ("eglInitialize", VG_IMAGE_ERROR_EGL);
        vgImageEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglBindAPI (EGL_OPENVG_API))
        {
        vgImageReportError ("eglBindAPI", VG_IMAGE_ERROR_EGL);
        vgImageEglDeinit (pDemo);
        return (NULL);
        }
    /*
     * EGL Initialization Step 2: Choose an EGL configuration
     * Query the Vector Graphics library (and fbdev driver) to find a compatible
     * configuration.
     */
    if (!eglChooseConfig (pDemo->display, configAttribs, &eglConfig, 1, &numConfigs) || (numConfigs <= 0))
        {
        /* Driver may not support a 32-bit configuration */
        vgImageReportError ("eglChooseConfig", VG_IMAGE_ERROR_EGL);
        vgImageEglDeinit (pDemo);
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
#else
    pDemo->surface = eglCreateWindowSurface (pDemo->display, eglConfig,
                                             NULL, NULL);
#endif
    if (pDemo->surface == EGL_NO_SURFACE)
        {
        vgImageReportError ("eglCreateWindowSurface", VG_IMAGE_ERROR_EGL);
        vgImageEglDeinit (pDemo);
        return (NULL);
        }
    (void)eglQuerySurface (pDemo->display, pDemo->surface,
                     EGL_WIDTH, &(pDemo->surfWidth));
    (void)eglQuerySurface (pDemo->display, pDemo->surface,
                     EGL_HEIGHT, &(pDemo->surfHeight));

    /*
     * EGL Initialization Step 4: Setup the context
     * Create a rendering context and set it as the current context being used
     */
    pDemo->context = eglCreateContext (pDemo->display, eglConfig,
                                       EGL_NO_CONTEXT, NULL);
    if (pDemo->context == EGL_NO_CONTEXT)
        {
        vgImageReportError ("eglCreateContext", VG_IMAGE_ERROR_EGL);
        vgImageEglDeinit (pDemo);
        return (NULL);
        }

    /* Set the context as current */
    if (!eglMakeCurrent (pDemo->display, pDemo->surface, pDemo->surface, pDemo->context))
        {
        vgImageReportError ("eglMakeCurrent", VG_IMAGE_ERROR_EGL);
        vgImageEglDeinit (pDemo);
        return (NULL);
        }

    (void)eglSwapInterval (pDemo->display, 1);

    /*
     * EGL Initialization Step 5: Clear screen
     * Clear the front, back, third buffers to begin with a clean slate
     */
    vgSetfv (VG_CLEAR_COLOR, 4, black);
    vgClear (0, 0, pDemo->surfWidth, pDemo->surfHeight);
    (void)eglSwapBuffers (pDemo->display, pDemo->surface);
    vgClear (0, 0, pDemo->surfWidth, pDemo->surfHeight);
    (void)eglSwapBuffers (pDemo->display, pDemo->surface);
    vgClear (0, 0, pDemo->surfWidth, pDemo->surfHeight);

    vgSeti (VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
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
 * vgImageDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgImageInit
 *
 */
static void vgImageDeinit
    (
    VG_IMAGE_SCENE* pDemo
    )
    {
    if (pDemo->jpgImage != VG_INVALID_HANDLE)
        {
        vgDestroyImage (pDemo->jpgImage);
        }
    if (pDemo->pngImage != VG_INVALID_HANDLE)
        {
        vgDestroyImage (pDemo->pngImage);
        }
    }

/*******************************************************************************
 *
 * vgImageInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgImageDeinit
 *
 */
static int vgImageInit
    (
    VG_IMAGE_SCENE* pDemo
    )
    {
    /* Load JPEG image */
    pDemo->jpgImage = jpeg_load_vg(JPEG_IMAGE, VG_sXRGB_8888);
    if (pDemo->jpgImage == VG_INVALID_HANDLE)
        {
        (void)fprintf (stderr, "Could not load image: %s.\n", JPEG_IMAGE);
        }

    /* Load PNG image */
    pDemo->pngImage = png_load_vg(PNG_IMAGE, VG_sARGB_8888_PRE);
    if (pDemo->pngImage == VG_INVALID_HANDLE)
        {
        (void)fprintf (stderr, "Could not load image: %s.\n", PNG_IMAGE);
        }

    vgSeti (VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);

    return TRUE;
    }

/*******************************************************************************
 *
 * vgImagePerfCount - calculate and show performance
 *
 * This routine calculates and displays the number of frames rendered every
 * 5 seconds.
 *
 * RETURNS: TRUE, or FALSE if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void vgImagePerfCount
    (
    VG_IMAGE_SCENE* pDemo
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
 * vgImageGetChar - gets an ASCII character from keyboard
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
static unsigned short vgImageGetChar
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
 * gfxVgImageDemo - core functionality of the program
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
static int gfxVgImageDemo
    (
    unsigned int device,
    int runTime
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
    VG_IMAGE_SCENE* pDemo;
    VGint imgWidth = 0;                     /* VG image width */
    VGint imgHeight = 0;                    /* VG image height */
    VGfloat scaleX = 0.0f;                  /* image X-scale */
    VGfloat scaleY = 0.0f;                  /* image Y-scale */
    VGfloat scale = 0.0f;                   /* acutal image scaling factor */
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                        /* terminate flag */
#endif
    /* Initialize demo */
    pDemo = vgImageEglInit (device);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "vgImageEglInit failed\n");
        return (EXIT_FAILURE);
        }

    /* Initialize demo */
    if (vgImageInit (pDemo) == FALSE)
        {
        (void)fprintf (stderr, "vgImageInit failed\n");
        vgImageDeinit (pDemo);
        vgImageEglDeinit (pDemo);
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
            unsigned short character = vgImageGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        vgImagePerfCount (pDemo);

        vgLoadIdentity ();

        /* Draw JPEG image */
        imgWidth = vgGetParameteri (pDemo->jpgImage, VG_IMAGE_WIDTH);
        imgHeight = vgGetParameteri (pDemo->jpgImage, VG_IMAGE_HEIGHT);

        /* Determine image scale */
        scaleX = (VGfloat)(pDemo->surfWidth>>1) / (VGfloat)imgWidth;
        scaleY = (VGfloat)pDemo->surfHeight / (VGfloat)imgHeight;
        scale = (scaleX < scaleY) ? scaleX : scaleY;

        /* Position image on the left side of the display */
        vgTranslate ((VGfloat)pDemo->surfWidth / 4.0f - (VGfloat)imgWidth * scale / 2.0f,
                     (VGfloat)pDemo->surfHeight / 2.0f - (VGfloat)imgHeight * scale / 2.0f);

        vgScale (scale, scale);

        vgDrawImage (pDemo->jpgImage);

        vgLoadIdentity ();

        /* Draw PNG image */
        imgWidth = vgGetParameteri (pDemo->pngImage, VG_IMAGE_WIDTH);
        imgHeight = vgGetParameteri (pDemo->pngImage, VG_IMAGE_HEIGHT);

        /* Determine image scale */
        scaleX = (VGfloat)(pDemo->surfWidth>>1) / (VGfloat)imgWidth;
        scaleY = (VGfloat)pDemo->surfHeight / (VGfloat)imgHeight;
        scale = (scaleX < scaleY) ? scaleX : scaleY;

        /* Position image on the right side of the display */
        vgTranslate ((VGfloat)pDemo->surfWidth * 3.0f / 4.0f - (VGfloat)imgWidth * scale / 2.0f,
                     (VGfloat)pDemo->surfHeight / 2.0f - (VGfloat)imgHeight * scale / 2.0f);
        vgScale (scale, scale);

        vgDrawImage (pDemo->pngImage);

        if (eglSwapBuffers (pDemo->display, pDemo->surface) != EGL_TRUE)
            {
            (void)fprintf (stderr, "eglSwapBuffers failed\n");
            break;
            }

        if (runTime != 0)
            {
            (void)gettimeofday (&curtime, NULL);
            diffTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000) - startTime;
            }
        }

    /* Deinitialize demo */
    vgImageDeinit (pDemo);
    vgImageEglDeinit (pDemo);
    (void)fprintf (stdout, "DEMO COMPLETED\n");
    return (EXIT_SUCCESS);
    }
