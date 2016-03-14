/* gfxFbDrawDemo.inl - Frame buffer draw demo */

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
17feb16,yat  Add GFX_FIXED_FBMODE_OFFSET (US67554)
12jun15,yat  Add offset for xlnxlcvc (US58560)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
28oct14,yat  Add support for full screen draw (US46449)
18jun14,yat  Added main demo for dynamic RTP (US11227)
24jan14,mgc  Modified for VxWorks 7 release
*/

/*

DESCRIPTION

This program provides the frame buffer draw demonstration program.

*/

/* includes */

#if defined(__vxworks)
#include <vxWorks.h>
#include <ioLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <rtpLib.h>
#include <fbdev.h>
#else
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#define FB_MAX_STR_CHARS            128
#define FB_DEVICE_PREFIX            "/dev/fb"
#define FB_MAX_DEVICE               5
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

/* Rectangle to fill on the frame buffer */
#define DRAW_X0             20
#define DRAW_Y0             20
#define DRAW_X1             200
#define DRAW_Y1             100

#define COLOR_START         0xFFFF0000
#define COUNT_START         100

#define SWAP_INTERVAL       1

#define EXCHANGE(_i, _j)    do                              \
                                {                           \
                                __typeof__(_i) tmp = _i;    \
                                _i = _j;                    \
                                _j = tmp;                   \
                                }                           \
                            while (0)

#define EXCHANGE3(_i, _j, _k) do                              \
                                  {                           \
                                  __typeof__(_i) tmp = _i;    \
                                  _i = _k;                    \
                                  _k = _j;                    \
                                  _j = tmp;                   \
                                  }                           \
                              while (0)

/* typedefs */

/* Data structure that is attached to the window, provides all
 * data required to display and manage the program.
 */

typedef struct
    {
    int                 fbDevFd;
#if defined(GFX_USE_EVDEV)
    int                 evDevFd;
#endif
    unsigned int        width, stride, height, bpp;
    unsigned int        offset;
    unsigned int        buffers;
    void*               firstBuf;
    void*               frontBuf;
    void*               backBuf;
    void*               thirdBuf;
    long                tRate0;
    long                frames;
    } FB_DRAW_SCENE;

/*******************************************************************************
 *
 * fbDrawDeinit - deinitialize the application
 *
 * This routine deinitializes the application
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fbDrawInit
 *
 */
static void fbDrawDeinit
    (
    FB_DRAW_SCENE* pDemo
    )
    {
    if (pDemo->fbDevFd > 0)
        {
        (void)close (pDemo->fbDevFd);
        }
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
 * fbDrawInit - initialize the application
 *
 * This routine initializes the application
 *
 * RETURNS: pointer to FB_DRAW_SCENE, or NULL if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fbDrawDeinit
 *
 */
static FB_DRAW_SCENE* fbDrawInit
    (
    unsigned int device
    )
    {
    FB_DRAW_SCENE* pDemo;
    char deviceName[FB_MAX_STR_CHARS];
    unsigned int i;
#if defined(GFX_USE_EVDEV)
    char evDevName[EV_DEV_NAME_LEN + 1];
    unsigned int e;
#endif
    /* Allocate basic structure to carry demo data */
    pDemo = (FB_DRAW_SCENE*)calloc (1, sizeof (FB_DRAW_SCENE));
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "calloc failed\n");
        return (NULL);
        }

    pDemo->tRate0 = -1;
#if defined(__vxworks)
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
        fbDrawDeinit (pDemo);
        return (NULL);
        }
    (void)fprintf (stdout, "Open device:%s\n", deviceName);

    if (ioctl (pDemo->fbDevFd, FB_IOCTL_GET_FB_INFO, &arg) == -1)
        {
        (void)fprintf (stderr, "ERROR: FB_IOCTL_GET_FB_INFO\n");
        fbDrawDeinit (pDemo);
        return (NULL);
        }

    pDemo->width = arg.getFbInfo.width;
    pDemo->stride = arg.getFbInfo.stride;
    pDemo->height = arg.getFbInfo.height;
    pDemo->bpp = arg.getFbInfo.bpp;
#if defined(GFX_FIXED_FBMODE_OFFSET)
    pDemo->offset = arg.getFbInfo.offset;
#else
    pDemo->offset = pDemo->stride * pDemo->height;
#endif
    pDemo->buffers = arg.getFbInfo.buffers;

    /*
     * Sets the front buffer, the back buffer and the third buffer.
     */
    pDemo->firstBuf = arg.getFbInfo.pFirstFb;
    pDemo->frontBuf = pDemo->firstBuf;
    if (pDemo->buffers < 2)
        {
        pDemo->backBuf = pDemo->frontBuf;
        pDemo->thirdBuf = pDemo->backBuf;
        }
    else if (pDemo->buffers == 2)
        {
        pDemo->backBuf = (void*)((char*)pDemo->frontBuf + (pDemo->offset));
        pDemo->thirdBuf = pDemo->backBuf;
        }
    else
        {
        pDemo->backBuf = (void*)((char*)pDemo->frontBuf + (pDemo->offset));
        pDemo->thirdBuf = (void*)((char*)pDemo->backBuf + (pDemo->offset));
        }

    /* Sets the frame buffer to the front buffer */
    arg.setFb.pFb = pDemo->frontBuf;
    arg.setFb.when = SWAP_INTERVAL;
    if (ioctl (pDemo->fbDevFd, FB_IOCTL_SET_FB, &arg) == -1)
        {
        (void)fprintf (stderr, "ERROR: FB_IOCTL_SET_FB\n");
        fbDrawDeinit (pDemo);
        return (NULL);
        }

    (void)fprintf (stdout, "Frame buffer info\n\tfirstBuf:%p\n\tfrontBuf:%p\n\tbpp:%d\n\twidth:%d\n\tstride:%d\n\theight:%d\n\toffset:%d\n\tvsync:%d\n\tbuffers:%d\n\tpixelFormat:%08x %d %d %d %d\n",
            pDemo->firstBuf, pDemo->frontBuf,
            pDemo->bpp,
            pDemo->width, pDemo->stride, pDemo->height,
            pDemo->offset,
            arg.getFbInfo.vsync, pDemo->buffers,
            arg.getFbInfo.pixelFormat.flags,
            arg.getFbInfo.pixelFormat.alphaBits,
            arg.getFbInfo.pixelFormat.redBits,
            arg.getFbInfo.pixelFormat.greenBits,
            arg.getFbInfo.pixelFormat.blueBits);
#else
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
        fbDrawDeinit (pDemo);
        return (NULL);
        }
    (void)fprintf (stdout, "Open device:%s\n", deviceName);

    if (ioctl (pDemo->fbDevFd, FBIOGET_FSCREENINFO, &finfo) == -1)
        {
        (void)fprintf (stderr, "Read fixed information error\n");
        fbDrawDeinit (pDemo);
        return (NULL);
        }

    if (ioctl (pDemo->fbDevFd, FBIOGET_VSCREENINFO, &vinfo) == -1)
        {
        (void)fprintf (stderr, "Read variable information error\n");
        fbDrawDeinit (pDemo);
        return (NULL);
        }

    pDemo->width = vinfo.xres;
    pDemo->stride = vinfo.xres * (vinfo.bits_per_pixel >> 3);
    pDemo->height = vinfo.yres;
    pDemo->bpp = vinfo.bits_per_pixel;
    pDemo->offset = pDemo->stride * pDemo->height;
    pDemo->buffers = 1;
    pDemo->firstBuf = (void *)mmap (0, pDemo->stride * pDemo->height,
                               PROT_READ | PROT_WRITE, MAP_SHARED,
                               pDemo->fbDevFd, 0);
    pDemo->frontBuf = pDemo->firstBuf;
    pDemo->backBuf = pDemo->frontBuf;
    pDemo->thirdBuf = pDemo->backBuf;

    (void)fprintf (stdout, "Frame buffer info\n\tfrontBuf:%p\n\tbpp:%d\n\twidth:%d\n\tstride:%d\n\theight:%d\n",
            pDemo->frontBuf,
            pDemo->bpp,
            pDemo->width, pDemo->stride, pDemo->height);
#endif /* __vxworks */
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
 * fbDrawPerfCount - calculate and show performance
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
static void fbDrawPerfCount
    (
    FB_DRAW_SCENE* pDemo
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
 * fbDrawGetChar - gets an ASCII character from keyboard
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
static unsigned short fbDrawGetChar
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
 * gfxFbDrawDemo - core functionality of the program
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
int gfxFbDrawDemo
    (
    unsigned int device,
    int runTime,
    int drawX0,
    int drawY0,
    int drawX1,
    int drawY1
    )
    {
    struct timeval curtime;
    long startTime;
    long diffTime;
#if defined(__vxworks)
    FB_IOCTL_ARG arg;
#endif /* __vxworks */
    FB_DRAW_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                                /* terminate flag */
#endif
    unsigned int x, y;
    unsigned int count = 0;
    unsigned int color = 0;
    unsigned short color16 = 0;
    unsigned int i;

    /* Initialize demo */
    pDemo = fbDrawInit (device);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "fbDrawInit failed\n");
        return (EXIT_FAILURE);
        }

    if ((drawX1 == 0) && (drawY1 == 0))
        {
        drawX0 = DRAW_X0;
        drawY0 = DRAW_Y0;
        drawX1 = DRAW_X1;
        drawY1 = DRAW_Y1;
        }
    else if ((drawX1 > pDemo->width) && (drawY1 > pDemo->height))
        {
        drawX0 = 0;
        drawY0 = 0;
        drawX1 = pDemo->width;
        drawY1 = pDemo->height;
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
            unsigned short character = fbDrawGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        fbDrawPerfCount (pDemo);

        if (count == 0)
            {
            count = COUNT_START;
            color = color >> 1;
            if (color == 0)
                color = COLOR_START;
            color16 = (unsigned short) (((color & 0xf80000) >> 8)
                      | ((color & 0xfc00) >> 5) | (color & 0xf8) >> 3);
            }
        count--;

        /* Draw color to each pixel on the back buffer */
        for (y = drawY0; y < drawY1; y++)
            {
            for (x = drawX0; x < drawX1; x++)
                {
                i = x * (pDemo->bpp >> 3) + y * pDemo->stride;
                switch (pDemo->bpp)
                    {
                    case 32:
                    *((unsigned int *) ((char*)pDemo->backBuf + i)) = color;
                    break;
                    case 16:
                    *((unsigned short *) ((char*)pDemo->backBuf + i)) = color16;
                    break;
                    }
                }
            }
#if defined(__vxworks)
        /* Sets the frame buffer to the back buffer */
        arg.setFb.pFb = pDemo->backBuf;
        arg.setFb.when = SWAP_INTERVAL;
        if (ioctl (pDemo->fbDevFd, FB_IOCTL_SET_FB, &arg) == -1)
            {
            (void)fprintf (stderr, "ERROR: FB_IOCTL_SET_FB\n");
            fbDrawDeinit (pDemo);
            return (EXIT_FAILURE);
            }

        /* Exchange the front and back buffers */
        if (pDemo->buffers == 2)
            EXCHANGE (pDemo->frontBuf, pDemo->backBuf);
        else if (pDemo->buffers > 2)
            EXCHANGE3 (pDemo->frontBuf, pDemo->backBuf, pDemo->thirdBuf);
#endif /* __vxworks */
        if (runTime != 0)
            {
            (void)gettimeofday (&curtime, NULL);
            diffTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000) - startTime;
            }
        }
#if defined(__vxworks)
    /* Sets the frame buffer to the first buffer */
    arg.setFb.pFb = pDemo->firstBuf;
    arg.setFb.when = SWAP_INTERVAL;
    if (ioctl (pDemo->fbDevFd, FB_IOCTL_SET_FB, &arg) == -1)
        {
        (void)fprintf (stderr, "ERROR: FB_IOCTL_SET_FB\n");
        fbDrawDeinit (pDemo);
        return (EXIT_FAILURE);
        }
#endif /* __vxworks */

    /* Deinitialize demo */
    fbDrawDeinit (pDemo);
    (void)fprintf (stdout, "DEMO COMPLETED\n");
    return (EXIT_SUCCESS);
    }
