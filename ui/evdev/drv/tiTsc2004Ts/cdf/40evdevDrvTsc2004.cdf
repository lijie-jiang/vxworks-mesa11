/* 40evdevDrvTsc2004.cdf - TI TSC2004 touch screen controller */

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
01sep14,y_f  removed INIT_RTN and CFG_PARAMS (V7GFX-208)
20jun14,y_f  updated to support VXBUS GEN2 (US42301)
23jul13,y_f  written
*/

Component   DRV_TOUCH_SCREEN_TI_TSC2004
    {
    NAME        TI TSC2004 Touch Screen Driver
    SYNOPSIS    TI TSC2004 Touch Screen Driver
    MODULES     evdevDrvTiTsc2004.o
    _CHILDREN   FOLDER_EVDEV_DRV
    LINK_SYMS   vxbFdtTiTsc2004TsDrv
    REQUIRES    INCLUDE_VXBUS       \
                INCLUDE_EVDEV_LIB_TS
    }