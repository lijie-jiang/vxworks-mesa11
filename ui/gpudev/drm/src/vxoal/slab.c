/* slab.c - slab  functions */

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
01jan13, af  created
*/

/*

DESCRIPTION

This file provides compatibility functions for the Linux slab operations.

NOMANUAL

*/

/* includes */

#include <stdio.h> /* for printf */
#include <string.h> /* for memset */
#include <memLib.h> /* for memalign */
#include <vxoal/krnl/log.h> /* for pr_err, SET_CUR */
#include <vxoal/krnl/slab.h> /* for extern */
#include <vxoal/krnl/mm.h> /* for kcalloc, kfree */

#if 0
#define DEBUG_SLAB pr_err
#else
#define DEBUG_SLAB(...)
#endif

struct kmem_cache * kmem_cache_create
    (
    const char *name,
    size_t size,
    size_t align,
    unsigned long flags,
    void (*ctor)(void *)
    )
    {
    struct kmem_cache *pCache;

    DEBUG_SLAB("\n");

    pCache = (struct kmem_cache *)kcalloc (1, sizeof (struct kmem_cache), GFP_KERNEL);
    if (pCache == (struct kmem_cache *)NULL) return pCache;

    pCache->name = name;
    pCache->obj_size = size;
    pCache->alignment = align;

    return pCache;
    }

void kmem_cache_destroy
    (
    struct kmem_cache *pCache
    )
    {
    DEBUG_SLAB("\n");

    if (pCache == NULL)
        return;

    (void)kfree (pCache);
    }

void *kmem_cache_zalloc
    (
    struct kmem_cache *pCache,
    gfp_t flags
    )
    {
    void *ptr;

    DEBUG_SLAB("\n");

    ptr = memalign (pCache->alignment, pCache->obj_size);
    if (ptr == NULL)
        {
        pr_err("memalign error size of %lx\n", pCache->obj_size);
        return NULL;
        }

    memset (ptr, 0, pCache->obj_size);

    return ptr;
    }

void kmem_cache_free
    (
    struct kmem_cache *pCache,
    void *ptr
    )
    {
    DEBUG_SLAB("\n");

    if (ptr == NULL)
        return;

    (void)kfree (ptr);
    }
