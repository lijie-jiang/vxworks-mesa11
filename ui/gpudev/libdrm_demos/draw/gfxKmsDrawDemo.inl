/* gfxKmsDrawDemo.inl - KMS Draw Demo */

/*
 * Copyright (c) 2015-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24feb16,yat  Fix static analysis defects (US75033)
16sep15,yat  Written (US24710)
*/

/*

DESCRIPTION

This program provides the KMS draw demonstration program.

*/

/* includes */

#if defined(__vxworks)
#include <vxWorks.h>
#include <ioLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <rtpLib.h>
#else
#include <sys/mman.h>
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

#define GFX_KMS_GET_CONN_STR
#define GFX_KMS_FIND_CONN
#define GFX_KMS_FIND_CRTC_CONN
#define GFX_KMS_FIND_CONN_CRTC
#define GFX_KMS_FIND_CRTC_FB
#define GFX_KMS_DESTROY_DRM_FB
#define GFX_KMS_OPEN_DRM_FB
#define GFX_KMS_CREATE_DRM_FB
#define GFX_KMS_MMAP_DRM_FB
#include <gfxKmsHelper.inl>

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

/* typedefs */

/* Data structure that is attached to the window, provides all
 * data required to display and manage the program.
 */

typedef struct
    {
    int                  drmDevFd;
    drmModeModeInfo      mode;
    uint32_t             connId;
    uint32_t             crtcId;
    uint64_t             fbSize;
    uint32_t             fbId;
    uint32_t             bpp;
    uint32_t             handle;
    void*                ptr;
#if defined(GFX_USE_EVDEV)
    int                  evDevFd;
#endif
    long                 tRate0;
    long                 frames;
    } DRAW_SCENE;

/*******************************************************************************
*
* kmsDrawDeinit - deinitialize the application
*
* This routine deinitializes the application
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO: kmsDrawInit
*/

static void kmsDrawDeinit
    (
    DRAW_SCENE* pDemo
    )
    {
    gfxKmsDestroyDrmFb (pDemo->drmDevFd, pDemo->fbId, pDemo->handle);
    gfxKmsCloseDrmDev (pDemo->drmDevFd);
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
* kmsDrawOpenDrmFb - open a DRM framebuffer
*
* This routine opens a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO: kmsDrawCreateDrmFb
*/

static int kmsDrawOpenDrmFb
    (
    DRAW_SCENE* pDemo
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

    if (!fb)
        {
        (void)fprintf (stderr, "No framebuffer found\n");
        return 1;
        }

    pDemo->fbId = fb->fb_id;
    pDemo->bpp = fb->bpp;

    if (gfxKmsFindConnCrtc (pDemo->drmDevFd, crtc, &conn))
        {
        (void)fprintf (stderr, "No connector found\n");
        drmModeFreeCrtc (crtc);
        return 1;
        }
    (void)fprintf (stderr, "Found %s %s %dx%d\n",
                   gfxKmsGetConnModeStr(DRM_MODE_CONNECTED),
                   gfxKmsGetConnTypeStr (conn->connector_type),
                   crtc->mode.hdisplay, crtc->mode.vdisplay);

    pDemo->bpp = 32;
    pDemo->mode = crtc->mode;
    pDemo->connId = conn->connector_id;
    pDemo->crtcId = crtc->crtc_id;
    pDemo->fbSize = pDemo->mode.hdisplay * pDemo->mode.vdisplay * (pDemo->bpp >> 3);
    drmModeFreeCrtc (crtc);
    drmModeFreeConnector (conn);

    if (gfxKmsOpenDrmFb (pDemo->drmDevFd, fb, &(pDemo->handle)))
        {
        return 1;
        }

    return gfxKmsMmapDrmFb (pDemo->drmDevFd, pDemo->fbId, pDemo->handle,
                            pDemo->fbSize, &(pDemo->ptr));
    }

/*******************************************************************************
*
* kmsDrawCreateDrmFb - create a DRM framebuffer
*
* This routine creates a DRM framebuffer.
*
* RETURNS: 0 if successful, or 1 otherwise
*
* ERRNO: N/A
*
* SEE ALSO: kmsDrawOpenDrmFb
*/

static int kmsDrawCreateDrmFb
    (
    DRAW_SCENE* pDemo
    )
    {
    drmModeCrtc*         crtc;
    drmModeConnector*    conn;

    if (gfxKmsFindConn (pDemo->drmDevFd, &conn, &(pDemo->mode)))
        {
        (void)fprintf (stderr, "No connector found\n");
        return 1;
        }

    pDemo->connId = conn->connector_id;

    if (gfxKmsFindCrtcConn (pDemo->drmDevFd, conn, &crtc))
        {
        (void)fprintf (stderr, "No crtc found\n");
        drmModeFreeConnector (conn);
        return 1;
        }

    (void)fprintf (stderr, "Found %s %s %dx%d\n",
                   gfxKmsGetConnModeStr(DRM_MODE_CONNECTED),
                   gfxKmsGetConnTypeStr (conn->connector_type),
                   crtc->mode.hdisplay, crtc->mode.vdisplay);

    pDemo->bpp = 32;
    pDemo->crtcId = crtc->crtc_id;
    pDemo->fbSize = pDemo->mode.hdisplay * pDemo->mode.vdisplay * 4;
    drmModeFreeCrtc (crtc);
    drmModeFreeConnector (conn);

    if (gfxKmsCreateDrmFb (pDemo->drmDevFd,
                           pDemo->mode.hdisplay, pDemo->mode.vdisplay,
                           &(pDemo->fbId), &(pDemo->handle),
                           pDemo->fbSize, NULL, NULL))
        {
        return 1;
        }

    if (drmModeSetCrtc (pDemo->drmDevFd, pDemo->crtcId, pDemo->fbId,
                        0, 0, &(pDemo->connId), 1, &(pDemo->mode)))
        {
        (void)fprintf (stderr, "drmModeSetCrtc error\n");
        return 1;
        }

    return gfxKmsMmapDrmFb (pDemo->drmDevFd, pDemo->fbId, pDemo->handle,
                            pDemo->fbSize, &(pDemo->ptr));
    }

/*******************************************************************************
*
* kmsDrawInit - initialize the application
*
* This routine initializes the application
*
* RETURNS: pointer to DRAW_SCENE, or NULL if the routine had to abort
*
* ERRNO: N/A
*
* SEE ALSO: kmsDrawDeinit
*/

static DRAW_SCENE* kmsDrawInit
    (
    unsigned int device
    )
    {
    DRAW_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    char evDevName[EV_DEV_NAME_LEN + 1];
    unsigned int e;
#endif
    /* Allocate basic structure to carry demo data */
    pDemo = (DRAW_SCENE*)calloc (1, sizeof (DRAW_SCENE));
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "calloc failed\n");
        return (NULL);
        }

    pDemo->tRate0 = -1;

    if (gfxKmsOpenDrmDev (&(pDemo->drmDevFd)))
        {
        free (pDemo);
        return (NULL);
        }

    if (kmsDrawOpenDrmFb (pDemo))
        {
        if (kmsDrawCreateDrmFb (pDemo))
            {
            kmsDrawDeinit (pDemo);
            return (NULL);
            }
        }
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
* kmsDrawPerfCount - calculate and show performance
*
* This routine calculates and displays the number of frames rendered every
* 5 seconds.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static void kmsDrawPerfCount
    (
    DRAW_SCENE* pDemo
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
* kmsDrawGetChar - gets an ASCII character from keyboard
*
* This routine gets an ASCII character from keyboard
*
* RETURNS: Ascii character, or NULL
*
* ERRNO: N/A
*
* SEE ALSO:
*/

static unsigned short kmsDrawGetChar
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
* gfxKmsDrawDemo - core functionality of the program
*
* This routine contains the core functionality for the program.
*
* RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
*
* ERRNO: N/A
*
* SEE ALSO:
*/

int gfxKmsDrawDemo
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
    DRAW_SCENE* pDemo;
#if defined(GFX_USE_EVDEV)
    int kbdQuit = 0;                                /* terminate flag */
#endif
    unsigned int width, stride, height;
    void* backBuf;
    unsigned int x, y;
    unsigned int count = 0;
    unsigned int color = 0;
    unsigned short color16 = 0;
    unsigned int i;

    /* Initialize demo */
    pDemo = kmsDrawInit (device);
    if (pDemo == NULL)
        {
        (void)fprintf (stderr, "kmsDrawInit failed\n");
        return (EXIT_FAILURE);
        }

    width = pDemo->mode.hdisplay;
    stride = width * (pDemo->bpp >> 3);
    height = pDemo->mode.vdisplay;
    backBuf = pDemo->ptr;

    (void)fprintf (stdout, "KMS info\n");
    (void)fprintf (stdout, "\tbackBuffer:%p\n", backBuf);
    (void)fprintf (stdout, "\tsize:%lud\n", pDemo->fbSize);
    (void)fprintf (stdout, "\tbpp:%d\n", pDemo->bpp);
    (void)fprintf (stdout, "\twidth:%d\n", width);
    (void)fprintf (stdout, "\tstride:%d\n", stride);
    (void)fprintf (stdout, "\theight:%d\n", height);
    (void)fprintf (stdout, "\tclock:%d\n", pDemo->mode.clock);
    (void)fprintf (stdout, "\thdisplay:%d\n", pDemo->mode.hdisplay);
    (void)fprintf (stdout, "\thsync_start:%d\n", pDemo->mode.hsync_start);
    (void)fprintf (stdout, "\thsync_end:%d\n", pDemo->mode.hsync_end);
    (void)fprintf (stdout, "\thtotal:%d\n", pDemo->mode.htotal);
    (void)fprintf (stdout, "\thskew:%d\n", pDemo->mode.hskew);
    (void)fprintf (stdout, "\tvdisplay:%d\n", pDemo->mode.vdisplay);
    (void)fprintf (stdout, "\tvsync_start:%d\n", pDemo->mode.vsync_start);
    (void)fprintf (stdout, "\tvsync_end:%d\n", pDemo->mode.vsync_end);
    (void)fprintf (stdout, "\tvtotal:%d\n", pDemo->mode.vtotal);
    (void)fprintf (stdout, "\tvscan:%d\n", pDemo->mode.vscan);

    if ((drawX1 == 0) && (drawY1 == 0))
        {
        drawX0 = DRAW_X0;
        drawY0 = DRAW_Y0;
        drawX1 = DRAW_X1;
        drawY1 = DRAW_Y1;
        }
    else if ((drawX1 > width) && (drawY1 > height))
        {
        drawX0 = 0;
        drawY0 = 0;
        drawX1 = width;
        drawY1 = height;
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
            unsigned short character = kmsDrawGetChar (pDemo->evDevFd);
            if (character == (unsigned short)'q') kbdQuit = 1;
            }
#endif
        kmsDrawPerfCount (pDemo);

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
                i = x * (pDemo->bpp >> 3) + y * stride;
                switch (pDemo->bpp)
                    {
                    case 32:
                    *((unsigned int *) ((char*)backBuf + i)) = color;
                    break;
                    case 16:
                    *((unsigned short *) ((char*)backBuf + i)) = color16;
                    break;
                    }
                }
            }

        (void)gfxKmsPageFlip (pDemo->drmDevFd, pDemo->crtcId, pDemo->fbId,
                              &(pDemo->connId), &(pDemo->mode), 20, 1);

        if (runTime != 0)
            {
            (void)gettimeofday (&curtime, NULL);
            diffTime = (curtime.tv_sec * 1000) + (curtime.tv_usec / 1000) - startTime;
            }
        }

    /* Deinitialize demo */
    kmsDrawDeinit (pDemo);
    (void)fprintf (stdout, "DEMO COMPLETED\n");
    return (EXIT_SUCCESS);
    }
