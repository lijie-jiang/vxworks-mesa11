/* gfxKmsDrawDemo.c - KMS Draw Demo */

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
16sep15,yat  Written (US24710)
*/

/*

DESCRIPTION

This program provides the KMS draw demonstration program.


The program is started by executing the application's entry point,
gfxKmsDrawDemoStart; as follows:

-> gfxKmsDrawDemoStart(0, 0)

*/

/* includes */

#include "gfxKmsDrawDemo.inl"

#if defined(_WRS_KERNEL)
/*******************************************************************************
 *
 * gfxKmsDrawDemoStart - start of the program in kernel mode
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
 * SEE ALSO: gfxKmsDrawDemoRtp
 *
 */
void gfxKmsDrawDemoStart
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int drawX0,
    unsigned int drawY0,
    unsigned int drawX1,
    unsigned int drawY1
    )
    {
    (void)taskSpawn ("tDraw", 100, VX_FP_TASK, 0x10000,
                     (FUNCPTR) gfxKmsDrawDemo,
                     device, runTime, drawX0, drawY0, drawX1, drawY1, 0, 0, 0, 0 );
    }

/*******************************************************************************
 *
 * gfxKmsDrawDemoRtp - start of the program for invoking RTP
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
 * SEE ALSO: gfxKmsDrawDemoStart
 *
 */
static void gfxKmsDrawDemoRtp0
    (
    const char *rtpStr,
    unsigned int device,
    unsigned int runTime,
    unsigned int drawX0,
    unsigned int drawY0,
    unsigned int drawX1,
    unsigned int drawY1
    )
    {
    int fd;
    const char *args[8];
    char deviceStr[32];
    char runTimeStr[32];
    char drawX0Str[32];
    char drawY0Str[32];
    char drawX1Str[32];
    char drawY1Str[32];

    (void)snprintf (deviceStr, 32, "%d", device);
    (void)snprintf (runTimeStr, 32, "%d", runTime);
    (void)snprintf (drawX0Str, 32, "%d", drawX0);
    (void)snprintf (drawY0Str, 32, "%d", drawY0);
    (void)snprintf (drawX1Str, 32, "%d", drawX1);
    (void)snprintf (drawY1Str, 32, "%d", drawY1);

    args[0] = rtpStr;
    args[1] = deviceStr;
    args[2] = runTimeStr;
    args[3] = drawX0Str;
    args[4] = drawY0Str;
    args[5] = drawX1Str;
    args[6] = drawY1Str;
    args[7] = NULL;

    if ((fd = open (args[0], O_RDONLY, 0)) == -1)
        {
        (void)fprintf (stderr, "Open RTP file %s errno:%08x\n", args[0], errno);
        return;
        }
    (void)close (fd);

    (void)rtpSpawn (args[0], args, NULL, 220, 0x10000, RTP_LOADED_WAIT, VX_FP_TASK);
    }

void gfxKmsDrawDemoRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int drawX0,
    unsigned int drawY0,
    unsigned int drawX1,
    unsigned int drawY1
    )
    {
    gfxKmsDrawDemoRtp0 ("gfxKmsDrawDemo.vxe", device, runTime, drawX0, drawY0, drawX1, drawY1);
    }

void gfxKmsDrawSoDemoRtp
    (
    unsigned int device,
    unsigned int runTime,
    unsigned int drawX0,
    unsigned int drawY0,
    unsigned int drawX1,
    unsigned int drawY1
    )
    {
    gfxKmsDrawDemoRtp0 ("gfxKmsDrawSoDemo.vxe", device, runTime, drawX0, drawY0, drawX1, drawY1);
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
    unsigned int drawX0 = 0;
    unsigned int drawY0 = 0;
    unsigned int drawX1 = 0;
    unsigned int drawY1 = 0;

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

    if (argc > 6)
        {
        i = (unsigned int) strtoul(argv[3], &endptr, 0);
        if (endptr != argv[3])
            {
            drawX0 = i;
            }
        i = (unsigned int) strtoul(argv[4], &endptr, 0);
        if (endptr != argv[4])
            {
            drawY0 = i;
            }
        i = (unsigned int) strtoul(argv[5], &endptr, 0);
        if (endptr != argv[5])
            {
            drawX1 = i;
            }
        i = (unsigned int) strtoul(argv[6], &endptr, 0);
        if (endptr != argv[6])
            {
            drawY1 = i;
            }
        }

    return (gfxKmsDrawDemo (device, runTime, drawX0, drawY0, drawX1, drawY1));
    }
#endif
