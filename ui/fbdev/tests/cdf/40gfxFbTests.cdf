/* 40gfxFbTests.cdf - Frame buffer tests CDF file */

/*
 * Copyright (c) 2013-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
17feb16,yat  Add frame buffer copy test (US73758)
02dec15,yat  Add support for INCLUDE_FBDEV_INIT (US71171)
22sep15,yat  Add missing REQUIRES INCLUDE_RTP (V7GFX-284)
22dec14,yat  Added frame buffer capture test (US50456)
24jan14,mgc  Modified for VxWorks 7 release
*/

Folder FOLDER_FBDEV_TESTS {
    NAME            Frame buffer tests components
    SYNOPSIS        Frame buffer tests
    DEFAULTS        INCLUDE_FBDEV_MAIN_TEST
    _CHILDREN       FOLDER_FBDEV
}

Component INCLUDE_FBDEV_CONSOLE_TEST {
    NAME            Frame buffer console test
    SYNOPSIS        This component includes the frame buffer console test that can be started from the VxWorks shell by executing "gfxFbConsoleTestStart"
    _CHILDREN       FOLDER_FBDEV_TESTS
    REQUIRES        INCLUDE_FBDEV_CONSOLE
    LINK_SYMS       gfxFbConsoleTestStart
}

Component INCLUDE_FBDEV_IOCTL_TEST {
    NAME            Frame buffer ioctl test 
    SYNOPSIS        This component includes the frame buffer ioctl test that can be started from the VxWorks shell by executing "gfxFbIoctlTestStart"
    _CHILDREN       FOLDER_FBDEV_TESTS
#if defined(_WRS_CONFIG_FBDEV_INIT)
    REQUIRES        INCLUDE_FBDEV_INIT \
                    INCLUDE_RTP \
                    INCLUDE_SHL
#else
    REQUIRES        INCLUDE_FBDEV \
                    INCLUDE_RTP \
                    INCLUDE_SHL
#endif
    LINK_SYMS       gfxFbIoctlTestStart
}

Component INCLUDE_FBDEV_CAPTURE_TEST {
    NAME            Frame buffer capture test
    SYNOPSIS        This component includes the frame buffer capture test that can be started from the VxWorks shell by executing "gfxFbCaptureTestStart"
    _CHILDREN       FOLDER_FBDEV_TESTS
#if defined(_WRS_CONFIG_FBDEV_INIT)
    REQUIRES        INCLUDE_FBDEV_INIT \
                    INCLUDE_RTP \
                    INCLUDE_SHL
#else
    REQUIRES        INCLUDE_FBDEV \
                    INCLUDE_RTP \
                    INCLUDE_SHL
#endif
    LINK_SYMS       gfxFbCaptureTestStart
}

Component INCLUDE_FBDEV_COPY_TEST {
    NAME            Frame buffer copy test
    SYNOPSIS        This component includes the frame buffer copy test that can be started from the VxWorks shell by executing "gfxFbCopyTestStart"
    _CHILDREN       FOLDER_FBDEV_TESTS
#if defined(_WRS_CONFIG_FBDEV_INIT)
    REQUIRES        INCLUDE_FBDEV_INIT
#else
    REQUIRES        INCLUDE_FBDEV
#endif
    LINK_SYMS       gfxFbCopyTestStart
}

Component INCLUDE_FBDEV_MAIN_TEST {
    NAME            Frame buffer main test
    SYNOPSIS        This component includes the frame buffer main test that can be started from the VxWorks shell by executing "gfxFbMainTestStart"
    _CHILDREN       FOLDER_FBDEV_TESTS
    REQUIRES        INCLUDE_FBDEV_IOCTL_TEST \
                    INCLUDE_FBDEV_COPY_TEST
    LINK_SYMS       gfxFbMainTestStart
}
