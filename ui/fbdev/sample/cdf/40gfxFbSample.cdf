/* 40gfxFbSample.cdf - Sample frame buffer driver */

/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
01apr15,yat  Change buffers to 2 (US48907)
24jan14,mgc  Modified for VxWorks 7 release
*/

Component INCLUDE_FBDEV_SAMPLE_0 {
    NAME            Sample frame buffer 0
    SYNOPSIS        Sample frame buffer 0
    _CHILDREN       INCLUDE_FBDEV
    ARCHIVE         libgfxSampleFb.a
    CONFIGLETTES    gfxSampleInit.c
    PROTOTYPE       void gfxSampleInit0(void);
    INIT_RTN        gfxSampleInit0();
    _INIT_ORDER     usrRoot
    INIT_AFTER      usrToolsInit
    INIT_BEFORE     INCLUDE_USER_APPL
    CFG_PARAMS      SAMPLE_FBDEV_DISPLAY_0 \
                    SAMPLE_FBDEV_RESOLUTION_0 \
                    SAMPLE_FBDEV_BUFFERS_0 \
                    SAMPLE_FBDEV_VSYNC_0
    REQUIRES        INCLUDE_FBDEV_MEMORY \
                    INCLUDE_IO_SYSTEM
}

Component INCLUDE_FBDEV_SAMPLE_1 {
    NAME            Sample frame buffer 1
    SYNOPSIS        Sample frame buffer 1
    _CHILDREN       INCLUDE_FBDEV
    ARCHIVE         libgfxSampleFb.a
    CONFIGLETTES    gfxSampleInit.c
    PROTOTYPE       void gfxSampleInit1(void);
    INIT_RTN        gfxSampleInit1();
    _INIT_ORDER     usrRoot
    INIT_AFTER      usrToolsInit
    INIT_BEFORE     INCLUDE_USER_APPL
    CFG_PARAMS      SAMPLE_FBDEV_DISPLAY_1 \
                    SAMPLE_FBDEV_RESOLUTION_1 \
                    SAMPLE_FBDEV_BUFFERS_1 \
                    SAMPLE_FBDEV_VSYNC_1
    REQUIRES        INCLUDE_FBDEV_MEMORY \
                    INCLUDE_IO_SYSTEM
}

Component INCLUDE_FBDEV_SAMPLE_2 {
    NAME            Sample frame buffer 2
    SYNOPSIS        Sample frame buffer 2
    _CHILDREN       INCLUDE_FBDEV
    ARCHIVE         libgfxSampleFb.a
    CONFIGLETTES    gfxSampleInit.c
    PROTOTYPE       void gfxSampleInit2(void);
    INIT_RTN        gfxSampleInit2();
    _INIT_ORDER     usrRoot
    INIT_AFTER      usrToolsInit
    INIT_BEFORE     INCLUDE_USER_APPL
    CFG_PARAMS      SAMPLE_FBDEV_DISPLAY_2 \
                    SAMPLE_FBDEV_RESOLUTION_2 \
                    SAMPLE_FBDEV_BUFFERS_2 \
                    SAMPLE_FBDEV_VSYNC_2
    REQUIRES        INCLUDE_FBDEV_MEMORY \
                    INCLUDE_IO_SYSTEM
}

Parameter SAMPLE_FBDEV_DISPLAY_0 {
    NAME            Display device
    SYNOPSIS        "HDMI" for HDMI, "LVDS panel" for LVDS panel, "Flat panel" for flat panel
    TYPE            string
    DEFAULT         "HDMI"
}

Parameter SAMPLE_FBDEV_DISPLAY_1 {
    NAME            Display device
    SYNOPSIS        "HDMI" for HDMI, "LVDS panel" for LVDS panel, "Flat panel" for flat panel
    TYPE            string
    DEFAULT         "LVDS panel"
}

Parameter SAMPLE_FBDEV_DISPLAY_2 {
    NAME            Display device
    SYNOPSIS        "HDMI" for HDMI, "LVDS panel" for LVDS panel, "Flat panel" for flat panel
    TYPE            string
    DEFAULT         "Flat panel"
}

Parameter SAMPLE_FBDEV_RESOLUTION_0 {
    NAME            Screen resolution
    SYNOPSIS        Screen resolution of "1920x1080-32", "1280x1024-32" for HDMI, "1280x768-32" for LVDS panel or "800x480-32" for flat panel
    TYPE            string
    DEFAULT         "1920x1080-32"
}

Parameter SAMPLE_FBDEV_RESOLUTION_1 {
    NAME            Screen resolution
    SYNOPSIS        Screen resolution of "1920x1080-32", "1280x1024-32" for HDMI, "1280x768-32" for LVDS panel or "800x480-32" for flat panel
    TYPE            string
    DEFAULT         "1024x768-32"
}

Parameter SAMPLE_FBDEV_RESOLUTION_2 {
    NAME            Screen resolution
    SYNOPSIS        Screen resolution of "1920x1080-32", "1280x1024-32" for HDMI, "1280x768-32" for LVDS panel or "800x480-32" for flat panel
    TYPE            string
    DEFAULT         "800x480-32"
}

Parameter SAMPLE_FBDEV_BUFFERS_0 {
    NAME            Number of buffers
    SYNOPSIS        Number of buffers
    TYPE            uint
    DEFAULT         2
}

Parameter SAMPLE_FBDEV_BUFFERS_1 {
    NAME            Number of buffers
    SYNOPSIS        Number of buffers
    TYPE            uint
    DEFAULT         2
}

Parameter SAMPLE_FBDEV_BUFFERS_2 {
    NAME            Number of buffers
    SYNOPSIS        Number of buffers
    TYPE            uint
    DEFAULT         2
}

Parameter SAMPLE_FBDEV_VSYNC_0 {
    NAME            Vertical synchronization
    SYNOPSIS        Vertical synchronization
    TYPE            bool
    DEFAULT         FALSE
}

Parameter SAMPLE_FBDEV_VSYNC_1 {
    NAME            Vertical synchronization
    SYNOPSIS        Vertical synchronization
    TYPE            bool
    DEFAULT         FALSE
}

Parameter SAMPLE_FBDEV_VSYNC_2 {
    NAME            Vertical synchronization
    SYNOPSIS        Vertical synchronization
    TYPE            bool
    DEFAULT         FALSE
}
