/* 40evdevDrvFslCRTouch.cdf - Freescale CRTouch screen controller */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
18Mar15,c_l  create. (US55213)
*/

Component   DRV_TOUCH_SCREEN_FSL_CRTOUCH
    {
    NAME        Freescale CRTouch Screen Driver
    SYNOPSIS    Freescale CRTouch Screen Driver
    MODULES     evdevDrvFslCRTouch.o
    _CHILDREN   FOLDER_EVDEV_DRV
    LINK_SYMS   vxbFdtFslCRTouchTsDrv
    REQUIRES    INCLUDE_VXBUS        \
                INCLUDE_EVDEV_LIB_TS \
                INCLUDE_GPIO_SYS     \
                DRV_BUS_FDT_ROOT
    }