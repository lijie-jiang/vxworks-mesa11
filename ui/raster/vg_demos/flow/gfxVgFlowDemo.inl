/* gfxVgFlowDemo.inl - Image Flow Vector Graphics Demo */

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
17apr15,yat  Fix image counter to be unsigned (V7GFX-240)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
18sep14,yat  Add support for dynamic RTP demos (US40486)
18jun14,yat  Fix build for tiimgt (US40486)
25jun13,mgc  Modified for VxWorks 7 release
13feb09,m_c  Released to Wind River engineering
20jul09,m_c  Written
*/

/*

DESCRIPTION

This example program demonstrates an Vector Graphics based "Image Flow" demo.  It
performs image loading and rendering with a few basic transformations.

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

#define VG_FLOW_ERROR_EGL           0
#define VG_FLOW_ERROR_OPENVG        1

#define NUM_IMAGES              ((int)sizeof(flowImage) / (int)sizeof(flowImage[0]))
#define NUM_VISIBLE_IMAGES      5
#define ANIMATION_STEPS         25
#define IMAGE_SCALE             0.5f

#define TWO_PI                  6.283185307179586476925286766559f
#define X_RADIUS_SCALE          0.25f
#define Z_RADIUS_SCALE          0.25f

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
    VGFont               font;
    } VG_FLOW_SCENE;

typedef struct
    {
    const char *    filename;
    VGImage         image;
    } VG_FLOW_IMAGE;

typedef struct
    {
    float x, z;
    VGImage image;
    } VG_FLOW_IMAGE_POS;

typedef enum
    {
    idleAct,
#if defined(GFX_USE_EVDEV)
    nextAct,
    prevAct
#else
    nextAct
#endif
    } VG_FLOW_ACTION;

/* locals */

/* Image database */
static VG_FLOW_IMAGE flowImage[] = {{"flow/data/01.jpg", VG_INVALID_HANDLE},
                                    {"flow/data/02.jpg", VG_INVALID_HANDLE},
                                    {"flow/data/03.jpg", VG_INVALID_HANDLE},
                                    {"flow/data/01.jpg", VG_INVALID_HANDLE},
                                    {"flow/data/02.jpg", VG_INVALID_HANDLE},
                                    {"flow/data/03.jpg", VG_INVALID_HANDLE}};

/* forward declarations */

IMPORT VGImage jpeg_load_vg (const char*, int);

/*******************************************************************************
 *
 * vgFlowReportError - reports on an EGL or Vector Graphics error
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
static void vgFlowReportError
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
    if (type == VG_FLOW_ERROR_EGL)
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
    else if (type == VG_FLOW_ERROR_OPENVG)
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
 * vgFlowEglDeinit - perform necessary steps to deinitialize
 *
 * This routine takes care of all the steps involved in cleaning-up EGL
 * resources. The EGL resources must be provided by the VG_FLOW_SCENE parameter
 * <pDemo>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgFlowEglInit
 *
 */
static void vgFlowEglDeinit
    (
    VG_FLOW_SCENE* pDemo
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
 * vgFlowEglInit - perform necessary steps to initialize
 *
 * This routine takes care of all the steps involved in initializing EGL,
 * creating a surface, and setting up the program for rendering.
 *
 * RETURNS: pointer to VG_FLOW_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgFlowEglDeinit
 *
 */
static VG_FLOW_SCENE* vgFlowEglInit
    (
    unsigned int device
    )
    {
    VG_FLOW_SCENE* pDemo;
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
    const float grey[4] = {0.4313f, 0.4313f, 0.4313f, 1.0f};

    /* Allocate basic structure to carry demo data */
    pDemo = (VG_FLOW_SCENE*)calloc(1, sizeof(VG_FLOW_SCENE));
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
        vgFlowReportError ("eglGetDisplay", VG_FLOW_ERROR_EGL);
        vgFlowEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglInitialize (pDemo->display, &eglMajor, &eglMinor))
        {
        vgFlowReportError ("eglInitialize", VG_FLOW_ERROR_EGL);
        vgFlowEglDeinit (pDemo);
        return (NULL);
        }

    if (!eglBindAPI (EGL_OPENVG_API))
        {
        vgFlowReportError ("eglBindAPI", VG_FLOW_ERROR_EGL);
        vgFlowEglDeinit (pDemo);
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
        vgFlowReportError ("eglChooseConfig", VG_FLOW_ERROR_EGL);
        vgFlowEglDeinit (pDemo);
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
        vgFlowReportError ("eglCreateWindowSurface", VG_FLOW_ERROR_EGL);
        vgFlowEglDeinit (pDemo);
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
        vgFlowReportError ("eglCreateContext", VG_FLOW_ERROR_EGL);
        vgFlowEglDeinit (pDemo);
        return (NULL);
        }

    /* Set the context as current */
    if (!eglMakeCurrent (pDemo->display, pDemo->surface, pDemo->surface, pDemo->context))
        {
        vgFlowReportError ("eglMakeCurrent", VG_FLOW_ERROR_EGL);
        vgFlowEglDeinit (pDemo);
        return (NULL);
        }

    (void)eglSwapInterval (pDemo->display, 1);

    /*
     * EGL Initialization Step 5: Clear screen
     * Clear the front, back, third buffers to begin with a clean slate
     */
    vgSetfv (VG_CLEAR_COLOR, 4, grey);
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
 * vgFlowDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgFlowInit
 *
 */
static void vgFlowDeinit
    (
    VG_FLOW_SCENE* pDemo
    )
    {
    int i;

    if (pDemo->font != VG_INVALID_HANDLE)
        {
        vgDestroyFont (pDemo->font);
        }
    for (i = 0; i < NUM_IMAGES; i++)
        {
        if (flowImage[i].image != VG_INVALID_HANDLE)
            {
            vgDestroyImage (flowImage[i].image);
            }
        }
    }

/*******************************************************************************
 *
 * vgFlowCompareSlot - compares 2 images based on their z-coordinate
 *
 * This routine will compare 2 images based on their z-coordinates in order to
 * facilitate sorting.
 *
 * RETURNS: 1 if z-coordinate is larger, -1 if z-coordinate is smaller,
 *          0 otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static int vgFlowCompareSlot
    (
    const VG_FLOW_IMAGE_POS* pSlot1,
    const VG_FLOW_IMAGE_POS* pSlot2
    )
    {
    if (pSlot1 != NULL)
        {
        if (pSlot2 != NULL)
            {
            if (pSlot1->z == pSlot2->z)
                {
                return (0);
                }
            else if (pSlot1->z < pSlot2->z)
                {
                return (1);
                }
            else
                {
                return (-1);
                }
             }
        }

    return (0);
    }

/*******************************************************************************
 *
 * vgFlowDisplayImages - renders a set of images along a carousel
 *
 * This routine will render a set of images at discrete positions along a
 * carousel path.  The images' positions can be manipulated using <relPos>
 * which causes the images to be rendered at a partial point between two
 * discrete positions.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void vgFlowDisplayImages
    (
    VG_FLOW_SCENE* pDemo,
    VGfloat relPos,
    VGfloat deltaPos,
    int front
    )
    {
    VGImage image = VG_INVALID_HANDLE;              /* image */
    VGfloat rotAngle = 0.0f;                        /* rotation angle */
    VGfloat scaleX = 0.0f;                          /* image X-scale */
    VGfloat scaleY = 0.0f;                          /* image Y-scale */
    VGfloat scale = 0.0f;                           /* scaling factor */
    VGfloat imgWidth = 0.0f;                        /* image width */
    VGfloat imgHeight = 0.0f;                       /* image height */
    VGfloat xRadius = 0.0f;                         /* width of the rotation */
    VGfloat zRadius = 0.0f;                         /* height variation in rotation */
    VG_FLOW_IMAGE_POS carousel[NUM_VISIBLE_IMAGES]; /* carousel */
    int i = 0;                                      /* counter */
    int j = 0;                                      /* counter */
    int k = 0;                                      /* counter */

    /* Clamp the relative position */
    if (relPos < 0.0f)
        {
        relPos = 0.0f;
        }
    else if (relPos > 1.0f)
        {
        relPos = 1.0f;
        }

    /* Generate the list of visible images */
    i = -(NUM_VISIBLE_IMAGES / 2);
    if (deltaPos > 0.0f)
        i -= (relPos > 0.75f);
    else if (deltaPos < 0.0f)
        i += (relPos < 0.25f);

    /* Calculate display metrics */
    xRadius = (VGfloat)pDemo->surfWidth * X_RADIUS_SCALE;
    zRadius = (VGfloat)pDemo->surfHeight * Z_RADIUS_SCALE;

    /* Recalculate the position of every image */
    for (j = 0; j < NUM_VISIBLE_IMAGES; i++, j++)
        {
        rotAngle = ((VGfloat)i + relPos) * (TWO_PI / (VGfloat)NUM_VISIBLE_IMAGES);
        carousel[j].x = -xRadius * (VGfloat) sin ((double)rotAngle);
        carousel[j].z = zRadius * (1.0f - (VGfloat)cos ((double)rotAngle));
        k = front + i;
        if (k < 0)
            k += NUM_IMAGES;
        k %= NUM_IMAGES;
        k = abs(k);
        carousel[j].image = flowImage[k].image;
        }

    /* Sort by z */
    qsort (carousel, NUM_VISIBLE_IMAGES, sizeof(VG_FLOW_IMAGE_POS),
           (FUNCPTR)vgFlowCompareSlot);

    /* Display images */
    vgSeti (VG_MATRIX_MODE, VG_MATRIX_IMAGE_USER_TO_SURFACE);
    for (i = 0; i < NUM_VISIBLE_IMAGES; i++)
        {
        image = carousel[i].image;
        if (image != VG_INVALID_HANDLE)
            {
            /* Get the image size */
            imgWidth = vgGetParameterf (image, VG_IMAGE_WIDTH);
            imgHeight = vgGetParameterf (image, VG_IMAGE_HEIGHT);

            /*
             * Scale the image based on the display size and its position in
             * the carousel
             */
            scaleX = (VGfloat)pDemo->surfWidth * IMAGE_SCALE / (VGfloat) imgWidth;
            scaleY = (VGfloat)pDemo->surfHeight * IMAGE_SCALE / (VGfloat) imgHeight;
            scale = (scaleX < scaleY) ? scaleX : scaleY;
            scale *= (1.0f - (0.6f * (carousel[i].z / (2 * zRadius))));

            vgLoadIdentity ();

            /* Scale the image and position it on the display */
            vgScale (scale, scale);
            vgTranslate ((((VGfloat)pDemo->surfWidth * 0.5f + carousel[i].x) / scale) - ((VGfloat)imgWidth / 2.0f),
                         ((VGfloat)pDemo->surfHeight * 0.5f / scale - (VGfloat)imgHeight / 2.0f));
            vgDrawImage (image);
            }
        }
    }

/*******************************************************************************
 *
 * vgFlowDrawString - renders a character buffer onto the current surface
 *
 * This routine will render the character found in <buffer> onto the current EGL
 * surface at the specified <x, y> coordinates and scaled according to the
 * <sx, sy> factors.  The font used to render the text is given by <font>.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static void vgFlowDrawString
    (
    const char* buffer,
    int x,
    int y,
    float sx,
    float sy,
    VGFont font
    )
    {
    VGint   charCnt;
    VGuint  indices[256];
    VGfloat origin[2];

    for (charCnt = 0; buffer[charCnt] != '\0'; charCnt++)
        {
        indices[charCnt] = buffer[charCnt];
        }

    /* Set the glyph origin */
    origin[0] = (VGfloat)x;
    origin[1] = (VGfloat)y;
    vgSetfv (VG_GLYPH_ORIGIN, 2, origin);

    /* Set the transformation matrix */
    vgSeti (VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);
    vgLoadIdentity ();
    vgScale (sx, sy);

    /* Draw */
    vgDrawGlyphs (font, charCnt, indices, NULL, NULL, VG_FILL_PATH, VG_TRUE);
    }

/*******************************************************************************
 *
 * vgFlowLoadFont - load a font for Vector Graphics use
 *
 * This routine takes care of all the steps involved in loading a font,
 * reading its glyph information, and making it available for Vector Graphics usage
 *
 * RETURNS: A valid VGFont on success, or VG_INVALID_HANDLE otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
static int vgFlowLoadFont
    (
    VG_FLOW_SCENE* pDemo
    )
    {
    VGPath fontPath = VG_INVALID_HANDLE;    /* font path */
    VGfloat origin[2] = {0.0, 0.0};         /* font origin coordinates */
    VGfloat escapement[2] = {0.0, 0.0};     /* font escapement coordinates */
    int i = 0;                              /* counter */
    int j = 0;                              /* counter */

    /* Font Creation */

    /* Font Creation Step 1: Create a new VG font */
    pDemo->font = vgCreateFont (NUM_ELEMENTS(fontInfo) / 4);
    if (pDemo->font == VG_INVALID_HANDLE)
        {
        vgFlowReportError ("vgCreateFont", VG_FLOW_ERROR_OPENVG);
        return FALSE;
        }

    origin[0] = 0.0;
    origin[1] = 0.0;
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
            vgFlowReportError ("vgCreatePath", VG_FLOW_ERROR_OPENVG);
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
                    vgFlowReportError ("vgAppendPathData", VG_FLOW_ERROR_OPENVG);
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
 * vgFlowInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: TRUE, or FALSE if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: vgFlowDeinit
 *
 */
static int vgFlowInit
    (
    VG_FLOW_SCENE* pDemo
    )
    {
    char buf[128];
    int i;

    /* Create a new font */
    if (vgFlowLoadFont (pDemo) == FALSE)
        {
        return FALSE;
        }

    /* Load bitmaps */
    for (i = 0; i < NUM_IMAGES; i++)
        {
        /* Draw loading string percentage */
        (void)snprintf (buf, 128, "Loading... %2d%%\r", (100 * i) / NUM_IMAGES);
        vgFlowDrawString ((const char*)buf, 5, 5, 0.75, 0.75, pDemo->font);
        (void)eglSwapBuffers (pDemo->display, pDemo->surface);

        if (flowImage[i].filename != NULL)
            {
            flowImage[i].image = jpeg_load_vg (flowImage[i].filename, VG_sXRGB_8888);
            if (flowImage[i].image == VG_INVALID_HANDLE)
                {
                /* Perform some clean-up and bail out */
                (void)fprintf (stderr, "Could not load image %s.\n", flowImage[i].filename);
                }
            }
        }

    vgFlowDrawString ((const char*)"Loading... done\n", 5, 5, 0.75, 0.75, pDemo->font);
    (void)eglSwapBuffers (pDemo->display, pDemo->surface);

    return TRUE;
    }

/*******************************************************************************
 *
 * vgFlowPerfCount - calculate and show performance
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
static void vgFlowPerfCount
    (
    VG_FLOW_SCENE* pDemo
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
 * vgFlowGetChar - gets an ASCII character from keyboard
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
static unsigned short vgFlowGetChar
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
 * gfxVgFlowDemo - core functionality of the program
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
static int gfxVgFlowDemo
    (
    unsigned int device,
    int runTime
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
    VG_FLOW_SCENE* pDemo;
    int front = 0;                      /* current front image */
    VGfloat relPos = 0.0f;              /* demo animation position*/
    VGfloat deltaPos = 0.0f;            /* demo animation */
    VG_FLOW_ACTION action = idleAct;    /* demo state */
    VG_FLOW_ACTION nextAction = nextAct;/* default demo change state */
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                    /* terminate flag */
#endif
    /* Initialize demo */
    pDemo = vgFlowEglInit (device);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "vgFlowEglInit failed\n");
        return (EXIT_FAILURE);
        }

    /* Initialize demo */
    if (vgFlowInit (pDemo) == FALSE)
        {
        (void)fprintf (stderr, "vgFlowInit failed\n");
        vgFlowDeinit (pDemo);
        vgFlowEglDeinit (pDemo);
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
            unsigned short character = vgFlowGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'z')
                {
                nextAction = nextAct;
                }
            else if (character == (unsigned short)'x')
                {
                nextAction = prevAct;
                }
            else if (character == (unsigned short)'q')
                {
                kbdQuit = 1;
                }
            }
#endif
        vgFlowPerfCount (pDemo);

        /* Start by clearing the screen */
        vgClear (0, 0, pDemo->surfWidth, pDemo->surfHeight);

        switch (action)
            {
            /* Idle */
            case idleAct:
                vgFlowDisplayImages (pDemo, relPos, deltaPos, front);

                /* keep moving to the next image */
                action = nextAction;
                relPos = 0.0f;
                deltaPos = 1.0f / (VGfloat)ANIMATION_STEPS;
                break;

            /* Next */
            case nextAct:
                relPos += deltaPos;
                if (relPos >= 1.0f)
                    {
                    action = idleAct;
                    front--;
                    relPos = 0.0f;
                    deltaPos = 0.0f;
                    }
                vgFlowDisplayImages (pDemo, relPos, deltaPos, front);
                break;
#if defined(GFX_USE_EVDEV)
            /* Previous */
            case prevAct:
                relPos += deltaPos;
                if (relPos <= 0.0f)
                    {
                    action = idleAct;
                    relPos = 0.0f;
                    deltaPos = 0.0f;
                    }
                vgFlowDisplayImages (pDemo, relPos, deltaPos, front);
                break;
#endif
            }

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
    vgFlowDeinit (pDemo);
    vgFlowEglDeinit (pDemo);
    (void)fprintf (stdout, "DEMO COMPLETED\n");
    return (EXIT_SUCCESS);
    }
