/* gfxSplashInit.c - Frame buffer driver initialization */

/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24jan14,mgc  Modified for VxWorks 7 release
*/

#if defined(INCLUDE_FBDEV_SPLASH)

/* includes */

#include <gfxSplash.h>

/* forward declarations */

IMPORT STATUS gfxFbSplashBlit1 (void*, int, int, int, int);

/*******************************************************************************
*
* gfxFbSplashBlit - Stretch-blit the splash screen
*
* RETURNS: N/A
*/
STATUS gfxFbSplashBlit
    (
    void*                   pFb,       /* frame buffer information */
    int                     xres,
    int                     yres,
    int                     bpp,
    int                     stride
    )
    {
    return gfxFbSplashBlit1(pFb, xres, yres, bpp, stride);
    }
#endif
