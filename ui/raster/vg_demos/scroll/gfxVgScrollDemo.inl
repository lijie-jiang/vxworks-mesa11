/* gfxVgScrollDemo.inl - Vector Graphics Scroll Demo */

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
26dec13,y_f  upgrade because the evdev redesigned
25jun13,mgc  Modified for VxWorks 7 release
13feb09,m_c  Released to Wind River engineering
23aug10,m_c  Written
*/

/*

DESCRIPTION

This example program demonstrates an Vector Graphics based "Text Scroll" demo.
It performs text rendering with a few basic transformations.

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
#include <math.h>
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

#define VG_SCROLL_ERROR_EGL           0
#define VG_SCROLL_ERROR_OPENVG        1

#define TWO_PI              6.283185307179586476925286766559f

#define ANIMATION_SPEED     6
#define MAX_TEXT_STATES     4
#define CHARACTER_WIDTH     32

/* Fonts Helpers */
#define NUM_ELEMENTS(_array)    (sizeof(_array) / sizeof((_array)[0]))
#define EVALUATOR(_x, _y)       PASTER(_x, _y)
#define PASTER(_x, _y)          _x ## _y

#if !defined(LUXIMR_FONT)
#define LUXIMR_FONT
#include "luximrFont.inl"
#define FONT_NAME               luximr
#define fontInfo                EVALUATOR(FONT_NAME, Info)
#define fontCoords              EVALUATOR(FONT_NAME, Coords)
#define fontEscapements         EVALUATOR(FONT_NAME, Escapements)
#define fontSegs                EVALUATOR(FONT_NAME, Segs)
#endif

/* Simple rule to bound an angle between 0 and TWO_PI */
#define CLAMP_ANGLE_RAD(_a) do                          \
                            {                           \
                                while ((_a) < 0.0)      \
                                    _a += TWO_PI;       \
                                while ((_a) > TWO_PI)   \
                                    _a -= TWO_PI;       \
                            } while (0)

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
    VGPaint              paint;
    VGFont               font;
    } VG_SCROLL_SCENE;

/* locals */

/* Scrolled text */
static char textScroll[] = "                        " \
                           "Welcome to the Wind River Text Scroll Demo!" \
                           "                        " \
                           "\001This demo shows how to render simple text using Vector Graphics." \
                           "                        " \
                           "\002Animation can quickly and easily be added to any project." \
                           "                        " \
                           "\003Matrix transformations give flare to even the simplest demos." \
                           "                        " \
                           "\004A pinch of color and things really start to liven up.  " \
                           "    This is 100% software rendering using Vector Graphics.  " \
                           "    Please go to http://www.windriver.com/ to find your local Wind River representative." \
                           "                        ";

/*******************************************************************************
 *
 * vgScrollReportError - reports on an EGL or Vector Graphics error
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
static void vgScrollReportError
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
    if (type == VG_SCROLL_ERROR_EGL)
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
    else if (type == VG_SCROLL_ERROR_OPENVG)
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
 * vgScrollCleanup - perform final cleanup
 *
 * This routine performs final cleanup
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgScrollEglDeinit
 *
 */
static void vgScrollCleanup
    (
    VG_SCROLL_SCENE* pDemo
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
 * vgScrollEglDeinit - perform necessary steps to deinitialize
 *
 * This routine takes care of all the steps involved in cleaning-up EGL
 * resources. The EGL resources must be provided by the VG_SCROLL_SCENE parameter
 * <pDemo>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgScrollEglInit
 *
 */
static void vgScrollEglDeinit
    (
    VG_SCROLL_SCENE* pDemo
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

    vgScrollCleanup (pDemo);
    }

/*******************************************************************************
 *
 * vgScrollEglInit - perform necessary steps to initialize
 *
 * This routine takes care of all the steps involved in initializing EGL,
 * creating a surface, and setting up the program for rendering.
 *
 * RETURNS: pointer to VG_SCROLL_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgScrollEglDeinit
 *
 */
static VG_SCROLL_SCENE* vgScrollEglInit
    (
    unsigned int device
    )
    {
    VG_SCROLL_SCENE* pDemo;
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
    pDemo = (VG_SCROLL_SCENE*)calloc (1, sizeof(VG_SCROLL_SCENE));
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
        vgScrollReportError ("eglGetDisplay", VG_SCROLL_ERROR_EGL);
        vgScrollEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglInitialize (pDemo->display, &eglMajor, &eglMinor))
        {
        vgScrollReportError ("eglInitialize", VG_SCROLL_ERROR_EGL);
        vgScrollEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglBindAPI (EGL_OPENVG_API))
        {
        vgScrollReportError ("eglBindAPI", VG_SCROLL_ERROR_EGL);
        vgScrollEglDeinit (pDemo);
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
        vgScrollReportError ("eglChooseConfig", VG_SCROLL_ERROR_EGL);
        vgScrollEglDeinit (pDemo);
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
        vgScrollReportError ("eglCreateWindowSurface", VG_SCROLL_ERROR_EGL);
        vgScrollEglDeinit (pDemo);
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
        vgScrollReportError ("eglCreateContext", VG_SCROLL_ERROR_EGL);
        vgScrollEglDeinit (pDemo);
        return (NULL);
        }

    /* Set the context as current */
    if (!eglMakeCurrent (pDemo->display, pDemo->surface, pDemo->surface, pDemo->context))
        {
        vgScrollReportError ("eglMakeCurrent", VG_SCROLL_ERROR_EGL);
        vgScrollEglDeinit (pDemo);
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
 * vgScrollDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgScrollInit
 *
 */
static void vgScrollDeinit
    (
    VG_SCROLL_SCENE* pDemo
    )
    {
    vgSetPaint (VG_INVALID_HANDLE, VG_FILL_PATH | VG_STROKE_PATH);
    if (pDemo->paint != VG_INVALID_HANDLE)
        {
        vgDestroyPaint (pDemo->paint);
        }
    if (pDemo->font != VG_INVALID_HANDLE)
        {
        vgDestroyFont (pDemo->font);
        }
    }

/*******************************************************************************
 *
 * vgScrollLoadFont - load a font for Vector Graphics use
 *
 * This routine takes care of all the steps involved in loading a font,
 * reading its glyph information, and making it available for Vector Graphics usage
 *
 * RETURNS: A valid VGFont on success, or VG_INVALID_HANDLE otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgScrollDemoUnloadFont
 *
 */
static int vgScrollLoadFont
    (
    VG_SCROLL_SCENE* pDemo
    )
    {
    VGPath fontPath = VG_INVALID_HANDLE;    /* font path */
    VGfloat origin[2] = {0.0f, 0.0f};       /* font origin coordinates */
    VGfloat escapement[2] = {0.0f, 0.0f};   /* font escapement coordinates */
    int i = 0;                              /* counter */
    int j = 0;                              /* counter */

    /* Font Creation */

    /* Font Creation Step 1: Create a new VG font */
    pDemo->font = vgCreateFont (NUM_ELEMENTS(fontInfo) / 4);
    if (pDemo->font == VG_INVALID_HANDLE)
        {
        vgScrollReportError ("vgCreateFont", VG_SCROLL_ERROR_OPENVG);
        return FALSE;
        }

    origin[0] = 0.0f;
    origin[1] = 0.0f;
    for (i = j = 0; j < NUM_ELEMENTS(fontInfo); i += 2, j += 4)
        {
        /*
         * Note: fontInfo[j + 0] = code point
         *       fontInfo[j + 0] = number of segments
         *       fontInfo[j + 0] = segments' index in fontSegs[]
         *       fontInfo[j + 0] = coordinates' index in fontCoords[]
         */

        /* Font Creation Step 2: Create a VG path for the current character */
        fontPath = vgCreatePath (VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,
                                 1.0, 0.0, 0, 0, VG_PATH_CAPABILITY_ALL);
        if (fontPath == VG_INVALID_HANDLE)
            {
            vgScrollReportError ("vgCreatePath", VG_SCROLL_ERROR_OPENVG);
            return FALSE;
            }
        else
            {
            if (fontInfo[j + 1] > 0)
                {
                /* Font Creation Step 3: Add font segments to path */
                vgAppendPathData (fontPath, fontInfo[j + 1],
                                  &fontSegs[fontInfo[j + 2]],
                                  &fontCoords[fontInfo[j + 3]]);
                if (vgGetError () != VG_NO_ERROR)
                    {
                    vgScrollReportError ("vgAppendPathData", VG_SCROLL_ERROR_OPENVG);
                    vgDestroyPath (fontPath);
                    return FALSE;
                    }
                }
            /* Font Creation Step 4: Add glyph to path */
            escapement[0] = fontEscapements[i];
            escapement[1] = fontEscapements[i + 1];
            vgSetGlyphToPath (pDemo->font, fontInfo[j], fontPath,
                              VG_TRUE, origin, escapement);
            vgDestroyPath (fontPath);
            }
        }

    return TRUE;
    }

/*******************************************************************************
 *
 * vgScrollInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: TRUE, or FALSE if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgScrollDeinit
 *
 */
static int vgScrollInit
    (
    VG_SCROLL_SCENE* pDemo
    )
    {
    const VGfloat stops[] = {0.0f,  0.4f, 0.0f, 0.6f, 0.80f,   /* VG color gradient stops */
                             0.25f, 0.9f, 0.5f, 0.1f, 0.45f,
                             0.50f, 0.8f, 0.8f, 0.0f, 1.0f,
                             0.75f, 0.0f, 0.3f, 0.5f, 0.90f,
                             1.0f,  0.4f, 0.4f, 0.6f, 0.80f};
    const VGfloat linGrad[] = {-60.0f, -20.0f, 60.0f, 20.0f}; /* VG linear gradients */

    /* Create a new font */
    if (vgScrollLoadFont (pDemo) == FALSE)
        {
        return FALSE;
        }

    /* Create a new paint */
    pDemo->paint = vgCreatePaint ();
    if (pDemo->paint == VG_INVALID_HANDLE)
        {
        vgScrollReportError ("vgCreatePaint", VG_SCROLL_ERROR_OPENVG);
        return FALSE;
        }
    /* Give the paint a linear gradient component */
    vgSetParameterfv (pDemo->paint, VG_PAINT_COLOR_RAMP_STOPS,
                      sizeof(stops) / sizeof(VGfloat), stops);
    vgSetParameterfv (pDemo->paint, VG_PAINT_LINEAR_GRADIENT, 4, linGrad);
    /* Give the paint a solid color component */
    vgSetColor (pDemo->paint, 0xFFFFFFFF);
    /* Select the paint to be applied as a solid fill color */
    vgSetParameteri (pDemo->paint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetPaint (pDemo->paint, VG_FILL_PATH);

    return TRUE;
    }

/*******************************************************************************
 *
 * vgScrollPerfCount - calculate and show performance
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
static void vgScrollPerfCount
    (
    VG_SCROLL_SCENE* pDemo
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
 * vgScrollDemoGetChar - gets an ASCII character from keyboard
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
static unsigned short vgScrollDemoGetChar
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
 * wave - computes a y-coordinate based on a wave function
 *
 * This routine will calculate the y-coordinate of a pixel, given its <x>
 * coordinate based on a wave function.
 *
 * RETURNS: Computed y-coordinate as a float
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static VGfloat wave (VGfloat x)
    {
    return (240.0f + ((30.0f * (VGfloat)cos ((double)((x - 1.0f) * 0.015f))) +
                      (20.0f * (VGfloat)cos ((double)((x + 132.0f) * 0.036f)))));
    }

/*******************************************************************************
 *
 * gfxVgScrollDemo - core functionality of the program
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
static int gfxVgScrollDemo
    (
    unsigned int device,
    int runTime
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
    VG_SCROLL_SCENE* pDemo;
    VGfloat origin[2] = {0.0f, 0.0f};   /* VG font origin coordinates */
    unsigned short character = 0;
    int i = 0;                          /* counter */
    int dx = 0;                         /* scroll x-position */
    int dy =0;                          /* scroll y-position */
    int pos = 0;                        /* scroll text position */
    unsigned short scrollState = 0;     /* scroll style indicator */
    int resetScroll = 0;                /* wrap-around the scroller */
    float bounce = 0.0f;                /* scroll bounce animation*/
    float angle = 0.0f;                 /* scroll text angle */
    float colorAngle = 0.0f;            /* scroll color animation effect */
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                    /* terminate flag */
#endif
    /* Initialize demo */
    pDemo = vgScrollEglInit (device);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "vgScrollEglInit failed\n");
        return (EXIT_FAILURE);
        }

    /* Initialize demo */
    if (vgScrollInit (pDemo) == FALSE)
        {
        (void)fprintf (stderr, "vgScrollInit failed\n");
        vgScrollDeinit (pDemo);
        vgScrollEglDeinit (pDemo);
        return (EXIT_FAILURE);
        }

    dy = pDemo->surfHeight / 2;

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
        vgScrollPerfCount (pDemo);

        /* Start by clearing the screen */
        vgClear (0, 0, pDemo->surfWidth, pDemo->surfHeight);

        /*
         * Iterate over the width of the display and
         * render the needed characters
         */
        for (dx = 0 - (pos & (CHARACTER_WIDTH-1)), i = pos / CHARACTER_WIDTH;
                dx < pDemo->surfWidth; dx += CHARACTER_WIDTH, i++)
            {
#if defined(GFX_USE_EVDEV)
            /* Grab a character from keyboard */
            if (dx == pDemo->surfWidth/2)
                {
                if (pDemo->evDevFd > 0)
                    {
                    character = vgScrollDemoGetChar (pDemo->evDevFd);
                    if (character) textScroll[i] = (char)character;
                    if (character == (unsigned short)'q') kbdQuit = 1;
                    }
                }
#endif
            /* Grab the next character */
            character = (unsigned short)textScroll[i];

            /* Check to see if a state change is required */
            if (character == 0)
                {
                /* Null character reached, restart at the beginning */
                i = 0;
                resetScroll = TRUE;
                scrollState = character;
                character = (unsigned short)textScroll[0];
                }
            else if (character <= MAX_TEXT_STATES)
                {
                /* Set the new scroll state and fetch the next character */
                scrollState = character;
                character = (unsigned short)textScroll[++i];
                }

            /* State 2+: Get the text moving in a wave */
            if (scrollState >= 2)
                {
                dy = (int)wave ((VGfloat)dx);
                }

            /* Set the location where the glyph will get rendered */
            origin[0] = (VGfloat)dx;
            origin[1] = (VGfloat)dy;
            vgSetfv (VG_GLYPH_ORIGIN, 2, origin);

            /* Set the transformation matrix to manipulate the glyph */
            vgSeti (VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);
            vgLoadIdentity ();

            /* State 3+: Add additional rotation to the text */
            if (scrollState >= 3)
                {
                angle = (VGfloat)atan2 ((double)((VGfloat)dy - wave ((VGfloat)dx-10.0f)), (double)(10.0f)) * 360.0f / TWO_PI;
                vgRotate (angle);
                }

            /* Draw the character */
            vgScale (3.0, 3.0);
            vgDrawGlyph (pDemo->font, character, VG_FILL_PATH, VG_TRUE);
            }

        /* Move the banner */
        pos += ANIMATION_SPEED;

        /* State 0: A one time reset and wrap-around of the scroller */
        if (resetScroll)
            {
            /* Reset the position and the paint type */
            vgSetParameteri (pDemo->paint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
            pos &= CHARACTER_WIDTH;
            resetScroll = FALSE;
            }

        /* State 1+: Move the text up and down the display */
        if (scrollState >= 1)
            {
            dy = (int)fabs ((double)(((VGfloat)pDemo->surfHeight / 2.0f) * (VGfloat)cos ((double)bounce)));
            bounce += 0.02f;
            CLAMP_ANGLE_RAD(bounce);
            }

        /* State 4+: Add color gradient to the text */
        if (scrollState >= 4)
            {
            /* Set the fill paint to use it linear gradient component */
            vgSetParameteri (pDemo->paint, VG_PAINT_TYPE,
                             VG_PAINT_TYPE_LINEAR_GRADIENT);
            /* Set the transformation matrix to manipulate the color */
            vgSeti (VG_MATRIX_MODE, VG_MATRIX_FILL_PAINT_TO_USER);
            vgLoadIdentity ();

            /* Cycle the color gradient */
            vgScale (0.1f, 0.1f);
            vgRotate (colorAngle * 360.0f / TWO_PI);
            colorAngle += 0.05f;
            CLAMP_ANGLE_RAD (colorAngle);
            }

        if (eglSwapBuffers (pDemo->display, pDemo->surface) != EGL_TRUE)
            {
            (void)fprintf (stderr, "eglSwapBuffers failed\n");
            vgScrollCleanup (pDemo);
            return (EXIT_FAILURE);
            }

        if (runTime != 0)
            {
            (void)gettimeofday (&curtime, NULL);
            diffTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000) - startTime;
            }
        }

    /* Deinitialize demo */
    vgScrollDeinit (pDemo);
    vgScrollEglDeinit (pDemo);
    (void)fprintf (stdout, "DEMO COMPLETED\n");
    return (EXIT_SUCCESS);
    }
