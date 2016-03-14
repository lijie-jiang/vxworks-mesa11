/* 40gfxFbItlgc.cdf - Intel Graphics Controller frame buffer driver */

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
24jan14,mgc  Modified for VxWorks 7 release
17may12,rfm  Written
*/

Component INCLUDE_FBDEV_ITLGC_0 {
    NAME            Intel Graphics Controller frame buffer 0
    SYNOPSIS        Intel Graphics Controller frame buffer 0
    _CHILDREN       INCLUDE_FBDEV
    _DEFAULTS       INCLUDE_FBDEV
    ARCHIVE         libgfxItlgcFb.a
    CONFIGLETTES    gfxItlgcInit.c
    PROTOTYPE       void gfxItlgcInit0(void);
    INIT_RTN        gfxItlgcInit0();
    _INIT_ORDER     usrRoot
    INIT_AFTER      usrToolsInit
    INIT_BEFORE     INCLUDE_USER_APPL
    CFG_PARAMS      ITLGC_FBDEV_RESOLUTION_0 \
                    ITLGC_FBDEV_BUFFERS_0 \
                    ITLGC_FBDEV_VSYNC_0
    REQUIRES        itl_x86atom \
                    INCLUDE_FBDEV_MEMORY \
                    INCLUDE_VXBUS \
                    INCLUDE_IO_SYSTEM
}

Parameter ITLGC_FBDEV_RESOLUTION_0 {
    NAME            Screen resolution
    SYNOPSIS        Screen resolution of "640x480-32" or "800x600-32"
    TYPE            string
    DEFAULT         "800x600-32"
}

Parameter ITLGC_FBDEV_BUFFERS_0 {
    NAME            Number of buffers
    SYNOPSIS        Number of buffers
    TYPE            uint
    DEFAULT         3
}

Parameter ITLGC_FBDEV_VSYNC_0 {
    NAME            Vertical synchronization
    SYNOPSIS        Vertical synchronization
    TYPE            bool
    DEFAULT         FALSE
}
