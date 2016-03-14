/* gfxFbCopyTest.inl - Frame buffer copy test */

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
17feb16,yat  Add frame buffer copy test (US73758)
*/

/*

DESCRIPTION

This program provides the frame buffer copy program.

*/

/* includes */

#if defined(__vxworks)
#include <vxWorks.h>
#include <ioLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <rtpLib.h>
#if defined(_WRS_KERNEL)
#include <vmLib.h>
#endif
#if defined(_UGL)
#include <ugl/ugl.h>
#include <ugl/uglos.h>
#include <ugl/ugldib.h>
#include <ugl/driver/graphics/generic/udgen.h>
#elif defined(_FBDEV)
#include <fbdev.h>
#endif
#else
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#define FB_MAX_STR_CHARS            128
#define FB_DEVICE_PREFIX            "/dev/fb"
#define FB_MAX_DEVICE               5
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif /* __vxworks */
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if defined(GFX_USE_EVDEV)
#include <evdevLib.h>
#endif

/* defines */

#define NPOINTS         4000
#include "point.inl"

#define COLOR_START     0xFFFF0000

/* typedefs */

/* Data structure that is attached to the window, provides all
 * data required to display and manage the program.
 */

typedef struct
    {
#if defined(_UGL)
    UGL_DEVICE_ID       devId;
    UGL_GC_ID           gc;
#else
    int                 fbDevFd;
#endif
#if defined(GFX_USE_EVDEV)
    int                 evDevFd;
#endif
    unsigned int        width, stride, height, bpp;
    unsigned int        fbSize;
    void*               frontBuf;
    int                 point[NPOINTS];
    } FB_COPY_SCENE;

static void fbCopyRender
    (
    FB_COPY_SCENE *pDemo,
    unsigned int left,
    unsigned int top,
    unsigned int right,
    unsigned int bottom,
    unsigned int color
    )
    {
    unsigned char *pBuf0;
    unsigned int x, y;

    pBuf0 = (unsigned char *)pDemo->frontBuf;
    pBuf0 += pDemo->stride * top;
    pBuf0 += left * (pDemo->bpp >> 3);
    for (y = top; y <= bottom; y++)
        {
        unsigned int *pBuf = (unsigned int *)pBuf0;
        for (x = left; x < right; x++)
            {
            *pBuf++ = color;
            }
        pBuf0 += pDemo->stride;
        }
    }

/*******************************************************************************
 *
 * fbCopyDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fbCopyInit
 *
 */
static void fbCopyDeinit
    (
    FB_COPY_SCENE *pDemo
    )
    {
#if defined(_UGL)
    if (pDemo->gc)
        {
        uglGcDestroy (pDemo->gc);
        }

    uglDisplayClose (NULL);
#else
    if (pDemo->fbDevFd > 0)
        {
        (void)close (pDemo->fbDevFd);
        }
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
 * fbCopyInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: pointer to FB_DRAW_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fbCopyDeinit
 *
 */
static FB_COPY_SCENE* fbCopyInit
    (
    unsigned int device
    )
    {
    FB_COPY_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    char evDevName[EV_DEV_NAME_LEN + 1];
    unsigned int e;
#endif
    /* Allocate basic structure to carry demo data */
    pDemo = (FB_COPY_SCENE*)calloc (1, sizeof (FB_COPY_SCENE));
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "calloc failed\n");
        return (NULL);
        }
#if defined(__vxworks)
#if defined(_UGL)
    UGL_STATUS status;
    UGL_UINT32 displayNumber;
    char * pDisplayName = UGL_NULL;
    UGL_REG_DATA *pRegistryData;
    UGL_FB_INFO fbInfo;

    status = uglDisplayOpen (NULL, &pDisplayName, &displayNumber);
    if (status == UGL_STATUS_ERROR)
        {
        (void)fprintf (stderr, "Display failed to initialize\n");
        fbCopyDeinit (pDemo);
        return (NULL);
        }
    else if (status == UGL_STATUS_FINISHED)
        {
        (void)fprintf (stderr, "Display already initialized\n");
        fbCopyDeinit (pDemo);
        return (NULL);
        }

    pRegistryData = uglRegistryFind (UGL_DISPLAY_TYPE, &displayNumber, 0, 0);
    if (pRegistryData == UGL_NULL)
        {
        (void)fprintf (stderr, "Display not found. Exiting.\n");
        fbCopyDeinit (pDemo);
        return (NULL);
        }

    pDemo->devId = (UGL_DEVICE_ID)pRegistryData->id;

    pDemo->gc = uglGcCreate (pDemo->devId);

    uglInfo (pDemo->devId, UGL_FB_INFO_REQ, &fbInfo);

    pDemo->width = fbInfo.width;
    pDemo->stride = fbInfo.width * 4;
    pDemo->height = fbInfo.height;
    pDemo->bpp = 32;
    UGL_GENERIC_DRIVER * pGenDriver = (UGL_GENERIC_DRIVER*)pDemo->gc->pDriver;
    pDemo->frontBuf = (void*)pGenDriver->fbAddress;
#elif defined(_FBDEV)
    char deviceName[FB_MAX_STR_CHARS];
    unsigned int i;
    FB_IOCTL_ARG arg;

    /* Open a frame buffer device */
    for (i = device; i < FB_MAX_DEVICE; i++)
        {
        (void)snprintf (deviceName, FB_MAX_STR_CHARS, "%s%d", FB_DEVICE_PREFIX, i);
        if ((pDemo->fbDevFd = open (deviceName, O_RDWR, 0666)) != -1)
            {
            break;
            }
        }
    if (i >= FB_MAX_DEVICE)
        {
        (void)fprintf (stderr, "Open device %s errno:%08x\n", deviceName, errno);
        fbCopyDeinit (pDemo);
        return (NULL);
        }
    (void)fprintf (stdout, "Open device:%s\n", deviceName);

    if (ioctl (pDemo->fbDevFd, FB_IOCTL_GET_FB_INFO, &arg) == -1)
        {
        (void)fprintf (stderr, "ERROR: FB_IOCTL_GET_FB_INFO\n");
        fbCopyDeinit (pDemo);
        return (NULL);
        }

    pDemo->width = arg.getFbInfo.width;
    pDemo->stride = arg.getFbInfo.stride;
    pDemo->height = arg.getFbInfo.height;
    pDemo->bpp = arg.getFbInfo.bpp;
    pDemo->frontBuf = arg.getFbInfo.pFb;
#else
    pDemo->width = 1024;
    pDemo->stride = 1024 * 4;
    pDemo->height = 768;
    pDemo->bpp = 32;
#endif
#else
    char deviceName[FB_MAX_STR_CHARS];
    unsigned int i;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;

    /* Open a frame buffer device */
    for (i = device; i < FB_MAX_DEVICE; i++)
        {
        (void)snprintf (deviceName, FB_MAX_STR_CHARS, "%s%d", FB_DEVICE_PREFIX, i);
        if ((pDemo->fbDevFd = open (deviceName, O_RDWR)) != -1)
            {
            break;
            }
        }
    if (i >= FB_MAX_DEVICE)
        {
        (void)fprintf (stderr, "Open device %s errno:%08x\n", deviceName, errno);
        fbCopyDeinit (pDemo);
        return (NULL);
        }
    (void)fprintf (stdout, "Open device:%s\n", deviceName);

    if (ioctl (pDemo->fbDevFd, FBIOGET_FSCREENINFO, &finfo) == -1)
        {
        (void)fprintf (stderr, "Read fixed information error\n");
        fbCopyDeinit (pDemo);
        return (NULL);
        }

    if (ioctl (pDemo->fbDevFd, FBIOGET_VSCREENINFO, &vinfo) == -1)
        {
        (void)fprintf (stderr, "Read variable information error\n");
        fbCopyDeinit (pDemo);
        return (NULL);
        }

    pDemo->width = vinfo.xres;
    pDemo->stride = vinfo.xres * (vinfo.bits_per_pixel >> 3);
    pDemo->height = vinfo.yres;
    pDemo->bpp = vinfo.bits_per_pixel;
    pDemo->frontBuf = (void *)mmap (0, pDemo->stride * pDemo->height,
                                    PROT_READ | PROT_WRITE, MAP_SHARED,
                                    pDemo->fbDevFd, 0);
#endif /* __vxworks */
    pDemo->fbSize = pDemo->stride * pDemo->height;

    (void)fprintf (stdout, "Frame buffer info\n\tfrontBuf:%p\n\tbpp:%d\n\twidth:%d\n\tstride:%d\n\theight:%d\n\tfbSize:%d\n",
            pDemo->frontBuf,
            pDemo->bpp,
            pDemo->width, pDemo->stride, pDemo->height,
            pDemo->fbSize);
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

#if defined(GFX_USE_EVDEV)
/*******************************************************************************
 *
 * fbCopyGetChar - gets an ASCII character from keyboard
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
static unsigned short fbCopyGetChar
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
                    (void)printf ("Syn x=%d y=%d slot=%d id=%d\n", m_x, m_y, m_slot, m_id);
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
                            (void)printf ("Key [%04x:%c] [%08x]\n", evDevEvent.code, evDevEvent.code, evDevEvent.value);
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
 * gfxFbCopyTest - core functionality of the program
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
int gfxFbCopyTest
    (
    unsigned int device,
    int runTime,
    int mode
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
    FB_COPY_SCENE *pDemo;
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                                /* terminate flag */
#endif
    long copyStartTime;
    long copyEndTime;
    unsigned int color = COLOR_START;
    unsigned int c, i;
    int *p;
    int area;

    /* Initialize demo */
    pDemo = fbCopyInit (device);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "fbCopyInit failed\n");
        return (EXIT_FAILURE);
        }
#if 0
    srand(0);
#endif
    area = 0;
    p = pDemo->point;
    for (i = 0; i < NPOINTS; i += 4, p += 4)
        {
#if 0
        int w1 = rand() % 1000;
        int h1 = rand() % 1000;
        int w2 = rand() % 1000;
        int h2 = rand() % 1000;
        (void)printf ("%d, %d, %d, %d,\n", w1, h1, w2, h2);
#else
        int w1 = point[i];
        int h1 = point[i + 1];
        int w2 = point[i + 2];
        int h2 = point[i + 3];
#endif
        w1 = w1 * pDemo->width / 1000;
        h1 = h1 * pDemo->height / 1000;
        w2 = w2 * pDemo->width / 1000;
        h2 = h2 * pDemo->height / 1000;
        *p = min(w1, w2);
        *(p+1) = min(h1, h2);
        *(p+2) = max(w1, w2);
        *(p+3) = max(h1, h2);
        area = (*(p+2) - *p) * (*(p+3) - *(p+1));
        }

    (void)fprintf (stdout, "\tarea:%d\n", area);
    (void)fprintf (stdout, "\tmode:%d\n", mode);
    (void)fprintf (stdout, "\tsizeoflong:%ld\n", sizeof(long));
    if (mode == 0)
        {
        }
    else if (mode == 10)
        {
        pDemo->frontBuf = (void *)malloc (pDemo->fbSize);
        }
#if defined(_WRS_KERNEL)
    else if (mode == 1)
        {
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE_NOT);
        }
    else if (mode == 2)
        {
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE);
        }
    else if (mode == 3)
        {
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    MMU_ATTR_SPL_MSK, MMU_ATTR_SPL_1);
        }
    else if (mode == 4)
        {
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE_NOT);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    MMU_ATTR_SPL_MSK, MMU_ATTR_SPL_1);
        }
    else if (mode == 5)
        {
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE_NOT);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    MMU_ATTR_SPL_MSK, MMU_ATTR_SPL_1);
        }
    else if (mode == 11)
        {
        pDemo->frontBuf = (void *)memalign (vmPageSizeGet(), pDemo->fbSize);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE_NOT);
        }
    else if (mode == 12)
        {
        pDemo->frontBuf = (void *)memalign (vmPageSizeGet(), pDemo->fbSize);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE);
        }
    else if (mode == 13)
        {
        pDemo->frontBuf = (void *)memalign (vmPageSizeGet(), pDemo->fbSize);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    MMU_ATTR_SPL_MSK, MMU_ATTR_SPL_1);
        }
    else if (mode == 14)
        {
        pDemo->frontBuf = (void *)memalign (vmPageSizeGet(), pDemo->fbSize);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE_NOT);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    MMU_ATTR_SPL_MSK, MMU_ATTR_SPL_1);
        }
    else if (mode == 15)
        {
        pDemo->frontBuf = (void *)memalign (vmPageSizeGet(), pDemo->fbSize);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE_NOT);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE);
        vmStateSet (NULL, (VIRT_ADDR)pDemo->frontBuf, pDemo->fbSize,
                    MMU_ATTR_SPL_MSK, MMU_ATTR_SPL_1);
        }
#endif
    else
        {
        (void)fprintf (stderr, "Invalid mode:%d\n", mode);
        fbCopyDeinit (pDemo);
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
            unsigned short character = fbCopyGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        (void)gettimeofday (&curtime, NULL);
        copyStartTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000);

        for (c = 0; c < 4; c++)
            {
            p = pDemo->point;
            for (i = 0; i < NPOINTS; i += 4, p += 4)
                {
                fbCopyRender (pDemo, *p, *(p+1), *(p+2), *(p+3), color);
                color = color >> 1;
                if (color == 0)
                    {
                    color = COLOR_START;
                    }
                }
            }

        (void)gettimeofday (&curtime, NULL);
        copyEndTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000);

        (void)fprintf (stdout, "Diff time:%ld - %ld = %ld msecs\n", copyEndTime, copyStartTime, copyEndTime - copyStartTime);

        if (runTime != 0)
            {
            (void)gettimeofday (&curtime, NULL);
            diffTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000) - startTime;
            }
        }

    if (mode >= 10)
        {
        free (pDemo->frontBuf);
        }

    /* Deinitialize demo */
    fbCopyDeinit (pDemo);
    (void)fprintf (stdout, "TEST COMPLETED\n");
    return (EXIT_SUCCESS);
    }
