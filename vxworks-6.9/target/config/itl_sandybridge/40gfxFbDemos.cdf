/* 40gfxFbDemos.cdf - Frame buffer demos CDF file */

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
02dec15,yat  Add support for INCLUDE_FBDEV_INIT (US71171)
22sep15,yat  Add missing REQUIRES INCLUDE_RTP (V7GFX-284)
24jan14,mgc  Modified for VxWorks 7 release
*/

Folder FOLDER_FBDEV_DEMOS {
    NAME            Frame buffer demos components
    SYNOPSIS        Frame buffer demos
    DEFAULTS        INCLUDE_FBDEV_MAIN_DEMO
    _CHILDREN       FOLDER_FBDEV
}

Component INCLUDE_FBDEV_DRAW_DEMO {
    NAME            Frame buffer draw demo
    SYNOPSIS        This component includes the frame buffer draw demo that can be started from the VxWorks shell by executing "gfxFbDrawDemoStart"
    _CHILDREN       FOLDER_FBDEV_DEMOS
#if defined(_WRS_CONFIG_FBDEV_INIT)
    REQUIRES        INCLUDE_FBDEV_INIT \
                    INCLUDE_RTP \
                    INCLUDE_SHL
#else
    REQUIRES        INCLUDE_FBDEV \
                    INCLUDE_RTP \
                    INCLUDE_SHL
#endif
    LINK_SYMS       gfxFbDrawDemoStart
}

Component INCLUDE_FBDEV_MAIN_DEMO {
    NAME            Frame buffer main demo
    SYNOPSIS        This component includes the frame buffer main demo that can be started from the VxWorks shell by executing "gfxFbMainDemoStart"
    _CHILDREN       FOLDER_FBDEV_DEMOS
    REQUIRES        INCLUDE_FBDEV_DRAW_DEMO
    LINK_SYMS       gfxFbMainDemoStart
}
