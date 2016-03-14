/* gfxConsoleTest.c - Frame buffer console test */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
01oct14,yat  Fix defects from static analysis run
24jan14,mgc  Modified for VxWorks 7 release
04apr13,mgc  Created
*/

/* includes */

#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <ioLib.h>
#include <fcntl.h>
#include <taskLib.h>
#include <fbdev.h>

extern int gfxFbConsoleWrite (FB_INFO*, const char*, size_t);

/*******************************************************************************
 *
 * gfxFbConsoleTest - perform console tests
 *
 * This routine performs console tests
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
int gfxFbConsoleTest
    (
    unsigned int device
    )
    {
    unsigned int        i;
    int                 fd;
    int                 result = 0;
    int                 maxCharPerLine = 0;
    unsigned short      c = 0;
    FB_IOCTL_ARG        arg;
    FB_INFO             fbInfo;
    char                deviceName[FB_MAX_STR_CHARS];

    (void)fprintf (stdout, "~~~ gfxConsoleTest ~~~\n");

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

    if (ioctl (fd, FB_IOCTL_GET_FB_INFO, &arg) == -1)
        {
        (void)fprintf (stdout, "ERROR: FB_IOCTL_GET_VIDEO_MODE error.\n");
        (void)close (fd);
        return (EXIT_FAILURE);
        }
    (void)fprintf (stdout, "\nFrame buffer dimmensions: width = %d, height = %d\n", 
               arg.getFbInfo.width, arg.getFbInfo.height);
    fbInfo = arg.getFbInfo;

    if (ioctl (fd, FB_IOCTL_CLEAR_SCREEN, &arg) == -1)
        {
        (void)fprintf (stdout, "ERROR: FB_IOCTL_CLEAR_SCREEN error.\n");
        (void)close (fd);
        return (EXIT_FAILURE);
        }

    /* TEST 1 */
    (void)fprintf (stdout, "\nCONSOLE TEST 1: Character rendering.\n");
    (void)fflush (stdout);
    result = 0;
    maxCharPerLine = fbInfo.width / 8;
    for (c = 0; c < 256; c++)
        {
        if ((c%maxCharPerLine == 0)&&(c > 0))
            {
            if (gfxFbConsoleWrite(&fbInfo, "\n", 1) != 1)
                {
                result = -1;
                break;
                }
            }
        if ((c == '\n')||(c == '\b')||(c == '\r')||(c == '\t'))
            {
            if (gfxFbConsoleWrite(&fbInfo, " ", 1) != 1)
                {
                result = -1;
                break;
                }
            continue;
            }
        if (gfxFbConsoleWrite(&fbInfo, (char *)&c, 1) !=  1)
            {
            result = -1;
            break;
            }
        }
    if (result == 0)
        {
        (void)fprintf (stdout, "\nPASS.\n");
        }
    else
        {
        (void)fprintf (stdout, "\nFAIL.\n");
        }

    /* TEST 2 */
    (void)fprintf (stdout, 
               "\nCONSOLE TEST 2: Console frame buffer bound checking.\n");
    (void)fflush (stdout);
    result = 0;
    maxCharPerLine = fbInfo.width / 8;
    for (c = 0; c < maxCharPerLine*100; c++)
        {
        if (gfxFbConsoleWrite(&fbInfo, "X", 1) !=  1)
            {
            result = -1;
            break;
            }
        }
    if (result == 0)
        {
        (void)fprintf (stdout, "\nPASS.\n");
        }
    else
        {
        (void)fprintf (stdout, "\nFAIL.\n");
        }

    /* TEST 3 */
    (void)fprintf (stdout, 
            "\nCONSOLE TEST 3: Console frame buffer reverse bound checking.\n");
    (void)fflush (stdout);
    result = 0;
    maxCharPerLine = fbInfo.width / 8;
    for (c = 0; c < maxCharPerLine*100; c++)
        {
        if (gfxFbConsoleWrite(&fbInfo, "X", 1) !=  1)
            {
            result = -1;
            break;
            }
        }
    for (c = 0; c < maxCharPerLine*100; c++)
        {
        if (gfxFbConsoleWrite(&fbInfo, "\b \b", 3) !=  3)
            {
            result = -1;
            break;
            }
        }
    if (result == 0)
        {
        (void)fprintf (stdout, "\nPASS.\n");
        }
    else
        {
        (void)fprintf (stdout, "\nFAIL.\n");
        }
    (void)fflush (stdout);

    /* TEST 5 */
    (void)fprintf (stdout, 
           "\nCONSOLE TEST 4: Console tab test.\n");
    (void)fflush (stdout);
    result = 0;
    if (gfxFbConsoleWrite(&fbInfo, "XXXX\tXXXX\n\tXXXX\t", 16) !=  16)
        {
        result = -1;
        }
    if (result == 0)
        {
        (void)fprintf (stdout, "\nPASS.\n");
        }
    else
        {
        (void)fprintf (stdout, "\nFAIL.\n");
        }
    (void)fflush (stdout);
    (void)close (fd);

    return (EXIT_SUCCESS);
    }

/*******************************************************************************
 *
 * gfxFbConsoleTestStart - start of the test program in kernel mode
 *
 * This is the kernel mode entry point for the test.
 *
 * RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
void gfxFbConsoleTestStart
    (
    unsigned int device
    )
    {
    (void)taskSpawn ("tConsole", 100, VX_FP_TASK, 0x10000, (FUNCPTR) gfxFbConsoleTest,
                     device, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    }
