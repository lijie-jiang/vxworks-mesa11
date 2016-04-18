/* pci.c - PCI  functions */

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
11feb16,yat  Clean up code (US73271)
28sep15,wap  correct pcidev_add_entry() so that it talks the device
             resource list using the appropriate APIs
22jan15,qsn  updated several pci functions
07jan15,qsn  modified to support VXBUS-GEN2
01jan03,af   created
*/

/*

DESCRIPTION

This file provides compatibility functions for the Linux pci operations.

NOMANUAL

*/

/* includes */

#include <vxWorks.h>
#include <stdio.h> /* for printf */
#include <vxoal/krnl/pci.h> /* for pci_dev */
#include <vxoal/krnl/mm.h> /* for ioremap */

static int globalPciDevsTot = -1;
struct pci_dev pGlobalPciDevs[MAX_PCI_DEVS];
VXB_DEV_ID pGlobalVxbDevs[MAX_PCI_DEVS];
static struct pci_bus pGpciBus[MAX_PCI_DEVS];
unsigned long pci_mem_start;

/*
 * pci_read_config_byte - read one byte from the PCI configuration space
 */

int pci_read_config_byte
    (
    struct pci_dev *dev,
    int        offset,
    UINT8 *    pData
    )
    {
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    VXB_DEV_ID busCtrlID = vxbDevParent(pGlobalVxbDevs[dev->vxbPos]);
    PCI_HARDWARE * pPciDev = vxbDevIvarsGet(pGlobalVxbDevs[dev->vxbPos]);

    if (VXB_PCI_CFG_READ(busCtrlID, pPciDev, offset, 1, pData) != OK)
        return -1;
#else
    if (VXB_PCI_BUS_CFG_READ (pGlobalVxbDevs[dev->vxbPos], offset, 1, *pData)!=OK)
         return -1;
#endif
    return 0;
    }

/*
 * pci_read_config_word - read one word from the PCI configuration space
 */

int pci_read_config_word
    (
    struct pci_dev *dev,
    UINT32     offset,
    void *     pData
    )
    {
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    VXB_DEV_ID busCtrlID = vxbDevParent(pGlobalVxbDevs[dev->vxbPos]);
    PCI_HARDWARE * pPciDev = vxbDevIvarsGet(pGlobalVxbDevs[dev->vxbPos]);

    if (VXB_PCI_CFG_READ(busCtrlID, pPciDev, offset, 2, pData) != OK)
        return -1;
#else
   
   if (VXB_PCI_BUS_CFG_READ (pGlobalVxbDevs[dev->vxbPos], offset, 2, *pData)!=OK)
         return -1;
#endif
    return 0;
    }

/*
 * pci_read_config_dword - read 4 bytes from the PCI configuration space
 */

int pci_read_config_dword
    (
    struct pci_dev *dev,
    int        offset,
    UINT32 *   pData
    )
    {
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    VXB_DEV_ID busCtrlID = vxbDevParent(pGlobalVxbDevs[dev->vxbPos]);
    PCI_HARDWARE * pPciDev = vxbDevIvarsGet(pGlobalVxbDevs[dev->vxbPos]);

    if (VXB_PCI_CFG_READ(busCtrlID, pPciDev, offset, 4, pData) != OK)
        return -1;
#else
   if (VXB_PCI_BUS_CFG_READ (pGlobalVxbDevs[dev->vxbPos], offset, 4, *pData)!=OK)
         return -1;
#endif
    return 0;
    }

/*
 * pci_write_config_byte - write one byte to the PCI configuration space
 */

int pci_write_config_byte
    (
    struct pci_dev *dev,
    int        offset,
    UINT8      data
    )
    {
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    VXB_DEV_ID busCtrlID = vxbDevParent(pGlobalVxbDevs[dev->vxbPos]);
    PCI_HARDWARE * pPciDev = vxbDevIvarsGet(pGlobalVxbDevs[dev->vxbPos]);

    if (VXB_PCI_CFG_WRITE(busCtrlID, pPciDev, offset, 1, data) != OK)
        return -1;
#else
   if (VXB_PCI_BUS_CFG_WRITE (pGlobalVxbDevs[dev->vxbPos], offset, 1, data)!=OK)
			 return -1;

 
#endif
    return 0;
    }

/*
 * pci_write_config_word - write one word to the PCI configuration space
 */

int pci_write_config_word
    (
    struct pci_dev *dev,
    int        offset,
    UINT16     data
    )
    {
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    VXB_DEV_ID busCtrlID = vxbDevParent(pGlobalVxbDevs[dev->vxbPos]);
    PCI_HARDWARE * pPciDev = vxbDevIvarsGet(pGlobalVxbDevs[dev->vxbPos]);

    if (VXB_PCI_CFG_WRITE(busCtrlID, pPciDev, offset, 2, data) != OK)
        return -1;
#else
    if (VXB_PCI_BUS_CFG_WRITE (pGlobalVxbDevs[dev->vxbPos], offset, 2, data)!=OK)
			 return -1;
#endif
    return 0;
    }

/*
 * pci_write_config_dword - write 4 bytes to the PCI configuration space
 */

int pci_write_config_dword
    (
    struct pci_dev *dev,
    int        offset,
    UINT32     data
    )
    {
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    VXB_DEV_ID busCtrlID = vxbDevParent(pGlobalVxbDevs[dev->vxbPos]);
    PCI_HARDWARE * pPciDev = vxbDevIvarsGet(pGlobalVxbDevs[dev->vxbPos]);

    if (VXB_PCI_CFG_WRITE(busCtrlID, pPciDev, offset, 4, data) != OK)
        return -1;
#else
if (VXB_PCI_BUS_CFG_WRITE (pGlobalVxbDevs[dev->vxbPos], offset, 4, data)!=OK)
			return -1;

#endif
    return 0;
    }

struct pci_dev *pci_get_bus_and_slot 
    (
    unsigned int bus,
    unsigned  int devfn
    )
    {
    int i;

    for (i = 0; i < globalPciDevsTot; i++)
        {
        if ((pGlobalPciDevs[i].bus->number == bus) &&
            (pGlobalPciDevs[i].devfn == devfn))
            {
            return &(pGlobalPciDevs[i]);
            }
        }
    return NULL;
    }

struct pci_dev *pci_get_subsys
    (
    unsigned int vendor,
    unsigned int device,
    unsigned int ss_vendor,
    unsigned int ss_device,
    struct pci_dev *from
    )
    {
    int i, startPos;

    if (from == NULL)
        startPos = 0;
    else
        {
        for (i = 0; i < globalPciDevsTot; i++)
            if (&(pGlobalPciDevs[i]) == from)
                break;
        startPos = i + 1;
        }

    for (i = startPos; i < globalPciDevsTot; i++)
        {
        if (!((pGlobalPciDevs[i].vendor == vendor) || (vendor == PCI_ANY_ID)))
            continue;
        if (!((pGlobalPciDevs[i].device == device) || (device == PCI_ANY_ID)))
            continue;
        if (!((pGlobalPciDevs[i].subsystem_vendor == ss_vendor) 
             || (ss_vendor == PCI_ANY_ID)))
            continue;
        if (!((pGlobalPciDevs[i].subsystem_device == ss_device) 
             || (ss_vendor == PCI_ANY_ID)))
            continue;
        return &(pGlobalPciDevs[i]);
        }

    return NULL;
    }

struct pci_dev *pci_get_class
    (
    unsigned int class, 
    struct pci_dev *from
    )
    {
    int i, startPos;

    if (from == NULL)
        startPos = 0;
    else
        {
        for (i = 0; i < globalPciDevsTot; i++)
            if (&(pGlobalPciDevs[i]) == from)
                break;
        startPos = i + 1;
        }

    for (i = startPos; i < globalPciDevsTot; i ++)
        {
        if (pGlobalPciDevs[i].vendor == 0)
            continue;
        if (pGlobalPciDevs[i].class == class)
            {
            return &(pGlobalPciDevs[i]);
            }
        }

    return NULL;
    }

void __iomem *pci_iomap
    (
    struct pci_dev *dev, 
    int bar, 
    unsigned long maxlen
    )
    {
    unsigned long size;

    DEBUG_PCI("\n");

    if (dev == NULL)
        return NULL;
    
    size = dev->resource[bar].end - dev->resource[bar].start;

    if (maxlen > size)
        return NULL;

    return (void __iomem *) ioremap(dev->resource[bar].start, maxlen);
    }

#if defined(GFX_USE_VXBPCIEXTCAPFIND)
/*******************************************************************************
*
* Refer to vxworks-7/pkgs/os/drv/vxbus/buslib/src/pci/vxbPci.c
*
* gfxVxbPciExtCapFind - find extended capability in ECP linked list
*
* This routine searches for an extended capability in the linked list of
* capabilities in config space. If found, the offset of the first byte
* of the capability of interest in config space is returned via pOffset.
*
* RETURNS: OK if Extended Capability found, ERROR otherwise
*
* ERRNO
*
*/
LOCAL STATUS gfxVxbPciExtCapFind
    (
    VXB_DEV_ID     pDev,
    PCI_HARDWARE * hardInfo,
    UINT8          extCapFindId,  /* Extended capabilities ID to search for */
    UINT8 *        pOffset        /* returned config space offset */
    )
    {
    STATUS retStat = ERROR;
    UINT16 tmpStat;
    UINT8  tmpOffset;
    UINT8  capOffset = 0;
    UINT8  capId = 0;

    retStat = VXB_PCI_CFG_READ (pDev, hardInfo, PCI_CFG_STATUS, 2, &tmpStat);
    if (retStat == ERROR)
        {
        return ERROR;
        }

    if ((tmpStat == (UINT16)0xFFFF) || ((tmpStat & PCI_STATUS_NEW_CAP) == 0))
        {
        return ERROR;
        }

    (void) VXB_PCI_CFG_READ(pDev, hardInfo, PCI_CFG_CAP_PTR, 1, &capOffset);

    capOffset = (UINT8)(capOffset & ~0x02);

    /* Bounds check the ECP offset */

    if (capOffset < 0x40)
        {
        return retStat;
        }

    /* Look for the specified Extended Cap item in the linked list */

    while (capOffset != 0)
        {
        /* Get the Capability ID and check */

        (void) VXB_PCI_CFG_READ(pDev, hardInfo, (int)capOffset, 1, &capId);

        if (capId == extCapFindId)
            {
            *pOffset = (UINT8)(capOffset + 0x02);
            retStat = OK;
            break;
            }

        /* Get the offset to the next New Capabilities item */

        tmpOffset = (UINT8)(capOffset + 0x01);

        (void) VXB_PCI_CFG_READ(pDev, hardInfo, (int)tmpOffset, 1, &capOffset);
        }

    return retStat;
    }
#endif /* GFX_USE_VXBPCIEXTCAPFIND */

static int pci_is_pcie
    (
    struct pci_dev *dev
    )
    {
    VXB_DEV_ID busCtrlID = vxbDevParent(pGlobalVxbDevs[dev->vxbPos]);
    UINT8 expOffset = 0;
#if defined(GFX_USE_VXBPCIEXTCAPFIND)
    PCI_HARDWARE * pPciDev = vxbDevIvarsGet(pGlobalVxbDevs[dev->vxbPos]);
    (void) gfxVxbPciExtCapFind (busCtrlID, pPciDev, PCI_EXT_CAP_EXP, &expOffset)
#else
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    (void) vxbPciExtCapFind (busCtrlID, PCI_EXT_CAP_EXP, &expOffset);
else
	 struct vxbPciDevice *pPciDevice = pGlobalVxbDevs[dev->vxbPos]->pBusSpecificDevInfo;
	 vxbPciConfigExtCapFind (vxbDevParent(pGlobalVxbDevs[dev->vxbPos]), PCI_EXT_CAP_EXP,
			pPciDevice->pciBus,
			pPciDevice->pciDev,
			pPciDevice->pciFunc,
			&expOffset);

#endif
#endif /* GFX_USE_VXBPCIEXTCAPFIND */
    DEBUG_PCI("%d\n", expOffset);

    if (expOffset == 0)
        return 0;
    else 
        return 1;
    }

void pci_set_master
    (
    struct pci_dev *dev
    )
    {
    static unsigned int pcibios_max_latency = 255;
    UINT16 cmd;
    UINT8 timer;

    DEBUG_PCI("\n");

    pci_read_config_word(dev, PCI_CFG_COMMAND, &cmd); 
    cmd |= PCI_CMD_MASTER_ENABLE;
    if (pci_write_config_word(dev, PCI_CFG_COMMAND, cmd) < 0)
        return;
    
    if (pci_is_pcie (dev))
        return;

    pci_read_config_byte(dev, PCI_CFG_LATENCY_TIMER, &timer);
    if (timer < 16)
        timer = (64 <= pcibios_max_latency) ? 64 : (UINT8)pcibios_max_latency;
    else if (timer > pcibios_max_latency)
        timer = (UINT8)pcibios_max_latency;
    else
        return;
    pci_write_config_byte(dev, PCI_CFG_LATENCY_TIMER, timer);
    }

void pcidev_add_entry
    (
    VXB_DEV_ID vxbDevId
    )
    {
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    VXB_RESOURCE * pRes;
    VXB_RESOURCE_ADR * pAdr;
    PCI_HARDWARE * pPciDev;
    struct pci_dev *dev;
    UINT16 i;

    DEBUG_PCI("\n");

    if (vxbDevId == NULL)
        return;

    if (globalPciDevsTot < 0)
        {
        memset (pGlobalPciDevs, 0, sizeof (pGlobalPciDevs));
        memset (pGlobalVxbDevs, 0, sizeof (pGlobalVxbDevs));
		memset (pGpciBus, 0, sizeof (pGpciBus));	
        pci_mem_start = ULONG_MAX;
        globalPciDevsTot = 0;
        }

    dev = &(pGlobalPciDevs[globalPciDevsTot]);
    dev->bus = (struct pci_bus *)kcalloc (1, sizeof (struct pci_bus), GFP_KERNEL);
    if (dev->bus == (struct pci_bus *)NULL) return;

    pGlobalVxbDevs[globalPciDevsTot] = vxbDevId;
    pPciDev = vxbDevIvarsGet(vxbDevId);

    dev->bus->number = pPciDev->pciBus;
    dev->devfn = ((pPciDev->pciDev & 0x1f) << 3 |
                  (pPciDev->pciFunc & 0x7));

    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_VENDOR_ID,
                         2, dev->vendor);
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_DEVICE_ID,
                         2, dev->device);
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_SUB_VENDER_ID,
                         2, dev->subsystem_vendor);
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_SUB_SYSTEM_ID,
                         2, dev->subsystem_device);
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_REVISION,
                         4, dev->class);
    dev->class = (dev->class & 0xFFFFFF00) >> 8;
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_REVISION,
                         1, dev->revision);

    for (i = 0; i < 6; i++)
        {
        /*
         * Check for a memory-mapped BAR at this index first,
         * and if there isn't one, check for an I/O space
         * BAR next.
         */
        pRes = vxbResourceAlloc (vxbDevId, VXB_RES_MEMORY, i);
        if (pRes == NULL)
            pRes = vxbResourceAlloc (vxbDevId, VXB_RES_IO, i);
        if (pRes != NULL)
            {
            pAdr = (VXB_RESOURCE_ADR *)pRes->pRes;
            dev->resource[i].start = (resource_size_t)pAdr->start;
            dev->resource[i].end = (resource_size_t)(pAdr->start + pAdr->size - 1);
            if (dev->resource[i].start < pci_mem_start)
                pci_mem_start = dev->resource[i].start;
            }
        }

    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_DEV_INT_LINE,
                         1, dev->irq);

    dev->vxbPos = globalPciDevsTot;

    globalPciDevsTot++;
#else
    
    struct pci_dev *dev;
    UINT16 i;
    struct vxbPciDevice *   pPciDev = (struct vxbPciDevice *)vxbDevId->pBusSpecificDevInfo;
    DEBUG_PCI("\n");

    if (vxbDevId == NULL)
        return;

    if (globalPciDevsTot < 0)
        {
        memset (pGlobalPciDevs, 0, sizeof (pGlobalPciDevs));
        memset (pGlobalVxbDevs, 0, sizeof (pGlobalVxbDevs));
        pci_mem_start = ULONG_MAX;
        globalPciDevsTot = 0;
        }

    dev = &(pGlobalPciDevs[globalPciDevsTot]);
	#if 0
    dev->bus = (struct pci_bus *)kcalloc (1, sizeof (struct pci_bus), GFP_KERNEL);
	#else
	dev->bus = (struct pci_bus *)&pGpciBus[globalPciDevsTot];
	#endif
    if (dev->bus == (struct pci_bus *)NULL) return;

    pGlobalVxbDevs[globalPciDevsTot] = vxbDevId;

    dev->bus->number = pPciDev->pciBus;
    dev->devfn = ((pPciDev->pciDev & 0x1f) << 3 |
                  (pPciDev->pciFunc & 0x7));

    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_VENDOR_ID,
                         2, dev->vendor);
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_DEVICE_ID,
                         2, dev->device);
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_SUB_VENDER_ID,
                         2, dev->subsystem_vendor);
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_SUB_SYSTEM_ID,
                         2, dev->subsystem_device);
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_REVISION,
                         4, dev->class);
    dev->class = (dev->class & 0xFFFFFF00) >> 8;
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_REVISION,
                         1, dev->revision);

     /*for (i = 0; i < VXB_MAXBARS; i++)
        {
        if (pInst->regBaseFlags[i] == VXB_REG_MEM)
            printf("membar %d = 0x%x\n",i,pInst->pRegBase[i]);
		if (pInst->regBaseFlags[i] == VXB_REG_IO)
            printf("IObar %d = 0x%x\n",i,pInst->pRegBase[i]);
        }*/
    for (i = 0; i < 6; i++)
        {
        /*
         * Check for a memory-mapped BAR at this index first,
         * and if there isn't one, check for an I/O space
         * BAR next.
         */
          void *		drmHandle;
        if (vxbDevId->regBaseFlags[i] == VXB_REG_MEM)
            {
            
			vxbRegMap (vxbDevId, i, &drmHandle);            
            dev->resource[i].start = (resource_size_t)vxbDevId->pRegBase[i];
            dev->resource[i].end = (resource_size_t)(vxbDevId->pRegBase[i] + vxbDevId->regBaseSize[i] - 1);
            if (dev->resource[i].start < pci_mem_start)
                pci_mem_start = dev->resource[i].start;
            }
		 else if (vxbDevId->regBaseFlags[i] == VXB_REG_IO)
		 	 {
            vxbRegMap (vxbDevId, i, &drmHandle); 
            dev->resource[i].start = (resource_size_t)vxbDevId->pRegBase[i];
            dev->resource[i].end = (resource_size_t)(vxbDevId->pRegBase[i] + vxbDevId->regBaseSize[i] - 1);
            if (dev->resource[i].start < pci_mem_start)
                pci_mem_start = dev->resource[i].start;
            }
        }
 
    VXB_PCI_BUS_CFG_READ(vxbDevId, PCI_CFG_DEV_INT_LINE,
                         1, dev->irq);

    dev->vxbPos = globalPciDevsTot;

    globalPciDevsTot++;
#endif
    }

struct pci_dev *pcidev_find_entry
    (
    VXB_DEV_ID vxbDevId
    )
    {
    int i;

    DEBUG_PCI("\n");

    for (i = 0; i < globalPciDevsTot; i++)
        {
        if (pGlobalVxbDevs[i] == vxbDevId)
            {
            return &(pGlobalPciDevs[i]);
            }
        }

    return NULL;
    }

static void pci_resource_show
    (
    int i,
    struct resource *p
    )
    {
    if (p == NULL) return;

    if (p->start)
        {
        unsigned long resource_size = p->end-p->start;
        void *mmio = ioremap(p->start, resource_size);
        printf("\t\tstart:%llx %p\n", p->start, mmio);
        printf("\t\tend:%llx\n", p->end);
        printf("\t\tsize:%ld\n", resource_size);
        printf("\t\tname:%s\n", p->name);
        printf("\t\tflags:%lx\n", p->flags);
        if (mmio && (i == 0))
            {
            printf("\t\tRING_TAIL:%x\n", *(unsigned int *)(mmio+0x2030));
            printf("\t\tRING_HEAD:%x\n", *(unsigned int *)(mmio+0x2034));
            printf("\t\tRING_START:%x\n", *(unsigned int *)(mmio+0x2038));
            printf("\t\tRING_CTL:%x\n", *(unsigned int *)(mmio+0x203c));
            printf("\t\tRING_MI_MODE:%x\n", *(unsigned int *)(mmio+0x209c));
            printf("\t\tRING_IMR:%x\n", *(unsigned int *)(mmio+0x20a8));
            printf("\t\tGFX_MODE:%x\n", *(unsigned int *)(mmio+0x229c));
            printf("\t\tGT_MODE:%x\n", *(unsigned int *)(mmio+0x7008));
            }
        }
    }

static void pci_show
    (
    struct pci_dev *p
    )
    {
    int i;

    if (p == NULL) return;

    if (p->vendor != PCI_VENDOR_ID_INTEL) return;

    if (p->bus) printf("\tbus:%d\n", p->bus->number);
    printf("\tslot:%d\n", PCI_SLOT(p->devfn));
    printf("\tfunc:%d\n", PCI_FUNC(p->devfn));
    printf("\tvendor:%x\n", p->vendor);
    printf("\tdevice:%x\n", p->device);
    printf("\tsubsystem_vendor:%x\n", p->subsystem_vendor);
    printf("\tsubsystem_device:%x\n", p->subsystem_device);
    printf("\tclass:%d\n", p->class);
    printf("\trevision:%d\n", p->revision);
    printf("\tirq:%d\n", p->irq);

    if ((p->class & (0xff << 16)) != (PCI_CLASS_DISPLAY_VGA << 8)) return;

    for (i = 0; i < DEVICE_COUNT_RESOURCE; i ++)
        {
        printf("\tresource:%d\n", i);
        pci_resource_show(i, &(p->resource[i]));
        }
    }

void show_drm_pci
    (
    void
    )
    {
    int i;

    for (i = 0; i < globalPciDevsTot; i ++)
        {
        printf("pci=%d\n", i);
        pci_show(&(pGlobalPciDevs[i]));
        }
    }
