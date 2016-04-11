/* 20profiles.cdf - BSP profile adjustments */

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
01a,07mar11,j_z  initial creation based on itl_nehalem version 01b
*/

Profile PROFILE_BOOTAPP {
    COMPONENTS +=        \
                INCLUDE_MPTABLE_BOOT_OP
}

Profile PROFILE_BOOTAPP_BASIC {
    COMPONENTS +=        \
                INCLUDE_MPTABLE_BOOT_OP
}

