/* 40evdevLibKbd.cdf - Keyboard Library Component Bundles */

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
12nov13,j_x  abstract kbd attach to shell to evdev kbd lib
19jul13,j_x  create
*/

Folder FOLDER_EVDEV_KBD
    {
    NAME            Keyboard Library
    SYNOPSIS        Keyboard Library
    _CHILDREN       FOLDER_EVDEV_LIB
    }

Parameter EVDEV_KBD_TYPE_MATIC_PERIOD
    {
    NAME            Type Matic Period (ms)
    TYPE            uint
    DEFAULT         200
    }

Component   INCLUDE_EVDEV_LIB_KBD
    {
    NAME            Keyboard Library
    SYNOPSIS        Event Devices Keyboard Library
    MODULES         evdevLibKbd.o
    CONFIGLETTES    evdevLibKbdCfg.c
    _CHILDREN       FOLDER_EVDEV_KBD
    PROTOTYPE       STATUS evdevKbdInit (UINT32 typeMaticRate);
    _INIT_ORDER     usrIosExtraInit
    INIT_AFTER      INCLUDE_EVDEV_LIB_CORE
    INIT_RTN        evdevKbdInit (EVDEV_KBD_TYPE_MATIC_PERIOD);
    CFG_PARAMS      EVDEV_KBD_TYPE_MATIC_PERIOD
    REQUIRES        INCLUDE_EVDEV_LIB_CORE
    }
    
Component INCLUDE_EVDEV_LIB_KBD_SHELL_ATTACH
    {
    NAME            Attach to VxWorks Shell
    SYNOPSIS        Attaches the evdev keyboard to the vxWorks target shell
    _CHILDREN       FOLDER_EVDEV_KBD
    REQUIRES        INCLUDE_EVDEV_LIB_KBD
    }