/* 40gfxDrmSample.cdf - sample DRM device driver CDF file */

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
24jul15,yat  Written (US60452)
*/

Component INCLUDE_DRMDEV_SAMPLE {
    NAME            Sample DRM device driver
    SYNOPSIS        Sample DRM device driver
    _CHILDREN       INCLUDE_DRMDEV
    ARCHIVE         libgfxSampleDrm.a
    CONFIGLETTES    gfxDrmSampleInit.c
    PROTOTYPE       void gfxDrmSampleInit(void);
    INIT_RTN        gfxDrmSampleInit();
    _INIT_ORDER     usrRoot
    INIT_AFTER      INCLUDE_DRM
    INIT_BEFORE     INCLUDE_USER_APPL
    REQUIRES        INCLUDE_DRM
}
