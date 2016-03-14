/* gfxDrmInit.c - Direct Rendering Manager driver initialization */

/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify, or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
14sep15,yat  Clean up DRM init (US66034)
22jan15,qsn  Created for VxWorks 7 (US48907).
*/

#ifndef __INCgfxDrmInitc
#define __INCgfxDrmInitc

#include <vxWorks.h>

extern int gfxDrmInit0 (void);
extern void drm_mmap_show_init(VOIDFUNCPTR);
extern void show_drm_mmap(void);
extern void drm_pci_show_init(VOIDFUNCPTR);
extern void show_drm_pci(void);
extern void drm_timer_show_init(VOIDFUNCPTR);
extern void show_drm_timer(void);
extern void drm_vxdev_show_init(VOIDFUNCPTR);
extern void show_drm_vxdev(void *, const char *);

/*******************************************************************************
*
* gfxDrmInit - DRM driver initialization
*
* This routine initializes the DRM driver
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void gfxDrmInit
    (
    void
    )
    {
    if (gfxDrmInit0() == ERROR)
        {
        (void)fprintf (stderr, "DRM initialization failed\n");
        return;
        }
#if defined(INCLUDE_DRM_SHOW)
    drm_mmap_show_init(show_drm_mmap);
    drm_pci_show_init(show_drm_pci);
    drm_timer_show_init(show_drm_timer);
    drm_vxdev_show_init(show_drm_vxdev);
#endif
    }

/*******************************************************************************
*
* gfxDrmWorkqueueTaskPriority - get workqueue task priority configuration
*
* This routine returns the configured workqueue task priority
*
* RETURNS: workqueue task priority configuration
*
* ERRNO: N/A
*
*/
unsigned int gfxDrmWorkqueueTaskPriority
    (
    void
    )
    {
    return DRM_WORKQUEUE_TASK_PRIORITY;
    }

/*******************************************************************************
*
* gfxDrmWorkqueueStackSize - get workqueue stack size configuration
*
* This routine returns the configured workqueue stack size
*
* RETURNS: workqueue stack size configuration
*
* ERRNO: N/A
*
*/
unsigned int gfxDrmWorkqueueStackSize
    (
    void
    )
    {
    return DRM_WORKQUEUE_STACK_SIZE;
    }

#endif /* __INCgfxDrmInitc */
