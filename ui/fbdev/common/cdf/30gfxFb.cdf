/* 30gfxFb.cdf - Graphics frame buffer driver CDF file */

/*
 * Copyright (c) 2012, 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
14nov14,yat  Remove FBDEV_PREEMPTIVE_RTP_MEMORY_FREE from pmap rework (US46449)
24jan14,mgc  Modified for VxWorks 7 release
11may12,rfm  Created.
*/

Folder FOLDER_FBDEV  {
    NAME            Frame buffer drivers components
    SYNOPSIS        Frame buffer drivers
    DEFAULTS        INCLUDE_FBDEV INCLUDE_FBDEV_SPLASH
    _CHILDREN       FOLDER_UI
}

Selection INCLUDE_FBDEV {
    NAME            Frame buffer drivers
    SYNOPSIS        Frame buffer drivers
    COUNT           1-1
    _CHILDREN       FOLDER_FBDEV
}

Component INCLUDE_FBDEV_MEMORY {
    NAME            Frame buffer memory
    SYNOPSIS        Frame buffer memory
    _CHILDREN       FOLDER_FBDEV
#if defined(_WRS_CONFIG_COMPAT69)
    CFG_PARAMS      FBDEV_MEMORY_SIZE \
                    FBDEV_PREEMPTIVE_RTP_MEMORY_FREE
#else
    CFG_PARAMS      FBDEV_MEMORY_SIZE
#endif
}

Parameter FBDEV_MEMORY_SIZE {
    NAME            Amount of memory
    SYNOPSIS        Amount of memory
    TYPE            uint
    DEFAULT         (1920*1080*4*2)
}

#if defined(_WRS_CONFIG_COMPAT69)
Parameter FBDEV_PREEMPTIVE_RTP_MEMORY_FREE {
    NAME            Preemptively free RTP memory 
    SYNOPSIS        Free frame buffer memory allocated to an RTP before creating a new allocation
    TYPE            bool
    DEFAULT         TRUE
}
#endif
