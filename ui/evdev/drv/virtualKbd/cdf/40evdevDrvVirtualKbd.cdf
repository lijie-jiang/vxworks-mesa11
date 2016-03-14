/* 40evdevDrvVirtualKbd.cdf - VxWorks Virtual Keyboard Driver */

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
16may14,yat  Add virtual keyboard driver. (US24741)
*/

Component   DRV_KEYBOARD_VIRTUAL
    {
    NAME        VxWorks Virtual Keyboard Driver
    SYNOPSIS    VxWorks Virtual Keyboard Driver
    _CHILDREN   FOLDER_EVDEV_DRV
    ARCHIVE     libevdevDrvVirtualKbd.a
    PROTOTYPE   void virtualKbdInit(void);
    INIT_RTN    virtualKbdInit();
    _INIT_ORDER usrRoot
    INIT_AFTER  usrIosCoreInit
    INIT_BEFORE INCLUDE_USER_APPL
    REQUIRES    INCLUDE_EVDEV_LIB_KBD   \
                INCLUDE_EVDEV_LIB_KBD_MAP_US
    }
