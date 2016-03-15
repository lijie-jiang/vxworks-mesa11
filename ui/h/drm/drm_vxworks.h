/*
 * Copyright (c) 2014-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
15jul15,yat  Clean up code (US60452)
22jan15,qsn  Port DRM to VxWorks 7 (US50702)
*/

#ifndef _DRM_VXWORKS_H
#define _DRM_VXWORKS_H

#include <vxWorks.h>
#include <ioLib.h> /* for DEV_HDR */
#include <vxoal/krnl/atomic.h> /* for kref */
#include <vxoal/krnl/slab.h> /* for kmem_cache */
#include <vxoal/krnl/scatterlist.h> /* for sg_table */
#include <vxoal/krnl/pci.h> /* for pci_dev, pci_device_id, pci_driver */
#include <vxoal/krnl/device.h> /* for file, inode, drm_device */
#include <vxoal/krnl/pci.h> /* for pci_driver */
#include <drm/drmP.h> /* for drm_minor */
#include <drm/drm_fb_helper.h> /* for drm_fb_helper */

#define TAB_LEN 32

#define SET_CUR                                                                \
    static int show_run = 0;                                                   \
    static void *prev = NULL;                                                  \
    char tab2[TAB_LEN];                                                        \
    snprintf(tab2, TAB_LEN, "%s\t", tab);                                      \
    printf("%s%s:%p\n", tab2, __func__, cur);                                  \
    if (!cur) return;                                                          \
    if (show_run != gfxDrmShowRun)                                             \
        {                                                                      \
        show_run = gfxDrmShowRun;                                              \
        prev = NULL;                                                           \
        }                                                                      \
    if (prev == (void *)cur) return;                                           \
    prev = (void *)cur;

extern int gfxDrmShowRun;

extern int gfxDrmShowDrmMm;

#define VX_DRM_MAX_DEV_NAME 256

struct drm_vxdev
    {
    DEV_HDR               header;
    char                  name[VX_DRM_MAX_DEV_NAME];
    struct file           filp;
    struct inode          inode;
    struct drm_device    *dev;
    };

extern const struct pci_device_id *gfxDrmDevPciList
    (
    void
    );

extern int device_add
    (
    struct device *dev    
    );

extern int drm_vx_install
    (
    struct file_operations *fops
    );

extern int pci_register_driver
    (
    struct pci_driver *pci_driver
    );

extern void pci_unregister_driver
    (
    struct pci_driver *pci_driver
    );

extern int request_irq
    (
    unsigned int irq,
    irq_handler_t handler,
    unsigned long flags,
    const char *name,
    void *dev
    );

extern void free_irq
    (
    unsigned int irq,
    void *dev_id
    );

extern void drm_mmap_show_init
    (
    VOIDFUNCPTR func
    );

extern void drm_pci_show_init
    (
    VOIDFUNCPTR func
    );

extern void drm_timer_show_init
    (
    VOIDFUNCPTR func
    );

extern void drm_vxdev_show_init
    (
    VOIDFUNCPTR func
    );

extern void drm_dev_private_show_init
    (
    VOIDFUNCPTR func
    );

extern void show_drm_vxdev
    (
    struct drm_vxdev *cur,
    const char *tab
    );

extern void show_drm_dev_private
    (
    void *cur,
    const char *tab
    );

extern void show_drm_list_head
    (
    struct list_head *cur,
    struct list_head *head,
    const char *tab
    );

#define show_list_head show_drm_list_head

extern void show_kref
    (
    struct kref *cur,
    const char *tab
    );

extern void show_io_mapping
    (
    struct io_mapping *cur,
    const char *tab
    );

extern void show_kmem_cache
    (
    struct kmem_cache *pCache,
    const char *tab
    );

extern void show_sg_table
    (
    struct sg_table *cur,
    const char *tab
    );

extern void show_drm_fb_helper
    (
    struct drm_fb_helper *cur,
    const char *tab
    );

extern void show_drm_gem_object
    (
    struct drm_gem_object *cur,
    const char *tab
    );

extern void show_drm_mm
    (
    struct drm_mm *cur,
    const char *tab
    );

extern void show_drm_mm_node
    (
    struct drm_mm_node *cur,
    const char *tab
    );

extern void show_drm_device
    (
    struct drm_device *dev,
    const char *tab
    );

#endif /* _DRM_VXWORKS_H */
