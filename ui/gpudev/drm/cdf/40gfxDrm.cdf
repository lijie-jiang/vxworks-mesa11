/* 40gfxDrm.cdf - DRM CDF file */

/*
 * Copyright (c) 2013-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
22jan16,rpc  Add requires for Posix timers (US73564)
14sep15,yat  Add DRM show (US66034)
22jan15,qsn  Created for VxWorks 7 (US48907).
*/

Folder FOLDER_DRM {
    NAME            Direct Rendering Manager components
    SYNOPSIS        Direct Rendering Manager
    _CHILDREN       FOLDER_GPUDEV
}

Component INCLUDE_DRM {
    NAME            Direct Rendering Manager
    SYNOPSIS        Direct Rendering Manager configuration
    _CHILDREN       FOLDER_DRM
    _DEFAULTS       FOLDER_DRM
    CFG_PARAMS      DRM_WORKQUEUE_TASK_PRIORITY \
                    DRM_WORKQUEUE_STACK_SIZE
    ARCHIVE         libgfxDrm.a
    CONFIGLETTES    gfxDrmInit.c
    PROTOTYPE       void gfxDrmInit(void);
    INIT_RTN        gfxDrmInit();
    _INIT_ORDER     usrRoot
    INIT_AFTER      usrToolsInit
    INIT_BEFORE     INCLUDE_USER_APPL
    REQUIRES        INCLUDE_DRMDEV \
                    INCLUDE_POSIX_TIMERS
}

Parameter DRM_WORKQUEUE_TASK_PRIORITY {
    NAME            Workqueue task priority
    SYNOPSIS        Workqueue task priority configuration
    TYPE            uint
    DEFAULT         99
}

Parameter DRM_WORKQUEUE_STACK_SIZE {
    NAME            Workqueue task stack size
    SYNOPSIS        Workqueue task stack size configuration
    TYPE            uint
    DEFAULT         0x10000
}

Selection INCLUDE_DRMDEV {
    NAME            Direct Rendering Manager device drivers
    SYNOPSIS        Direct Rendering Manager device drivers
    COUNT           1-1
    _CHILDREN       FOLDER_DRM
}

Component INCLUDE_DRM_SHOW {
    NAME            Direct Rendering Manager show
    SYNOPSIS        This component includes the Direct Rendering Manager show that can be started from the VxWorks shell by executing "gfxDrmShow"
    _CHILDREN       FOLDER_DRM
    ARCHIVE         libgfxDrmShow.a
    REQUIRES        INCLUDE_DRM
}
