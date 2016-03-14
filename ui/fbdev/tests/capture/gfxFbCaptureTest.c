/* gfxFbCaptureTest.c - Frame buffer capture test */

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
24feb16,yat  Fix static analysis defects (US75033)
22dec14,yat  Created frame buffer capture test (US50456)
*/

/*

DESCRIPTION

This program captures the frame buffer.


The program is started by executing the application's entry point,
gfxFbCaptureTestStart; as follows:

-> gfxFbCaptureTestStart(0)

*/

/* includes */

#include "gfxFbCaptureTest.inl"

#if defined(_WRS_KERNEL)
/*******************************************************************************
 *
 * gfxFbCaptureTestStart - start of the program in kernel mode
 *
 * This is the kernel mode entry point for the program.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gfxFbCaptureTestRtp
 *
 */
void gfxFbCaptureTestStart
    (
    unsigned int device,
    unsigned int skipWrite
    )
    {
    (void)taskSpawn ("tCapture", 100, VX_FP_TASK, 0x10000,
                     (FUNCPTR) gfxFbCaptureTest,
                     device, skipWrite, 0, 0, 0, 0, 0, 0, 0, 0 );
    }

/*******************************************************************************
 *
 * gfxFbCaptureTestRtp - start of the program for invoking RTP
 *
 * This is the entry point for invoking RTP.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gfxFbCaptureTestStart
 *
 */
static void gfxFbCaptureTestRtp0
    (
    const char *rtpStr,
    unsigned int device,
    unsigned int skipWrite
    )
    {
    int fd;
    const char *args[4];
    char deviceStr[32];
    char skipWriteStr[32];

    (void)snprintf (deviceStr, 32, "%d", device);
    (void)snprintf (skipWriteStr, 32, "%d", skipWrite);

    args[0] = rtpStr;
    args[1] = deviceStr;
    args[2] = skipWriteStr;
    args[3] = NULL;

    if ((fd = open (args[0], O_RDONLY, 0)) == -1)
        {
        (void)fprintf (stderr, "Open RTP file %s errno:%08x\n", args[0], errno);
        return;
        }
    (void)close (fd);

    (void)rtpSpawn (args[0], args, NULL, 220, 0x10000, RTP_LOADED_WAIT, VX_FP_TASK);
    }

void gfxFbCaptureTestRtp
    (
    unsigned int device,
    unsigned int skipWrite
    )
    {
    gfxFbCaptureTestRtp0 ("gfxFbCaptureTest.vxe", device, skipWrite);
    }
#else
/*******************************************************************************
 *
 * main - start of the program in RTP mode
 *
 * This is the RTP mode entry point.
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
    unsigned int skipWrite = 0;

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
            skipWrite = i;
            }
        }

    return (gfxFbCaptureTest (device, skipWrite));
    }
#endif
