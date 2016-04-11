/* 40gfxI915.cdf - i915 driver CDF file */

/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
25may15,yat  Added FOLDER_I915 for I915_TOOLS (US59462)
22jan15,qsn  Created for VxWorks 7 (US48907).
*/

Component INCLUDE_I915 {
    NAME            Intel HD Graphics driver
    SYNOPSIS        Intel HD Graphics driver
    _CHILDREN       INCLUDE_DRMDEV
    _DEFAULTS       INCLUDE_DRMDEV
    ARCHIVE         libgfxItlI915.a
    CONFIGLETTES    gfxI915Init.c
    PROTOTYPE       void gfxI915Init(void);
    INIT_RTN        gfxI915Init();
    _INIT_ORDER     usrRoot
    INIT_AFTER      INCLUDE_DRM
    INIT_BEFORE     INCLUDE_USER_APPL
    REQUIRES        INCLUDE_DRM
}
