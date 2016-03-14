/* gfxFbCopyTest.c - Frame buffer copy test */

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
17feb16,yat  Add frame buffer copy test (US73758)
01apr15,yat  Resolve defects from static analysis run (US50633)
01oct14,yat  Fix defects from static analysis run
24jan14,mgc  Modified for VxWorks 7 release
*/

/*

DESCRIPTION

This program provides the frame buffer copy program.


The program is started by executing the application's entry point,
gfxFbCopyTestStart; as follows:

-> gfxFbCopyTestStart(0, 0)

*/

/* includes */

#include "gfxFbCopyTest.inl"

#if defined(_WRS_KERNEL)
/*******************************************************************************
 *
 * gfxFbCopyTestStart - start of the program in kernel mode
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
 * SEE ALSO: gfxFbCopyTestRtp
 *
 */
void gfxFbCopyTestStart
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int mode
    )
    {
    (void)taskSpawn ("tDraw", 100, VX_FP_TASK, 0x10000,
                     (FUNCPTR) gfxFbCopyTest,
                     device, runTime, mode, 0, 0, 0, 0, 0, 0, 0 );
    }

/*******************************************************************************
 *
 * gfxFbCopyTestRtp - start of the program for invoking RTP
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
 * SEE ALSO: gfxFbCopyTestStart
 *
 */
static void gfxFbCopyTestRtp0
    (
    const char *rtpStr,
    unsigned int device,
    unsigned int runTime,
    unsigned int mode
    )
    {
    int fd;
    const char *args[5];
    char deviceStr[32];
    char runTimeStr[32];
    char modeStr[32];

    (void)snprintf (deviceStr, 32, "%d", device);
    (void)snprintf (runTimeStr, 32, "%d", runTime);
    (void)snprintf (modeStr, 32, "%d", mode);

    args[0] = rtpStr;
    args[1] = deviceStr;
    args[2] = runTimeStr;
    args[3] = modeStr;
    args[4] = NULL;

    if ((fd = open (args[0], O_RDONLY, 0)) == -1)
        {
        (void)fprintf (stderr, "Open RTP file %s errno:%08x\n", args[0], errno);
        return;
        }
    (void)close (fd);

    (void)rtpSpawn (args[0], args, NULL, 220, 0x10000, RTP_LOADED_WAIT, VX_FP_TASK);
    }

void gfxFbCopyTestRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int mode
    )
    {
    gfxFbCopyTestRtp0 ("gfxFbCopyTest.vxe", device, runTime, mode);
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
    unsigned int runTime = 0;
    unsigned int mode = 0;

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
            mode = i;
            }
        }

    return (gfxFbCopyTest (device, runTime, mode));
    }
#endif
