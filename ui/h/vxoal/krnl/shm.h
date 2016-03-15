/* shm.h - shm header file*/

/*
 * Copyright (c) 1999-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
15jul15,yat  Clean up vxoal (US60452)
05jun15,rpc  DRM/i915 3.18 port (US59495)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux shm operations.

NOMANUAL
*/

#ifndef _VXOAL_SHM_H
#define _VXOAL_SHM_H

#include <memLib.h> /* for memalign */
#include <vmLib.h> /* for vmTranslate */
#include <vxoal/krnl/types.h> /* for pgoff_t */
#include <vxoal/krnl/log.h> /* for pr_err */
#include <vxoal/krnl/device.h> /* for inode */
#include <vxoal/krnl/mm.h> /* for page, address_space, etc */

static inline struct file *shmem_file_setup
    (
    const char *name,
    size_t size,
    unsigned long flags,
    unsigned long virtAddr,
    unsigned long physAddr
    )
    {
    struct file *filp;
    struct page *page;

    filp = (struct file *)kcalloc (1, sizeof (struct file), GFP_KERNEL);
    if (filp == (struct file *)NULL) return filp;

    filp->name = name;

    filp->f_inode = (struct inode *)kcalloc (1, sizeof (struct inode), GFP_KERNEL);
    if (filp->f_inode == (struct inode *)NULL) goto nodeErr;

    filp->f_inode->i_mapping = (struct address_space *)kcalloc (1, sizeof (struct address_space), GFP_KERNEL);
    if (filp->f_inode->i_mapping == (struct address_space *)NULL) goto mapErr;

    page = drm_alloc_npages(GFP_DMA32 | __GFP_ZERO,
                            (int)((PAGE_ALIGN(size)) >> PAGE_SHIFT),
                            1, (VIRT_ADDR)virtAddr, (PHYS_ADDR)physAddr);
    if (page == NULL)
        {
        goto pageErr;
        }

    filp->f_inode->i_mapping->private_data = page;

    return filp;

pageErr:
    (void)kfree (filp->f_inode->i_mapping);
mapErr:
    (void)kfree (filp->f_inode);
nodeErr:
    (void)kfree (filp);
    return NULL;
    }

static inline struct page *shmem_read_mapping_page_gfp
    (
    struct address_space *mapping,
    pgoff_t index, 
    int gfp
    )
    {
    struct page *page;

    if (mapping == NULL)
        return NULL;

    page = mapping->private_data;
    page += index;

    return page;
    }

#define mapping_gfp_mask(m) (((m)->flags) & __GFP_BITS_MASK)

#define mapping_set_gfp_mask(m, mask)                                          \
   ((m)->flags = ((((m)->flags) & ~__GFP_BITS_MASK) | (mask)))

#define invalidate_mapping_pages(m, start, end) (m=m)

#define shmem_read_mapping_page(mapping, index)                                \
    shmem_read_mapping_page_gfp(mapping, index, mapping_gfp_mask(mapping))

#define shmem_truncate_range(inode, lstart, lend)

#endif /* _VXOAL_SHM_H */
