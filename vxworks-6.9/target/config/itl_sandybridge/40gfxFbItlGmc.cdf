/* 40gfxFbItlGmc.cdf - Intel Graphics and Memory Controller frame buffer driver */

/*
 * Copyright (c) 2014-2015, Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
02dec15,yat  Add support for INCLUDE_FBDEV_INIT (US71171)
29jul15,rpc  Add entries for LVDS and DVI (US50700,US50615)
24jul15,rpc  Add DP modes for kontron target (US50700)
25feb15,tlu  Enable LVDS/VGA displays (US50700, US50616)
11mar15,yat  Add show and FBDEV task support (US55410)
22jan15,qsn  Set default VSYNC to FALSE until VSYNC is implemented (US50612)
06jan15,qsn  Modified for itl_64_vx7 bsp (US50612)
20dec14,qsn  Initial VxWorks 7 release (US48907)
*/

Parameter FBDEV_MEMORY_SIZE {
    DEFAULT         (1920*1080*4*3)
}

Component INCLUDE_FBDEV_ITLGMC_0 {
    NAME            Intel Graphics and Memory Controller frame buffer 0
    SYNOPSIS        Intel Graphics and Memory Controller frame buffer 0
    _CHILDREN       INCLUDE_FBDEV
    _DEFAULTS       INCLUDE_FBDEV
    ARCHIVE         libgfxItlGmcFb.a
    CONFIGLETTES    gfxItlGmcInit.c
    CFG_PARAMS      ITLGMC_FBDEV_DISPLAY_0 \
                    ITLGMC_FBDEV_RESOLUTION_0 \
                    ITLGMC_FBDEV_BUFFERS_0 \
                    ITLGMC_FBDEV_VSYNC_0

    REQUIRES        INCLUDE_DRMDEV \
                    INCLUDE_FBDEV_TASK \
                    INCLUDE_FBDEV_MEMORY \
                    INCLUDE_FBDEV_PAGE_FLIP \
                    INCLUDE_IO_SYSTEM
}

Component INCLUDE_FBDEV_ITLGMC_1 {
    NAME            Intel Graphics and Memory Controller frame buffer 1
    SYNOPSIS        Intel Graphics and Memory Controller frame buffer 1
    _CHILDREN       INCLUDE_FBDEV
    ARCHIVE         libgfxItlGmcFb.a
    CONFIGLETTES    gfxItlGmcInit.c
    CFG_PARAMS      ITLGMC_FBDEV_DISPLAY_1 \
                    ITLGMC_FBDEV_RESOLUTION_1 \
                    ITLGMC_FBDEV_BUFFERS_1 \
                    ITLGMC_FBDEV_VSYNC_1
    REQUIRES        INCLUDE_DRMDEV \
                    INCLUDE_FBDEV_TASK \
                    INCLUDE_FBDEV_MEMORY \
                    INCLUDE_FBDEV_PAGE_FLIP \
                    INCLUDE_IO_SYSTEM

}

Component INCLUDE_FBDEV_ITLGMC_2 {
    NAME            Intel Graphics and Memory Controller frame buffer 2
    SYNOPSIS        Intel Graphics and Memory Controller frame buffer 2
    _CHILDREN       INCLUDE_FBDEV
    ARCHIVE         libgfxItlGmcFb.a
    CONFIGLETTES    gfxItlGmcInit.c
    CFG_PARAMS      ITLGMC_FBDEV_DISPLAY_2 \
                    ITLGMC_FBDEV_RESOLUTION_2 \
                    ITLGMC_FBDEV_BUFFERS_2 \
                    ITLGMC_FBDEV_VSYNC_2
    REQUIRES        INCLUDE_DRMDEV \
                    INCLUDE_FBDEV_TASK \
                    INCLUDE_FBDEV_MEMORY \
                    INCLUDE_FBDEV_PAGE_FLIP \
                    INCLUDE_IO_SYSTEM

}

Component INCLUDE_FBDEV_ITLGMC_3 {
    NAME            Intel Graphics and Memory Controller frame buffer 3
    SYNOPSIS        Intel Graphics and Memory Controller frame buffer 3
    _CHILDREN       INCLUDE_FBDEV
    ARCHIVE         libgfxItlGmcFb.a
    CONFIGLETTES    gfxItlGmcInit.c
    CFG_PARAMS      ITLGMC_FBDEV_DISPLAY_3 \
                    ITLGMC_FBDEV_RESOLUTION_3 \
                    ITLGMC_FBDEV_BUFFERS_3 \
                    ITLGMC_FBDEV_VSYNC_3
    REQUIRES        INCLUDE_DRMDEV \
                    INCLUDE_FBDEV_TASK \
                    INCLUDE_FBDEV_MEMORY \
                    INCLUDE_FBDEV_PAGE_FLIP \
                    INCLUDE_IO_SYSTEM

}

Component INCLUDE_FBDEV_ITLGMC_4 {
    NAME            Intel Graphics and Memory Controller frame buffer 4
    SYNOPSIS        Intel Graphics and Memory Controller frame buffer 4
    _CHILDREN       INCLUDE_FBDEV
    ARCHIVE         libgfxItlGmcFb.a
    CONFIGLETTES    gfxItlGmcInit.c
    CFG_PARAMS      ITLGMC_FBDEV_DISPLAY_4 \
                    ITLGMC_FBDEV_RESOLUTION_4 \
                    ITLGMC_FBDEV_BUFFERS_4 \
                    ITLGMC_FBDEV_VSYNC_4
    REQUIRES        INCLUDE_DRMDEV \
                    INCLUDE_FBDEV_TASK \
                    INCLUDE_FBDEV_MEMORY \
                    INCLUDE_FBDEV_PAGE_FLIP \
                    INCLUDE_IO_SYSTEM

}

Component INCLUDE_FBDEV_INIT {
    NAME            Frame buffer initialization
    SYNOPSIS        Frame buffer initialization
    _CHILDREN       FOLDER_FBDEV
    _DEFAULTS       FOLDER_FBDEV
    PROTOTYPE       void gfxItlGmcInit0(void);
    INIT_RTN        gfxItlGmcInit0();
    _INIT_ORDER     usrRoot
    INIT_AFTER      INCLUDE_DRM
    INIT_BEFORE     INCLUDE_USER_APPL
    REQUIRES        INCLUDE_FBDEV
}

Parameter ITLGMC_FBDEV_DISPLAY_0 {
    NAME            Display device
    SYNOPSIS        "VGA" for VGA, "DP" for DisplayPort, "HDMI" for HDMI, "DVI" for DVI, "LVDS panel" for LVDS panel
    TYPE            string
    DEFAULT         "VGA"
}

Parameter ITLGMC_FBDEV_DISPLAY_1 {
    NAME            Display device
    SYNOPSIS        "VGA" for VGA, "DP" for DisplayPort, "HDMI" for HDMI, "DVI" for DVI, "LVDS panel" for LVDS panel
    TYPE            string
    DEFAULT         "DP"
}

Parameter ITLGMC_FBDEV_DISPLAY_2 {
    NAME            Display device
    SYNOPSIS        "VGA" for VGA, "DP" for DisplayPort, "HDMI" for HDMI, "DVI" for DVI, "LVDS panel" for LVDS panel
    TYPE            string
    DEFAULT         "HDMI"
}

Parameter ITLGMC_FBDEV_DISPLAY_3 {
    NAME            Display device
    SYNOPSIS        "VGA" for VGA, "DP" for DisplayPort, "HDMI" for HDMI, "DVI" for DVI, "LVDS panel" for LVDS panel
    TYPE            string
    DEFAULT         "DVI"
}

Parameter ITLGMC_FBDEV_DISPLAY_4 {
    NAME            Display device
    SYNOPSIS        "VGA" for VGA, "DP" for DisplayPort, "HDMI" for HDMI, "DVI" for DVI, "LVDS panel" for LVDS panel
    TYPE            string
    DEFAULT         "LVDS panel"
}

Parameter ITLGMC_FBDEV_RESOLUTION_0 {
    NAME            Screen resolution
    SYNOPSIS        Screen resolution of "640x480-32", "1024x768-32", "1280x1024-32" or "1920x1080-32" for VGA, DP, HDMI, DVI, "640x480-32" or "1024x768-32" for LVDS panel
    TYPE            string
    DEFAULT         "1920x1080-32"
}

Parameter ITLGMC_FBDEV_RESOLUTION_1 {
    NAME            Screen resolution
    SYNOPSIS        Screen resolution of "640x480-32", "1024x768-32", "1280x1024-32" or "1920x1080-32" for VGA, DP, HDMI, DVI, "640x480-32" or "1024x768-32" for LVDS panel
    TYPE            string
    DEFAULT         "1920x1080-32"
}

Parameter ITLGMC_FBDEV_RESOLUTION_2 {
    NAME            Screen resolution
    SYNOPSIS        Screen resolution of "640x480-32", "1024x768-32", "1280x1024-32" or "1920x1080-32" for VGA, DP, HDMI, DVI, "640x480-32" or "1024x768-32" for LVDS panel
    TYPE            string
    DEFAULT         "1920x1080-32"
}

Parameter ITLGMC_FBDEV_RESOLUTION_3 {
    NAME            Screen resolution
    SYNOPSIS        Screen resolution of "640x480-32", "1024x768-32", "1280x1024-32" or "1920x1080-32" for VGA, DP, HDMI, DVI, "640x480-32" or "1024x768-32" for LVDS panel
    TYPE            string
    DEFAULT         "1920x1080-32"
}

Parameter ITLGMC_FBDEV_RESOLUTION_4 {
    NAME            Screen resolution
    SYNOPSIS        Screen resolution of "640x480-32", "1024x768-32", "1280x1024-32" or "1920x1080-32" for VGA, DP, HDMI, DVI, "640x480-32" or "1024x768-32" for LVDS panel
    TYPE            string
    DEFAULT         "1024x768-32"
}

Parameter ITLGMC_FBDEV_BUFFERS_0 {
    NAME            Number of buffers
    SYNOPSIS        Number of buffers
    TYPE            uint
    DEFAULT         3
}

Parameter ITLGMC_FBDEV_BUFFERS_1 {
    NAME            Number of buffers
    SYNOPSIS        Number of buffers
    TYPE            uint
    DEFAULT         3
}

Parameter ITLGMC_FBDEV_BUFFERS_2 {
    NAME            Number of buffers
    SYNOPSIS        Number of buffers
    TYPE            uint
    DEFAULT         3
}

Parameter ITLGMC_FBDEV_BUFFERS_3 {
    NAME            Number of buffers
    SYNOPSIS        Number of buffers
    TYPE            uint
    DEFAULT         3
}

Parameter ITLGMC_FBDEV_BUFFERS_4 {
    NAME            Number of buffers
    SYNOPSIS        Number of buffers
    TYPE            uint
    DEFAULT         3
}

Parameter ITLGMC_FBDEV_VSYNC_0 {
    NAME            Vertical synchronization
    SYNOPSIS        Vertical synchronization
    TYPE            bool
    DEFAULT         TRUE
}

Parameter ITLGMC_FBDEV_VSYNC_1 {
    NAME            Vertical synchronization
    SYNOPSIS        Vertical synchronization
    TYPE            bool
    DEFAULT         TRUE
}

Parameter ITLGMC_FBDEV_VSYNC_2 {
    NAME            Vertical synchronization
    SYNOPSIS        Vertical synchronization
    TYPE            bool
    DEFAULT         TRUE
}

Parameter ITLGMC_FBDEV_VSYNC_3 {
    NAME            Vertical synchronization
    SYNOPSIS        Vertical synchronization
    TYPE            bool
    DEFAULT         TRUE
}

Parameter ITLGMC_FBDEV_VSYNC_4 {
    NAME            Vertical synchronization
    SYNOPSIS        Vertical synchronization
    TYPE            bool
    DEFAULT         TRUE
}

Component INCLUDE_FBDEV_PAGE_FLIP {
    NAME            Frame buffer swapping page flip
    SYNOPSIS        Use drmModePageFlip() or drmModeSetCrtc() for frame buffer swapping
    _CHILDREN       FOLDER_FBDEV
    CFG_PARAMS      FBDEV_PAGE_FLIP_TIMEOUT
}

Parameter FBDEV_PAGE_FLIP_TIMEOUT {
    NAME            Frame buffer swapping page flip timeout
    SYNOPSIS        Frame buffer swapping page flip timeout in milliseconds with 0 milliseconds to use drmModeSetCrtc() or greater than 0 milliseconds to use drmModePageFlip() for frame buffer swapping
    TYPE            uint
    DEFAULT         20
}

Component INCLUDE_FBDEV_TASK {
    NAME            Frame buffer driver task
    SYNOPSIS        Frame buffer driver task
    _CHILDREN       FOLDER_FBDEV
    CFG_PARAMS      FBDEV_TASK_PRIORITY \
                    FBDEV_STACK_SIZE
}

Parameter FBDEV_TASK_PRIORITY {
    NAME            Task priority
    SYNOPSIS        Task priority configuration
    TYPE            uint
    DEFAULT         99
}

Parameter FBDEV_STACK_SIZE {
    NAME            Task stack size
    SYNOPSIS        Task stack size configuration
    TYPE            uint
    DEFAULT         0x10000
}


Component INCLUDE_FBDEV_SHOW {
    NAME            Frame buffer driver show
    SYNOPSIS        This component includes the Frame buffer show that can be started from the VxWorks shell by executing "gfxItlGmcShow"
    _CHILDREN       FOLDER_FBDEV
    ARCHIVE         libgfxItlGmcFbShow.a
    LINK_SYMS       gfxItlGmcShow
    REQUIRES        INCLUDE_FBDEV
}
