/* 40gfxFbConsole.cdf - Frame buffer console CDF file */

/*
 * Copyright (c) 2012, 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
24jan14,mgc  Modified for VxWorks 7 release
11may12,rfm  Created.
*/

Folder FOLDER_FBDEV_CONSOLE {
    NAME            Frame buffer console components
    SYNOPSIS        Frame buffer console
    DEFAULTS        INCLUDE_FBDEV_CONSOLE
    _CHILDREN       FOLDER_FBDEV
}

Component INCLUDE_FBDEV_CONSOLE  {
    NAME            Frame buffer console
    SYNOPSIS        Frame buffer console
    HDR_FILES       gfxConsoleInit.h
    CONFIGLETTES    gfxConsoleInit.c
    _CHILDREN       FOLDER_FBDEV_CONSOLE
    REQUIRES        INCLUDE_FBDEV SELECT_FB_CONSOLE_FONT
    CFG_PARAMS      FB_CONSOLE_REDIRECT \
                    FB_CONSOLE_BGCOLOR  \
                    FB_CONSOLE_FGCOLOR  \
                    FB_CONSOLE_TAB      \
                    FB_CONSOLE_WRITE
}

Parameter FB_CONSOLE_REDIRECT {
    NAME            Standard output redirection
    TYPE            bool
    DEFAULT         TRUE
}

Parameter FB_CONSOLE_BGCOLOR {
    NAME            Background color
    TYPE            uint
    DEFAULT         0x1d3753
}

Parameter FB_CONSOLE_FGCOLOR {
    NAME            Foreground color
    TYPE            uint
    DEFAULT         0xffffff
}

Parameter FB_CONSOLE_TAB {
    NAME            Tab size
    TYPE            uint
    DEFAULT         4
}

Parameter FB_CONSOLE_WRITE {
    NAME            Write function
    TYPE            funcptr
    DEFAULT         FB_DEFAULT_CONSOLE_WRITE
}

Selection SELECT_FB_CONSOLE_FONT {
    NAME            Font
    _CHILDREN       FOLDER_FBDEV_CONSOLE
    COUNT           1-1 
    CHILDREN        INCLUDE_FB_CONSOLE_CP437_L  \
                    INCLUDE_FB_CONSOLE_CP437_S
    DEFAULTS        INCLUDE_FB_CONSOLE_CP437_L
    REQUIRES        INCLUDE_FBDEV_CONSOLE
}

Component INCLUDE_FB_CONSOLE_CP437_L {
    NAME            CP437 Large - 8 x 16
}

Component INCLUDE_FB_CONSOLE_CP437_S {
    NAME            CP437 Small - 8 x 8
}
