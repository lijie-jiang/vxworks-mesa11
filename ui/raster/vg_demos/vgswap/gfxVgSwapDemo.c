/* gfxVgSwapDemo.c - Vector Graphics Swap Demo */

/*
 * Copyright (c) 2014-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
30mar15,rpc  Static analysis fixes (US50633)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
13nov14,yat  Created Vector Graphics swap demo (US46449)
*/

/*

DESCRIPTION

This example program provides the Vector Graphics Swap program.

The program is started by executing the application's entry point,
gfxVgSwapDemoStart; as follows:

-> gfxVgSwapDemoStart(0,0,1)

*/

/* includes */

#include "gfxVgSwapDemo.inl"
#if !defined(_WRS_KERNEL)
#include <pthread.h>
#endif

/*******************************************************************************
 *
 * gfxRasterVgSwapDemo - start of the program
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
int gfxRasterVgSwapDemo
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int interval
    )
    {
    return gfxVgSwapDemo(device, runTime, interval);
    }

#if defined(_WRS_KERNEL)
/*******************************************************************************
 *
 * gfxVgSwapDemoStart - start of the program in kernel mode
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
 * SEE ALSO: gfxVgSwapDemoRtp
 *
 */
void gfxVgSwapDemoStart
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int interval
    )
    {
    (void)taskSpawn ("tSwap", 100, VX_FP_TASK, 0x10000,
                     (FUNCPTR) gfxRasterVgSwapDemo,
                     device, runTime, interval, 0, 0, 0, 0, 0, 0, 0 );
    }

/*******************************************************************************
 *
 * gfxVgSwapDemoRtp - start of the program for invoking RTP
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
 * SEE ALSO: gfxVgSwapDemoStart
 *
 */
static void gfxVgSwapDemoRtp0
    (
    const char *rtpStr,
    unsigned int device,
    unsigned int runTime,
    unsigned int interval
    )
    {
    int fd;
    const char *args[5];
    char deviceStr[32];
    char runTimeStr[32];
    char intervalStr[32];

    (void)snprintf (deviceStr, 32, "%d", device);
    (void)snprintf (runTimeStr, 32, "%d", runTime);
    (void)snprintf (intervalStr, 32, "%d", interval);

    args[0] = rtpStr;
    args[1] = deviceStr;
    args[2] = runTimeStr;
    args[3] = intervalStr;
    args[4] = NULL;

    if ((fd = open (args[0], O_RDONLY, 0)) == -1)
        {
        (void)fprintf (stderr, "Open RTP file %s errno:%08x\n", args[0], errno);
        return;
        }
    (void)close (fd);

    (void)rtpSpawn (args[0], args, NULL, 220, 0x10000, RTP_LOADED_WAIT, VX_FP_TASK);
    }

void gfxVgSwapDemoRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int interval
    )
    {
    gfxVgSwapDemoRtp0 ("gfxVgSwapDemo.vxe", device, runTime, interval);
    }

void gfxVgSwapSoDemoRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int interval
    )
    {
    gfxVgSwapDemoRtp0 ("gfxVgSwapSoDemo.vxe", device, runTime, interval);
    }
#else
static void dummy_function
    (
    void
    )
    {
    pthread_exit(0);
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
    unsigned int interval = 1;
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
            interval = i;
            }
        }

    if (pthread_create (&thread1, NULL, (void *) &dummy_function, NULL) == ENOSYS)
        {
        (void)fprintf (stderr, "Add INCLUDE_POSIX_PTHREAD_SCHEDULER in kernel to run demo as RTP\n");
        return (EXIT_FAILURE);
        }

    (void)pthread_join(thread1, NULL);

    return (gfxRasterVgSwapDemo (device, runTime, interval));
    }
#endif
