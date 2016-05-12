/* 41evdevLibKbdMapUS.cdf - US keyboard mapping Component Bundles */

/*
 * Copyright (c) 2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
03sep14,wyy  Init Keyboard and Mouse driver before network driver (V7CON-179)
15oct13,y_f  create
*/

Component   INCLUDE_EVDEV_LIB_KBD_MAP_US
    {
    NAME        US Keyboard Mapping
    SYNOPSIS    US Keyboard Mapping
    MODULES     evdevLibKbdMapUS.o
    _CHILDREN   FOLDER_EVDEV_KBD
    PROTOTYPE   STATUS evdevKbdMapUSInit ();
    _INIT_ORDER usrIosExtraInit
    INIT_AFTER  INCLUDE_EVDEV_LIB_KBD
    INIT_RTN    evdevKbdMapUSInit ();
    INIT_BEFORE INCLUDE_BOOT_LINE_INIT
    REQUIRES    INCLUDE_EVDEV_LIB_KBD
    }
