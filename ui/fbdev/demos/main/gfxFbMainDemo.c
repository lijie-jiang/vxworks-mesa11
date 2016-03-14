/* gfxFbMainDemo.c - Main frame buffer demo */

/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
19nov15,yat  Fix bzero (US24710)
30mar15,rpc  Static analysis fixes (US50633)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
18jun14,yat  Added main demo for dynamic RTP (US11227)
24jan14,mgc  Modified for VxWorks 7 release
*/

/*

DESCRIPTION

This small program cycles through each of the program. Each program is run
for a pre-determined amount of time (controlled by RUN_TIME_SECS in seconds).

*/

/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ioLib.h>
#include <taskLib.h>
#include <rtpLib.h>
#include <fbdev.h>
#if !defined(_WRS_KERNEL)
#include <gfxFbDrawDemo.inl>
#endif

/* defines */

#define MAX_COUNT           3
#define RUN_TIME_SECS       20
#define MAIN_SLEEP_SECS     5

/* forward declarations */

#if defined(_WRS_KERNEL)
extern int gfxFbDrawDemo (unsigned int, int, int, int, int, int);
#endif

/*******************************************************************************
 *
 * gfxFbMainDemo - start of the program
 *
 * This is the entry point for the program. The program cycles
 * through all the programs, allow each to execute for 'RUN_TIME_SECS' seconds
 * before moving on to the next.
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
int gfxFbMainDemo
    (
    unsigned int device,
    unsigned int count
    )
    {
    int i, j;
    int fd;
    FB_IOCTL_ARG arg;
    FB_VIDEO_MODE fbModes[FB_MAX_VIDEO_MODES];
    char deviceName[FB_MAX_STR_CHARS];

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
    (void)printf ("Open device:%s\n", deviceName);

    bzero ((void *)&(fbModes[0]), sizeof (FB_VIDEO_MODE) * FB_MAX_VIDEO_MODES);
    arg.getVideoModes.pVideoModes = &(fbModes[0]);
    if (ioctl (fd, FB_IOCTL_GET_VIDEO_MODES, &arg) == -1)
        {
        (void)fprintf (stderr, "ERROR: FB_IOCTL_GET_VIDEO_MODES errno:%08x\n",
                        errno);
        (void)close (fd);
        return (EXIT_FAILURE);
        }

    (void)close (fd);

    if (count == 0) count = MAX_COUNT;

    for (i = 0; i < FB_MAX_VIDEO_MODES; i++)
        {
        if (!fbModes[i].xres) break;
        if (i > 0)
            {
            if ((fd = open (deviceName, O_RDWR, 0666)) == -1)
                {
                (void)fprintf (stderr, "Open device %s errno:%08x\n", deviceName, errno);
                return (EXIT_FAILURE);
                }

            arg.setVideoMode.videoMode = fbModes[i].name;
            if (ioctl (fd, FB_IOCTL_SET_VIDEO_MODE, &arg) == -1)
                {
                (void)fprintf (stderr, "ERROR: FB_IOCTL_SET_VIDEO_MODE errno:%08x\n", errno);
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)printf ("Set video mode errno:%08x videoMode:%s\n", errno,
                          fbModes[i].name);

            (void)close (fd);
            }
        for (j = 1; j <= count; j++)
            {
            (void)printf ("Run fbdev draw demo:%d of %d\n", j, count);
            if (gfxFbDrawDemo (device, RUN_TIME_SECS, 0, 0, 0, 0) == EXIT_FAILURE)
                {
                return (EXIT_FAILURE);
                }
            }

        (void)sleep (MAIN_SLEEP_SECS);
        }

    (void)printf ("DEMO COMPLETED WITHOUT ERROR\n");

    return (EXIT_SUCCESS);
    }

#if defined(_WRS_KERNEL)
/*******************************************************************************
 *
 * gfxFbMainDemoStart - start of the program in kernel mode
 *
 * This is the kernel mode entry point. An unsigned integer argument
 * can be used when invoking to set an upper limit (in seconds) on how
 * long the program should run.  A value of 0 allows the program to run for an
 * indefinite amount of time.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gfxFbMainDemoRtp
 *
 */
void gfxFbMainDemoStart
    (
    unsigned int device,
    unsigned int count
    )
    {
    (void)taskSpawn ("tMain", 100, VX_FP_TASK, 0x10000,
                     (FUNCPTR) gfxFbMainDemo,
                     device, count, 0, 0, 0, 0, 0, 0, 0, 0 );
    }

/*******************************************************************************
 *
 * gfxFbMainDemoRtp - start of the program for invoking RTP
 *
 * This is the entry point for invoking RTP. An unsigned integer argument
 * can be used when invoking to set an upper limit (in seconds) on how
 * long the program should run.  A value of 0 allows the program to run for an
 * indefinite amount of time.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gfxFbMainDemoStart
 *
 */
static void gfxFbMainDemoRtp0
    (
    const char *rtpStr,
    unsigned int device,
    unsigned int count
    )
    {
    int fd;
    const char *args[4];
    char deviceStr[32];
    char countStr[32];

    (void)snprintf (deviceStr, 32, "%d", device);
    (void)snprintf (countStr, 32, "%d", count);

    args[0] = rtpStr;
    args[1] = deviceStr;
    args[2] = countStr;
    args[3] = NULL;

    if ((fd = open (args[0], O_RDONLY, 0)) == -1)
        {
        (void)fprintf (stderr, "Open RTP file %s errno:%08x\n", args[0], errno);
        return;
        }
    (void)close (fd);

    (void)rtpSpawn (args[0], args, NULL, 220, 0x10000, RTP_LOADED_WAIT, VX_FP_TASK);
    }

void gfxFbMainDemoRtp
    (
    unsigned int device,
    unsigned int count
    )
    {
    gfxFbMainDemoRtp0 ("gfxFbMainDemo.vxe", device, count);
    }

void gfxFbMainSoDemoRtp
    (
    unsigned int device,
    unsigned int count
    )
    {
    gfxFbMainDemoRtp0 ("gfxFbMainSoDemo.vxe", device, count);
    }
#else
/*******************************************************************************
 *
 * main - start of the program in RTP mode
 *
 * This is the RTP mode entry point. An unsigned integer argument
 * can be used when invoking to set an upper limit (in seconds) on how
 * long the program should run.  A value of 0 allows the program to run for an
 * indefinite amount of time.
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
int main
    (
    int argc,
    char *argv[]
    )
    {
    char *endptr;
    unsigned int i;
    unsigned int device = 0;
    unsigned int count = 0;

    if (argc > 1)
        {
        i = (unsigned int) strtoul(argv[1], &endptr, 0);
        if (endptr != argv[1])
            {
            device = i;
            }
        }

    if (argc > 2)
        {
        i = (unsigned int) strtoul(argv[2], &endptr, 0);
        if (endptr != argv[2])
            {
            count = i;
            }
        }

    return (gfxFbMainDemo (device, count));
    }
#endif
