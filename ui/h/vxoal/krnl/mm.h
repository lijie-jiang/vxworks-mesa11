/* mm.h - mm functionality header file*/

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
17sep15,rpc  Clean up vxoal (US66439)
15jul15,yat  Clean up vxoal (US60452)
09jul15,qsn  Added defines, structs and prototypes related to GEM mmap (US60453)
07jan15,qsn  Added more function prototypes
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility functions for the Linux mm operations.

NOMANUAL
*/

#ifndef _VXOAL_MM_H
#define _VXOAL_MM_H

#include <stdlib.h> /* for calloc, malloc, realloc */
#include <memLib.h> /* for memalign */
#include <string.h> /* for memcpy */
#include <vmLib.h> /* for vmPageSizeGet, VM_STATE_MASK_VALID, etc */
#include <semLib.h> /* for SEM_Q_PRIORITYk, etc */
#include <ffsLib.h> /* for ffs64Lsb */
#include <vxAtomicLib.h> /* for VX_MEM_BARRIER_RW, etc */
#include <drv/timer/timerDev.h> /* for sysClkRateGet */

#include <vxoal/krnl/types.h> /* for gfp_t, loff_t, resource_size_t */
#include <vxoal/krnl/log.h> /* for pr_err */
#include <vxoal/krnl/ilog2.h> /* for ilog2 */
#include <vxoal/krnl/list.h> /* for list_head */
#include <vxoal/krnl/spinlock.h> /* for rw_semaphore */
#include <vxoal/krnl/mutex.h> /* for mutex */
#include <vxoal/krnl/module.h> /* for KBUILD_MODNAME, THIS_MODULE */

#define __GFP_DMA               ((gfp_t)0x01u)
#define __GFP_HIGHMEM           ((gfp_t)0x02u)
#define __GFP_DMA32             ((gfp_t)0x04u)
#define __GFP_MOVABLE           ((gfp_t)0x08u)
#define __GFP_WAIT              ((gfp_t)0x10u)
#define __GFP_HIGH              ((gfp_t)0x20u)
#define __GFP_IO                ((gfp_t)0x40u)
#define __GFP_FS                ((gfp_t)0x80u)
#define __GFP_COLD              ((gfp_t)0x100u)
#define __GFP_NOWARN            ((gfp_t)0x200u)
#define __GFP_REPEAT            ((gfp_t)0x400u)
#define __GFP_NOFAIL            ((gfp_t)0x800u)
#define __GFP_NORETRY           ((gfp_t)0x1000u)
#define __GFP_MEMALLOC          ((gfp_t)0x2000u)
#define __GFP_COMP              ((gfp_t)0x4000u)
#define __GFP_ZERO              ((gfp_t)0x8000u)
#define __GFP_NOMEMALLOC        ((gfp_t)0x10000u)
#define __GFP_HARDWALL          ((gfp_t)0x20000u)
#define __GFP_THISNODE          ((gfp_t)0x40000u)
#define __GFP_RECLAIMABLE       ((gfp_t)0x80000u)
#define __GFP_NOTRACK           ((gfp_t)0x200000u)
#define __GFP_NO_KSWAPD         ((gfp_t)0x400000u)
#define __GFP_OTHER_NODE        ((gfp_t)0x800000u)
#define __GFP_WRITE             ((gfp_t)0x1000000u)

#define __GFP_BITS_SHIFT        25
#define __GFP_BITS_MASK         ((1 << __GFP_BITS_SHIFT) - 1)

#define GFP_ATOMIC              (__GFP_HIGH)
#define GFP_USER                (__GFP_WAIT | __GFP_IO | __GFP_FS |            \
                                 __GFP_HARDWALL)
#define GFP_HIGHUSER            (__GFP_WAIT | __GFP_IO | __GFP_FS |            \
                                 __GFP_HARDWALL | __GFP_HIGHMEM)
#define GFP_KERNEL              (__GFP_WAIT | __GFP_IO | __GFP_FS)
#define GFP_TEMPORARY           (__GFP_WAIT | __GFP_IO | __GFP_FS |            \
                                 __GFP_RECLAIMABLE)
#define GFP_NOWAIT              (GFP_ATOMIC & ~__GFP_HIGH)
#define GFP_DMA32               (__GFP_DMA32)

#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))
#define ALIGN(x,a)              __ALIGN_MASK(x,(__typeof__(x))(a)-1)

#define DMA_BIT_MASK(n)         (((n) == 64) ? ~0ULL : ((1ULL<<(n))-1))
#define DMA_MASK_NONE           0x0ULL

#define PAGE_SIZE               ((int)vmPageSizeGet())
#define PAGE_SHIFT              (ffs64Lsb(PAGE_SIZE) - 1)
#define PAGE_MASK               (~(PAGE_SIZE-1))

#define PAGE_ALIGN(addr)        ALIGN(addr, PAGE_SIZE)

#define VM_READ                 0x00000001
#define VM_WRITE                0x00000002
#define VM_EXEC                 0x00000004
#define VM_SHARED               0x00000008
#define VM_MAYWRITE             0x00000020
#define VM_MAYSHARE             0x00000080
#define VM_PFNMAP               0x00000400
#define VM_LOCKED               0x00002000
#define VM_IO                   0x00004000
#define VM_DONTCOPY             0x00020000
#define VM_DONTEXPAND           0x00040000
#define VM_RESERVED             0x00080000
#define VM_NORESERVE            0x00200000
#define VM_DONTDUMP             0x04000000
#define VM_MIXEDMAP             0x10000000

#define VERIFY_READ             0
#define VERIFY_WRITE            1

#define FAULT_FLAG_WRITE        0x01

#define VM_FAULT_OOM            0x0001
#define VM_FAULT_SIGBUS         0x0002
#define VM_FAULT_NOPAGE         0x0100  

extern void *high_memory;  /* used by drm_vma_info */

#define put_user(x, ptr)                                            \
    ({                                                              \
    __typeof__(*(ptr)) __x = (x);                                   \
    int __pu_err = -EFAULT;                                         \
    switch (sizeof (*(ptr)))                                        \
        {                                                           \
        case 1:                                                     \
        case 2:                                                     \
        case 4:                                                     \
        case 8:                                                     \
            __pu_err = __put_user_fn(sizeof (*(ptr)), ptr, &__x);   \
            break;                                                  \
        default:                                                    \
            pr_err("Bad size\n");                                   \
            break;                                                  \
        }                                                           \
    __pu_err;                                                       \
    })

#define get_user(x, ptr)                                            \
    ({                                                              \
    __typeof__(*(ptr)) __x;                                         \
    int __gu_err = -EFAULT;                                         \
    switch (sizeof(*(ptr)))                                         \
        {                                                           \
        case 1:                                                     \
        case 2:                                                     \
        case 4:                                                     \
        case 8:                                                     \
            {                                                       \
            __gu_err = __get_user_fn(sizeof (*(ptr)), ptr, &__x);   \
            (x) = *( __typeof__(*(ptr)) *) &__x;                    \
            break;                                                  \
            };                                                      \
        default:                                                    \
            pr_err("Bad size\n");                                   \
            break;                                                  \
        }                                                           \
    __gu_err;                                                       \
    })

#define mb()         VX_MEM_BARRIER_RW()
#define rmb()        VX_MEM_BARRIER_R()
#define wmb()        VX_MEM_BARRIER_W()
#define barrier()    VX_CODE_BARRIER()
/* taskDelay or else hw reset for HDMI */
#define mmiowb()     (void)taskDelay((100*sysClkRateGet())/1000);VX_MEM_BARRIER_W()
#define smp_mb()     mb()
#define smp_rmb()    rmb()
#define smp_wmb()    wmb()

/* Atomic operations are already serializing on x86 */
#define smp_mb__before_atomic()     barrier()
#define smp_mb__after_atomic()      barrier()

#define PROT_READ       0x01
#define PROT_WRITE      0x02
#define MAP_SHARED      0x001

typedef unsigned long pgprot_t;

#define pgprot_val(x)   (x)
#define vm_get_page_prot(flags) ((pgprot_t)0)
#define pgprot_noncached(prot) ((pgprot_t)0)
#define pgprot_writecombine(prot) ((pgprot_t)0)

#define PAGE_KERNEL   0

#define _PAGE_PRESENT 0
#define _PAGE_RW      0
#define _PAGE_PWT     0
#define _PAGE_PCD     0
#define _PAGE_PAT     0

struct address_space
    {
    unsigned long flags;
    void *private_data;
    };

struct page
    {
    VIRT_ADDR virtAddr;
    PHYS_ADDR physAddr;
    int index;
    int maxIndex;
    };

struct vm_fault
    {
    unsigned int flags;
    void *virtual_address;
    struct page *page;
    };

struct vm_area_struct;

struct mm_struct
    {
    struct vm_area_struct * mmap;
    struct rw_semaphore mmap_sem;
    atomic_t mm_count;
    };

struct vm_operations_struct
    {
    void (*open)(struct vm_area_struct * area);
    void (*close)(struct vm_area_struct * area);
    int (*fault)(struct vm_area_struct *vma, struct vm_fault *vmf);
    };

struct vm_area_struct
    {
    unsigned long vm_start;     /* Our start address within vm_mm. */
#if defined(__vxworks)
    unsigned long vm_start_phys;/* Corresponding physical address */
#endif /* __vxworks */
    unsigned long vm_end;       /* The first byte after our end address within vm_mm. */

    pgprot_t vm_page_prot;      /* Access permissions of this VMA. */
    unsigned long vm_flags;     /* Flags, see mm.h. */

    /* Function pointers to deal with this struct. */
    const struct vm_operations_struct *vm_ops;

    /* Information about our backing store: */
    unsigned long vm_pgoff;     /* Offset (within vm_file) in PAGE_SIZE units, *not* PAGE_CACHE_SIZE */
    struct file * vm_file;      /* File we map to (can be NULL). */

    void * vm_private_data;     /* was vm_pte (shared mem) */
    };

struct io_mapping
    {
    resource_size_t base;
    unsigned long size;
    pgprot_t prot;
    };

/* I/O memory mapping */

extern void *ioremap
    (
    unsigned long offset,
    unsigned long size
    );

extern void iounmap
    (
    void *addr
    );

#define ioremap_wc(offset, size) ioremap(offset, size)
#define acpi_os_ioremap(phys, size) ioremap(phys, size)
#define ioremap_nocache(offset, size) ioremap(offset, size)

extern struct io_mapping *io_mapping_create_wc
    (
    resource_size_t base,
    unsigned long size
    );
extern void io_mapping_free
    (
    struct io_mapping *mapping
    );

#define io_mapping_map_wc(mapping, offset)                                     \
    ioremap((mapping)->base + (offset), PAGE_SIZE)
#define io_mapping_unmap(addr) iounmap(addr)
#define io_mapping_map_atomic_wc(mapping, offset)                              \
    io_mapping_map_wc(mapping, offset)
#define io_mapping_unmap_atomic(addr) io_mapping_unmap(addr)

#define arch_phys_wc_add(base, size) (0)
#define arch_phys_wc_del(handle)
#define arch_phys_wc_index(handle) ({WARN_DEV; -1;})

#define devm_request_mem_region(p1, p2, p3, p4) ((struct resource *)((long)(p2)))

/* Generic memory allocation */

#define kmalloc(size, flags)                                                   \
    ({                                                                         \
    void *p = malloc (size);                                                   \
    if (!p) { pr_err ("malloc error %lx\n", (unsigned long)(size)); }                          \
    p;                                                                         \
    })

#define kcalloc(n, size, flags)                                                \
    ({                                                                         \
    void *p = calloc ((n), (size));                                            \
    if (!p) { pr_err ("calloc error %lx\n", (unsigned long)(size)); }                          \
    p;                                                                         \
    })

#define krealloc(src, size, flags)                                             \
    ({                                                                         \
    void *p = realloc ((src), (size));                                         \
    if (!p) { pr_err ("realloc error %lx\n", (unsigned long)(size)); }                         \
    p;                                                                         \
    })

#define kmemdup(src, size, flags)                                              \
    ({                                                                         \
    void *p = kmalloc ((size), GFP_KERNEL);                                    \
    if (p) { memcpy(p, (src), (size)); }                                       \
    p;                                                                         \
    })

#define kfree(p)                                                               \
    ({                                                                         \
    if (!p) { pr_err("NULL ptr\n"); }                                          \
    else { (void)free (p); }                                                   \
    })

#define kmalloc_array(n, size, flags) kcalloc(n, size, flags)
#define kzalloc(size, flags) kcalloc(1, size, flags)
#define devm_kzalloc(dev, size, flags) kcalloc(1, size, flags)
#define kvfree kfree
#define vfree kfree

static inline void drm_kfree
    (
    void *p
    )
    {
    (void)kfree (p);
    }

static void global_cache_flush
    (
    void
    )
    {
    }

/* Page memory allocation */

extern struct page *drm_alloc_npages
    (
    int mask,
    int numPages,
    int map,
    VIRT_ADDR virtAddr,
    PHYS_ADDR physAddr
    );

extern void drm_free_npages
    (
    struct page *page,
    int numPages
    );

extern int set_pages_uc
    (
    struct page *page,
    int numPages
    );

extern struct page *vmalloc_to_page
    (
    void *addr
    );

extern void *__vmalloc
    (
    unsigned long size,
    gfp_t mask,
    pgprot_t prot
    );

#define vmalloc_32(size) __vmalloc(size, GFP_KERNEL | __GFP_HIGHMEM | __GFP_ZERO, 0)
#define vzalloc(size) __vmalloc(size, GFP_KERNEL | __GFP_HIGHMEM | __GFP_ZERO, 0)

#define get_order(size) ilog2(size)
#define alloc_pages(mask, order) drm_alloc_npages(mask, 1 << order,            \
                    0, (VIRT_ADDR)NULL, (PHYS_ADDR)NULL)
#define alloc_page(mask) alloc_pages(mask, 0)
#define __free_pages(page, order) drm_free_npages(page, 1 << order)
#define __free_page(page) __free_pages(page, 0)

#define get_page(page)
#define put_page(page)

#define offset_in_page(page) (((unsigned long)(page)) % PAGE_SIZE)
#define virt_to_page(addr) vmalloc_to_page(addr)
#define virt_to_phys(addr) ({WARN_DEV; 0;})
#define pfn_to_page(pfn) (void *)((pfn) << PAGE_SHIFT)
#define is_vmalloc_addr(addr) (0)

static inline dma_addr_t page_to_phys
    (
    struct page *page
    )
    {
    if (!page)
        {
        WARN_DEV;
        return (dma_addr_t)0;
        }

    return (dma_addr_t)(page->physAddr);
    }

static inline unsigned long page_to_pfn
    (
    struct page *page
    )
    {
    if (!page)
        { 
        WARN_DEV;
        return 0;
        }

    return ((unsigned long)(page->physAddr)) >> PAGE_SHIFT;
    }

/* Assumes all pages are consecutive */
#define nth_page(page, _n) ((page) + (_n))

#define set_page_dirty(page) ({(page)=(page);})
#define mark_page_accessed(page)
#define page_cache_release(page)
#define release_pages(pages, nr, cold)
#define set_pages_wb(page, numpages)
#define ClearPageReserved(page)
#define SetPageReserved(page)

static inline void* kmap
    (
    struct page *page
    )
    {
    if (!page || !(page->virtAddr))
        {
        WARN_DEV;
        return NULL;
        }

    return (void*)(page->virtAddr);
    }

#define kmap_atomic(page) kmap(page)
#define kunmap(page)
#define kunmap_atomic(page) kunmap(page)

#define vmap(pages, count, flags, prot) kmap((pages)[0])
#define vunmap(pages)

#define unmap_mapping_range(mapping, holebegin, holelen, even_cows)

#define dma_map_page(dev, page, offset, size, dir) ({(dev)=(dev);page_to_phys(page);})
#define dma_unmap_page(dev, daddr, size, dir)
#define dma_mapping_error(dev, daddr) (0)

static inline unsigned long vma_pages
    (
    struct vm_area_struct *vma
    )
    {
    if (!vma)
        {
        WARN_DEV;
        return 0;
        }

    return (vma->vm_end - vma->vm_start) >> PAGE_SHIFT;
    }

#define find_vma(mm, addr) ({WARN_DEV; NULL;})

#define fault_in_multipages_readable(uaddr, size) (0)
#define fault_in_multipages_writeable(uaddr, size) (0)
#define pagefault_enable()
#define pagefault_disable()
#define pagefault_disabled() (0)
#define access_ok(type, addr, len) ({(len)=(len);1;})

#define __get_user_pages_fast(ptr, num_pages, write, pages) ({WARN_DEV; 0;})
#define get_user_pages(task, ptr, start, num_pages, write, force, pages, vm) ({WARN_DEV; (ptr)=(ptr); 0;})

#define remap_pfn_range(p1, p2, p3, p4, p5) ({WARN_DEV; 0;})
#define io_remap_pfn_range(p1, p2, p3, p4, p5) ({WARN_DEV; 0;})

/* Memcpy to/from IO */

static inline void memcpy_fromio
    (
    void *dst,
    const volatile void *src,
    size_t count
    )
    {
    memcpy(dst, (void *)src, count);
    }

static inline void memcpy_toio
    (
    volatile void *dst,
    const void *src,
    size_t count
    )
    {
    memcpy((void *)dst, src, count);
    };

static inline void memset_io
    (
    volatile void *addr,
    unsigned char val,
    size_t count
    )
    {
    memset((void *)addr, val, count);
    }

/* Copy from/to user space */

static inline unsigned int copy_to_user
    (
    void * to,
    const void * from,
    unsigned long n
    )
    {
    memcpy((char *)to, (char *)from, n);
    return (0);
    }

static inline unsigned int copy_from_user
    (
    void * to,
    const void * from,
    unsigned long n
    )
    {
    memcpy((char *)to, (char *)from, n);
    return (0);
    }

#define __copy_to_user copy_to_user
#define __copy_to_user_inatomic copy_to_user

#define __copy_from_user copy_from_user
#define __copy_from_user_inatomic copy_from_user
#define __copy_from_user_inatomic_nocache copy_from_user

/* Put/get user */

static inline int __put_user_fn
    (
    size_t size,
    void *ptr,
    void *x
    )
    {
    size = __copy_to_user(ptr, x, size);
    return (size) ? -EFAULT : size;
    }

static inline int __get_user_fn
    (
    size_t size,
    void *ptr,
    void *x
    )
    {
    size = copy_from_user(x, ptr, size);
    return (size) ? -EFAULT : size;
    }

static inline void *memchr_inv
    (
    const void *start,
    int c,
    size_t bytes
    )
    {
    unsigned char *ptr = (unsigned char *)start;
    while (ptr < (unsigned char *)start + bytes)
        {
        if (*ptr != c)
            return ptr;
        }
    return NULL;
    }

/* DMA buf */

enum dma_data_direction
    {
    DMA_BIDIRECTIONAL = 0,
    DMA_TO_DEVICE = 1,
    DMA_FROM_DEVICE = 2,
    DMA_NONE = 3,
    };

struct dma_buf;

struct dma_buf_attachment
    {
    struct dma_buf *dmabuf;
    struct device *dev;
    struct list_head node;
    void *priv;
    };

/**
 * struct dma_buf_ops - operations possible on struct dma_buf
 * @attach: [optional] allows different devices to 'attach' themselves to the
 *          given buffer. It might return -EBUSY to signal that backing storage
 *          is already allocated and incompatible with the requirements
 *          of requesting device.
 * @detach: [optional] detach a given device from this buffer.
 * @map_dma_buf: returns list of scatter pages allocated, increases usecount
 *               of the buffer. Requires atleast one attach to be called
 *               before. Returned sg list should already be mapped into
 *               _device_ address space. This call may sleep. May also return
 *               -EINTR. Should return -EINVAL if attach hasn't been called yet.
 * @unmap_dma_buf: decreases usecount of buffer, might deallocate scatter
 *                 pages.
 * @release: release this buffer; to be called after the last dma_buf_put.
 * @begin_cpu_access: [optional] called before cpu access to invalidate cpu
 *                    caches and allocate backing storage (if not yet done)
 *                    respectively pin the objet into memory.
 * @end_cpu_access: [optional] called after cpu access to flush caches.
 * @kmap_atomic: maps a page from the buffer into kernel address
 *               space, users may not block until the subsequent unmap call.
 *               This callback must not sleep.
 * @kunmap_atomic: [optional] unmaps a atomically mapped page from the buffer.
 *                 This Callback must not sleep.
 * @kmap: maps a page from the buffer into kernel address space.
 * @kunmap: [optional] unmaps a page from the buffer.
 * @mmap: used to expose the backing storage to userspace. Note that the
 *        mapping needs to be coherent - if the exporter doesn't directly
 *        support this, it needs to fake coherency by shooting down any ptes
 *        when transitioning away from the cpu domain.
 * @vmap: [optional] creates a virtual mapping for the buffer into kernel
 *        address space. Same restrictions as for vmap and friends apply.
 * @vunmap: [optional] unmaps a vmap from the buffer
 */
struct dma_buf_ops
    {
    int (*attach)(struct dma_buf *, struct device *,
                  struct dma_buf_attachment *);

    void (*detach)(struct dma_buf *, struct dma_buf_attachment *);

    /* For {map,unmap}_dma_buf below, any specific buffer attributes
     * required should get added to device_dma_parameters accessible
     * via dev->dma_params.
     */
    struct sg_table * (*map_dma_buf)(struct dma_buf_attachment *,
                                     enum dma_data_direction);
    void (*unmap_dma_buf)(struct dma_buf_attachment *,
                          struct sg_table *,
                          enum dma_data_direction);
    /* after final dma_buf_put() */
    void (*release)(struct dma_buf *);

    int (*begin_cpu_access)(struct dma_buf *, size_t, size_t,
                            enum dma_data_direction);
    void (*end_cpu_access)(struct dma_buf *, size_t, size_t,
                           enum dma_data_direction);
    void *(*kmap_atomic)(struct dma_buf *, unsigned long);
    void (*kunmap_atomic)(struct dma_buf *, unsigned long, void *);
    void *(*kmap)(struct dma_buf *, unsigned long);
    void (*kunmap)(struct dma_buf *, unsigned long, void *);

    int (*mmap)(struct dma_buf *, struct vm_area_struct *vma);

    void *(*vmap)(struct dma_buf *);
    void (*vunmap)(struct dma_buf *, void *vaddr);
    };

/**
 * struct dma_buf - shared buffer object
 * @size: size of the buffer
 * @file: file pointer used for sharing buffers across, and for refcounting.
 * @attachments: list of dma_buf_attachment that denotes all devices attached.
 * @ops: dma_buf_ops associated with this buffer object.
 * @exp_name: name of the exporter; useful for debugging.
 * @list_node: node for dma_buf accounting and debugging.
 * @priv: exporter specific private data for this buffer object.
 */
struct dma_buf
    {
    size_t size;
    struct file *file;
    struct list_head attachments;
    const struct dma_buf_ops *ops;
    /* mutex to serialize list manipulation, attach/detach and vmap/unmap */
    struct mutex lock;
    unsigned vmapping_counter;
    void *vmap_ptr;
    const char *exp_name;
    struct list_head list_node;
    void *priv;
    };

#if defined(GFX_USE_DRM_3_18)
#define dma_buf_export(p1, p2, p3, p4, p5) ({pr_err("%p\n", (p1));WARN_DEV; NULL;});
#else
#define dma_buf_export(p1) ({pr_err("%p\n", (p1));WARN_DEV; NULL;});
#endif
#define get_dma_buf(p1) ({WARN_DEV;})
#define dma_buf_get(p1) ({WARN_DEV; NULL;})
#define dma_buf_put(p1) ({WARN_DEV;p1=p1;});
#define dma_buf_attach(p1, p2) ({WARN_DEV; NULL;});
#define dma_buf_detach(p1, p2) ({WARN_DEV;})
#define dma_buf_fd(p1, p2) ({WARN_DEV;(-EINVAL);});
#define dma_buf_map_attachment(p1, p2) ({WARN_DEV; NULL;});
#define dma_buf_unmap_attachment(p1, p2, p3) ({WARN_DEV;})

struct dma_buf_export_info
    {
    const char *exp_name;
    struct module *owner;
    const struct dma_buf_ops *ops;
    size_t size;
    int flags;
    struct reservation_object *resv;
    void *priv;
    };

/**
 * helper macro for exporters; zeros and fills in most common values
 */
#define DEFINE_DMA_BUF_EXPORT_INFO(a)   \
        struct dma_buf_export_info a = { .exp_name = KBUILD_MODNAME, \
                                         .owner = THIS_MODULE }

/* Shrinker */

#define SHRINK_STOP (~0UL)
#define DEFAULT_SEEKS 2

struct shrink_control
    {
    unsigned long nr_to_scan;
    };

struct shrinker
    {
    unsigned long (*count_objects)(struct shrinker *,
                                   struct shrink_control *sc);
    unsigned long (*scan_objects)(struct shrinker *,
                                  struct shrink_control *sc);
    int seeks;      /* seeks to recreate an obj */
    };

extern void gfxDrmMmapInit
    (
    void
    );

#define fence_wait(p1, p2)
#define fence_put(p1)

#endif /* _VXOAL_MM_H */
