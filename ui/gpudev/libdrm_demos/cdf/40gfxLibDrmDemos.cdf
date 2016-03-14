/* 40gfxLibDrmDemos.cdf - libdrm demos CDF file */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
16sep15,yat  Written (US24710)
*/

Folder FOLDER_LIBDRM_DEMOS {
    NAME            Direct Rendering Manager demos components
    SYNOPSIS        Direct Rendering Manager demos
    DEFAULTS        INCLUDE_LIBDRM_DRAW_DEMO
    _CHILDREN       FOLDER_DRM
}

Component INCLUDE_LIBDRM_DRAW_DEMO {
    NAME            Kernel Mode Setting draw demo
    SYNOPSIS        This component includes the Kernel Mode Setting draw demo that can be started from the VxWorks shell by executing "gfxKmsDrawDemoStart"
    _CHILDREN       FOLDER_LIBDRM_DEMOS
    ARCHIVE         libgfxLibDRMDemos.a
    REQUIRES        INCLUDE_DRM \
                    INCLUDE_RTP \
                    INCLUDE_SHL
    LINK_SYMS       gfxKmsDrawDemoStart
}
