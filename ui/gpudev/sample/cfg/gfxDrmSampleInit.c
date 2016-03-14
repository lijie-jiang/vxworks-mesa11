/* gfxDrmSampleInit.c - Sample DRM driver initialization */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify, or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01sep15,yat  Written (US60452)
*/

#ifndef __INCgfxDrmSampleInitc
#define __INCgfxDrmSampleInitc

#include <vxWorks.h>

extern int gfxDrmDevInit (void);

/*******************************************************************************
*
* gfxDrmSampleInit - sample DRM driver initialization
*
* This routine initializes the sample DRM driver
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void gfxDrmSampleInit
    (
    void
    )
    {
#if !defined(INCLUDE_FBDEV_MEMORY)
    if (gfxDrmDevInit()) return;
#endif
    }

#endif /* __INCgfxDrmSampleInitc */
