/* 01vxTest_evdev.cdf - VxTest EVDEV components groups */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
10nov15,zjl   Written.


DESCRIPTION

This file contains the definition of the components for OS vxTest components
groups.Do not add test module components or parameters to this file.

*/

Component               INCLUDE_TM_EVDEV_LIB_TEST {
        NAME            KERNEL EVDEV Test components group
        SYNOPSIS        This component adds all EVDEV components
        REQUIRES        INCLUDE_VXTEST_DRIVER
}

InitGroup       usrVxTestEvdevInit {
    INIT_RTN        usrVxTestEvdevInit ();
    SYNOPSIS        VxTest EVDEV tests initialization sequence
    INIT_ORDER      INCLUDE_TM_EVDEV_LIB_TEST
    _INIT_ORDER     usrVxTestInit
}

/*
 * Tests Folder
 */
Folder          FOLDER_VXTEST_EVDEV {
    NAME        VxTest evdev test components
    SYNOPSIS    Used to group evdev test components
    CHILDREN    INCLUDE_TM_EVDEV_LIB_TEST
    DEFAULTS    INCLUDE_TM_EVDEV_LIB_TEST
    _CHILDREN   FOLDER_VXTEST_EVDEV_TESTS
}

