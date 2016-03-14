/* 40gfxFbSplash.cdf - Frame buffer splash CDF file */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
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

Component INCLUDE_FBDEV_SPLASH {
    NAME            Frame buffer splash screen
    SYNOPSIS        Frame buffer splash screen
    HDR_FILES       gfxSplashInit.h
    CONFIGLETTES    gfxSplashInit.c
    ARCHIVE         libgfxSplash.a
    _CHILDREN       FOLDER_FBDEV
    REQUIRES        INCLUDE_FBDEV
    CFG_PARAMS      FB_SPLASH_WR \
                    FB_SPLASH_BLIT
}

Parameter FB_SPLASH_WR {
    NAME            Wind River splash screen
    SYNOPSIS        Wind River splash screen
    TYPE            bool
    DEFAULT         TRUE
}

Parameter FB_SPLASH_BLIT {
    NAME            Blit function
    TYPE            funcptr
    DEFAULT         FB_DEFAULT_SPLASH_BLIT
}

