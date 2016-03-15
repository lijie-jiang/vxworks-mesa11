/* idr.h - idr functionality header file*/

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
14sep15,yat  Clean up code (US66034)
24jan14,mgc  Modified for VxWorks 7 release
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux idr operations.

NOMANUAL
*/

#ifndef _VXOAL_IDR_H
#define _VXOAL_IDR_H

#include <vxoal/krnl/types.h> /* for gfp_t */

#define MAX_IDR_FREE      20
#define KEEP_IDR_FREE      0
#define REMOVE_IDR_FREE    1

struct idr_element
    {
    struct idr_element  * pNext;
    int                   id;
    void                * pData;
    };

struct idr
    {
    struct idr_element * pInUse;
    struct idr_element * pFree;
    int lastUsed;
    int freeCnt;
    };

struct ida
    {
    struct idr            idr;
    };

extern void idr_init
    (
    struct idr *idp
    );

extern void *idr_replace
    (
    struct idr *idp,
    void *ptr,
    int id
    );

extern void *idr_find
    (
    struct idr *idp,
    int id
    );

extern void idr_remove
    (
    struct idr *idp,
    int id
    );

extern void idr_remove_all
    (
    struct idr *idp
    );

extern void idr_destroy
    (
    struct idr *idp
    );

extern int idr_for_each
    (
    struct idr *idp,
    int (*fn)(int id, void *p, void *data),
    void *data
    );

extern void ida_init
    (
    struct ida *ida
    );

extern void ida_destroy
    (
    struct ida *ida
    );

extern void ida_remove
    (
    struct ida *pIda,
    int id
    );

extern int idr_alloc
    (
    struct idr *idr,
    void *ptr,
    int start,
    int end,
    gfp_t gfp_mask
    );

extern int ida_simple_get
    (
    struct ida *ida,
    unsigned int start,
    unsigned int end,
    gfp_t gfp_mask
    );

#define idr_preload(mask)
#define idr_preload_end()

#define idr_get_next(p1, p2) ({WARN_DEV;NULL;})

#define idr_for_each_entry(idr, ptr, id) \
    for ((id) = 0; ((ptr) = idr_get_next(idr, &(id))) != NULL; (id)++)

#endif /* _VXOAL_IDR_H */
