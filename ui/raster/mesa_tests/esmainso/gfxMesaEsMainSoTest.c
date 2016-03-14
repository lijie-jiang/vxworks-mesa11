/* gfxMesaEsMainTest.c - Main test */

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
21sep15,yat  Add support for Mesa GPU DRI (US24710)
21apr15,yat  Add check for INCLUDE_POSIX_PTHREAD_SCHEDULER (V7GFX-249)
08apr15,yat  Add multi-shader test
22dec14,yat  Create OpenGL demo (US24712)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
04nov14,yat  Created test for OpenCL (US48896)
18sep14,yat  Add support for dynamic RTP demos (US40486)
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
#include <string.h>
#include <strings.h>
#include <ioLib.h>
#include <taskLib.h>
#include <rtpLib.h>
#include <fbdev.h>
#if !defined(_WRS_KERNEL)
#if defined(_GL)
#include <gfxPolygonTest.inl>
#include <gfxTexturingTest.inl>
#endif
#if defined(_ES2)
#include <gfxEs2MultiGearTest.inl>
#include <gfxEs2MultiShaderTest.inl>
#include <gfxEs2PolygonTest.inl>
#include <gfxEs2TexturingTest.inl>
#endif
#if defined(_CL)
#include <gfxClInfoTest.inl>
#include <gfxClLoadStoreTest.inl>
#include <gfxClMathTest.inl>
#include <gfxClFftTest.inl>
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
extern int gfxMesaPolygonTest (unsigned int, unsigned int, unsigned int, unsigned int);
extern int gfxMesaTexturingTest (unsigned int, unsigned int, unsigned int, unsigned int);
#endif
#if defined(_ES2)
extern int gfxMesaEs2MultiGearTest (unsigned int, unsigned int, unsigned int, unsigned int);
extern int gfxMesaEs2MultiShaderTest (unsigned int, unsigned int, unsigned int);
extern int gfxMesaEs2PolygonTest (unsigned int, unsigned int, unsigned int, unsigned int);
extern int gfxMesaEs2TexturingTest (unsigned int, unsigned int, unsigned int, unsigned int);
#endif
#if defined(_CL)
extern int gfxMesaClInfoTest (unsigned int);
extern int gfxMesaClLoadStoreTest (unsigned int, unsigned int);
extern int gfxMesaClMathTest (unsigned int, unsigned int);
extern int gfxMesaClFftTest (unsigned int, unsigned int);
#endif
#if defined(_DEMOS)
#if defined(_GL)
extern int gfxMesaSwapDemo (unsigned int, unsigned int, unsigned int, unsigned int);
extern int gfxMesaGearDemo (unsigned int, unsigned int, unsigned int);
#endif
#if defined(_ES1)
extern int gfxMesaEs1GearDemo (unsigned int, unsigned int);
#endif
#if defined(_ES2)
extern int gfxMesaEs2SwapDemo (unsigned int, unsigned int, unsigned int, unsigned int);
extern int gfxMesaEs2GearDemo (unsigned int, unsigned int, unsigned int);
extern int gfxMesaEs2LogoDemo (unsigned int, unsigned int, unsigned int, unsigned int);
#endif
#if defined(_ES3)
extern int gfxMesaEs3SwapDemo (unsigned int, unsigned int, unsigned int, unsigned int);
#endif
#if defined(_VG)
extern int gfxMesaVgSwapDemo (unsigned int, unsigned int, unsigned int);
#if defined(_VG_IMAGE)
extern int gfxRasterVgFlowDemo (unsigned int, unsigned int);
extern int gfxRasterVgImageDemo (unsigned int, unsigned int);
#endif
extern int gfxMesaVgPatternDemo (unsigned int, unsigned int);
extern int gfxMesaVgScrollDemo (unsigned int, unsigned int);
extern int gfxMesaVgSubwayDemo (unsigned int, unsigned int);
#endif
#if defined(_CL)
extern int gfxMesaClSimpleDemo (unsigned int);
#endif
#endif
#else
#if defined(_GL)
#define gfxMesaPolygonTest gfxPolygonTest
#define gfxMesaTexturingTest gfxTexturingTest
#endif
#if defined(_ES2)
#define gfxMesaEs2MultiGearTest gfxEs2MultiGearTest
#define gfxMesaEs2MultiShaderTest gfxEs2MultiShaderTest
#define gfxMesaEs2PolygonTest gfxEs2PolygonTest
#define gfxMesaEs2TexturingTest gfxEs2TexturingTest
#endif
#if defined(_CL)
#define gfxMesaClInfoTest gfxClInfoTest
#define gfxMesaClLoadStoreTest gfxClLoadStoreTest
#define gfxMesaClMathTest gfxClMathTest
#define gfxMesaClFftTest gfxClFftTest
#endif
#endif

/*******************************************************************************
 *
 * gfxMesaEsMainTest - start of the program
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
int gfxMesaEsMainTest
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
            (void)printf ("Run OpenGL polygon test:%d of %d\n", j, count);
            if (gfxMesaPolygonTest (device, RUN_TIME_SECS, eglDriver, 6000) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL texturing test:%d of %d\n", j, count);
            if (gfxMesaTexturingTest (device, RUN_TIME_SECS, eglDriver, 1) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_ES2)
            (void)printf ("Run OpenGL ES2 multi-gear test:%d of %d\n", j, count);
            if (gfxMesaEs2MultiGearTest (device, RUN_TIME_SECS, eglDriver, 10) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES2 multi-shader test:%d of %d\n", j, count);
            if (gfxMesaEs2MultiShaderTest (device, RUN_TIME_SECS, eglDriver) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES2 polygon test:%d of %d\n", j, count);
            if (gfxMesaEs2PolygonTest (device, RUN_TIME_SECS, eglDriver, 6000) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES2 texturing test:%d of %d\n", j, count);
            if (gfxMesaEs2TexturingTest (device, RUN_TIME_SECS, eglDriver, 1) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_CL)
            if (runCl)
                {
            (void)printf ("Run OpenCL Info test:%d of %d\n", j, count);
            if (gfxMesaClInfoTest (RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenCL LoadStore test:%d of %d\n", j, count);
            if (gfxMesaClLoadStoreTest (RUN_TIME_SECS, 100) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenCL Math test:%d of %d\n", j, count);
            if (gfxMesaClMathTest (RUN_TIME_SECS, 100) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenCL FFT test:%d of %d\n", j, count);
            if (gfxMesaClFftTest (RUN_TIME_SECS, 1024) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
                }
#endif
#if defined(_WRS_KERNEL)
#if defined(_DEMOS)
#if defined(_GL)
            (void)printf ("Run OpenGL swap interval 1 demo:%d of %d\n", j, count);
            if (gfxMesaSwapDemo (device, RUN_TIME_SECS, eglDriver, 1) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL swap interval 2 demo:%d of %d\n", j, count);
            if (gfxMesaSwapDemo (device, RUN_TIME_SECS, eglDriver, 2) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL gear demo:%d of %d\n", j, count);
            if (gfxMesaGearDemo (device, RUN_TIME_SECS, eglDriver) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_ES1)
            (void)printf ("Run OpenGL ES1 gear demo:%d of %d\n", j, count);
            if (gfxMesaEs1GearDemo (device, RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_ES2)
            (void)printf ("Run OpenGL ES2 swap interval 1 demo:%d of %d\n", j, count);
            if (gfxMesaEs2SwapDemo (device, RUN_TIME_SECS, eglDriver, 1) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES2 swap interval 2 demo:%d of %d\n", j, count);
            if (gfxMesaEs2SwapDemo (device, RUN_TIME_SECS, eglDriver, 2) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES2 gear demo:%d of %d\n", j, count);
            if (gfxMesaEs2GearDemo (device, RUN_TIME_SECS, eglDriver) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES2 logo demo:%d of %d\n", j, count);
            if (gfxMesaEs2LogoDemo (device, RUN_TIME_SECS, eglDriver, 10) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_ES3)
            (void)printf ("Run OpenGL ES3 swap interval 1 demo:%d of %d\n", j, count);
            if (gfxMesaEs3SwapDemo (device, RUN_TIME_SECS, eglDriver, 1) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenGL ES3 swap interval 2 demo:%d of %d\n", j, count);
            if (gfxMesaEs3SwapDemo (device, RUN_TIME_SECS, eglDriver, 2) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_VG)
            (void)printf ("Run OpenVG swap interval 1 demo:%d of %d\n", j, count);
            if (gfxMesaVgSwapDemo (device, RUN_TIME_SECS, 1) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenVG swap interval 2 demo:%d of %d\n", j, count);
            if (gfxMesaVgSwapDemo (device, RUN_TIME_SECS, 2) == EXIT_FAILURE)
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
            if (gfxMesaVgPatternDemo (device, RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenVG scroll demo:%d of %d\n", j, count);
            if (gfxMesaVgScrollDemo (device, RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
            (void)printf ("Run OpenVG subway demo:%d of %d\n", j, count);
            if (gfxMesaVgSubwayDemo (device, RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#if defined(_CL)
            if (!runCl) continue;
            (void)printf ("Run OpenCL simple demo:%d of %d\n", j, count);
            if (gfxMesaClSimpleDemo (RUN_TIME_SECS) == EXIT_FAILURE)
                {
                (void)close (fd);
                return (EXIT_FAILURE);
                }
            (void)sleep (MAIN_SLEEP_SECS);
#endif
#endif
#endif
            }
        }

    (void)close (fd);
    (void)printf ("Close device:%s\n", deviceName);
    (void)printf ("TEST COMPLETED WITHOUT ERROR\n");

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
 * gfxMesaEsMainTestStart - start of the program in kernel mode
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
 * SEE ALSO: gfxMesaEsMainTestRtp
 *
 */
void gfxMesaEsMainTestStart
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
                     (FUNCPTR) gfxMesaEsMainTest,
                     device, count, eglDriver, runCl, 0, 0, 0, 0, 0, 0 );
    }
#if defined(GFX_USE_GBM)
void gfxMesaDriEsMainTestStart
    (
    unsigned int device,
    unsigned int count
    )
    {
    gfxMesaEsMainTestStart (device, count, GFX_USE_EGL_DRI);
    }
#endif
/*******************************************************************************
 *
 * gfxMesaEsMainTestRtp - start of the program for invoking RTP
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
 * SEE ALSO: gfxMesaEsMainTestStart
 *
 */
static void gfxMesaEsMainTestRtp0
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

void gfxMesaEsMainTestRtp
    (
    unsigned int device,
    unsigned int count,
    unsigned int eglDriver
    )
    {
    gfxMesaEsMainTestRtp0 ("gfxMesaEsMainTest.vxe", device, count, eglDriver);
    }

void gfxMesaEsMainSoTestRtp
    (
    unsigned int device,
    unsigned int count,
    unsigned int eglDriver
    )
    {
    gfxMesaEsMainTestRtp0 ("gfxMesaEsMainSoTest.vxe", device, count, eglDriver);
    }

#if defined(GFX_USE_GBM)
void gfxMesaDriEsMainTestRtp
    (
    unsigned int device,
    unsigned int count
    )
    {
    gfxMesaEsMainTestRtp (device, count, GFX_USE_EGL_DRI);
    }

void gfxMesaDriEsMainSoTestRtp
    (
    unsigned int device,
    unsigned int count
    )
    {
    gfxMesaEsMainSoTestRtp (device, count, GFX_USE_EGL_DRI);
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
        (void)fprintf (stderr, "\nAdd INCLUDE_POSIX_PTHREAD_SCHEDULER in kernel to run test as RTP\n");
        return (EXIT_FAILURE);
        }

    (void)pthread_join(thread1, NULL);
#endif
    return (gfxMesaEsMainTest (device, count, eglDriver, runCl));
    }
#endif
