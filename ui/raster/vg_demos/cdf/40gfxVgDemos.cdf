/* 40gfxVgDemos.cdf - Vector Graphics demos CDF file */

/*
 * Copyright (c) 2012-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
25jan16,yat  Rewrite SYNOPSIS
22sep15,yat  Add missing REQUIRES INCLUDE_POSIX_PTHREAD_SCHEDULER (V7GFX-284)
08dec14,yat  Add missing ARCHIVE libgfxVgDemos.a (US46449)
24jan14,mgc  Modified for VxWorks 7 release
17may12,rfm  Written
*/

Folder FOLDER_VG_DEMOS {
    NAME            Frame buffer software Vector Graphics demos components
    SYNOPSIS        Frame buffer software Vector Graphics demos
    DEFAULTS        INCLUDE_VG_MAIN_DEMO
    _CHILDREN       FOLDER_RASTER_VG
}

Component INCLUDE_VG_SWAP_DEMO {
    NAME            Vector Graphics swap demo
    SYNOPSIS        This component includes the Vector Graphics swap demo that can be started from the VxWorks shell by executing "gfxVgSwapDemoStart"
    _CHILDREN       FOLDER_VG_DEMOS
    ARCHIVE         libgfxVgDemos.a
    REQUIRES        INCLUDE_VG \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxVgSwapDemoStart
}

Component INCLUDE_VG_FLOW_DEMO {
    NAME            Vector Graphics flow demo
    SYNOPSIS        This component includes the Vector Graphics flow demo that can be started from the VxWorks shell by executing "gfxVgFlowDemoStart"
    _CHILDREN       FOLDER_VG_DEMOS
    ARCHIVE         libgfxVgDemos.a
    REQUIRES        INCLUDE_VG \
                    INCLUDE_VG_JPEG_LOADER \
                    INCLUDE_VG_PNG_LOADER \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxVgFlowDemoStart
}

Component INCLUDE_VG_IMAGE_DEMO {
    NAME            Vector Graphics image demo
    SYNOPSIS        This component includes the Vector Graphics image demo that can be started from the VxWorks shell by executing "gfxVgImageDemoStart"
    _CHILDREN       FOLDER_VG_DEMOS
    ARCHIVE         libgfxVgDemos.a
    REQUIRES        INCLUDE_VG \
                    INCLUDE_VG_JPEG_LOADER \
                    INCLUDE_VG_PNG_LOADER \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxVgImageDemoStart
}

Component INCLUDE_VG_PATTERN_DEMO {
    NAME            Vector Graphics pattern demo
    SYNOPSIS        This component includes the Vector Graphics pattern demo that can be started from the VxWorks shell by executing "gfxVgPatternDemoStart"
    _CHILDREN       FOLDER_VG_DEMOS
    ARCHIVE         libgfxVgDemos.a
    REQUIRES        INCLUDE_VG \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxVgPatternDemoStart
}

Component INCLUDE_VG_SCROLL_DEMO {
    NAME            Vector Graphics scroll demo
    SYNOPSIS        This component includes the Vector Graphics scroll demo that can be started from the VxWorks shell by executing "gfxVgScrollDemoStart"
    _CHILDREN       FOLDER_VG_DEMOS
    ARCHIVE         libgfxVgDemos.a
    REQUIRES        INCLUDE_VG \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxVgScrollDemoStart
}

Component INCLUDE_VG_SUBWAY_DEMO {
    NAME            Vector Graphics subway demo
    SYNOPSIS        This component includes the Vector Graphics subway demo that can be started from the VxWorks shell by executing "gfxVgSubwayDemoStart"
    _CHILDREN       FOLDER_VG_DEMOS
    ARCHIVE         libgfxVgDemos.a
    REQUIRES        INCLUDE_VG \
                    INCLUDE_RTP \
                    INCLUDE_SHL \
                    INCLUDE_POSIX_PTHREAD_SCHEDULER
    LINK_SYMS       gfxVgSubwayDemoStart
}

Component INCLUDE_VG_MAIN_DEMO {
    NAME            Vector Graphics main demo
    SYNOPSIS        This component includes the Vector Graphics main demo that can be started from the VxWorks shell by executing "gfxVgMainDemoStart"
    _CHILDREN       FOLDER_VG_DEMOS
    ARCHIVE         libgfxVgDemos.a
    REQUIRES        INCLUDE_VG_SWAP_DEMO \
                    INCLUDE_VG_FLOW_DEMO \
                    INCLUDE_VG_IMAGE_DEMO \
                    INCLUDE_VG_PATTERN_DEMO \
                    INCLUDE_VG_SCROLL_DEMO \
                    INCLUDE_VG_SUBWAY_DEMO
    LINK_SYMS       gfxVgMainDemoStart
}
