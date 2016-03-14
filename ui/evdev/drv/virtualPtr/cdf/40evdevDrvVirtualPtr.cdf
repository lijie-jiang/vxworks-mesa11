/* 40evdevDrvVirtualPtr.cdf - VxWorks Virtual Pointer Driver */

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
16may14,yat  Add virtual pointer driver. (US24741)
*/

Component   DRV_POINTER_VIRTUAL
    {
    NAME        VxWorks Virtual Pointer Driver
    SYNOPSIS    VxWorks Virtual Pointer Driver
    _CHILDREN   FOLDER_EVDEV_DRV
    ARCHIVE     libevdevDrvVirtualPtr.a
    PROTOTYPE   void virtualPtrInit(void);
    INIT_RTN    virtualPtrInit();
    _INIT_ORDER usrRoot
    INIT_AFTER  usrIosCoreInit
    INIT_BEFORE INCLUDE_USER_APPL
    REQUIRES    INCLUDE_EVDEV_LIB_PTR
    }
