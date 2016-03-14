/* gfxFbIoctlTest.inl - Frame buffer ioctl test */

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
24feb16,yat  Remove unused include tickLib.h (US75033)
12jun15,yat  Add offset for xlnxlcvc (US58560)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
18jun14,yat  Added main demo for dynamic RTP (US11227)
24jan14,mgc  Modified for VxWorks 7 release
*/

/*

DESCRIPTION

This program tests the frame buffer ioctls.

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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

/* defines */

#define COLOR_BLACK         0x00000000
#define COLOR_RED           0x00FF0000
#define COLOR_GREEN         0x0000FF00
#define COLOR_BLUE          0x000000FF
#define COLOR_CYAN          0x0000FFFF
#define COLOR_YELLOW        0x00FFFF00
#define COLOR_MAGENTA       0x00FF00FF
#define COLOR_WHITE         0x00FFFFFF

#define SWAP_INTERVAL       2

#define IOCTL_SLEEP_SECS    2

/* locals */

static char videoModeStr[FB_MAX_VIDEO_MODE_LEN] = "";

/*******************************************************************************
 *
 * draw - draw a rectangle to the front buffer
 *
 * This routine draws a rectangle to the front buffer.
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
LOCAL int draw
    (
    int fd,
    unsigned int color
    )
    {
    FB_IOCTL_ARG arg;
    unsigned int x, y, location;
    unsigned short color16;

    if (ioctl (fd, FB_IOCTL_GET_FB_INFO, &arg) == -1)
        {
        (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_GET_FB_INFO errno:%08x\n", videoModeStr, errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }
    color16 = (unsigned short) (((color & 0xf80000) >> 8)
              | ((color & 0xfc00) >> 5) | (color & 0xf8) >> 3);

    for (y = 100; y < arg.getFbInfo.height - 100; y++)
        {
        for (x = 100; x < arg.getFbInfo.width - 100; x++)
            {
            location = x * (arg.getFbInfo.bpp >> 3)
                    + y * arg.getFbInfo.stride;
            switch (arg.getFbInfo.bpp)
                {
                case 32:
                *((unsigned int*) ((char*)(arg.getFbInfo.pFb) + location)) =
                        color;
                break;
                case 16:
                *((unsigned short*) ((char*)(arg.getFbInfo.pFb)
                        + location)) = color16;
                break;
                }
            }
        }

    (void)sleep (IOCTL_SLEEP_SECS);

    return (EXIT_SUCCESS);
    }

/*******************************************************************************
 *
 * setfb - test various set frame buffer scenarios
 *
 * This routine tests various set frame buffer scenarios
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
LOCAL int setfb
    (
    int fd
    )
    {
    FB_IOCTL_ARG arg;
    unsigned int offset;

    if (ioctl (fd, FB_IOCTL_VSYNC_ENABLE, &arg) != -1)
        {
        (void)fprintf (stdout, "%s: Enable vsync errno:%08x\n", videoModeStr, errno);
        (void)fprintf (stdout, "%s: PASS: rectangle is CYAN\n", videoModeStr);
        if (draw(fd, COLOR_CYAN) == EXIT_FAILURE)
            {
            return (EXIT_FAILURE);
            }

        /******************/

        if (ioctl (fd, FB_IOCTL_GET_FB_INFO, &arg) == -1)
            {
            (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_GET_FB_INFO errno:%08x\n", videoModeStr, errno);
            (void)close (fd);
            return (EXIT_FAILURE);
            }
        if (arg.getFbInfo.buffers > 1)
            {
#if defined(_WRS_CONFIG_FBDEV_XLNXLCVCFB)
            offset = arg.getFbInfo.offset;
#else
            offset = arg.getFbInfo.stride * arg.getFbInfo.height;
#endif
            }
        else
            {
            offset = 0;
            }
        arg.setFb.pFb = (void*) ((char*)arg.getFbInfo.pFirstFb + offset);
        arg.setFb.when = SWAP_INTERVAL;
        if (ioctl (fd, FB_IOCTL_SET_FB, &arg) == -1)
            {
            (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_SET_FB errno:%08x\n", videoModeStr, errno);
            (void)close (fd);
            return (EXIT_FAILURE);
            }
        (void)fprintf (stdout, "%s: Set frame buffer address errno:%08x pBuf:%p\n", videoModeStr, errno,
                arg.setFb.pFb);
        if (arg.getFbInfo.buffers > 1)
            (void)fprintf (stdout, "%s: PASS: rectangle is SPLASH SCREEN or BLACK\n", videoModeStr);
        else
            (void)fprintf (stdout, "%s: PASS: rectangle is CYAN\n", videoModeStr);
        (void)sleep (IOCTL_SLEEP_SECS);

        /******************/

        if (ioctl (fd, FB_IOCTL_VSYNC, &arg) == -1)
            {
            (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_VSYNC errno:%08x\n", videoModeStr, errno);
            (void)close (fd);
            return (EXIT_FAILURE);
            }
        (void)fprintf (stdout, "%s: Wait for vsync errno:%08x\n", videoModeStr, errno);
        (void)fprintf (stdout, "%s: PASS: rectangle is YELLOW\n", videoModeStr);
        if (draw(fd, COLOR_YELLOW) == EXIT_FAILURE)
            {
            return (EXIT_FAILURE);
            }

        /******************/

        if (ioctl (fd, FB_IOCTL_GET_FB_INFO, &arg) == -1)
            {
            (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_GET_FB_INFO errno:%08x\n", videoModeStr, errno);
            (void)close (fd);
            return (EXIT_FAILURE);
            }
        arg.setFb.pFb = arg.getFbInfo.pFirstFb;
        arg.setFb.when = SWAP_INTERVAL;
        if (ioctl (fd, FB_IOCTL_SET_FB, &arg) == -1)
            {
            (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_SET_FB errno:%08x\n", videoModeStr, errno);
            (void)close (fd);
            return (EXIT_FAILURE);
            }
        (void)fprintf (stdout, "%s: Set frame buffer address errno:%08x pBuf:%p\n", videoModeStr, errno,
                arg.setFb.pFb);
        (void)fprintf (stdout, "%s: PASS: rectangle is CYAN\n", videoModeStr);
        (void)sleep (IOCTL_SLEEP_SECS);

        /******************/

        if (ioctl (fd, FB_IOCTL_VSYNC, &arg) == -1)
            {
            (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_VSYNC errno:%08x\n", videoModeStr, errno);
            (void)close (fd);
            return (EXIT_FAILURE);
            }
        (void)fprintf (stdout, "%s: Wait for vsync errno:%08x\n", videoModeStr, errno);
        (void)fprintf (stdout, "%s: PASS: rectangle is MAGENTA\n", videoModeStr);
        if (draw(fd, COLOR_MAGENTA) == EXIT_FAILURE)
            {
            return (EXIT_FAILURE);
            }

        /******************/

        if (ioctl (fd, FB_IOCTL_VSYNC_DISABLE, &arg) == -1)
            {
            (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_VSYNC_DISABLE errno:%08x\n",
                    videoModeStr, errno);
            (void)close (fd);
            return (EXIT_FAILURE);
            }
        (void)fprintf (stdout, "%s: Disable vsync errno:%08x\n", videoModeStr, errno);
        (void)fprintf (stdout, "%s: PASS: rectangle is CYAN\n", videoModeStr);
        if (draw(fd, COLOR_CYAN) == EXIT_FAILURE)
            {
            return (EXIT_FAILURE);
            }
        }

    if (ioctl (fd, FB_IOCTL_GET_FB_INFO, &arg) == -1)
        {
        (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_GET_FB_INFO errno:%08x\n", videoModeStr, errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }
    if (arg.getFbInfo.buffers > 1)
        {
#if defined(_WRS_CONFIG_FBDEV_XLNXLCVCFB)
        offset = arg.getFbInfo.offset;
#else
        offset = arg.getFbInfo.stride * arg.getFbInfo.height;
#endif
        }
    else
        {
        offset = 0;
        }
    arg.setFb.pFb = (void*) ((char*)arg.getFbInfo.pFirstFb + offset);
    arg.setFb.when = SWAP_INTERVAL;
    if (ioctl (fd, FB_IOCTL_SET_FB, &arg) == -1)
        {
        (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_SET_FB errno:%08x\n", videoModeStr, errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }
    (void)fprintf (stdout, "%s: Set frame buffer address errno:%08x pBuf:%p\n", videoModeStr, errno,
            arg.setFb.pFb);
    (void)fprintf (stdout, "%s: PASS: rectangle is YELLOW\n", videoModeStr);
    if (draw(fd, COLOR_YELLOW) == EXIT_FAILURE)
        {
        return (EXIT_FAILURE);
        }

    return (EXIT_SUCCESS);
}

/*******************************************************************************
 *
 * gfxFbIoctlTest - perform ioctl tests
 *
 * This routine performs ioctl tests
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
int gfxFbIoctlTest
    (
    unsigned int device
    )
    {
    unsigned int i;
    int fd;
    FB_IOCTL_ARG arg;
    FB_CONFIG fbConfigs[FB_MAX_CONFIGS];
    FB_VIDEO_MODE fbModes[FB_MAX_VIDEO_MODES];
    char deviceName[FB_MAX_STR_CHARS];
    unsigned int vsync;
    unsigned int offset;

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
        return (EXIT_FAILURE);
        }
    (void)fprintf (stdout, "Open device:%s\n", deviceName);

    /******************/

    if (ioctl (fd, FB_IOCTL_GET_FB_INFO, &arg) == -1)
        {
        (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_GET_FB_INFO errno:%08x\n", videoModeStr, errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }
    vsync = arg.getFbInfo.vsync;
#if defined(_WRS_CONFIG_FBDEV_XLNXLCVCFB)
    offset = arg.getFbInfo.offset;
#else
    offset = arg.getFbInfo.stride * arg.getFbInfo.height;
#endif
    (void)fprintf (stdout, "Get frame buffer info errno:%08x\n\tpFirstFb:%p\n\tpFb:%p\n\tbpp:%d\n\twidth:%d\n\tstride:%d\n\theight:%d\n\toffset:%d\n\tvsync:%d\n\tbuffers:%d\n\tpixelFormat:%08x %d %d %d %d\n",
            errno, arg.getFbInfo.pFirstFb, arg.getFbInfo.pFb,
            arg.getFbInfo.bpp,
            arg.getFbInfo.width, arg.getFbInfo.stride, arg.getFbInfo.height,
            offset,
            vsync, arg.getFbInfo.buffers,
            arg.getFbInfo.pixelFormat.flags,
            arg.getFbInfo.pixelFormat.alphaBits,
            arg.getFbInfo.pixelFormat.redBits,
            arg.getFbInfo.pixelFormat.greenBits,
            arg.getFbInfo.pixelFormat.blueBits);

    /******************/

    arg.setFb.pFb = arg.getFbInfo.pFirstFb;
    arg.setFb.when = SWAP_INTERVAL;
    if (ioctl(fd, FB_IOCTL_SET_FB, &arg) == -1)
        {
        (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_SET_FB errno:%08x\n", videoModeStr, errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }

    /******************/

    bzero ((void*)videoModeStr, FB_MAX_VIDEO_MODE_LEN);
    arg.getVideoMode.pBuf = videoModeStr;
    if (ioctl (fd, FB_IOCTL_GET_VIDEO_MODE, &arg) == -1)
        {
        (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_GET_VIDEO_MODE errno:%08x\n", videoModeStr, errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }
    (void)fprintf (stdout, "%s: Get video mode errno:%08x pBuf:%s\n", videoModeStr, errno,
            videoModeStr);
    (void)fprintf (stdout, "%s: PASS: rectangle is RED\n", videoModeStr);
    if (draw(fd, COLOR_RED) == EXIT_FAILURE)
        {
        return EXIT_FAILURE;
        }

    /******************/

    if (ioctl (fd, FB_IOCTL_DEV_SHOW, 10) == -1)
        {
        (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_DEV_SHOW errno:%08x\n", videoModeStr, errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }
    (void)fprintf (stdout, "%s: Show errno:%08x\n", videoModeStr, errno);
    (void)fprintf (stdout, "%s: PASS: rectangle is GREEN\n", videoModeStr);
    if (draw(fd, COLOR_GREEN) == EXIT_FAILURE)
        {
        return EXIT_FAILURE;
        }

    /******************/

    bzero ((void*)&(fbConfigs[0]), sizeof(FB_CONFIG)*FB_MAX_CONFIGS);
    arg.getConfigs.pConfigs = &(fbConfigs[0]);
    if (ioctl (fd, FB_IOCTL_GET_CONFIGS, &arg) == -1)
        {
        (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_GET_CONFIGS errno:%08x\n", videoModeStr, errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }

    for (i = 0; i < FB_MAX_CONFIGS; i++)
        {
        (void)fprintf (stdout,
                "%s: Get configs errno:%08x id:%d pixelFormat:%08x %d %d %d %d\n",
                videoModeStr, errno, i, arg.getConfigs.pConfigs[i].pixelFormat.flags,
                arg.getConfigs.pConfigs[i].pixelFormat.alphaBits,
                arg.getConfigs.pConfigs[i].pixelFormat.redBits,
                arg.getConfigs.pConfigs[i].pixelFormat.greenBits,
                arg.getConfigs.pConfigs[i].pixelFormat.blueBits);
        }
    (void)fprintf (stdout, "%s: PASS: rectangle is BLUE\n", videoModeStr);
    if (draw(fd, COLOR_BLUE) == EXIT_FAILURE)
        {
        return EXIT_FAILURE;
        }

    /******************/

    bzero ((void*)&(fbModes[0]), sizeof(FB_VIDEO_MODE)*FB_MAX_VIDEO_MODES);
    arg.getVideoModes.pVideoModes = &(fbModes[0]);
    if (ioctl (fd, FB_IOCTL_GET_VIDEO_MODES, &arg) == -1)
        {
        (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_GET_VIDEO_MODES errno:%08x\n", videoModeStr, errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }
    for (i = 0; i < FB_MAX_VIDEO_MODES; i++)
        {
        (void)fprintf (stdout, "%s: Get video modes errno:%08x name:%s\n", videoModeStr, errno,
                fbModes[i].name);
        }
    (void)fprintf (stdout, "%s: PASS: rectangle is RED\n", videoModeStr);
    if (draw(fd, COLOR_RED) == EXIT_FAILURE)
        {
        return EXIT_FAILURE;
        }

    /******************/

    if (setfb (fd) == EXIT_FAILURE)
        return (EXIT_FAILURE);

    /******************/

    if (vsync)
        {
        if (ioctl (fd, FB_IOCTL_VSYNC_ENABLE, &arg) == -1)
            {
            (void)fprintf (stderr, "%s: ERROR: FB_IOCTL_VSYNC_ENABLE errno:%08x\n",
                    videoModeStr, errno);
            (void)close (fd);
            return (EXIT_FAILURE);
            }
        (void)fprintf (stdout, "%s: Enable vsync errno:%08x\n", videoModeStr, errno);
        (void)fprintf (stdout, "%s: PASS: rectangle is GREEN\n", videoModeStr);
        if (draw(fd, COLOR_GREEN) == EXIT_FAILURE)
            {
            return EXIT_FAILURE;
            }
        }

    /******************/

    if (ioctl (fd, FB_IOCTL_CLEAR_SCREEN, &arg) == -1)
        {
        }
    (void)fprintf (stdout, "%s: Clear screen errno:%08x\n", videoModeStr, errno);
    (void)fprintf (stdout, "%s: PASS: screen is BLACK\n", videoModeStr);
    if (draw(fd, COLOR_BLACK) == EXIT_FAILURE)
        {
        return EXIT_FAILURE;
        }

    /******************/

    (void)close (fd);
    (void)printf ("Close device:%s\n", deviceName);
    (void)fprintf (stdout, "TEST COMPLETED\n");

    return (EXIT_SUCCESS);
    }
