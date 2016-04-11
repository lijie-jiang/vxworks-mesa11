/* gfxI915Init.c - i915 driver initialization */

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
31jul15,yat  Written (US60452)
*/

#ifndef __INCgfxI915Initc
#define __INCgfxI915Initc

#include <vxWorks.h>

#if defined(INCLUDE_PC_CONSOLE)
#define GFX_PC_CONSOLE_CHECK                                                   \
    {                                                                          \
    (void)printf ("The Intel HD Graphics driver will be initialized but\nthe driver may not work properly with the PC console,\nplease remove INCLUDE_PC_CONSOLE from the kernel\nif the graphics does not work properly\n"); \
    }
#else
#define GFX_PC_CONSOLE_CHECK
#endif

#if defined(INCLUDE_SMP_INIT)
#define GFX_SMP_CHECK
#else
#define GFX_SMP_CHECK                                                          \
    {                                                                          \
    (void)printf ("The Intel HD Graphics driver will be initialized but\nthe driver may not work properly on some non-SMP configuration,\nplease use SMP configuration with VX_SMP_NUM_CPUS >= 1\nif the graphics does not work properly\n"); \
    }
#endif

extern int gfxDrmDevInit (void);
extern void drm_dev_private_show_init(VOIDFUNCPTR);
extern void show_drm_i915_private(void* dev_private, const char *tab);

/*******************************************************************************
*
* gfxI915Init - i915 driver initialization
*
* This routine initializes the i915 driver
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void gfxI915Init
    (
    void
    )
    {
#if !defined(INCLUDE_FBDEV_INIT)
    GFX_PC_CONSOLE_CHECK;
    GFX_SMP_CHECK;

    if (gfxDrmDevInit()) return;
#endif
#if defined(INCLUDE_DRM_SHOW)
    drm_dev_private_show_init(show_drm_i915_private);
#endif
    }

#endif /* __INCgfxI915Initc */
