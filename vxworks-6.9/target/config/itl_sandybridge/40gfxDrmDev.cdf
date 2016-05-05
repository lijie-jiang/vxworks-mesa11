/* 40drmGpu.cdf - Component configuration file */
                                                                                
/*
 * Copyright (c) 2008-2009 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */
                                                                                
/*
modification history
--------------------

*/

Component	INCLUDE_DRM_PCI_DEV {
	NAME		Intel GPU Driver
	SYNOPSIS	Intel GPU Driver
	_INIT_ORDER	hardWareInterFaceBusInit
	INIT_RTN	drmDevicePciRegister();
	_CHILDREN       FOLDER_DRM
	REQUIRES	INCLUDE_PLB_BUS \
			INCLUDE_PCI_BUS 
	INIT_AFTER	INCLUDE_PCI_BUS
}


