/* 40evdevDrvVxSimKbd.cdf - VxWorks Simulator Keyboard Driver */

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
16may14,yat  Add ARCHIVE libevdevDrvVxSimKbd.a. (US11127)
16may14,yat  Change init before INCLUDE_USER_APPL. (US11127)
27aug13,y_f  written
*/

Component   DRV_KEYBOARD_VXSIM
    {
    NAME        VxWorks Simulator Keyboard Driver
    SYNOPSIS    VxWorks Simulator Keyboard Driver
    MODULES     evdevDrvVxSimKbd.o
    _CHILDREN   FOLDER_EVDEV_DRV
    ARCHIVE     libevdevDrvVxSimKbd.a
    PROTOTYPE   void vxSimKbdInit(void);
    INIT_RTN    vxSimKbdInit();
    _INIT_ORDER usrRoot
    INIT_AFTER  INCLUDE_FBDEV_VXSIM_0
    INIT_BEFORE INCLUDE_USER_APPL
    REQUIRES    INCLUDE_EVDEV_LIB_KBD   \
                INCLUDE_FBDEV_VXSIM_0   \
                INCLUDE_EVDEV_LIB_KBD_MAP_US
    }
