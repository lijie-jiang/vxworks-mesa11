/* 20EvdevTest.cdf - BSPVTS evdev test suite test components  */

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
25nov15,zjl  update fot vxTest automation
19Nov15,jnl  create


DESCRIPTION

This file contains vxTest test component definitions for test evdev

### NOTE ###
The test module components defined in this file are delivered with the
vxWorks product release. Do not add any test module components to this
file unless you intend to deliver the corresponding test modules as
part of the vxWorks product.

*/

Component               INCLUDE_TM_EVDEVTEST {
        NAME            Evdev CoreLib Test Module
        SYNOPSIS        This component adds the Evdev CoreLib Test Module
        REQUIRES        INCLUDE_VXTEST_DRIVER \
                        INCLUDE_EVDEV_LIB_CORE \
                        INCLUDE_EVDEV_LIB_KBD \
                        INCLUDE_EVDEV_LIB_PTR \
                        INCLUDE_EVDEV_LIB_TS
        MODULES         tmEvDevTest.o
        INCLUDE_WHEN    INCLUDE_TM_EVDEV_LIB_TEST
        PROTOTYPE       void tmEvdevTestInit(void);
        INIT_RTN        tmEvdevTestInit();
}



/*
 * Test Init Group
 */
InitGroup		usrVxTestEvdevTestsInit {
    INIT_RTN	    usrVxTestEvdevTestsInit ();
    SYNOPSIS		VxTest evdev tests initialization sequence
    INIT_ORDER		INCLUDE_TM_EVDEVTEST
    _INIT_ORDER 	usrVxTestEvdevInit
}

/*
 * evdev VTS Tests Folder
 */
Folder			FOLDER_VXTEST_EVDEV_LIB {
	NAME		evdev test components
	SYNOPSIS	Used to group evdev test components
	CHILDREN	INCLUDE_TM_EVDEVTEST
	DEFAULTS	INCLUDE_TM_EVDEVTEST
    _CHILDREN	FOLDER_VXTEST_EVDEV_TESTS
}
