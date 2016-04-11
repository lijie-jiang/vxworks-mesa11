/* 20cpuPwrMgmt.cdf - CPU Power management component description file */

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
01a,11apr11,j_z  initial creation based on itl_nehalem version 01b
*/

/*
DESCRIPTION

This file contains the CPU power Management framework and power managers
components.
*/


Component INCLUDE_CPU_PERFORMANCE_MGMT {
    NAME            CPU Performance Management 
    SYNOPSIS        CPU Dynamic power management (speed-stepping)
    INIT_RTN        perfMgrInit (SELECT_PRIORITY_PERF, ISR_PSTATE,\
                    DEF_PROFILE_INDEX, PERF_MGR_PROF0, PERF_MGR_PROF1,\
                    PERF_MGR_PROF2, PERF_MGR_PROF3 ); \
                    perfMgrEnable ();
    HDR_FILES       perfMgrLib.h  
    REQUIRES        INCLUDE_CPU_PWR_ARCH \
                    INCLUDE_ERF \
                    SELECT_CPU_CONFIG
    EXCLUDES        INCLUDE_CPU_TURBO \
                    INCLUDE_CPU_LIGHT_PWR_MGR
    CFG_PARAMS      SELECT_PRIORITY_PERF \
                    ISR_PSTATE \
                    DEF_PROFILE_INDEX \
                    PERF_MGR_PROF0 \
                    PERF_MGR_PROF1 \
                    PERF_MGR_PROF2 \
                    PERF_MGR_PROF3 
    _CHILDREN       FOLDER_PWR_MGMT 
}

