/* gfxFbIoctlTest.c - Frame buffer ioctl test */

/*
 * Copyright (c) 2013-2014, 2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24feb16,yat  Fix static analysis defects (US75033)
08dec14,yat  Add rtpSpawn to invoke in RTP mode (US46449)
01oct14,yat  Resolve defects from static analysis run
24jan14,mgc  Modified for VxWorks 7 release
*/

/*

DESCRIPTION

This program tests the frame buffer ioctls.


The program is started by executing the application's entry point,
gfxFbIoctlTestStart; as follows:

-> gfxFbIoctlTestStart(0)

*/

/* includes */

#include "gfxFbIoctlTest.inl"

#if defined(_WRS_KERNEL)
/*******************************************************************************
 *
 * gfxFbIoctlTestStart - start of the program in kernel mode
 *
 * This is the kernel mode entry point for the program.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gfxFbIoctlTestRtp
 *
 */
void gfxFbIoctlTestStart
    (
    unsigned int device
    )
    {
    (void)taskSpawn ("tIoctl", 100, VX_FP_TASK, 0x10000,
                     (FUNCPTR) gfxFbIoctlTest,
                     device, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    }

/*******************************************************************************
 *
 * gfxFbIoctlTestRtp - start of the program for invoking RTP
 *
 * This is the entry point for invoking RTP.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: gfxFbIoctlTestStart
 *
 */
static void gfxFbIoctlTestRtp0
    (
    const char *rtpStr,
    unsigned int device
    )
    {
    int fd;
    const char *args[3];
    char deviceStr[32];

    (void)snprintf (deviceStr, 32, "%d", device);

    args[0] = rtpStr;
    args[1] = deviceStr;
    args[2] = NULL;

    if ((fd = open (args[0], O_RDONLY, 0)) == -1)
        {
        (void)fprintf (stderr, "Open RTP file %s errno:%08x\n", args[0], errno);
        return;
        }
    (void)close (fd);

    (void)rtpSpawn (args[0], args, NULL, 220, 0x10000, RTP_LOADED_WAIT, VX_FP_TASK);
    }

void gfxFbIoctlTestRtp
    (
    unsigned int device
    )
    {
    gfxFbIoctlTestRtp0 ("gfxFbIoctlTest.vxe", device);
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

    if (argc > 1)
        {
        i = (unsigned int) strtoul(argv[1], &endptr, 0);
        if (endptr != argv[1])
            {
            device = i;
            }
        }

    return (gfxFbIoctlTest (device));
    }
#endif
