/* slab.h - slab functionality header file*/

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
24jan14,mgc  Modified for VxWorks 7 release
24jan14,af   Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux slab operations.

NOMANUAL
*/

#ifndef _VXOAL_SLAB_H
#define _VXOAL_SLAB_H

#include <vxWorks.h> /* for _CACHE_ALIGN_SIZE */
#include <vxoal/krnl/types.h> /* for gfp_t */

#ifndef SLAB_HWCACHE_ALIGN
#define SLAB_HWCACHE_ALIGN _CACHE_ALIGN_SIZE
#endif

struct kmem_cache
    {
    size_t          obj_size;     /* Size of the objects contained
                                     within this slab cache */
    size_t          alignment;    /* Minimal alignment of all objects */
    const char     *name;         /* Human readable name of the memory
                                     cache */
    };

extern struct kmem_cache * kmem_cache_create
    (
    const char *name,
    size_t size,
    size_t align,
    unsigned long flags,
    void (*ctor)(void *)
    );

extern void kmem_cache_destroy
    (
    struct kmem_cache *pCache
    );

extern void *kmem_cache_zalloc 
    (
    struct kmem_cache *pCache,
    gfp_t flags
    );

extern void kmem_cache_free
    (
    struct kmem_cache *pCache,
    void *pObj
    );

#endif /* _VXOAL_SLAB_H */
