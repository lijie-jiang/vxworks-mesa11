/* gfxMesaEs2MultiShaderTest.c - Standard OpenGL ES2 Shaders Test */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
21sep15,yat  Add support for Mesa GPU DRI (US24710)
28jan15,yat  Create shader test (US46449)
*/

/*

DESCRIPTION

This example program provides the standard OpenGL shader program.

The program is started by executing the application's entry point,
gfxMesaEs2MultiShaderTestStart; as follows:

-> gfxMesaEs2MultiShaderTestStart(0, 0)

*/

/* includes */

#include "gfxEs2MultiShaderTest.inl"
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
 * gfxMesaEs2MultiShaderTest - start of the program
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
int gfxMesaEs2MultiShaderTest
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int eglDriver
    )
    {
    return gfxEs2MultiShaderTest(device, runTime, eglDriver);
    }

#if defined(_WRS_KERNEL)
/*******************************************************************************
 *
 * gfxMesaEs2MultiShaderTestStart - start of the program in kernel mode
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
 * SEE ALSO: gfxMesaEs2MultiShaderTestRtp
 *
 */
void gfxMesaEs2MultiShaderTestStart
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int eglDriver
    )
    {
    (void)taskSpawn ("tShader", 100, VX_FP_TASK, DEFAULT_STACK_SIZE,
                     (FUNCPTR) gfxMesaEs2MultiShaderTest,
                     device, runTime, eglDriver,
                     0, 0, 0, 0, 0, 0, 0 );
    }
#if defined(GFX_USE_GBM)
void gfxMesaDriEs2MultiShaderTestStart
    (
    unsigned int device,
    unsigned int runTime
    )
    {
    gfxMesaEs2MultiShaderTestStart (device, runTime, GFX_USE_EGL_DRI);
    }
#endif
/*******************************************************************************
 *
 * gfxMesaEs2MultiShaderTestRtp - start of the program for invoking RTP
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
 * SEE ALSO: gfxMesaEs2MultiShaderTestStart
 *
 */
static void gfxMesaEs2MultiShaderTestRtp0
    (
    const char *rtpStr,
    unsigned int device,
    unsigned int runTime,
    unsigned int eglDriver
    )
    {
    int fd;
    const char *args[5];
    char deviceStr[32];
    char runTimeStr[32];
    char eglDriverStr[32];

    (void)snprintf (deviceStr, 32, "%d", device);
    (void)snprintf (runTimeStr, 32, "%d", runTime);
    (void)snprintf (eglDriverStr, 32, "%d", eglDriver);

    args[0] = rtpStr;
    args[1] = deviceStr;
    args[2] = runTimeStr;
    args[3] = eglDriverStr;
    args[4] = NULL;

    if ((fd = open (args[0], O_RDONLY, 0)) == -1)
        {
        (void)fprintf (stderr, "Open RTP file %s errno:%08x\n", args[0], errno);
        return;
        }
    (void)close (fd);

    (void)rtpSpawn (args[0], args, NULL, 220, DEFAULT_STACK_SIZE,
                    RTP_LOADED_WAIT, VX_FP_TASK);
    }

void gfxMesaEs2MultiShaderTestRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int eglDriver
    )
    {
    gfxMesaEs2MultiShaderTestRtp0 ("gfxMesaEs2MultiShaderTest.vxe", device, runTime, eglDriver);
    }

void gfxMesaEs2MultiShaderSoTestRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int eglDriver
    )
    {
    gfxMesaEs2MultiShaderTestRtp0 ("gfxMesaEs2MultiShaderSoTest.vxe", device, runTime, eglDriver);
    }

#if defined(GFX_USE_GBM)
void gfxMesaDriEs2MultiShaderTestRtp
    (
    unsigned int device,
    unsigned int runTime
    )
    {
    gfxMesaEs2MultiShaderTestRtp (device, runTime, GFX_USE_EGL_DRI);
    }

void gfxMesaDriEs2MultiShaderSoTestRtp
    (
    unsigned int device,
    unsigned int runTime
    )
    {
    gfxMesaEs2MultiShaderSoTestRtp (device, runTime, GFX_USE_EGL_DRI);
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

    if (pthread_create (&thread1, NULL, &dummy_function, NULL) == ENOSYS)
        {
        (void)fprintf (stderr, "\nAdd INCLUDE_POSIX_PTHREAD_SCHEDULER in kernel to run test as RTP\n");
        return (EXIT_FAILURE);
        }

    (void)pthread_join(thread1, NULL);

    return (gfxMesaEs2MultiShaderTest (device, runTime, eglDriver));
    }
#endif
