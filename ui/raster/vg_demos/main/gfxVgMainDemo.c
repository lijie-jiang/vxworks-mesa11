/* gfxVgMainDemo.c - Main demo */

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
19nov15,yat  Fix bzero (US24710)
30mar15,rpc  Static analysis fixes (US50633)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
18jun14,yat  Added main demo for dynamic RTP (US11227)
25jun13,mgc  Modified for VxWorks 7 release
15apr13,af   Written (based on Pattern Demo written by m_c)
*/

/*

DESCRIPTION

This small program cycles through each of the program. Each program is run
for a pre-determined amount of time (controlled by RUN_TIME_SECS in seconds).

*/

/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ioLib.h>
#include <taskLib.h>
#include <rtpLib.h>
#include <fbdev.h>
#if !defined(_WRS_KERNEL)
#if defined(_GL)
#include <gfxSwapDemo.inl>
#include <gfxGearDemo.inl>
#endif
#if defined(_ES1)
#include <gfxEs1GearDemo.inl>
#endif
#if defined(_ES2)
#include <gfxEs2SwapDemo.inl>
#include <gfxEs2GearDemo.inl>
#include <gfxEs2LogoDemo.inl>
#endif
#if defined(_ES3)
#include <gfxEs3SwapDemo.inl>
#endif
#if defined(_VG)
#include <gfxVgSwapDemo.inl>
#if defined(_VG_IMAGE)
#include <gfxVgFlowDemo.inl>
#include <gfxVgImageDemo.inl>
#endif
#include <gfxVgPatternDemo.inl>
#include <gfxVgScrollDemo.inl>
#include <gfxVgSubwayDemo.inl>
#endif
#if defined(_CL)
#include <gfxClSimpleDemo.inl>
#endif
#if defined(_VG)||(_MESA)
#include <pthread.h>
#endif
#endif

/* defines */

#define MAX_COUNT           3
#define RUN_TIME_SECS       20
#define MAIN_SLEEP_SECS     5

#if !defined(GFX_USE_EGL_FBDEV)
#define GFX_USE_EGL_FBDEV 0
#define GFX_USE_EGL_DRI   1
#endif

#ifdef DEFAULT_STACK_SIZE
#undef DEFAULT_STACK_SIZE
#endif
#define DEFAULT_STACK_SIZE 0x50000

/* forward declarations */

#if defined(_WRS_KERNEL)
#if defined(_GL)
extern int gfxRasterSwapDemo (unsigned int, unsigned int, unsigned int, unsigned int);
extern int gfxRasterGearDemo (unsigned int, unsigned int, unsigned int);
#endif
#if defined(_ES1)
extern int gfxRasterEs1GearDemo (unsigned int, unsigned int);
#endif
#if defined(_ES2)
extern int gfxRasterEs2SwapDemo (unsigned int, unsigned int, unsigned int, unsigned int);
extern int gfxRasterEs2GearDemo (unsigned int, unsigned int, unsigned int);
extern int gfxRasterEs2LogoDemo (unsigned int, unsigned int, unsigned int, unsigned int);
#endif
#if defined(_ES3)
extern int gfxRasterEs3SwapDemo (unsigned int, unsigned int, unsigned int, unsigned int);
#endif
#if defined(_VG)
extern int gfxRasterVgSwapDemo (unsigned int, unsigned int, unsigned int);
#if defined(_VG_IMAGE)
extern int gfxRasterVgFlowDemo (unsigned int, unsigned int);
extern int gfxRasterVgImageDemo (unsigned int, unsigned int);
#endif
extern int gfxRasterVgPatternDemo (unsigned int, unsigned int);
extern int gfxRasterVgScrollDemo (unsigned int, unsigned int);
extern int gfxRasterVgSubwayDemo (unsigned int, unsigned int);
#endif
#if defined(_CL)
extern int gfxRasterClSimpleDemo (unsigned int);
#endif
#else
#if defined(_GL)
#define gfxRasterSwapDemo gfxSwapDemo
#define gfxRasterGearDemo gfxGearDemo
#endif
#if defined(_ES1)
#define gfxRasterEs1GearDemo gfxEs1GearDemo
#endif
#if defined(_ES2)
#define gfxRasterEs2SwapDemo gfxEs2SwapDemo
#define gfxRasterEs2GearDemo gfxEs2GearDemo
#define gfxRasterEs2LogoDemo gfxEs2LogoDemo
#endif
#if defined(_ES3)
#define gfxRasterEs3SwapDemo gfxEs3SwapDemo
#endif
#if defined(_VG)
#define gfxRasterVgSwapDemo gfxVgSwapDemo
#if defined(_VG_IMAGE)
#define gfxRasterVgFlowDemo gfxVgFlowDemo
#define gfxRasterVgImageDemo gfxVgImageDemo
#endif
#define gfxRasterVgPatternDemo gfxVgPatternDemo
#define gfxRasterVgScrollDemo gfxVgScrollDemo
#define gfxRasterVgSubwayDemo gfxVgSubwayDemo
#endif
#if defined(_CL)
#define gfxRasterClSimpleDemo gfxClSimpleDemo
#endif
#endif

/*******************************************************************************
 *
 * gfxVgMainDemo - start of the program
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
int gfxVgMainDemo
    (
    unsigned int device,
    unsigned int count,
    unsigned int eglDriver,
    unsigned int runCl
    )
    {
    int i, j;
    int fd;
    FB_IOCTL_ARG arg;
    FB_VIDEO_MODE fbModes[FB_MAX_VIDEO_MODES];
    char deviceName[FB_MAX_STR_CHARS];
#if defined(_CL)
    (void)fprintf (stdout, "Run OpenCL program: %s\n", runCl?"yes":"no");
    if (runCl)
        {
        if (chdir ("/ram0") != OK)
            {
            (void)fprintf (stderr, "chdir /ram0 failed\n");
            return (EXIT_SUCCESS);
            }
        }
#endif
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
        fd = 0;
        }
    else
        {
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
        }

    if (count == 0) count = MAX_COUNT;

    for (i = 0; i < FB_MAX_VIDEO_MODES; i++)
        {
        if (fd && !fbModes[i].xres) break;
#if defined(_SET_VIDEO_MODE)
        if (fd && (i > 0))
            {
            arg.setVideoMode.videoMode = fbModes[i].name;
            if (ioctl (fd, FB_IOCTL_SET_VIDEO_MODE, &arg) == -1)
                {
                (void)fprintf (stderr, "ERROR: FB_IOCTL_SET_VIDEO_MODE errno:%08x\n", errno);
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)printf ("Set video mode errno:%08x videoMode:%s\n", errno,
                          fbModes[i].name);
            }
#else
        if (i > 0) break;
#endif
        for (j = 1; j <= count; j++)
            {
#if defined(_GL)
            (void)printf ("Run OpenGL swap interval 1 demo:%d of %d\n", j, count);
            if (gfxRasterSwapDemo (device, RUN_TIME_SECS, eglDriver, 1) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL swap interval 2 demo:%d of %d\n", j, count);
            if (gfxRasterSwapDemo (device, RUN_TIME_SECS, eglDriver, 2) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL gear demo:%d of %d\n", j, count);
            if (gfxRasterGearDemo (device, RUN_TIME_SECS, eglDriver) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_ES1)
            (void)printf ("Run OpenGL ES1 gear demo:%d of %d\n", j, count);
            if (gfxRasterEs1GearDemo (device, RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_ES2)
            (void)printf ("Run OpenGL ES2 swap interval 1 demo:%d of %d\n", j, count);
            if (gfxRasterEs2SwapDemo (device, RUN_TIME_SECS, eglDriver, 1) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES2 swap interval 2 demo:%d of %d\n", j, count);
            if (gfxRasterEs2SwapDemo (device, RUN_TIME_SECS, eglDriver, 2) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES2 gear demo:%d of %d\n", j, count);
            if (gfxRasterEs2GearDemo (device, RUN_TIME_SECS, eglDriver) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES2 logo demo:%d of %d\n", j, count);
            if (gfxRasterEs2LogoDemo (device, RUN_TIME_SECS, eglDriver, 10) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_ES3)
            (void)printf ("Run OpenGL ES3 swap interval 1 demo:%d of %d\n", j, count);
            if (gfxRasterEs3SwapDemo (device, RUN_TIME_SECS, eglDriver, 1) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES3 swap interval 2 demo:%d of %d\n", j, count);
            if (gfxRasterEs3SwapDemo (device, RUN_TIME_SECS, eglDriver, 2) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_VG)
            (void)printf ("Run OpenVG swap interval 1 demo:%d of %d\n", j, count);
            if (gfxRasterVgSwapDemo (device, RUN_TIME_SECS, 1) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenVG swap interval 2 demo:%d of %d\n", j, count);
            if (gfxRasterVgSwapDemo (device, RUN_TIME_SECS, 2) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#if defined(_VG_IMAGE)
            (void)printf ("Run OpenVG flow demo:%d of %d\n", j, count);
            if (gfxRasterVgFlowDemo (device, RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenVG image demo:%d of %d\n", j, count);
            if (gfxRasterVgImageDemo (device, RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
            (void)printf ("Run OpenVG pattern demo:%d of %d\n", j, count);
            if (gfxRasterVgPatternDemo (device, RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenVG scroll demo:%d of %d\n", j, count);
            if (gfxRasterVgScrollDemo (device, RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenVG subway demo:%d of %d\n", j, count);
            if (gfxRasterVgSubwayDemo (device, RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_CL)
            if (!runCl) continue;
            (void)printf ("Run OpenCL simple demo:%d of %d\n", j, count);
            if (gfxRasterClSimpleDemo (RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
            }
        }

    (void)close (fd);
    (void)printf ("Close device:%s\n", deviceName);
    (void)printf ("DEMO COMPLETED WITHOUT ERROR\n");

    return (EXIT_SUCCESS);
    }

#if defined(_WRS_KERNEL)
#if defined(_CL)
#include <dosFsLib.h>
#include <iosLib.h>

extern DEV_HDR *iosDevMatch (const char * name);
#endif
/*******************************************************************************
 *
 * gfxVgMainDemoStart - start of the program in kernel mode
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
 * SEE ALSO: gfxVgMainDemoRtp
 *
 */
void gfxVgMainDemoStart
    (
    unsigned int device,
    unsigned int count,
    unsigned int eglDriver
    )
    {
#if defined(_CL)
    int runCl = TRUE;

    if (!iosDevMatch ("/ram0"))
        {
        (void)fprintf (stderr, "Add INCLUDE_RAM_DISK in kernel to run OpenCL program\n");
        runCl = FALSE;
        }
    else
        {
        if (dosFsVolFormat ("/ram0", DOS_OPT_DEFAULT, NULL) != OK)
            {
            (void)fprintf (stderr, "dosFsVolFormat /ram0 failed\n");
            return;
            }
        }
#else
    int runCl = FALSE;
#endif
    (void)taskSpawn ("tMain", 220, VX_FP_TASK, DEFAULT_STACK_SIZE,
                     (FUNCPTR) gfxVgMainDemo,
                     device, count, eglDriver, runCl, 0, 0, 0, 0, 0, 0 );
    }
#if defined(GFX_USE_GBM)
void gfxVgDriMainDemoStart
    (
    unsigned int device,
    unsigned int count,
    unsigned int eglDriver
    )
    {
    gfxVgMainDemoStart (device, count, GFX_USE_EGL_DRI);
    }
#endif
/*******************************************************************************
 *
 * gfxVgMainDemoRtp - start of the program for invoking RTP
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
 * SEE ALSO: gfxVgMainDemoStart
 *
 */
static void gfxVgMainDemoRtp0
    (
    const char *rtpStr,
    unsigned int device,
    unsigned int count,
    unsigned int eglDriver
    )
    {
    int fd;
    const char *args[6];
    char deviceStr[32];
    char countStr[32];
    char eglDriverStr[32];
    char runClStr[32];
#if defined(_CL)
    int runCl = TRUE;

    if (!iosDevMatch ("/ram0"))
        {
        (void)fprintf (stderr, "Add INCLUDE_RAM_DISK in kernel to run OpenCL program\n");
        runCl = FALSE;
        }
    else
        {
        if (dosFsVolFormat ("/ram0", DOS_OPT_DEFAULT, NULL) != OK)
            {
            (void)fprintf (stderr, "dosFsVolFormat /ram0 failed\n");
            return;
            }
        }
#else
    int runCl = FALSE;
#endif
    (void)snprintf (deviceStr, 32, "%d", device);
    (void)snprintf (countStr, 32, "%d", count);
    (void)snprintf (eglDriverStr, 32, "%d", eglDriver);
    (void)snprintf (runClStr, 32, "%d", runCl);

    args[0] = rtpStr;
    args[1] = deviceStr;
    args[2] = countStr;
    args[3] = eglDriverStr;
    args[4] = runClStr;
    args[5] = NULL;

    if ((fd = open (args[0], O_RDONLY, 0)) == -1)
        {
        (void)fprintf (stderr, "Open RTP file %s errno:%08x\n", args[0], errno);
        return;
        }
    (void)close (fd);

    (void)rtpSpawn (args[0], args, NULL, 220, DEFAULT_STACK_SIZE,
                    RTP_LOADED_WAIT, VX_FP_TASK);
    }

void gfxVgMainDemoRtp
    (
    unsigned int device,
    unsigned int count,
    unsigned int eglDriver
    )
    {
    gfxVgMainDemoRtp0 ("gfxVgMainDemo.vxe", device, count, eglDriver);
    }

void gfxVgMainSoDemoRtp
    (
    unsigned int device,
    unsigned int count,
    unsigned int eglDriver
    )
    {
    gfxVgMainDemoRtp0 ("gfxVgMainSoDemo.vxe", device, count, eglDriver);
    }

#if defined(GFX_USE_GBM)
void gfxVgDriMainDemoRtp
    (
    unsigned int device,
    unsigned int count
    )
    {
    gfxVgMainDemoRtp (device, count, GFX_USE_EGL_DRI);
    }

void gfxVgDriMainSoDemoRtp
    (
    unsigned int device,
    unsigned int count
    )
    {
    gfxVgMainSoDemoRtp (device, count, GFX_USE_EGL_DRI);
    }
#endif
#else
#if defined(_VG)||(_MESA)
static void* dummy_function
    (
    void* p
    )
    {
    pthread_exit(0);
    return p;
    }
#endif
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
    unsigned int eglDriver = 0;
    unsigned int runCl = FALSE;
#if defined(_VG)||(_MESA)
    pthread_t thread1;
#endif
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

    if (argc > 3)
        {
        i = (unsigned int) strtoul(argv[3], &endptr, 0);
        if (endptr != argv[3])
            {
            eglDriver = i;
            }
        }
#if defined(_CL)
    if (argc > 4)
        {
        i = (unsigned int) strtoul(argv[4], &endptr, 0);
        if (endptr != argv[4])
            {
            runCl = i;
            }
        }
#endif
#if defined(_VG)||(_MESA)
    if (pthread_create (&thread1, NULL, &dummy_function, NULL) == ENOSYS)
        {
        (void)fprintf (stderr, "\nAdd INCLUDE_POSIX_PTHREAD_SCHEDULER in kernel to run demo as RTP\n");
        return (EXIT_FAILURE);
        }

    (void)pthread_join(thread1, NULL);
#endif
    return (gfxVgMainDemo (device, count, eglDriver, runCl));
    }
#endif
