/* gfxSplashInit.h - Frame buffer driver definitions */

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

#ifndef __INC_gfxSplashInit_h
#define __INC_gfxSplashInit_h

/* includes */

#include <vxWorks.h>
#include <fbdev.h>

/* defines */

#define FB_DEFAULT_SPLASH_BLIT          gfxFbSplashBlit

/* forward declarations */

IMPORT STATUS gfxFbSplashBlit (void*, int, int, int, int);

#endif /* __INC_gfxSplashInit_h */
