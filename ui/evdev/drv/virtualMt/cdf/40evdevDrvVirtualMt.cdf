/* 40evdevDrvVirtualMt.cdf - Virtual Multitouch */

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
25jun14,y_f  create  (US41403)
*/

Parameter DRV_TOUCH_SCREEN_VIRTUAL_MT_DIS_WIDTH
    {
    NAME            Width of Display Resolution 
    TYPE            uint
    DEFAULT         800
    }

Parameter DRV_TOUCH_SCREEN_VIRTUAL_MT_DIS_HEIGHT
    {
    NAME            Height of Display Resolution 
    TYPE            uint
    DEFAULT         480
    }

Component   DRV_TOUCH_SCREEN_VIRTUAL_MT
    {
    NAME        Virtual Multitouch Driver
    SYNOPSIS    Virtual Multitouch Driver
    ARCHIVE     libevdevDrvVirtualMt.a
    _CHILDREN   FOLDER_EVDEV_DRV
    PROTOTYPE   STATUS virtualMtInit (UINT32 width, UINT32 height);
    _INIT_ORDER usrRoot
    INIT_BEFORE INCLUDE_USER_APPL
    INIT_AFTER  usrIosExtraInit
    INIT_RTN    virtualMtInit (DRV_TOUCH_SCREEN_VIRTUAL_MT_DIS_WIDTH, DRV_TOUCH_SCREEN_VIRTUAL_MT_DIS_HEIGHT);
    CFG_PARAMS  DRV_TOUCH_SCREEN_VIRTUAL_MT_DIS_WIDTH    \
                DRV_TOUCH_SCREEN_VIRTUAL_MT_DIS_HEIGHT
    REQUIRES    INCLUDE_EVDEV_LIB_TS
    }