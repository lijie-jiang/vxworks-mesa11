/* gfxMesaEs2LogoDemo.c - OpenGL ES2 Logo Demo */

/*
 * Copyright (c) 2011-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
21sep15,yat  Add support for Mesa GPU DRI (US24710)
21apr15,yat  Add check for INCLUDE_POSIX_PTHREAD_SCHEDULER (V7GFX-249)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
01oct14,yat  Resolve defects from static analysis run
10mar14,yat  Modified for VxWorks 7 release from demo provided by kka
*/

/*

DESCRIPTION

This example program provides the OpenGL ES2 logo program.


The program is started by executing the application's entry point,
gfxMesaEs2LogoDemoStart; as follows:

-> gfxMesaEs2LogoDemoStart(0, 0)

*/

/* includes */

#include "gfxEs2LogoDemo.inl"
#if !defined(_WRS_KERNEL)
#include <pthread.h>
#endif

/* defines */

#ifdef DEFAULT_STACK_SIZE
#undef DEFAULT_STACK_SIZE
#endif
#define DEFAULT_STACK_SIZE 0x50000

/*******************************************************************************
 *
 * gfxMesaEs2LogoDemo - start of the program
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
int gfxMesaEs2LogoDemo
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int eglDriver,
    unsigned int angle
    )
    {
    return gfxEs2LogoDemo(device, runTime, eglDriver, (GLfloat)angle);
    }

#if defined(_WRS_KERNEL)
/*******************************************************************************
 *
 * gfxMesaEs2LogoDemoStart - start of the program in kernel mode
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
 * SEE ALSO: gfxMesaEs2LogoDemoRtp
 *
 */
void gfxMesaEs2LogoDemoStart
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int eglDriver,
    unsigned int angle
    )
    {
    (void)taskSpawn ("tLogo", 100, VX_FP_TASK, DEFAULT_STACK_SIZE,
                     (FUNCPTR) gfxMesaEs2LogoDemo,
                     device, runTime, eglDriver, angle, 0, 0, 0, 0, 0, 0 );
    }
#if defined(GFX_USE_GBM)
void gfxMesaDriEs2LogoDemoStart
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int angle
    )
    {
    gfxMesaEs2LogoDemoStart (device, runTime, GFX_USE_EGL_DRI, angle);
    }
#endif
/*******************************************************************************
 *
 * gfxMesaEs2LogoDemoRtp - start of the program for invoking RTP
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
 * SEE ALSO: gfxMesaEs2LogoDemoStart
 *
 */
static void gfxMesaEs2LogoDemoRtp0
    (
    const char *rtpStr,
    unsigned int device,
    unsigned int runTime,
    unsigned int eglDriver,
    unsigned int angle
    )
    {
    int fd;
    const char *args[6];
    char deviceStr[32];
    char runTimeStr[32];
    char eglDriverStr[32];
    char angleStr[32];

    (void)snprintf (deviceStr, 32, "%d", device);
    (void)snprintf (runTimeStr, 32, "%d", runTime);
    (void)snprintf (eglDriverStr, 32, "%d", eglDriver);
    (void)snprintf (angleStr, 32, "%d", angle);

    args[0] = rtpStr;
    args[1] = deviceStr;
    args[2] = runTimeStr;
    args[3] = eglDriverStr;
    args[4] = angleStr;
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

void gfxMesaEs2LogoDemoRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int eglDriver,
    unsigned int angle
    )
    {
    gfxMesaEs2LogoDemoRtp0 ("gfxMesaEs2LogoDemo.vxe", device, runTime, eglDriver, angle);
    }

void gfxMesaEs2LogoSoDemoRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int eglDriver,
    unsigned int angle
    )
    {
    gfxMesaEs2LogoDemoRtp0 ("gfxMesaEs2LogoSoDemo.vxe", device, runTime, eglDriver, angle);
    }

#if defined(GFX_USE_GBM)
void gfxMesaDriEs2LogoDemoRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int angle
    )
    {
    gfxMesaEs2LogoDemoRtp (device, runTime, GFX_USE_EGL_DRI, angle);
    }

void gfxMesaDriEs2LogoSoDemoRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int angle
    )
    {
    gfxMesaEs2LogoSoDemoRtp (device, runTime, GFX_USE_EGL_DRI, angle);
    }
#endif
#else
static void* dummy_function
    (
    void* p
    )
    {
    pthread_exit(0);
    return p;
    }

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
    unsigned int runTime = 0;
    unsigned int eglDriver = 0;
    unsigned int angle = 0;
    pthread_t thread1;

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
            runTime = i;
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

    if (argc > 4)
        {
        i = (unsigned int) strtoul(argv[4], &endptr, 0);
        if (endptr != argv[4])
            {
            angle = i;
            }
        }

    if (pthread_create (&thread1, NULL, &dummy_function, NULL) == ENOSYS)
        {
        (void)fprintf (stderr, "\nAdd INCLUDE_POSIX_PTHREAD_SCHEDULER in kernel to run demo as RTP\n");
        return (EXIT_FAILURE);
        }

    (void)pthread_join(thread1, NULL);

    return (gfxMesaEs2LogoDemo (device, runTime, eglDriver, angle));
    }
#endif
