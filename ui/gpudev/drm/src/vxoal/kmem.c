/* kmem.c - kernel Memory management functions */

/*
 * Copyright (c) 1999-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
25feb16,yat  Add GFX_USE_PMAP for non COMPAT69 (US76256)
14sep15,yat  Clean up code (US66034)
30mar15,rpc  Static analysis fixes 2nd pass
27mar15,rpc  Static analysis fixes (US50633)
07jan15,qsn  Modified ioremap() to support 64 bit address
24jan14,mgc  Modified for VxWorks 7 release
14oct99,msr  Written.
*/

/*

DESCRIPTION

This file provides platform independent memory management functions.

NOMANUAL

*/

/* includes */

#include <ctype.h> /* for isascii, isprint */
#include <string.h> /* for memset */
#include <semLib.h> /* for semMCreate */
#include <vxoal/krnl/log.h> /* for pr_err */
#include <vxoal/krnl/mm.h> /* for extern, kcalloc, kfree */
#if defined(GFX_USE_PMAP)
#include <pmapLib.h> /* for pmapGlobalMap */
#endif
#include "private/vmLibP.h" /* for vmTranslate */

/* defines */

#if defined(GFX_USE_PMAP)
#define GFX_PMAP_ATTR               (MMU_ATTR_SUP_RW |          \
                                     MMU_ATTR_USR_RW |          \
                                     MMU_ATTR_CACHE_OFF |       \
                                     MMU_ATTR_VALID)
#endif

#define GFX_IS_RTP_CTX              (MY_CTX_ID() != kernelId)

/*******************************************************************************
*
* Memory mapping
*
*/

typedef struct _GFX_MMAP
    {
    RTP_ID                          rtpId;    /* RTP ID */
    int                             preAllocMap;/* Preallocated and premapped */
    PHYS_ADDR                       physAddr; /* Physical address */
    VIRT_ADDR                       virtAddr; /* Mapped virtual address */
    void*                           addr;     /* Unmapped virtual address */
    unsigned long                   size;     /* Size of memory */
    struct _GFX_MMAP*               prev;
    struct _GFX_MMAP*               next;
    } GFX_MMAP;

static GFX_MMAP*                    memMapHead = (GFX_MMAP*)NULL;
static GFX_MMAP*                    preMapHead = (GFX_MMAP*)NULL;
static SEM_ID                       mmapSemId = SEM_ID_NULL;

/*******************************************************************************
*
* gfxDrmMmapInit - initialize semaphore mmap
*
* RETURNS: N/A
*
* SEE ALSO: 
*/
void gfxDrmMmapInit
    (
    void
    )
    {
    mmapSemId = semMCreate (SEM_Q_FIFO);
    if (mmapSemId == SEM_ID_NULL)
        {
        pr_err ("semMCreate failed\n");
        }
    }

/*******************************************************************************
*
* find_mmap_phys - find a memory map based on physical address
*
* RETURNS: memory map
*
* SEE ALSO: 
*/
static GFX_MMAP *find_mmap_phys
    (
    GFX_MMAP*       tmpMap,
    PHYS_ADDR       physAddr,
    unsigned long   size
    )
    {
    unsigned long   addr = (unsigned long)physAddr;
    unsigned long   tmpAddr;

    while (tmpMap)
        {
        tmpAddr = (unsigned long)(tmpMap->physAddr);
        if ((tmpAddr <= addr) &&
            ((addr + size) <= (tmpAddr + tmpMap->size)))
            {
            return tmpMap;
            }
        tmpMap = tmpMap->next;
        }

    return (GFX_MMAP*)NULL;
    }

/*******************************************************************************
*
* find_mmap_virt - find a memory map based on virtual address
*
* RETURNS: memory map
*
* SEE ALSO: 
*/
static GFX_MMAP *find_mmap_virt
    (
    GFX_MMAP*       tmpMap,
    VIRT_ADDR       virtAddr
    )
    {
    unsigned long   addr = (unsigned long)virtAddr;
    unsigned long   tmpAddr;

    while (tmpMap)
        {
        tmpAddr = (unsigned long)(tmpMap->virtAddr);
        if ((tmpAddr <= addr) &&
            (addr <= (tmpAddr + tmpMap->size)))
            {
            return tmpMap;
            }
        tmpMap = tmpMap->next;
        }

    return (GFX_MMAP*)NULL;
    }

/*******************************************************************************
*
* find_virt_mmap_phys - find virtual address based on physical address
*
* RETURNS: virtual address
*
* SEE ALSO: 
*/
static VIRT_ADDR find_virt_mmap_phys
    (
    GFX_MMAP*       tmpMap,
    PHYS_ADDR       physAddr,        
    unsigned long   size             
    )
    {
    tmpMap = find_mmap_phys(tmpMap, physAddr, size);
    if (tmpMap == NULL)
        {
        return (VIRT_ADDR)NULL;
        }

    return (VIRT_ADDR)((unsigned long)(tmpMap->virtAddr) +
           ((unsigned long)physAddr - (unsigned long)(tmpMap->physAddr)));
    }

/*******************************************************************************
*
* find_mmap_size - find a memory map based on size
*
* RETURNS: memory map
*
* SEE ALSO:
*/
static GFX_MMAP *find_mmap_size_premap
    (
    GFX_MMAP*       tmpMap,
    unsigned long   size,
    int             preAllocMap
    )
    {
    while (tmpMap)
        {
        if ((tmpMap->size == size) &&
            (tmpMap->preAllocMap == preAllocMap))
            {
            return tmpMap;
            }
        tmpMap = tmpMap->next;
        }

    return (GFX_MMAP*)NULL;
    }

/*******************************************************************************
*
* create_mmap - create a memory map
*
* RETURNS: OK if successful, ERROR otherwise
*
* SEE ALSO: 
*/
static STATUS create_mmap
    (
    GFX_MMAP**      tmpMapHead,
    PHYS_ADDR       physAddr,        /* Physical address space */
    VIRT_ADDR       virtAddr,        /* Mapped virtual address space */
    void*           addr,            /* Unmapped virtual address space */
    unsigned long   size,            /* Size of memory chunk to map */
    int             preAllocMap
    )
    {
    GFX_MMAP*       tmpMap;
    GFX_MMAP*       newMap;

    newMap = (GFX_MMAP*)kcalloc (1, sizeof (GFX_MMAP), GFP_KERNEL);
    if (newMap == (GFX_MMAP*)NULL) return ERROR;

    newMap->rtpId = MY_CTX_ID ();
    newMap->preAllocMap = preAllocMap;
    newMap->physAddr = physAddr;
    newMap->virtAddr = virtAddr;
    newMap->addr = addr;
    newMap->size = size;

    if (*tmpMapHead == (GFX_MMAP*)NULL)
        {
        *tmpMapHead = newMap;
        }
    else
        {
        tmpMap = *tmpMapHead;
        while (tmpMap->next)
            {
            tmpMap = tmpMap->next;
            }
        newMap->prev = tmpMap;
        tmpMap->next = newMap;
        }

    return OK;
    }

/*******************************************************************************
*
* destroy_mmap - destroy a memory map
*
* RETURNS: OK if successful, ERROR otherwise
*
* SEE ALSO: 
*/
static STATUS destroy_mmap
    (
    GFX_MMAP**      tmpMapHead,
    GFX_MMAP*       tmpMap
    )
    {
    if (tmpMap == (GFX_MMAP*)NULL)
        return ERROR;

    if (tmpMap == *tmpMapHead)
        *tmpMapHead = tmpMap->next;

    if (tmpMap->prev)
        tmpMap->prev->next = tmpMap->next;

    if (tmpMap->next)
        tmpMap->next->prev = tmpMap->prev;

    (void)kfree (tmpMap);

    return OK;
    }

/*******************************************************************************
*
* show_mmap - show memory map
*
* RETURNS:
*
* SEE ALSO: 
*/
static void show_mmap
    (
    GFX_MMAP*       tmpMap
    )
    {
    (void)semTake (mmapSemId, WAIT_FOREVER);

    while (tmpMap)
        {
        printf("map:%p\n", tmpMap);
        printf("\trtpId:%p-kernelId:%p\n", tmpMap->rtpId, kernelId);
        printf("\tpreAllocMap:%d\n", tmpMap->preAllocMap);
        printf("\tphysAddr:%lx\n", tmpMap->physAddr);
        printf("\tvirtAddr:%lx\n", tmpMap->virtAddr);
        printf("\taddr:%p\n", tmpMap->addr);
        printf("\tsize:%ld\n", tmpMap->size);
        tmpMap = tmpMap->next;
        }

    (void)semGive (mmapSemId);
    }

/*******************************************************************************
*
* show_mmap - show memory map
*
* RETURNS:
*
* SEE ALSO: 
*/
void show_drm_mmap
    (
    void
    )
    {
    show_mmap(memMapHead);
    show_mmap(preMapHead);
    }

/*******************************************************************************
*
* map_mmap - map memory
*
* RETURNS: mapped virtual address
*
* SEE ALSO:
*/
static void *map_mmap
    (
    GFX_MMAP**      tmpMapHead,
    PHYS_ADDR       physAddr,
    void*           addr,
    unsigned long   size
    )
    {
    VIRT_ADDR virtAddr;

    if (size == 0)
        {
        pr_err ("Bad size:%ld\n", size);
        return NULL;
        }

    (void)semTake (mmapSemId, WAIT_FOREVER);

    /* Find mmap based on physAddr */
    virtAddr = find_virt_mmap_phys(*tmpMapHead, physAddr, size);
    if (virtAddr)
        {
        /* Found mmap, return virtAddr */
        (void)semGive (mmapSemId);
        return (void*)virtAddr;
        }

    /* Check for RTP */
    if (GFX_IS_RTP_CTX)
        {
#if defined(GFX_USE_PMAP)
        virtAddr = (VIRT_ADDR)pmapPrivateMap (physAddr, size, GFX_PMAP_ATTR);
        if (virtAddr == (VIRT_ADDR)PMAP_FAILED)
            {
            pr_err ("pmapPrivateMap size:%ld failed\n", size);
            (void)semGive (mmapSemId);
            return NULL;
            }
#else
        pr_err ("RTP mmap not supported\n");
        (void)semGive (mmapSemId);
        return NULL;
#endif
        }
    else
        {
#if defined(GFX_USE_PMAP)
        virtAddr = (VIRT_ADDR)pmapGlobalMap (physAddr, size, GFX_PMAP_ATTR);
        if (virtAddr == (VIRT_ADDR)PMAP_FAILED)
            {
            pr_err ("pmapGlobalMap size:%ld failed\n", size);
            (void)semGive (mmapSemId);
            return NULL;
            }
#else
        virtAddr = (VIRT_ADDR)physAddr;
#endif
        }

    if (create_mmap(tmpMapHead, physAddr, virtAddr, addr, size, 0) == ERROR)
        {
#if defined(GFX_USE_PMAP)
        if (GFX_IS_RTP_CTX)
            {
            (void)pmapPrivateUnmap ((void *)virtAddr, size);
            }
        else
            {
            (void)pmapGlobalUnmap ((void *)virtAddr, size);
            }
#endif
        (void)semGive (mmapSemId);
        return NULL;
        }

    (void)semGive (mmapSemId);
    return (void *)virtAddr;
    }

/*******************************************************************************
*
* unmap_mmap - unmap memory
*
* RETURNS: unmapped virtual address
*
* SEE ALSO:
*/
static void *unmap_mmap
    (
    GFX_MMAP*       tmpMap,
    VIRT_ADDR       virtAddr
    )
    {
    void *addr;

    (void)semTake (mmapSemId, WAIT_FOREVER);

    /* Find mmap based on virtAddr */
    tmpMap = find_mmap_virt (tmpMap, virtAddr);
    if (tmpMap == (GFX_MMAP*)NULL)
        {
        /* Not found, return virtAddr to free */
        (void)semGive (mmapSemId);
        return (void*)virtAddr;
        }

    /* Found mmap, check for matching virtAddr */
    if (tmpMap->virtAddr != virtAddr)
        {
        /* No match, return NULL to do nothing */
        (void)semGive (mmapSemId);
        return NULL;
        }

    /* Check for preAllocMap */
    if (tmpMap->preAllocMap)
        {
        /* Free preAllocMap, return NULL to do nothing */
        GFX_MMAP* tmpMap2 = find_mmap_size_premap(preMapHead, tmpMap->size, 1);
        if (tmpMap2 != (GFX_MMAP*)NULL)
            {
            tmpMap2->preAllocMap = 0;
            }
        addr = NULL;
        }
    else
        {
        /* Unmap, return addr to free */
#if defined(GFX_USE_PMAP)
        if (GFX_IS_RTP_CTX)
            {
            (void)pmapPrivateUnmap ((void *)(tmpMap->virtAddr), tmpMap->size);
            }
        else
            {
            (void)pmapGlobalUnmap ((void *)(tmpMap->virtAddr), tmpMap->size);
            }
#endif
        addr = tmpMap->addr;
        }

    (void)destroy_mmap (&memMapHead, tmpMap);

    (void)semGive (mmapSemId);
    return addr;
    }

/*******************************************************************************
*
* ioremap - map I/O space
*
* RETURNS: map
*
* SEE ALSO:
*/
void *ioremap
    (
    unsigned long offset,
    unsigned long size
    )
    {
    return map_mmap (&memMapHead, (PHYS_ADDR)offset, NULL, size);
    }

/*******************************************************************************
*
* iounmap - unmap I/O space
*
* RETURNS: N/A
*
* SEE ALSO:
*/
void iounmap
    (
    void *addr
    )
    {
    (void)unmap_mmap (memMapHead, (VIRT_ADDR)addr);
    }

/*******************************************************************************
*
* io_mapping_create_wc - create I/O mapping
*
* RETURNS: I/O mapping
*
* SEE ALSO: 
*/
struct io_mapping *io_mapping_create_wc
    (
    resource_size_t base,
    unsigned long size
    )
    {
    struct io_mapping *iomap;

    iomap = (struct io_mapping *)kcalloc (1, sizeof (struct io_mapping), GFP_KERNEL);
    if (iomap == (struct io_mapping *)NULL) return iomap;

    if (map_mmap (&memMapHead, (PHYS_ADDR)base, NULL, size) == NULL)
        {
        (void)kfree (iomap);
        return (struct io_mapping*)NULL;
        }

    iomap->base = base;
    iomap->size = size;
    iomap->prot = 0;

    return (iomap);
    }

/*******************************************************************************
*
* io_mapping_free - free I/O mapping
*
* RETURNS: I/O mapping
*
* SEE ALSO: 
*/
void io_mapping_free
    (
    struct io_mapping *mapping
    )
    {
    (void)unmap_mmap (memMapHead, (VIRT_ADDR)(mapping->base));
    (void)kfree (mapping);
    }

/*******************************************************************************
*
* alloc_map_mmap - alloc and map memory
*
* RETURNS: virtual address
*
* SEE ALSO:
*/
static VIRT_ADDR alloc_map_mmap
    (
    GFX_MMAP**      tmpMapHead,
    PHYS_ADDR*      pPhysAddr,
    unsigned long   size,
    int             map
    )
    {
    VIRT_ADDR virtAddr;

    virtAddr = (VIRT_ADDR) memalign (PAGE_SIZE, size);
    if (virtAddr == (VIRT_ADDR)NULL)
        {
        pr_err ("memalign error size of %lx\n", size);
        return (VIRT_ADDR)NULL;
        }

    if (ERROR == vmTranslate (NULL, virtAddr, pPhysAddr))
        {
        pr_err ("vmTranslate failed\n");
        (void)kfree ((void*)virtAddr);
        return (VIRT_ADDR)NULL;
        }
 
    /* Check for need to map */
    if (map)
        {
        VIRT_ADDR virtAddr2 = (VIRT_ADDR)map_mmap (tmpMapHead, *pPhysAddr, (void*)virtAddr, size);
        if (virtAddr2 == (VIRT_ADDR)NULL)
            {
            pr_err ("map_mmap error size of %lx\n", size);
            (void)kfree ((void*)virtAddr);
            return (VIRT_ADDR)NULL;
            }
        virtAddr = virtAddr2;
        }

    return virtAddr;
    }

/*******************************************************************************
*
* drm_alloc_pre - alloc and map
*
* RETURNS:
*
* SEE ALSO:
*/
void drm_alloc_pre
    (
    unsigned long   size
    )
    {
    PHYS_ADDR       physAddr;

    (void)alloc_map_mmap (&preMapHead, &physAddr, size, 1);
    }

/*******************************************************************************
*
* drm_alloc_npages - alloc pages
*
* RETURNS: pointer to first page
*
* SEE ALSO: 
*/
struct page *drm_alloc_npages
    (
    int mask,
    int numPages,
    int map,
    VIRT_ADDR virtAddr0,
    PHYS_ADDR physAddr0
    )
    {
    struct page *page;
    VIRT_ADDR virtAddr = (VIRT_ADDR)NULL;
    PHYS_ADDR physAddr;
    int i, size;

    if (numPages <= 0)
        {
        pr_err ("Bad numPages:%d\n", numPages);
        return NULL;
        }

    size = numPages * PAGE_SIZE;

    page = (struct page *)kcalloc (numPages, sizeof (struct page), GFP_KERNEL);
    if (page == (struct page *)NULL) return page;

    /* Check for allocated memory */
    if (virtAddr0)
        {
        virtAddr = virtAddr0;
        physAddr = physAddr0;
        }
    /* Check for RTP and need to map */
    else if (GFX_IS_RTP_CTX && map)
        {
        (void)semTake (mmapSemId, WAIT_FOREVER);

        /* Find preAllocMap */
        GFX_MMAP* tmpMap = find_mmap_size_premap(preMapHead, size, 0);
        if (tmpMap != (GFX_MMAP*)NULL)
            {
            /* Found preAllocMap */
            physAddr = (PHYS_ADDR)tmpMap->physAddr;
            virtAddr = (VIRT_ADDR)tmpMap->virtAddr;
            if (create_mmap(&memMapHead, physAddr, virtAddr, tmpMap->addr, size, 1) == ERROR)
                {
                pr_err ("create_mmap error size of %x\n", size);
                (void)semGive (mmapSemId);
                (void)kfree (page);
                return (struct page *)NULL;
                }
            /* Mark preAllocMap used */
            tmpMap->preAllocMap = 1;
            }
        (void)semGive (mmapSemId);
        }

    if (virtAddr == (VIRT_ADDR)NULL)
        {
        virtAddr = alloc_map_mmap (&memMapHead, &physAddr, size, map);
        if (virtAddr == (VIRT_ADDR)NULL)
            {
            pr_err ("alloc_map_mmap error size of %x\n", size);
            (void)kfree (page);
            return (struct page *)NULL;
            }
        }

    if (mask & __GFP_ZERO)
        {
        (void) memset ((void *)virtAddr, 0, size);
        }

    for (i = 0; i < numPages; i++)
        {
        page[i].virtAddr = virtAddr;
        page[i].physAddr = physAddr;
        virtAddr += PAGE_SIZE;
        physAddr += PAGE_SIZE;
        page[i].index = i;
        page[i].maxIndex = numPages - 1;
        }

    return (page);
    }

/*******************************************************************************
*
* drm_free_npages - free pages
*
* RETURNS:
*
* SEE ALSO: 
*/
void drm_free_npages
    (
    struct page *page,
    int numPages
    )
    {
    void *addr;

    if (page == (struct page *)NULL) return;
    if (page[0].maxIndex != (numPages - 1)) return;

    /* Check for mmap */
    addr = unmap_mmap (memMapHead, page->virtAddr);
    if (addr)
        {
        (void)kfree (addr);
        }

    (void)kfree (page);
    }

/*******************************************************************************
*
* set_pages_uc - set pages cache off
*
* RETURNS: 1 if error, 0 otherwise
*
* SEE ALSO: 
*/
int set_pages_uc
    (
    struct page *page,
    int numPages
    )
    {
    int size = numPages * PAGE_SIZE;

    if (ERROR == vmStateSet (NULL, page->virtAddr, size,
                             VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE_NOT))
        {
        return 1;
        }

    return 0;
    }

/*******************************************************************************
*
* vmalloc_to_page - find page based on virtual address
*
* RETURNS: page
*
* SEE ALSO: 
*/
struct page *vmalloc_to_page
    (
    void *addr
    )
    {
    WARN_DEV;

    return alloc_page (GFP_KERNEL);
    }

/*******************************************************************************
*
* __vmalloc - allocate memory from page
*
* RETURNS: virtual address
*
* SEE ALSO: 
*/
void *__vmalloc
    (
    unsigned long size,
    gfp_t mask,
    pgprot_t prot
    )
    {
    struct page *page;

    page = drm_alloc_npages(mask, (int)(size >> PAGE_SHIFT),
                            0, (VIRT_ADDR)NULL, (PHYS_ADDR)NULL);
    if (page == (struct page *)NULL)
        {
        return NULL;
        }

    return (void *)(page[0].virtAddr);
    }

#if 0
const static char hex_asc[] = "0123456789abcdef";
#define hex_asc_lo(x)   hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)   hex_asc[((x) & 0xf0) >> 4]
void hex_dump_to_buffer(const void *buf, size_t len, int rowsize,
                        int groupsize, char *linebuf, size_t linebuflen,
                        bool ascii)
{
        const UINT8 *ptr = buf;
        UINT8 ch;
        long j, lx = 0;
        long ascii_column;

        if (rowsize != 16 && rowsize != 32)
                rowsize = 16;

        if (!len)
                goto nil;
        if (len > rowsize)              /* limit to one line at a time */
                len = rowsize;
        if ((len % groupsize) != 0)     /* no mixed size output */
                groupsize = 1;

        switch (groupsize) {
        case 8: {
                const UINT64 *ptr8 = buf;
                long ngroups = len / groupsize;

                for (j = 0; j < ngroups; j++)
                        lx += scnprintf(linebuf + lx, linebuflen - lx,
                                        "%s%16.16llx", j ? " " : "",
                                        (unsigned long long)*(ptr8 + j));
                ascii_column = 17 * ngroups + 2;
                break;
        }

        case 4: {
                const UINT32 *ptr4 = buf;
                long ngroups = len / groupsize;

                for (j = 0; j < ngroups; j++)
                        lx += scnprintf(linebuf + lx, linebuflen - lx,
                                        "%s%8.8x", j ? " " : "", *(ptr4 + j));
                ascii_column = 9 * ngroups + 2;
                break;
        }

        case 2: {
                const UINT16 *ptr2 = buf;
                long ngroups = len / groupsize;

                for (j = 0; j < ngroups; j++)
                        lx += scnprintf(linebuf + lx, linebuflen - lx,
                                        "%s%4.4x", j ? " " : "", *(ptr2 + j));
                ascii_column = 5 * ngroups + 2;
                break;
        }

        default:
                for (j = 0; (j < len) && (lx + 3) <= linebuflen; j++) {
                        ch = ptr[j];
                        linebuf[lx++] = hex_asc_hi(ch);
                        linebuf[lx++] = hex_asc_lo(ch);
                        linebuf[lx++] = ' ';
                }
                if (j)
                        lx--;

                ascii_column = 3 * rowsize + 2;
                break;
        }
        if (!ascii)
                goto nil;

        while (lx < (linebuflen - 1) && lx < (ascii_column - 1))
                linebuf[lx++] = ' ';
        for (j = 0; (j < len) && (lx + 2) < linebuflen; j++) {
                ch = ptr[j];
                linebuf[lx++] = (isascii(ch) && isprint(ch)) ? ch : '.';
        }
nil:
        linebuf[lx++] = '\0';
}
#endif
