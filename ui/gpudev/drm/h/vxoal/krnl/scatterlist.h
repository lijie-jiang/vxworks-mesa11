/* scatterlist.h - scatterlist functionality header file*/

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
21jul15,yat  Clean up vxoal (US63380)
24jan14,mgc  Modified for VxWorks 7 release
10jan14,af   Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux scatterlist operations.

NOMANUAL
*/

#ifndef _VXOAL_SCATTERLIST_H_
#define _VXOAL_SCATTERLIST_H_

#include <vxoal/krnl/types.h> /* for dma_addr_t */
#include <vxoal/krnl/mm.h> /* for page, PAGE_ALIGN */

#if 0
#define DEBUG_SG pr_err
#else
#define DEBUG_SG(...)
#endif

struct sg_table;

struct scatterlist
    {
    unsigned long           page_link;
    unsigned int            offset;
    unsigned int            length;
    dma_addr_t              dma_address;
    unsigned int            index;
    struct sg_table         *table;
    };

struct sg_table
    {
    struct scatterlist      *sgl;           /* the list */
    unsigned int            nents;          /* number of mapped entries */
    unsigned int            orig_nents;     /* original size of list */
    };

struct sg_page_iter
    {
    struct scatterlist      *sg;            /* sg holding the page */
    unsigned long           sg_pgoffset;    /* page offset within the sg */
    unsigned int            __nents;        /* remaining sg entries */
    int                     __pg_advance;   /* nr pages to advance at the
                                             * next step */
    };

#define sg_page(sg)             ((struct page *)((sg)->page_link))
#define sg_dma_len(sg)          ((sg)->length)
#define sg_dma_address(sg)      ((sg)->dma_address)
#define sg_nents(sgl)           ((sgl)->table->nents)

static inline int sg_alloc_table
    (
    struct sg_table *st, 
    unsigned int num, 
    int mask
    )
    {
    int i;

    DEBUG_SG("\n");

    if (st == NULL)
        return 1;

    if (num == 0)
        return 1;

    st->sgl = (struct scatterlist *)kcalloc (1, num * sizeof(struct scatterlist), GFP_KERNEL);
    if (st->sgl == (struct scatterlist *)NULL) return 1;

    for (i = 0; i < num; i ++)
        {
        st->sgl[i].index = i;
        st->sgl[i].table = st;
        }

    st->nents = num;
    st->orig_nents = num;

    return 0;
    }

static inline int sg_alloc_table_from_pages
    (
    struct sg_table *st,
    struct page **pages,
    unsigned int n_pages,
    unsigned long offset,
    unsigned long size,
    int mask
    )
    {
    DEBUG_SG("\n");

    return sg_alloc_table(st, n_pages, mask);
    }

static inline int sg_free_table
    (
    struct sg_table *st
    )
    {
    DEBUG_SG("\n");

    if (st == NULL)
        return 1;

    if (st->sgl)
        {
        (void)kfree (st->sgl);
        st->sgl = NULL;
        }

    st->nents = 0;
    st->orig_nents = 0;

    return 0;
    }

static inline struct scatterlist *sg_next
    (
    struct scatterlist *sg
    )
    {
    struct scatterlist *next;

    DEBUG_SG("\n");

    if (sg == NULL)
        return NULL; 

    if (sg->index < sg->table->nents - 1)
        next = sg++;
    else
        next = NULL;

    return next;
    }

static inline void sg_set_page
    (
    struct scatterlist *sg,
    struct page *page,
    unsigned int len,
    unsigned int offset
    )
    {
    DEBUG_SG("\n");

    if (sg == NULL)
        {
        WARN_DEV;
        return;
        }

    sg->page_link = (unsigned long)page;
    sg->offset = offset;
    sg->length = len;
    }

static inline void sg_mark_end
    (
    struct scatterlist *sg
    )
    {
    return;
    }

static inline struct page *sg_page_iter_page
    (
    struct sg_page_iter *piter
    )
    {
    struct page *head;

    head = (struct page *)(piter->sg->page_link);
    if (piter->sg_pgoffset > (head->maxIndex - head->index))
        return NULL;

    return (head + piter->sg_pgoffset);
    }

static inline void __sg_page_iter_start
    (
    struct sg_page_iter *piter,
    struct scatterlist *sglist,
    unsigned int nents,
    unsigned long pgoffset
    )
    {
    piter->sg = sglist;
    piter->sg_pgoffset = pgoffset;
    piter->__nents = nents;
    piter->__pg_advance = 0;
    }

static inline long sg_page_count
    (
    struct scatterlist *sg
    )
    {
    return PAGE_ALIGN(sg->offset + sg->length) >> PAGE_SHIFT;
    }

static inline int __sg_page_iter_next
    (
    struct sg_page_iter *piter
    )
    {
    if (!piter->__nents || !piter->sg)
        return 0;

    piter->sg_pgoffset += piter->__pg_advance;
    piter->__pg_advance = 1;

    while (piter->sg_pgoffset >= sg_page_count(piter->sg))
        {
        piter->sg_pgoffset -= sg_page_count(piter->sg);
        piter->sg = sg_next(piter->sg);
        if (!--piter->__nents || !piter->sg)
            return 0;
        }

    return 1;
    }

#define for_each_sg_page(sglist, piter, nents, pgoffset)                       \
    for (__sg_page_iter_start((piter), (sglist), (nents), (pgoffset));         \
         __sg_page_iter_next(piter);)

#define for_each_sg(sglist, sg, nents, i)                                      \
    for ((i) = 0, (sg) = (sglist); ((i) < (nents)) && (sg); (i)++, (sg) = sg_next(sg))

static inline int dma_map_sg
    (
    struct device *dev,
    struct scatterlist *sglist,
    int nents,
    int direction
    )
    {
    struct scatterlist *sg;
    int i;

    DEBUG_SG("\n");

    for_each_sg(sglist, sg, nents, i)
        {
        sg_dma_address(sg) = (dma_addr_t)((sg_page(sg))->physAddr + sg->offset);
        }

    return nents;
    }

static inline void dma_unmap_sg
    (
    struct device *dev,
    struct scatterlist *sglist,
    int nents,
    int direction
    )
    {
    struct scatterlist *sg;
    int i;

    DEBUG_SG("\n");

    for_each_sg(sglist, sg, nents, i)
        {
        }
    }

static inline dma_addr_t sg_page_iter_dma_address
    (
    struct sg_page_iter *piter
    )
    {
    return sg_dma_address(piter->sg) + (piter->sg_pgoffset << PAGE_SHIFT);
    }

/* Overload least sig. 2 bits of pointer */
#define sg_is_chain(sg)         ((sg)->page_link & 0x01)
#define sg_is_last(sg)          ((sg)->page_link & 0x02)
#define sg_chain_ptr(sg)        \
        ((struct scatterlist *) ((sg)->page_link & ~0x03))

#define sg_copy_from_buffer(sgl, n, data, size) ({(sgl)=(sgl);WARN_DEV;0;})
#define sg_pcopy_to_buffer(sgl, n, data, size, offset) ({(sgl)=(sgl);bzero((void*)(data), sizeof (data));})

#endif /* _VXOAL_SCATTERLIST_H */
