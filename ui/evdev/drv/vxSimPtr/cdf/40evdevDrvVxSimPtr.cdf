/* 40evdevDrvVxSimPtr.cdf - VxWorks Simulator Pointer Driver */

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
16may14,yat  Add ARCHIVE libevdevDrvVxSimPtr.a. (US11127)
16may14,yat  Change init before INCLUDE_USER_APPL. (US11127)
26aug13,y_f  written
*/

Component   DRV_POINTER_VXSIM
    {
    NAME        VxWorks Simulator Pointer Driver
    SYNOPSIS    VxWorks Simulator Pointer Driver
    MODULES     evdevDrvVxSimPtr.o
    _CHILDREN   FOLDER_EVDEV_DRV
    ARCHIVE     libevdevDrvVxSimPtr.a
    PROTOTYPE   void vxSimPtrInit(void);
    INIT_RTN    vxSimPtrInit();
    _INIT_ORDER usrRoot
    INIT_AFTER  INCLUDE_FBDEV_VXSIM_0
    INIT_BEFORE INCLUDE_USER_APPL
    REQUIRES    INCLUDE_EVDEV_LIB_PTR   \
                INCLUDE_FBDEV_VXSIM_0
    }
