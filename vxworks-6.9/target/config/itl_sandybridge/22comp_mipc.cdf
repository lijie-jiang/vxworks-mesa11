/* 22comp_mipc.cdf - configuration for MCB/MSD/MIPC */

/*
 * Copyright (c) 2011 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01b,05jul11,zhw  Removed unnecessary MSD_CFG_STR parameter (WIND00279265)
01a,11apr11,j_z  initial creation based on itl_nehalem version 01b
*/

Folder FOLDER_MIPC {
	_CHILDREN	FOLDER_MULTIOS
}

Component INCLUDE_MIPC_UNSUPPORTED {
	_CHILDREN	FOLDER_NOT_VISIBLE
}

/*
 * MIPC_SM_SYSTEM_POOL_BASE, MIPC_SM_SYSTEM_POOL_SIZE, and MIPC_SM_NODE_IRQ 
 * are only used in unsupervised systems and these parameters are ignored in 
 * a supervised system
 */

Parameter MIPC_SM_SYSTEM_POOL_BASE {
    SYNOPSIS    The base address on the itl_AMP is at 0x7FC00000. The \
shared memory extends up to 0x80000000. MIPC, SM, and DSHM must share that \
space if included. Care must be taken so that it is not overflown, since this \
will corrupt kernel data and/or text.
	DEFAULT     0x7FC00000
}

Parameter MIPC_SM_SYSTEM_POOL_SIZE {
	DEFAULT		0x400000
}

Parameter MSD_NUM_DEVS {
        DEFAULT (INCLUDE_AMP_CPU_00)::(7) \
                (1)
}

Parameter MIPC_SM_NODE_IRQ {
	DEFAULT		3
}

Parameter MIPC_SM_NODES {
    NAME        Maximum nodes per bus
    SYNOPSIS    Specifies the maximum number of nodes a bus can support, unless overridden by the bus configuration string.
    TYPE        UINT
    DEFAULT     8
}

/* 
 * Example configuration of MND; 
 */
Parameter MND_CFG_STR {
	DEFAULT	(INCLUDE_AMP_CPU_00)::("#unit=0 segment=0 port=23 bus=main") \
		(INCLUDE_AMP_CPU_01)::("#unit=1 segment=0 port=23 bus=main") \
		(INCLUDE_AMP_CPU_02)::("#unit=2 segment=0 port=23 bus=main") \
		(INCLUDE_AMP_CPU_03)::("#unit=3 segment=0 port=23 bus=main") \
		(INCLUDE_AMP_CPU_04)::("#unit=4 segment=0 port=23 bus=main") \
		(INCLUDE_AMP_CPU_05)::("#unit=5 segment=0 port=23 bus=main") \
		(INCLUDE_AMP_CPU_06)::("#unit=6 segment=0 port=23 bus=main") \
		(INCLUDE_AMP_CPU_07)::("#unit=7 segment=0 port=23 bus=main") \
		"#unit=0 segment=0 port=23 bus=main"
}
