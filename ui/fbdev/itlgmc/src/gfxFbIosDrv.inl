/* gfxFbIosDrv.c - Frame buffer driver common code */

/*
 * Copyright (c) 2013-2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
22jan16,yat  Fix ios device cleanup (US73564)
12jan16,yat  Add notifyFbAddrChangeFuncPtr feature (US73564)
23nov15,yat  Fix static analysis defect (US71171)
01oct15,yat  Add GFX_FIXED_FBMODE_OFFSET (US67342)
15sep15,yat  Add support for GFX_USE_CURRENT_VIDEO_MODES (US66034)
15sep15,yat  Do not free firstVirtAddr for pmap virtAddr (US66034)
12jun15,yat  Add offset for xlnxlcvc (US58560)
22may15,jnl  Updated for VxBus GEN2. (US58560)
27mar15,qsn  Add a feature allowing use of VSYNC task instead of VSYNC IRQ (US54417)
10dec14,qsn  Add a feature allowing disabling video memory allocation (US49436)
14nov14,yat  Rework pmap of frame buffer for RTP restart (US46449)
28oct14,yat  Rework freeMemAllRtp for RTP restart (US46449)
01oct14,yat  Resolve defects from static analysis run
18sep14,yat  Remove invalid pmapPrivateUnmap from freeMemAllRtp (V7GFX-214)
30may14,yat  Change spinlockIsr to semaphore and enable lock (V7GFX-167)
22may14,yat  Move VxBus driver code to frame buffer common code
08may14,yat  Change vxbus variables from global to local
06sep13,mgc  Prepared for VxWorks 7 release
*/

/*
DESCRIPTION

The following definitions and routines represent common code for all fbdev
drivers.  The common elements are consolidated into source .inl files to
differentiate from device-specific code and to allow asynchronous and
independent updates to drivers.

Common functionality include memory handling and primary driver interface
routines.  When creating new drivers, developers are encouraged to refer to the
.inl code as a template on how to proceed.
*/

/* includes */

#include <vxWorks.h>
#include <iosLib.h>
#include <semLib.h>
#include <vmLib.h>
#include <private/rtpLibP.h>
#if defined(GFX_VXBUS_GEN2)
#include <hwif/vxBus.h>
#include <subsys/int/vxbIntLib.h>
#endif
#if CPU_FAMILY==ARM
#include <arch/arm/ivArm.h>
#endif
#if defined(GFX_USE_PMAP)
#include <pmapLib.h>
#endif
#include <ctype.h>
#include <string.h>

/* defines */

/* Display strings */
#define GFX_DISP_HDMI               "HDMI"
#define GFX_DISP_DP                 "DP"
#define GFX_DISP_DVI                "DVI"
#define GFX_DISP_VGA                "VGA"
#define GFX_DISP_LVDS_PANEL         "LVDS panel"
#define GFX_DISP_FLAT_PANEL         "Flat panel"

#if defined(GFX_USE_PMAP)
#ifndef GFX_PMAP_ATTR
#define GFX_PMAP_ATTR               (MMU_ATTR_VALID |         \
                                     MMU_ATTR_SUP_RW |        \
                                     MMU_ATTR_USR_RW |        \
                                     MMU_ATTR_CACHE_OFF)
#endif
#endif

#ifndef GFX_VM_STATE
#define GFX_VM_STATE                (VM_STATE_VALID |         \
                                     VM_STATE_WRITABLE |      \
                                     VM_STATE_CACHEABLE_NOT)
#endif

#ifndef GFX_VM_STATE_MASK
#define GFX_VM_STATE_MASK           (VM_STATE_MASK_VALID |    \
                                     VM_STATE_MASK_WRITABLE | \
                                     VM_STATE_MASK_CACHEABLE)
#endif

/* Page shortcuts */
#define GFX_PAGE_SIZE               vmPageSizeGet()
#define GFX_PAGE_MASK               (~(GFX_PAGE_SIZE - 1))
#define GFX_PAGE_ALIGN(_size)       (_size + GFX_PAGE_SIZE - 1) & GFX_PAGE_MASK;

/* Memory translate */
#define GFX_VM_TRANSLATE(_baseAddr1, _curAddr2, _baseAddr2)       \
                                    (void*)((ULONG)(_baseAddr1) + \
                                            (ULONG)(_curAddr2) -  \
                                            (ULONG)(_baseAddr2));

/* Locking/unlocking shortcuts */
#if defined(GFX_NO_LOCK)
#define LOCK_INIT(_dev)
#define LOCK(_dev)
#define UNLOCK(_dev)
#else
#define LOCK_TYPE                   VX_MUTEX_SEMAPHORE
#define LOCK_INIT(_dev)             (void) semMInitialize((_dev)->lock, \
                                        SEM_Q_PRIORITY |                \
                                        SEM_DELETE_SAFE |               \
                                        SEM_INVERSION_SAFE);
#define LOCK(_dev)                  (void) semTake((SEM_ID)((_dev)->lock), \
                                        WAIT_FOREVER)
#define UNLOCK(_dev)                (void) semGive((SEM_ID)((_dev)->lock))
#endif

/* RTP context check */
#if defined(GFX_USE_FLAT_MEM)
#define GFX_IS_RTP_CTX              FALSE
#else
#define GFX_IS_RTP_CTX              (MY_CTX_ID() != kernelId)
#endif

/* typedefs */

#if !defined(GFX_USE_PMAP)
/* Video memory for RTP */
typedef struct  _GFX_ADRMAP
    {
    RTP_ID                          rtpId;
    void*                           firstPhysAddr;
    void*                           firstVirtAddr;
    void*                           firstVirtAddrRtp;
    ULONG                           size;           /* bytes */
    struct _GFX_ADRMAP*             prev;
    struct _GFX_ADRMAP*             next;
    } GFX_ADRMAP;
#endif

/* Graphics component */
typedef struct
    {
    int                             init;
#if !defined(GFX_NO_LOCK)
    LOCK_TYPE                       (lock);
#endif
    /* Video memory */
    void*                           videoMemory;
#if !defined(GFX_USE_PMAP)
    GFX_ADRMAP*                     adrMapHead;
#endif
    /* Other */
    FUNCPTR                         consoleFuncPtr; /* console write function */
    FUNCPTR                         splashFuncPtr;  /* splash function */
#if defined(GFX_VXBUS_GEN2)
    VXB_DEV_ID                      vxbDev;
    VXB_RESOURCE                    *pResIrq;
#endif
    } GFX_COMP;

/* forward declarations */

#if defined(GFX_VXBUS_GEN2)
LOCAL STATUS fdtFbdevAttach (VXB_DEV_ID);
LOCAL STATUS fdtFbdevProbe (VXB_DEV_ID);
#endif

/* locals */

LOCAL GFX_COMP gfxComp = { FALSE };

/* Driver number assigned by the VxWorks I/O system */
LOCAL int drvNum = ERROR;

#if defined(GFX_VXBUS_GEN2)
/* Matching FDT entry for frame buffer device */
LOCAL VXB_FDT_DEV_MATCH_ENTRY fdtFbdevMatch[] =
    {
        {
        "fbdev",                /* compatible */
        NULL
        },

        {}                      /* Empty terminated list */
    };

/* VxBus methods for frame buffer device */
LOCAL VXB_DRV_METHOD fdtFbdevMethodList[] =
    {
    /* DEVICE API */
    { VXB_DEVMETHOD_CALL(vxbDevProbe),  fdtFbdevProbe  },
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtFbdevAttach },
    { 0, NULL }
    };
#endif

/* globals */

#if defined(GFX_VXBUS_GEN2)
/* VxBus driver for frame buffer */
VXB_DRV vxbFdtFbdevDrv =
    {
    { NULL } ,
    "fbdev",                            /* Name */
    "FBDEV FDT driver",                 /* Description */
    VXB_BUSID_FDT,                      /* Class */
    0,                                  /* Flags */
    0,                                  /* Reference count */
    &fdtFbdevMethodList[0]              /* Method table */
    };

VXB_DRV_DEF(vxbFdtFbdevDrv)
#endif

/*******************************************************************************
 *
 * MEMORY HANDLING ROUTINES
 *
 ******************************************************************************/

#if !defined(GFX_USE_PMAP)
/*******************************************************************************
 *
 * createAdrMap - create a memory mapping
 *
 * When an RTP application opens an fbdev file descriptor, the driver's
 * frame-buffer is mapped into that RTP's memory space.  The mapping is tracked
 * using a linked list.  The list keeps detailed information on the
 * frame-buffer's physical and virtual address within the kernel and within the
 * application's RTP memory space, this allows the fbdev driver to operate on
 * an RTP's frame-buffer mapping while taking into consideration asynchronous
 * operations from other driver connections.
 *
 * The routine is called using the frame-buffer's starting physical and virtual
 * kernel memory addresses, <physAddr> and <virtAddr> respectively.  The RTP's
 * virtual address, <virtAddrRtp> should be mapped to the start of the
 * frame-buffer's <physAddr> address. The <size> parameter represents the amount
 * of memory mapped in bytes.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: destroyAdrMap, findAdrMapPhys, allocMemRtp, freeMemRtp
 *
 */
LOCAL STATUS createAdrMap
    (
    void*           physAddr,
    void*           virtAddr,
    void*           virtAddrRtp,
    ULONG           size
    )
    {
    GFX_ADRMAP*     tmpMap;
    GFX_ADRMAP*     newMap;

    newMap = (GFX_ADRMAP*)malloc (sizeof (GFX_ADRMAP));
    if (newMap == NULL)
        {
        return ERROR;
        }

    newMap->rtpId = MY_CTX_ID ();
    newMap->firstPhysAddr = physAddr;
    newMap->firstVirtAddr = virtAddr;
    newMap->firstVirtAddrRtp = virtAddrRtp;
    newMap->size = size;
    newMap->prev = NULL;
    newMap->next = NULL;

    if (gfxComp.adrMapHead == NULL)
        {
        gfxComp.adrMapHead = newMap;
        }
    else
        {
        tmpMap = gfxComp.adrMapHead;
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
 * destoryAdrMap - delete a memory mapping
 *
 * Removes a specified RTP memory mapping, <adrMap>, from the linked list.  This
 * routine does not free any mapping resources specified in the list node.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: createAdrMap, findAdrMapPhys, allocMemRtp, freeMemRtp
 *
 */
LOCAL STATUS destroyAdrMap
    (
    GFX_ADRMAP*     adrMap
    )
    {
    if (adrMap == NULL)
        {
        return ERROR;
        }

    if (adrMap == gfxComp.adrMapHead)
        {
        gfxComp.adrMapHead = adrMap->next;
        }

    if (adrMap->prev)
        {
        adrMap->prev->next = adrMap->next;
        }

    if (adrMap->next)
        {
        adrMap->next->prev = adrMap->prev;
        }

    free (adrMap);

    return OK;
    }

/*******************************************************************************
 *
 * findAdrMapPhys - find a memory mapping based on physical address
 *
 * Searches the set of memory mappings to find if <physAddr> address is
 * contained within one of the list's nodes.  <physAddr> does not need to point
 * to the start of a mapping, it can fall in a node's range [firstPhysAddr,
 * firstPhysAddr+size].
 *
 * RETURNS: pointer to memory mapping on success, NULL otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: createAdrMap, destroyAdrMap, allocMemRtp, freeMemRtp
 *
 */
LOCAL GFX_ADRMAP* findAdrMapPhys
    (
    void*           physAddr
    )
    {
    GFX_ADRMAP*     tmpMap;
    ULONG           addr = (ULONG)physAddr;
    ULONG           firstAddr;

    tmpMap = gfxComp.adrMapHead;
    while (tmpMap)
        {
        firstAddr = (ULONG)(tmpMap->firstPhysAddr);
        /*
         * Find a node with the same context and a
         * mapping range to which physAddr belongs
         */
        if ((tmpMap->rtpId == MY_CTX_ID()) &&
            (firstAddr <= addr) &&
            (addr < (firstAddr + tmpMap->size)))
            {
            return tmpMap;
            }
        tmpMap = tmpMap->next;
        }

    return NULL;
    }

/*******************************************************************************
 *
 * findAdrMapVirtRtp - find a memory mapping based on virtual address
 *
 * Searches the set of memory mappings to find if <virtAddrRtp> address is
 * contained within one of the list's nodes.  <virtAddrRtp> does not need to point
 * to the start of a mapping, it can fall in a node's range [firstPhysAddr,
 * firstPhysAddr+size].
 *
 * RETURNS: pointer to memory mapping on success, NULL otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: createAdrMap, destroyAdrMap, allocMemRtp, freeMemRtp
 *
 */
LOCAL GFX_ADRMAP* findAdrMapVirtRtp
    (
    void*           virtAddrRtp
    )
    {
    GFX_ADRMAP*     tmpMap;
    ULONG           addr = (ULONG)virtAddrRtp;
    ULONG           firstAddr;

    tmpMap = gfxComp.adrMapHead;
    while (tmpMap)
        {
        firstAddr = (ULONG)(tmpMap->firstVirtAddrRtp);
        /*
         * Find a node with the same context and a
         * mapping range to which physAddr belongs
         */
        if ((tmpMap->rtpId == MY_CTX_ID()) &&
            (firstAddr <= addr) &&
            (addr < (firstAddr + tmpMap->size)))
            {
            return tmpMap;
            }
        tmpMap = tmpMap->next;
        }

    return NULL;
    }

/*******************************************************************************
 *
 * allocMemRtp - allocate memory for RTP
 *
 * Allocates memory within an RTP's memory space and maps it to the
 * frame-buffer.  The amount of memory alloated and mapped is a function of the
 * frame-buffer configuration and the number of buffers desired.  The mapping is
 * performed via the createAdrMap() routine.  The routine makes use of the fbdev
 * device pointer, <pDev> to perform the allocation.  The routine operates on
 * the RTP context return by the macro MY_CTX_ID().
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: createAdrMap, destroyAdrMap, findAdrMapPhys, freeMemRtp
 *
 */
LOCAL STATUS allocMemRtp
    (
    GFX_FBDEV*      pDev
    )
    {
    PHYS_ADDR       physAddr;
    VIRT_ADDR       virtAddr;

    physAddr = (PHYS_ADDR)(pDev->firstPhysAddr);

    /*
     * Allocate virtual memory into the RTP's memory space and map it to the
     * frame-buffer's physical address
     */
    virtAddr = adrSpacePageAlloc (
                                  RTP_VM_CONTEXT_GET ((RTP_ID)MY_CTX_ID()),
                                  0, /* virt addr */
                                  physAddr, /* phys addr */
                                  pDev->fbSize / GFX_PAGE_SIZE,
                                  ADR_SPACE_OPT_CONTIG | ADR_SPACE_OPT_MAPPED |
                                  ADR_SPACE_OPT_NOPHYSALLOC | MMU_ATTR_SUP_RW |
                                  MMU_ATTR_USR_RW | VM_STATE_CACHEABLE_NOT);
    if (virtAddr == (VIRT_ADDR)NONE)
        {
        (void)fprintf (stderr, "adrSpacePageAlloc failed\n");
        errno = ENOMEM;
        return ERROR;
        }

    if (ERROR == createAdrMap (
                     pDev->firstPhysAddr, pDev->firstVirtAddr,
                     (void*)virtAddr, pDev->fbSize))
        {
        errno = ENOMEM;
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
 *
 * freeMemRtp - free memory for RTP
 *
 * Frees memory within an RTP's memory space and clears any mapping associated
 * to it using the destroyAdrMap() routine.  The routine makes use of the fbdev
 * device pointer, <pDev> to perform the de-allocation.  The routine operates on
 * the RTP context return by the macro MY_CTX_ID().
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: createAdrMap, destroyAdrMap, findAdrMapPhys, allocMemRtp
 *
 */
LOCAL void freeMemRtp
    (
    GFX_FBDEV*      pDev,
    int             physFree
    )
    {
    GFX_ADRMAP*     adrMap;
    VIRT_ADDR       virtAddr;

    /*
     * Find a mapping containing the frame-buffers physical address and
     * belonging to the current RTP context
     */
    adrMap = findAdrMapPhys (pDev->firstPhysAddr);
    if (adrMap)
        {
        virtAddr = (VIRT_ADDR)(adrMap->firstVirtAddrRtp);
        if (ERROR == adrSpacePageFree (NULL,
                          virtAddr,
                          pDev->fbSize / GFX_PAGE_SIZE,
                          physFree?
                          ADR_SPACE_OPT_PHYS_FREE:
                          ADR_SPACE_OPT_PHYS_NOFREE))
            {
            (void)fprintf (stderr, "adrSpacePageFree failed\n");
            }
        (void)destroyAdrMap (adrMap);
        }
    }

/*******************************************************************************
 *
 * freeMemAllRtp - free memory for all RTPs
 *
 * Frees memory of all RTP's memory space and clears any mapping associated
 * to it using the destroyAdrMap() routine.  The routine makes use of the fbdev
 * device pointer, <pDev> to perform the de-allocation.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: createAdrMap, destroyAdrMap, findAdrMapPhys, allocMemRtp
 *
 */
LOCAL void freeMemAllRtp
    (
    GFX_FBDEV*      pDev,
    int             physFree
    )
    {
    GFX_ADRMAP*     adrMap;
    VIRT_ADDR       virtAddr;

    for (;;) {
        adrMap = gfxComp.adrMapHead;
        if (adrMap == NULL)
            {
            return;
            }

        virtAddr = (VIRT_ADDR)(adrMap->firstVirtAddrRtp);
        if (ERROR == adrSpacePageFree (NULL,
                          virtAddr,
                          pDev->fbSize / GFX_PAGE_SIZE,
                          physFree?
                          ADR_SPACE_OPT_PHYS_FREE:
                          ADR_SPACE_OPT_PHYS_NOFREE))
            {
            (void)fprintf (stderr, "adrSpacePageFree failed\n");
            }
        (void)destroyAdrMap (adrMap);
        }
    }
#endif

#if !defined(GFX_HWDRV_ALLOC_VIDMEM)
/*******************************************************************************
 *
 * createMem - create a memory partition
 *
 * Allocates a block of memory and creates a memory partition.  The allocation
 * size is based on the parameter FBDEV_MEMORY_SIZE specified in the VxWorks
 * Image Project and cannot be changed at runtime.  The allocation is used to
 * create a memory partition from which the driver allocates the
 * frame-buffer(s).
 *
 * The routine does not take any parameters, but instead refers to the global
 * gfxComp structure which holds all of the VxWorks driver-independent
 * configuration parameters.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: freeMem, allocMem, reallocMem
 *
 */
LOCAL STATUS createMem
    (
    GFX_FBDEV*      pDev
    )
    {
    VIRT_ADDR       virtAddr;
    PHYS_ADDR       physAddr;

#if defined(GFX_FIXED_VIDMEM_PHYS_ADDR)
#if defined(GFX_USE_PMAP)
    virtAddr = (VIRT_ADDR)pmapGlobalMap (GFX_FIXED_VIDMEM_PHYS_ADDR,
                              pDev->fbSize, GFX_PMAP_ATTR);
    if (virtAddr == (VIRT_ADDR)PMAP_FAILED)
        {
        (void)fprintf (stderr, "pmapGlobalMap failed\n");
        return ERROR;
        }
#else
    size_t vmPgSizeLocal = vmPageSizeGet();
    if (ERROR == pgPoolVirtRemoveFromPool (userRgnPoolId, 
                                           (VIRT_ADDR)GFX_FIXED_BUF_MEM_REMOVE_ADDR,
                                           GFX_FIXED_BUF_MEM_REMOVE_SIZE / 
                                           vmPgSizeLocal))
        {
        return ERROR;
        }
    if (ERROR == pgPoolVirtRemoveFromPool (globalRAMPgPoolId, 
                                           (VIRT_ADDR)GFX_FIXED_BUF_MEM_REMOVE_ADDR,
                                           GFX_FIXED_BUF_MEM_REMOVE_SIZE / 
                                           vmPgSizeLocal))
        {
        return ERROR;
        }

    if (ERROR == pgPoolVirtAddToPool (kernelVirtPgPoolId, GFX_FIXED_BUF_MEM_REMOVE_ADDR, 
                         GFX_FIXED_BUF_MEM_REMOVE_SIZE / vmPgSizeLocal, PAGES_FREE))
        {
        return ERROR;
        }
    (void)pgPoolVirtAllocAt (kernelVirtPgPoolId, GFX_FIXED_BUF_MEM_REMOVE_ADDR, 
                             GFX_FIXED_BUF_MEM_REMOVE_SIZE / vmPgSizeLocal);
    if (ERROR == vmPageMap (0,
                            GFX_FIXED_VIDMEM_PHYS_ADDR, /* virt addr */
                            GFX_FIXED_VIDMEM_PHYS_ADDR, /* phys addr */
                            pDev->fbSize,
                            (MMU_ATTR_VALID_MSK | MMU_ATTR_PROT_MSK | 
                             MMU_ATTR_CACHE_MSK),
                            (MMU_ATTR_VALID | MMU_ATTR_SUP_RWX | 
                             MMU_ATTR_CACHE_GUARDED | MMU_ATTR_CACHE_OFF | 
                             MMU_ATTR_CACHE_COHERENCY)))
        {
        return ERROR;
        }
    virtAddr = GFX_FIXED_VIDMEM_PHYS_ADDR;

#endif
    if (ERROR == vmTranslate (NULL, virtAddr, &physAddr))
        {
        (void)fprintf (stderr, "vmTranslate failed\n");
        return ERROR;
        }
#else
    /*
     * Allocate page-alligned memory based on the video memory size specified
     * at build time
     */
    gfxComp.videoMemory = (void*)memalign (GFX_PAGE_SIZE, pDev->fbSize);
    if (gfxComp.videoMemory == NULL)
        {
        (void)fprintf (stderr, "Unable to allocate memory partition\n");
        return ERROR;
        }
    virtAddr = (VIRT_ADDR)gfxComp.videoMemory;
    /*
     * The memory should be writeable and non-cached, this is important for
     * supporting frame-buffer operations.
     */
    if (ERROR == vmStateSet (NULL, virtAddr, pDev->fbSize,
                             GFX_VM_STATE_MASK, GFX_VM_STATE))
        {
        free (gfxComp.videoMemory);
        gfxComp.videoMemory = NULL;
        (void)fprintf (stderr, "vmStateSet error\n");
        return ERROR;
        }
    if (ERROR == vmStateSet (NULL, virtAddr, pDev->fbSize,
                             VM_STATE_MASK_CACHEABLE, VM_STATE_CACHEABLE_NOT))
        {
        free (gfxComp.videoMemory);
        gfxComp.videoMemory = NULL;
        (void)fprintf (stderr, "vmStateSet error\n");
        return ERROR;
        }
    if (ERROR == vmTranslate (NULL, virtAddr, &physAddr))
        {
        free (gfxComp.videoMemory);
        gfxComp.videoMemory = NULL;
        (void)fprintf (stderr, "vmTranslate failed\n");
        return ERROR;
        }
#if defined(GFX_USE_PMAP)
    virtAddr = (VIRT_ADDR)pmapGlobalMap (physAddr, pDev->fbSize, GFX_PMAP_ATTR);
    if (virtAddr == (VIRT_ADDR)PMAP_FAILED)
        {
        free (gfxComp.videoMemory);
        gfxComp.videoMemory = NULL;
        (void)fprintf (stderr, "pmapGlobalMap failed\n");
        return ERROR;
        }
#endif
#endif
    pDev->firstVirtAddr = (void*)virtAddr;
    pDev->firstPhysAddr = (void*)((VIRT_ADDR)physAddr);

    return OK;
    }
#endif

/*******************************************************************************
 *
 * VIDEO MODE HANDLING ROUTINES
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * getFbMode - convert the mode string to binary
 *
 * Parses a video mode string into a FB_VIDEO_MODE structure.  The <modeStr>
 * parameter must be a valid video mode string in one of the following formats:
 *
 *   'width'x'height'-'bpp'@'refresh'
 *   'width'x'height'-'bpp'
 *   'width'x'height'@'refresh'
 *   'width'x'height'
 *
 *   where 'width'x'height'  is the display resolution,
 *         'bpp'             is the number of bits per pixel, and
 *         'refresh'         is the display refresh rate
 *
 * The routine must be called with <pDev>, the fbdev device structuring
 * containing a list of all valid modes, and <pFbMode>, a pointer to the
 * resulting video mode data.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: setVideoMode
 *
 */
LOCAL STATUS getFbMode
    (
    GFX_FBDEV*      pDev,
    const char*     modeStr,
    FB_VIDEO_MODE*  pFbMode
    )
    {
    ULONG           n, i;

    if (strcmp (modeStr, FB_DEFAULT_VIDEO_MODE) == 0)
        {
        modeStr = pDev->fbModesDb[0].name;
        }

    if (sscanf (modeStr, "%dx%d-%d@%d",
        &(pFbMode->xres), &(pFbMode->yres),
        &(pFbMode->bpp), &(pFbMode->refresh)) == 4)
        {
        n = strchr(modeStr, '-') - modeStr;
        }
    else if (sscanf (modeStr, "%dx%d-%d",
        &(pFbMode->xres), &(pFbMode->yres),
        &(pFbMode->bpp)) == 3)
        {
        n = strchr(modeStr, '-') - modeStr;
        pFbMode->refresh = 0;
        }
    else if (sscanf (modeStr, "%dx%d@%d",
        &(pFbMode->xres), &(pFbMode->yres),
        &(pFbMode->refresh)) == 3)
        {
        n = strchr(modeStr, '@') - modeStr;
        pFbMode->bpp = 32;
        }
    else if (sscanf (modeStr, "%dx%d",
        &(pFbMode->xres), &(pFbMode->yres)) == 2)
        {
        n = strlen(modeStr);
        pFbMode->bpp = 32;
        pFbMode->refresh = 0;
        }
    else return ERROR;

    /* Search the list of supported video modes for the right one */
    for (i = 0; i < pDev->fbModesCount; i++)
        {
        if (strncmp(pDev->fbModesDb[i].name, modeStr, n) == 0)
            {
            if (
                ((pFbMode->bpp == 0) ||
                 (pDev->fbModesDb[i].bpp == pFbMode->bpp)) &&
                ((pFbMode->refresh == 0) ||
                 (pDev->fbModesDb[i].refresh == pFbMode->refresh))
                )
                {
                bcopy ((void*)&(pDev->fbModesDb[i]), (void*)pFbMode,
                       sizeof (FB_VIDEO_MODE));
                return OK;
                }
            }
        }

    return ERROR;
    }

/*******************************************************************************
 *
 * setVideoMode - set video mode
 *
 * This routine attempts to set a display controller's video mode.  It requires
 * the following parameters:
 *    <pDev> - pointer to a valid fbdev device structure
 *    <modeStr> - the desire video mode string
 *
 * The routine does not implement any of the device specific functionality,
 * instead it relies on the setVideoModeExFuncPtr() function pointer of <pDev>
 * to perform these operations.  The routine will take care of all the necessary
 * device-independent functionality such as parsing the video mode string,
 * allocating the frame-buffer, rendering the splash screen, etc.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: getFbMode
 *
 */
LOCAL STATUS setVideoMode
    (
    GFX_FBDEV*      pDev,
    const char*     modeStr
    )
    {
    FB_VIDEO_MODE   fbMode;

    if (!pDev->setVideoModeExFuncPtr)
        {
        errno = EINVAL;
        return ERROR;
        }

    bzero ((void*)&fbMode, sizeof (FB_VIDEO_MODE));

    if (ERROR == getFbMode (pDev, modeStr, &fbMode))
        {
        errno = EINVAL;
        return ERROR;
        }
 
    /* Do not set video mode for the same video mode */
    if (strncmp (pDev->fbMode.name, fbMode.name, FB_MAX_STR_CHARS-1) == 0)
        {
        return OK;
        }

    fbMode.vsync = pDev->fbMode.vsync;
    fbMode.buffers = pDev->fbMode.buffers;

    pDev->frontVirtAddr = (void*)pDev->firstVirtAddr;
    pDev->frontPhysAddr = (void*)pDev->firstPhysAddr;

    /* Set the video mode */
    if (OK != (pDev->setVideoModeExFuncPtr) (pDev, &fbMode))
        {
        errno = EINVAL;
        return ERROR;
        }

    bcopy ((void*)&fbMode, (void*)&(pDev->fbMode), sizeof (FB_VIDEO_MODE));

    /*
     * If enabled, render the splash screen onto all frame-buffers
     */
    if (gfxComp.splashFuncPtr)
        {
        int i;
        void* pBuf = pDev->firstVirtAddr;
        for (i = 0; i < pDev->fbMode.buffers; i++)
            {
            (void)(gfxComp.splashFuncPtr) (pBuf, pDev->fbMode.xres,
                pDev->fbMode.yres, pDev->fbMode.bpp, pDev->fbMode.stride);
#if defined(GFX_FIXED_FBMODE_OFFSET)
            pBuf = (void*)((char*)pBuf + pDev->fbMode.offset);
#else
            pBuf = (void*)((char*)pBuf + pDev->fbMode.yres * pDev->fbMode.stride);
#endif
            }
        }

    return OK;
    }

/*******************************************************************************
 *
 * SHOW ROUTINES
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * show - print information about the display device and its driver
 *
 * Prints out information about the driver to stdout.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * SEE ALSO: devIoctl
 *
 */
LOCAL void show
    (
    GFX_FBDEV*      pDev,
    ULONG           verboseLevel
    )
    {
    (void)printf ("Driver build date: %s\n", __DATE__);

    if (verboseLevel > 0)
        {
        (void)printf ("\tinit           = %d\n", gfxComp.init);
        (void)printf ("\tpDev           = %8p\n", pDev);
        (void)printf ("\tdrvNum         = %d\n", drvNum);
        (void)printf ("\tenabled        = %d\n", pDev->enabled);
        (void)printf ("\tdisplay        = %d\n", pDev->disp);
        (void)printf ("\tvideo mode     = %dx%d-%d@%d\n",
                                                 pDev->fbMode.xres,
                                                 pDev->fbMode.yres,
                                                 pDev->fbMode.bpp,
                                                 pDev->fbMode.refresh);
        (void)printf ("\tvsync          = %d\n", pDev->fbMode.vsync);
        (void)printf ("\tbuffers        = %d\n", pDev->fbMode.buffers);
        (void)printf ("\tsize           = %ld\n", pDev->fbSize);
        (void)printf ("\tfirstPhysAddr  = %8p\n", pDev->firstPhysAddr);
        (void)printf ("\tfirstVirtAddr  = %8p\n", pDev->firstVirtAddr);
        (void)printf ("\tfrontPhysAddr  = %8p\n", pDev->frontPhysAddr);
        (void)printf ("\tfrontVirtAddr  = %8p\n", pDev->frontVirtAddr);
        (void)printf ("\tdeviceName     = %s\n", pDev->deviceName);
        }
    }

#if defined(GFX_VXBUS_GEN2)
/*******************************************************************************
 *
 * VXBUS DRIVER ROUTINES
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * fdtFbdevProbe - probe for frame buffer device presence
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fdtFbdevAttach
 *
 */
LOCAL STATUS fdtFbdevProbe
    (
    VXB_DEV_ID      vxbDev
    )
    {
    return vxbFdtDevMatch (vxbDev, &fdtFbdevMatch[0], NULL);
    }

/*******************************************************************************
 *
 * fdtFbdevAttach - attach frame buffer device
 *
 * This initializes the global structure and associates with the VxBus driver.
 * The frame buffer initialization is not performed here but later as part
 * of the post kernel initialization as other initializations need to be
 * performed first before the frame buffer iniitialization.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * ERRNO: N/A
 *
 * SEE ALSO: fdtFbdevProbe
 *
 */
LOCAL STATUS fdtFbdevAttach
    (
    VXB_DEV_ID      vxbDev
    )
    {
    if (gfxComp.init == FALSE)
        {
        bzero ((void*)&gfxComp, sizeof (gfxComp));

        gfxComp.init = TRUE;

        LOCK_INIT(&gfxComp);

        gfxComp.vxbDev = vxbDev;
        }

    return OK;
    }
#endif

/*******************************************************************************
 *
 * DRIVER CORE ROUTINES (open, close, ioctl, write)
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * devOpen - open the display device
 *
 * Opens a new connection to the display device.  This routine is called
 * whenever an application calls open() on an fbdev file descriptor.  At this
 * point the driver will have been entirely initialized, it is not necessary to
 * set the video mode or perform any frame-buffer allocations.  The exception is
 * if this routine is being called from an RTP context, the frame-buffer will be
 * allocated and mapped into the RTP's memory space.
 *
 * RETURNS: Pointer to fbdev device, -1 if an error occurs
 *
 * SEE ALSO: devClose, devIoctl, devWrite
 *
 */
LOCAL void* devOpen
    (
    DEV_HDR *       pDevHdr,
    const char *    fileName,
    int             flags,
    int             mode
    )
    {
    GFX_FBDEV*  pDev = (GFX_FBDEV*)pDevHdr;

    /* Check arguments */
    if (pDev == NULL)
        {
        errno = EFAULT;
        return (void*)ERROR;
        }

#if !defined(GFX_USE_PMAP)
    if (GFX_IS_RTP_CTX)
        {
        if (pDev->fbRtpFreeMem)
            {
#if defined(GFX_USE_RTP_PHYS_FREE)
            freeMemAllRtp (pDev, TRUE);
#else
            freeMemAllRtp (pDev, FALSE);
#endif
            }
        if (ERROR == allocMemRtp (pDev))
            return (void*)ERROR;
        }
#endif
#if defined(GFX_USE_DEV_OPEN_CLOSE)
    if (pDev->openFuncPtr)
        {
        if (OK != (pDev->openFuncPtr) (pDev))
            {
            errno = EFAULT;
            return (void*)ERROR;
            }
        }
#endif
    errno = 0;
    return (void*)pDev;
    }

/*******************************************************************************
 *
 * devClose - close the display device
 *
 * Closes a connection to the display device.  This routine is called whenever
 * an application calls close() on an fbdev file descriptor.  The driver will
 * not de-initialize or de-allocate any display device resources.  The exception
 * is if an RTP this routine is being called from RTP context, in which case the
 * frame-buffer will be freed and unmapped from within its context.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * SEE ALSO: devOpen, devIoctl, devWrite
 *
 */
LOCAL int devClose
    (
    void*      pFileDesc
    )
    {
    GFX_FBDEV*  pDev = (GFX_FBDEV*)pFileDesc;
    errno = 0;

    /* Check arguments */
    if (pDev == NULL)
        {
        errno = EFAULT;
        return ERROR;
        }
#if defined(GFX_USE_DEV_OPEN_CLOSE)
    if (pDev->closeFuncPtr)
        {
        if (OK != (pDev->closeFuncPtr) (pDev))
            {
            errno = EFAULT;
            }
        }
#endif
#if !defined(GFX_USE_PMAP)
    if (GFX_IS_RTP_CTX)
        {
#if defined(GFX_USE_RTP_PHYS_FREE)
        freeMemRtp (pDev, TRUE);
#else
        freeMemRtp (pDev, FALSE);
#endif
        }
#endif
    if (errno != 0)
        {
        return ERROR;
        }
    return OK;
    }

/*******************************************************************************
 *
 * devIoctl - perform driver specific operations
 *
 * Performs driver specific operations on a display device.  This routine is
 * called whenever an application calls ioctl() on an fbdev file descriptor.
 * For complete descriptions of each IOCTL operations see fbdev.h.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * SEE ALSO: devOpen, devClose, devWrite
 *
 */
LOCAL int devIoctl
    (
    void*           pFileDesc,
    int             function,
    _Vx_ioctl_arg_t arg
    )
    {
    ULONG           i;
    FB_IOCTL_ARG*   pArg = (FB_IOCTL_ARG*)arg;
    int             rc = ERROR;
    GFX_FBDEV*      pDev = (GFX_FBDEV*)pFileDesc;

    /* Check arguments */
    if (pDev == NULL)
        {
        errno = EFAULT;
        return ERROR;
        }

    errno = 0;

    LOCK (&gfxComp);

    switch (function)
        {
        /* REQUIRED - Return the current video mode as a string */
        case FB_IOCTL_GET_VIDEO_MODE:
            if ((pArg == NULL) || (pArg->getVideoMode.pBuf == NULL))
                {
                errno = EINVAL;
                }
            else if (!pDev->enabled)
                {
                pArg->getVideoMode.pBuf = NULL;
                errno = EAGAIN;
                }
            else
                {
                (void)snprintf (pArg->getVideoMode.pBuf, FB_MAX_VIDEO_MODE_LEN,
                                        "%dx%d-%d@%d",
                                        pDev->fbMode.xres,
                                        pDev->fbMode.yres,
                                        pDev->fbMode.bpp,
                                        pDev->fbMode.refresh);
                rc = OK;
                }
            break;

        /* REQUIRED - Set video mode from internal database */
        case FB_IOCTL_SET_VIDEO_MODE:
            if ((pArg == NULL) || (pArg->setVideoMode.videoMode == NULL))
                {
                errno = EINVAL;
                }
            else
                {
                /*
                 * This routine will eventually call into
                 * device-specific functionality
                 */
                if (ERROR == setVideoMode (pDev, pArg->setVideoMode.videoMode))
                    {
                    errno = EINVAL;
                    }
                else
                    {
                    rc = OK;
                    }
                }
            break;

        /* REQUIRED - Return information about the frame buffer */
        case FB_IOCTL_GET_FB_INFO:
            if (pArg == NULL)
                {
                bzero ((void*)&(pArg->getFbInfo), sizeof (FB_INFO));
                errno = EINVAL;
                }
            else if (!pDev->enabled)
                {
                bzero ((void*)&(pArg->getFbInfo), sizeof (FB_INFO));
                errno = EAGAIN;
                }
            else
                {
                /*
                 * The primary purpose of this IOCTL is to provide a valid
                 * pointer to the current frame-buffer being displayed.
                 *
                 * This operation is straight-forward for kernel application,
                 * but requires additional computation to support RTPs.
                 */
                pArg->getFbInfo.pFirstFbPhys = pDev->firstPhysAddr;
#if defined(GFX_USE_PMAP)
                pArg->getFbInfo.pFirstFb = pDev->firstVirtAddr;
#else
                if (!GFX_IS_RTP_CTX)
                    {
                    pArg->getFbInfo.pFirstFb = pDev->firstVirtAddr;
                    }
                else
                    {
                    /*
                     * If calling from RTP context, search the address mappings
                     * to find the RTP's starting address of the frame-buffer(s)
                     */
                    GFX_ADRMAP* adrMap = findAdrMapPhys (pDev->firstPhysAddr);
                    if (adrMap == NULL)
                        {
                        errno = EINVAL;
                        break;
                        }
                    pArg->getFbInfo.pFirstFb = adrMap->firstVirtAddrRtp;
                    }
#endif
                pArg->getFbInfo.pFbPhys = pDev->frontPhysAddr;
#if defined(GFX_USE_PMAP)
                pArg->getFbInfo.pFb = pDev->frontVirtAddr;
#else
                if (!GFX_IS_RTP_CTX)
                    {
                    pArg->getFbInfo.pFb = pDev->frontVirtAddr;
                    }
                else
                    {
                    /*
                     * If calling from RTP context, search the address mappings
                     * to find in what address range the onscreen buffer is
                     * located (the mapping is likely to be the same as above
                     * since all frame-buffers are allocated in the same
                     * partition by default).
                     */
                    GFX_ADRMAP* adrMap = findAdrMapPhys (pDev->frontPhysAddr);
                    if (adrMap == NULL)
                        {
                        errno = EINVAL;
                        break;
                        }
                    /*
                     * The RTP's oncreen frame-buffer address is calculated
                     * using the offset between the kernel's onscreen and start
                     * frame-buffer address
                     */
                    pArg->getFbInfo.pFb = GFX_VM_TRANSLATE (
                                              adrMap->firstVirtAddrRtp,
                                              pDev->frontVirtAddr,
                                              pDev->firstVirtAddr);
                    }
#endif
                pArg->getFbInfo.bpp = pDev->fbMode.bpp;
                pArg->getFbInfo.width = pDev->fbMode.xres;
                pArg->getFbInfo.stride = pDev->fbMode.stride;
                pArg->getFbInfo.height = pDev->fbMode.yres;
#if defined(GFX_FIXED_FBMODE_OFFSET)
                pArg->getFbInfo.offset = pDev->fbMode.offset;
#endif
                pArg->getFbInfo.vsync = pDev->fbMode.vsync;
                pArg->getFbInfo.buffers = pDev->fbMode.buffers;
                if (pDev->fbMode.bpp == 16)
                    {
                    pArg->getFbInfo.pixelFormat.flags = FB_PIX_NATIVE;
                    pArg->getFbInfo.pixelFormat.alphaBits = 0;
                    pArg->getFbInfo.pixelFormat.redBits = 5;
                    pArg->getFbInfo.pixelFormat.greenBits = 6;
                    pArg->getFbInfo.pixelFormat.blueBits = 5;
                    }
                else if (pDev->fbMode.bpp == 32)
                    {
                    pArg->getFbInfo.pixelFormat.flags = FB_PIX_NATIVE;
                    pArg->getFbInfo.pixelFormat.alphaBits = 8;
                    pArg->getFbInfo.pixelFormat.redBits = 8;
                    pArg->getFbInfo.pixelFormat.greenBits = 8;
                    pArg->getFbInfo.pixelFormat.blueBits = 8;
                    }

                rc = OK;
                }
            break;

        /* REQUIRED - Return the supported surface configurations */
        case FB_IOCTL_GET_CONFIGS:
            if ((pArg == NULL) || (pArg->getConfigs.pConfigs == NULL))
                {
                errno = EINVAL;
                }
            else if (!pDev->enabled)
                {
                errno = EAGAIN;
                }
            else
                {
                /* Supported configurations */
                if (pDev->fbMode.bpp == 16)
                    {
                    pArg->getConfigs.pConfigs[0].id = 1;
                    pArg->getConfigs.pConfigs[0].pixelFormat.flags = FB_PIX_NATIVE;
                    pArg->getConfigs.pConfigs[0].pixelFormat.alphaBits = 0;
                    pArg->getConfigs.pConfigs[0].pixelFormat.redBits = 5;
                    pArg->getConfigs.pConfigs[0].pixelFormat.greenBits = 6;
                    pArg->getConfigs.pConfigs[0].pixelFormat.blueBits = 5;
                    }
                else if (pDev->fbMode.bpp == 32)
                    {
                    pArg->getConfigs.pConfigs[0].id = 1;
                    pArg->getConfigs.pConfigs[0].pixelFormat.flags = FB_PIX_NATIVE;
                    pArg->getConfigs.pConfigs[0].pixelFormat.alphaBits = 8;
                    pArg->getConfigs.pConfigs[0].pixelFormat.redBits = 8;
                    pArg->getConfigs.pConfigs[0].pixelFormat.greenBits = 8;
                    pArg->getConfigs.pConfigs[0].pixelFormat.blueBits = 8;
                    }
                for (i = 1; i < FB_MAX_CONFIGS; i++)
                    {
                    pArg->getConfigs.pConfigs[i].id = 0;
                    }
                rc = OK;
                }
            break;

        /* REQUIRED - Set the frame buffer address */
        case FB_IOCTL_SET_FB:
            if (pArg == NULL)
                {
                errno = EINVAL;
                }
            else if (!pDev->enabled)
                {
                errno = EAGAIN;
                }
            else if (pArg->setFb.pFb == NULL)
                {
                errno = EINVAL;
                }
            else
                {
                void* virtAddr;
                void* physAddr;
                ULONG addr;
                ULONG firstAddr;
#if defined(GFX_USE_PMAP)
                virtAddr = pArg->setFb.pFb;
#else
                if (!GFX_IS_RTP_CTX)
                    {
                    virtAddr = pArg->setFb.pFb;
                    }
                else
                    {
                    /*
                     * If calling from RTP context, search the address mappings
                     * to find in what address range the onscreen buffer is
                     * located
                     */
                    GFX_ADRMAP* adrMap = findAdrMapVirtRtp (pArg->setFb.pFb);
                    if (adrMap == NULL)
                        {
                        errno = EINVAL;
                        break;
                        }
                    /*
                     * Calculate the kernel virtual address of the desired
                     * frame-buffer using the offset between the RTP's provided
                     * address and its starting frame-buffer address
                     */
                    virtAddr = GFX_VM_TRANSLATE (
                                   adrMap->firstVirtAddr,
                                   pArg->setFb.pFb,
                                   adrMap->firstVirtAddrRtp);
                    }
#endif
                addr = (ULONG)virtAddr;
                firstAddr = (ULONG)(pDev->firstVirtAddr);
                if (!((firstAddr <= addr) &&
                      (addr < (firstAddr + pDev->fbSize))))
                    {
                    errno = EINVAL;
                    break;
                    }
                pDev->frontVirtAddr = virtAddr;
                physAddr = GFX_VM_TRANSLATE (
                               pDev->firstPhysAddr,
                               pDev->frontVirtAddr,
                               pDev->firstVirtAddr);
                /*
                 * For both kernel and RTP, if the physical address of the
                 * onscreen frame-buffer changed then swap the display contents
                 */
                if (pDev->frontPhysAddr != physAddr)
                    {
                    pDev->frontPhysAddr = physAddr;
                    /* wait for vsync interrupts if configured so */
#if defined(GFX_VSYNC_IRQ) || defined(GFX_VSYNC_VXBUS_IRQ)
                    if (pDev->fbMode.vsync)
                        {
                        pDev->whenSwap = pArg->setFb.when;
                        pDev->needSwap = TRUE;
#if defined(GFX_USE_NOTIFY_FBADDR_CHANGE)
                        /* Some hardware designs require announcement of frame buffer address change */
                        if (pDev->notifyFbAddrChangeFuncPtr != NULL)
                                {
                                pDev->notifyFbAddrChangeFuncPtr(pDev);
                                }
#endif
                        (void)semTake ((SEM_ID)pDev->vsync, WAIT_FOREVER);
                        }
                    else
#endif
                        {
                        /*
                         * If vsync isn't enabled or supported on the device,
                         * call the device-specific function ptr that will take
                         * care of the swap.
                         */
#if defined(GFX_VSYNC_VXSIM)
                        pDev->whenSwap = pArg->setFb.when;
#endif
                        if (pDev->setFbAddrFuncPtr)
                            {
                            if (OK != (pDev->setFbAddrFuncPtr) (pDev))
                                {
                                errno = EFAULT;
                                break;
                                }
                            }
                        }
                    }
                rc = OK;
                }
            break;

        /* Clear screen */
        case FB_IOCTL_CLEAR_SCREEN:
            if (pArg == NULL)
                {
                errno = EINVAL;
                }
            else if (!pDev->enabled)
                {
                errno = EAGAIN;
                }
            else
                {
#if defined(GFX_NO_RTP_CLEAR_SCREEN)
                if (GFX_IS_RTP_CTX)
                    {
                    errno = EINVAL;
                    break;
                    }
#endif
                /*
                 * If setSplash is TRUE and splash is enabled,
                 * render the splash screen onto the front frame-buffer,
                 * otherwise clear the front frame-buffer
                 */
                if ((pArg->clearScreen.setSplash == TRUE) &&
                    gfxComp.splashFuncPtr)
                    {
                    (void)(gfxComp.splashFuncPtr) (pDev->frontVirtAddr,
                        pDev->fbMode.xres, pDev->fbMode.yres, pDev->fbMode.bpp,
                        pDev->fbMode.stride);
                    }
                else
                    {
#if defined(GFX_FIXED_FBMODE_OFFSET)
                    bzero ((void*)(pDev->frontVirtAddr), pDev->fbMode.offset);
#else
                    bzero ((void*)(pDev->frontVirtAddr),
                           pDev->fbMode.yres * pDev->fbMode.stride);
#endif
                    }
                rc = OK;
                }
            break;

        /* Wait for the next VBL interrupt */
        case FB_IOCTL_VSYNC:
#if defined(GFX_VSYNC_IRQ) || defined(GFX_VSYNC_VXBUS_IRQ)
            if (pArg == NULL)
                {
                errno = EINVAL;
                }
            else if ((!pDev->enabled) || (pDev->fbMode.vsync != TRUE))
                {
                errno = EAGAIN;
                }
            else
                {
                pDev->needSwap = FALSE;
                pDev->needVsync = TRUE;
                (void)semTake ((SEM_ID)pDev->vsync, WAIT_FOREVER);
                rc = OK;
                }
#elif defined(GFX_VSYNC_POLL)
            rc = OK;
#else
            errno = EINVAL;
#endif
            break;

        /* Device show */
        case FB_IOCTL_DEV_SHOW:
            show (pDev, arg);
            rc = OK;
            break;

        /* Enable sync */
        case FB_IOCTL_VSYNC_ENABLE:
#if defined(GFX_VSYNC_IRQ) || defined(GFX_VSYNC_VXBUS_IRQ) || defined(GFX_VSYNC_POLL)
            if (pArg == NULL)
                {
                errno = EINVAL;
                }
            else if (!pDev->enabled)
                {
                errno = EAGAIN;
                }
#if !defined(GFX_VSYNC_POLL)
            else if (!pDev->intHandlerVsyncFuncPtr)
                {
                errno = EINVAL;
                }
#endif
            else
                {
#if defined(GFX_VSYNC_POLL)
                if (pDev->fbMode.vsync == TRUE)
                    {
                    rc = OK;
                    break;
                    }
#endif
                pDev->fbMode.vsync = TRUE;
#if defined(GFX_VSYNC_POLL)
                if (pDev->setFbAddrFuncPtr)
                    {
                    (void)(pDev->setFbAddrFuncPtr) (pDev);
                    }
#endif
                rc = OK;
                }
#else
            errno = EINVAL;
#endif
            break;

        /* Disable sync */
        case FB_IOCTL_VSYNC_DISABLE:
#if defined(GFX_VSYNC_IRQ) || defined(GFX_VSYNC_VXBUS_IRQ) || defined(GFX_VSYNC_POLL)
            if (pArg == NULL)
                {
                errno = EINVAL;
                }
            else if (!pDev->enabled)
                {
                errno = EAGAIN;
                }
#if !defined(GFX_VSYNC_POLL)
            else if (!pDev->intHandlerVsyncFuncPtr)
                {
                errno = EINVAL;
                }
#endif
            else
                {
                pDev->fbMode.vsync = FALSE;
                rc = OK;
                }
#else
            errno = EINVAL;
#endif
            break;

        /* Return all the video modes as an array of string */
        case FB_IOCTL_GET_VIDEO_MODES:
            if ((pArg == NULL) || (pArg->getVideoModes.pVideoModes == NULL))
                {
                errno = EINVAL;
                }
            else if (!pDev->enabled)
                {
                errno = EAGAIN;
                }
            else
                {
#if defined(GFX_USE_CURRENT_VIDEO_MODES)
                bcopy ((void*)&(pDev->fbMode),
                       (void*)&(pArg->getVideoModes.pVideoModes[0]),
                       sizeof (FB_VIDEO_MODE));
#else
                ULONG j;
                j = (pDev->fbModesCount > FB_MAX_VIDEO_MODES)?
                    FB_MAX_VIDEO_MODES:pDev->fbModesCount;
                for (i = 0; i < j; i++)
                    {
                    bcopy ((void*)&(pDev->fbModesDb[i]),
                           (void*)&(pArg->getVideoModes.pVideoModes[i]),
                           sizeof (FB_VIDEO_MODE));
                    }
#endif
                rc = OK;
                }
            break;

        default:
            errno = EINVAL;
            break;
        }

    UNLOCK (&gfxComp);

    return rc;
    }

/*******************************************************************************
 *
 * devWrite - write to the console
 *
 * Write a character buffer to the display device.  This routine is called
 * whenever an application calls write() on an fbdev file descriptor.  This
 * routine is used to support graphical console functionality.  If the VxWorks
 * Image Project was configured with INCLUDE_FBDEV_CONSOLE, the characters are
 * rendered onto the display using a specified function pointer.  Otherwise, no
 * action is taken.
 *
 * RETURNS: The numbers of bytes written if successful, otherwise -1
 *
 * SEE ALSO: devOpen, devClose, devIoctl
 *
 */
LOCAL ssize_t devWrite
    (
    void*           pFileDesc,
    char*           pBuf,
    size_t          maxBytes
    )
    {
    FB_INFO         fbInfo;
    int             numBytes = 0;
    GFX_FBDEV*      pDev = (GFX_FBDEV*)pFileDesc;

    /* Check arguments */
    if (pDev == NULL)
        {
        errno = EFAULT;
        return -1;
        }
    if (pBuf == NULL)
        {
        errno = EINVAL;
        return -1;
        }

    errno = 0;

    /* Output characters */
    if (gfxComp.consoleFuncPtr)
        {
        if (OK == devIoctl(pDev, FB_IOCTL_GET_FB_INFO, (_Vx_ioctl_arg_t)&fbInfo))
            {
            numBytes = (gfxComp.consoleFuncPtr)(&fbInfo, pBuf, maxBytes);
            }
        }

    return (ssize_t)numBytes;
    }

/*******************************************************************************
 *
 * DRIVER SETUP ROUTINES
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * drvInit - initialize the display device driver
 *
 * This routine initializes the display device driver and prepares it for
 * install and device addition.  The complete initialization sequence of an
 * fbdev driver is as follows:
 *
 * 1) Driver initialization drvInit() - Prepares common data structures
 * 2) Driver install drvInstall() - Installs driver into Vx ios sub-system
 * 3) Device addition devAdd() - Add new device instance into Vx ios sub-system
 *
 * The sequence is infrastructured to allow drivers to be implemented using
 * either a VxBus-centric approach or to bypass VxBus entirely.
 *
 * Drivers implementions using the VxBus model are required to call the
 * initialization routines as per the sequence dictated by VxBus.  For
 * additional information on driver development using VxBus, refer to the "VxBus
 * Device Driver Developer's Guide".
 *
 * For driver implementations bypassing VxBus, the initialization sequence is
 * executed in its entirety (i.e. drvInit() -> drvInstall() -> devAdd()) within
 * the driver's device-specific initialization routine.  This routine is called
 * during kernel boot-up as specified within the driver's VxWorkd configuration
 * file (i.e. CDF).
 *
 * The <getCfgFuncPtr> parameter is a function pointer used to extract the fbdev
 * driver configuration parameters selected in the VxWorks Image Project.  The
 * <vidMemMaxSize> allows developers to limit the amount of video memory
 * allocated by the driver.  The parameter will overide the VxWorks Image
 * Project configuration parameter if the latter exceeds <vidMemMaxSize> in
 * bytes.  If 0 is passed, no limitation is placed on the size of video memory
 * allocated.
 *
 * RETURNS: pointer to frame buffer display device, NULL otherwise
 *
 * SEE ALSO: drvInstall, devAdd, drvCleanup
 *
 */
LOCAL GFX_FBDEV* drvInit
    (
    FUNCPTR         getCfgFuncPtr,
    size_t          vidMemMaxSize
    )
    {
    GFX_FBDEV*      pDev;
    size_t          vidMemSize;
    BOOL            rtpFreeMem;
    char*           pDeviceName;
    char*           pDisplayName;
    char*           pResolution;
    unsigned int    buffers;
    unsigned int    vsync;
    FUNCPTR         consoleFuncPtr;
    FUNCPTR         splashFuncPtr;

    if (!getCfgFuncPtr)
        {
        return NULL;
        }

    /*
     * Extract the fbdev configuration parameters from the device-specific
     * function pointer.  All drivers need to provide the following attributes.
     */
    (void)getCfgFuncPtr (&vidMemSize, &rtpFreeMem,
                   &pDeviceName, &pDisplayName, &pResolution,
                   &buffers, &vsync, &consoleFuncPtr, &splashFuncPtr);

    if (gfxComp.init == FALSE)
        {
        bzero ((void*)&gfxComp, sizeof (gfxComp));

        gfxComp.init = TRUE;

        LOCK_INIT(&gfxComp);
        }

    /*
     * Setup the splash and console function pointers, these may be null if
     * they have not been configured by the VxWorks Image Project
     */
    gfxComp.consoleFuncPtr = consoleFuncPtr;
    gfxComp.splashFuncPtr = splashFuncPtr;

    /* Allocate the fbdev device structure and setup its parameters */
    pDev = (GFX_FBDEV *)calloc (1, sizeof (GFX_FBDEV));
    if (pDev == NULL)
        {
        (void)fprintf (stderr, "Unable to allocate memory\n");
        return NULL;
        }

    (void)snprintf (pDev->deviceName, FB_MAX_STR_CHARS, pDeviceName);
    (void)snprintf (pDev->displayName, FB_MAX_STR_CHARS, pDisplayName);
    (void)snprintf (pDev->modeStr, FB_MAX_VIDEO_MODE_LEN, pResolution);
    if (buffers < 1)
        {
        pDev->fbMode.buffers = 1;
        }
    else if (buffers > FB_MAX_BUFFERS)
        {
        pDev->fbMode.buffers = FB_MAX_BUFFERS;
        }
    else
        {
        pDev->fbMode.buffers = buffers;
        }
    pDev->fbMode.vsync = vsync;

    /* Cap the total amount of video memory (if needed) */
    if (vidMemMaxSize && (vidMemSize > vidMemMaxSize))
        {
        pDev->fbSize = GFX_PAGE_ALIGN (vidMemMaxSize);
        }
    else
        {
        pDev->fbSize = GFX_PAGE_ALIGN (vidMemSize);
        }
    if (pDev->fbSize == 0)
        {
        free (pDev);
        (void)fprintf (stderr, "Frame buffer size cannot be zero\n");
        return NULL;
        }
#if !defined(GFX_USE_PMAP)
    pDev->fbRtpFreeMem = rtpFreeMem;
#endif

    /* Allocate video memory only if required */
#if !defined(GFX_HWDRV_ALLOC_VIDMEM)
    /* Allocate all the required video memory */
    if (ERROR == createMem (pDev))
        {
        free (pDev);
        (void)fprintf (stderr, "Unable to allocate frame buffer\n");
        return NULL;
        }
#endif
#if defined(GFX_VXBUS_GEN2)
    pDev->vxbDev = gfxComp.vxbDev;

    vxbDevSoftcSet (gfxComp.vxbDev, pDev);
#endif

    return pDev;
    }

/*******************************************************************************
 *
 * drvInstall - install the display device driver
 *
 * This routine installs the display device driver into the VxWorks ios
 * sub-system.  The routine is a part of the complete fbdev driver
 * initialization sequence (for additional details see drvInit()).
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * SEE ALSO: drvInit, drvCleanup, devAdd
 *
 */
LOCAL STATUS drvInstall
    (
    void
    )
    {
    /* Install the driver */
    drvNum = iosDrvInstall ((DRV_CREATE_PTR)NULL,      /* creat() */
                            (DRV_REMOVE_PTR)NULL,      /* remove() */
                            (DRV_OPEN_PTR)devOpen,     /* open() */
                            (DRV_CLOSE_PTR)devClose,   /* close() */
                            (DRV_READ_PTR)NULL,        /* read() */
                            (DRV_WRITE_PTR)devWrite,   /* write() */
                            (DRV_IOCTL_PTR)devIoctl);  /* ioctl() */
    if (drvNum == ERROR)
        {
        (void)fprintf (stderr, "No supported device found\n");
        return ERROR;
        }

    return OK;
    }

#if defined(GFX_VSYNC_IRQ) || defined(GFX_VSYNC_VXBUS_IRQ)
LOCAL STATUS intConnectVsync (GFX_FBDEV*);
LOCAL void intDisconnectVsync (GFX_FBDEV*);
#endif

/*******************************************************************************
 *
 * devAdd - add an instance of the display device driver
 *
 * This routine adds an instance of the display device driver into the VxWorks
 * ios sub-system.  The routine is a part of the complete fbdev driver
 * initialization sequence (for additional details see drvInit()).
 *
 * The <pDev> parameter represents the device structure being instantiated.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * SEE ALSO: drvInit, drvInstall, drvCleanup
 *
 */
LOCAL STATUS devAdd
    (
    GFX_FBDEV*      pDev
    )
    {
    /* Add the device to the kernel I/O system */
    if (iosDevAdd ((DEV_HDR*)pDev, pDev->deviceName, drvNum) != OK)
        {
        (void)fprintf (stderr, "Unable to add device to ios\n");
        return ERROR;
        }
#if defined(GFX_VSYNC_IRQ) || defined(GFX_VSYNC_VXBUS_IRQ)
    /* Connect the interrupt handler */
    if (pDev->intHandlerVsyncFuncPtr)
        {
        if (ERROR == intConnectVsync (pDev))
            {
            (void)fprintf (stderr, "Unable to connect vsync interrupt handler\n");

            (void)iosDevDelete ((DEV_HDR*)pDev);

            return ERROR;
            }
        }
#endif
    /* Set video mode */
    if (ERROR == setVideoMode (pDev, pDev->modeStr))
        {
        (void)fprintf (stderr, "Unable to set video mode %s\n", pDev->modeStr);
#if defined(GFX_VSYNC_IRQ) || defined(GFX_VSYNC_VXBUS_IRQ)
        if (pDev->intHandlerVsyncFuncPtr)
            {
            intDisconnectVsync (pDev);
            }
#endif
        (void)iosDevDelete ((DEV_HDR*)pDev);

        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
 *
 * drvCleanup - cleanup the display device driver
 *
 * The routine attempts to cleanup any device resources and prepares the driver
 * for removal.  This routine should only be called in extreme circumstances
 * such as a catastrophic failure during initialization.
 *
 * RETURNS: N/A
 *
 * SEE ALSO: drvInit
 *
 */
LOCAL void drvCleanup
    (
    GFX_FBDEV*      pDev
    )
    {
    if (drvNum != ERROR)
        {
        (void)iosDrvRemove (drvNum, TRUE);
        }

    if (gfxComp.init && gfxComp.videoMemory)
        {
        if (ERROR == vmStateSet (NULL, (VIRT_ADDR)gfxComp.videoMemory, pDev->fbSize,
                                 MMU_ATTR_CACHE_MSK, MMU_ATTR_CACHE_DEFAULT))
            {
            (void)fprintf (stderr, "vmStateSet error\n");
            }

        free (gfxComp.videoMemory);
        gfxComp.videoMemory = NULL;
        }

    if (pDev)
        {
        free (pDev);
        }
    }

/*******************************************************************************
 *
 * INTERRUPT HANDLING ROUTINES
 *
 ******************************************************************************/

#if defined(GFX_VSYNC_IRQ) || defined(GFX_VSYNC_VXBUS_IRQ)
/*******************************************************************************
 *
 * intHandlerVsync - display interrupt handler
 *
 * This routine is the common interrupt handler routine for all fbdev drivers.
 * The routine will handoff execution to a device-specific handler.
 *
 * RETURNS: N/A
 *
 * SEE ALSO: intConnectVsync
 *
 */
LOCAL void intHandlerVsync
    (
    GFX_FBDEV*      pDev
    )
    {
    if (pDev->intHandlerVsyncFuncPtr)
        {
        (pDev->intHandlerVsyncFuncPtr) (pDev);
        }
    }

/*******************************************************************************
 *
 * intDisconnectVsync - disconnect interrupt handler routine from vsync IRQ
 *
 * This routine disconnects the vsync interrupt handler routine from vsync IRQ.
 *
 * RETURNS: N/A
 *
 * SEE ALSO: intConnectVsync
 *
 */
LOCAL void intDisconnectVsync
    (
    GFX_FBDEV*      pDev
    )
    {
#if defined(GFX_VSYNC_IRQ)
    (void)intDisable (GFX_VSYNC_IRQ);

    (void)intDisconnect (INUM_TO_IVEC(GFX_VSYNC_IRQ),
                         (VOIDFUNCPTR)intHandlerVsync,
                         (int)pDev);
#elif defined(GFX_VSYNC_VXBUS_IRQ)
#if !defined(GFX_VXBUS_GEN2)
    (void)vxbIntDisable (pDev->vxbDev,
                         GFX_VSYNC_VXBUS_IRQ,
                         (VOIDFUNCPTR)intHandlerVsync,
                         (void*)pDev);

    (void)vxbIntDisconnect (pDev->vxbDev,
                            GFX_VSYNC_VXBUS_IRQ,
                            (VOIDFUNCPTR)intHandlerVsync,
                            (void*)pDev);
#else
    (void)vxbIntEnable (pDev->vxbDev, gfxComp.pResIrq);

    (void)vxbIntDisconnect (pDev->vxbDev, gfxComp.pResIrq);

    (void)vxbResourceFree (pDev->vxbDev, gfxComp.pResIrq);
    gfxComp.pResIrq = NULL;
#endif /* GFX_VXBUS_GEN2 */
#endif /* GFX_VSYNC_VXBUS_IRQ */
    }

/*******************************************************************************
 *
 * intConnectVsync - connect interrupt handler routine to vsync IRQ
 *
 * This routine connects the vsync interrupt handler routine to vsync IRQ.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 * SEE ALSO: intDisconnectVsync, intHandlerVsync
 *
 */
LOCAL STATUS intConnectVsync
    (
    GFX_FBDEV*      pDev
    )
    {
    if (semBInitialize (pDev->vsync, SEM_Q_FIFO, SEM_EMPTY) == (SEM_ID)NULL)
        {
        return ERROR;
        }
#if defined(GFX_VSYNC_IRQ)
    if (ERROR == intConnect (INUM_TO_IVEC(GFX_VSYNC_IRQ),
                             (VOIDFUNCPTR)intHandlerVsync,
                             (int)pDev))
        {
        return ERROR;
        }

    if (ERROR == intEnable (GFX_VSYNC_IRQ))
        {
        (void)intDisconnect (INUM_TO_IVEC(GFX_VSYNC_IRQ),
                             (VOIDFUNCPTR)intHandlerVsync,
                             (int)pDev);
        return ERROR;
        }
#elif defined(GFX_VSYNC_VXBUS_IRQ)
#if !defined(GFX_VXBUS_GEN2)
    if (ERROR == vxbIntConnect (pDev->vxbDev,
                                GFX_VSYNC_VXBUS_IRQ,
                                (VOIDFUNCPTR)intHandlerVsync,
                                (void*)pDev))
        {
        return ERROR;
        }

    if (ERROR == vxbIntEnable (pDev->vxbDev,
                               GFX_VSYNC_VXBUS_IRQ,
                               (VOIDFUNCPTR)intHandlerVsync,
                               (void*)pDev))
        {
        (void)vxbIntDisconnect (pDev->vxbDev,
                                GFX_VSYNC_VXBUS_IRQ,
                                (VOIDFUNCPTR)intHandlerVsync,
                                (void*)pDev);
        return ERROR;
        }
#else
    gfxComp.pResIrq = vxbResourceAlloc (pDev->vxbDev, VXB_RES_IRQ, 0);
    if (gfxComp.pResIrq == NULL)
        {
        return ERROR;
        }

    if (ERROR == vxbIntConnect (pDev->vxbDev,
                                gfxComp.pResIrq,
                                (VOIDFUNCPTR)intHandlerVsync,
                                (void*)pDev))
        {
        (void)vxbResourceFree (pDev->vxbDev, gfxComp.pResIrq);
        gfxComp.pResIrq = NULL;
        return ERROR;
        }

    if (ERROR == vxbIntEnable (pDev->vxbDev, gfxComp.pResIrq))
        {
        (void)vxbIntDisconnect (pDev->vxbDev, gfxComp.pResIrq);
        (void)vxbResourceFree (pDev->vxbDev, gfxComp.pResIrq);
        gfxComp.pResIrq = NULL;
        return ERROR;
        }
#endif /* GFX_VXBUS_GEN2 */
#endif /* GFX_VSYNC_VXBUS_IRQ */

    return OK;
    }
#endif /* GFX_VSYNC_IRQ || GFX_VSYNC_VXBUS_IRQ */

/*******************************************************************************
 *
 * MISC / HELPER ROUTINES
 *
 ******************************************************************************/

/*******************************************************************************
 *
 * gfxStrcmp - compare two strings while ignoring case and spacing differences
 *
 * RETURNS: OK for match, and ERROR otherwise
 *
 */
LOCAL STATUS gfxStrcmp
    (
    const char* s1,
    const char* s2
    )
    {
    char cmps1[FB_MAX_STR_CHARS];
    char cmps2[FB_MAX_STR_CHARS];
    int  i, size = 0;

    for (i = 0; i < strlen (s1); i ++)
        {
        if ((s1[i] != ' ') && (s1[i] != '\t'))
            {
            cmps1[size] = toupper(s1[i]);
            size ++;
            }
        }
    cmps1[size] = 0;

    size = 0;
    for (i = 0; i < strlen (s2); i ++)
        {
        if ((s2[i] != ' ') && (s2[i] != '\t'))
            {
            cmps2[size] = toupper(s2[i]);
            size ++;
            }
        }
    cmps2[size] = 0;
    if (strcmp (cmps1, cmps2) != 0)
        return ERROR;
    else
        return OK;
    }
