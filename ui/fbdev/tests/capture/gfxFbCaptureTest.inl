/* gfxFbCaptureTest.inl - Frame buffer capture test */

/*
 * Copyright (c) 2014, 2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24feb16,yat  Remove unused include tickLib.h (US75033)
22dec14,yat  Created frame buffer capture test (US50456)
*/

/*

DESCRIPTION

This program captures the frame buffer.

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

/*******************************************************************************
 *
 * gfxFbCaptureTest - capture the frame buffer
 *
 * This routine captures the frame buffer
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
int gfxFbCaptureTest
    (
    unsigned int device,
    unsigned int skipWrite
    )
    {
    unsigned int i;
    int fd;
    FB_IOCTL_ARG arg;
    char deviceName[FB_MAX_STR_CHARS];
    FILE *pFile;

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

    if (ioctl (fd, FB_IOCTL_GET_FB_INFO, &arg) == -1)
        {
        (void)fprintf (stderr, "ERROR: FB_IOCTL_GET_FB_INFO errno:%08x\n", errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }
    (void)fprintf (stdout, "Get frame buffer info errno:%08x\n\tpFirstFb:%p\n\tpFb:%p\n\tbpp:%d\n\twidth:%d\n\tstride:%d\n\theight:%d\n\tvsync:%d\n\tbuffers:%d\n\tpixelFormat:%08x %d %d %d %d\n",
            errno, arg.getFbInfo.pFirstFb, arg.getFbInfo.pFb,
            arg.getFbInfo.bpp,
            arg.getFbInfo.width, arg.getFbInfo.stride, arg.getFbInfo.height,
            arg.getFbInfo.vsync, arg.getFbInfo.buffers,
            arg.getFbInfo.pixelFormat.flags,
            arg.getFbInfo.pixelFormat.alphaBits,
            arg.getFbInfo.pixelFormat.redBits,
            arg.getFbInfo.pixelFormat.greenBits,
            arg.getFbInfo.pixelFormat.blueBits);

    if (!skipWrite)
        {
        if ((pFile = fopen ("capture.bin", "wb")) == NULL)
            {
            (void)fprintf (stderr, "fopen for write failed\n");
            (void)close (fd);
            return (EXIT_FAILURE);
            }

        (void)fprintf (stdout, "Writing to file\n");
        if (fwrite (arg.getFbInfo.pFb, arg.getFbInfo.stride*arg.getFbInfo.height, 1, pFile) < 1)
            {
            (void)fprintf (stderr, "fwrite failed\n");
            }

        (void)fclose (pFile);
        }

    if ((pFile = fopen ("capture.bin", "rb")) == NULL)
        {
        (void)fprintf (stderr, "fopen for read failed\n");
        (void)close (fd);
        return (EXIT_FAILURE);
        }

    (void)fprintf (stdout, "Reading from file\n");
    if (fread (arg.getFbInfo.pFb, arg.getFbInfo.stride*arg.getFbInfo.height, 1, pFile) < 1)
        {
        (void)fprintf (stderr, "fread failed\n");
        }

    (void)fclose (pFile);

    (void)close (fd);
    (void)printf ("Close device:%s\n", deviceName);
    (void)fprintf (stdout, "TEST COMPLETED\n");

    return (EXIT_SUCCESS);
    }
