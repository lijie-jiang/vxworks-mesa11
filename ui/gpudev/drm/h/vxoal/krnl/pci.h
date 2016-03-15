/* pci.h - pci header file*/

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
18jun15,yat  Add stub code for PCI (US59462)
22jan15,qsn  Added more function prototypes
07jan15,qsn  Modified to support VXBUS-GEN2
20aug10,jlb  Written.
*/

/*
DESCRIPTION

This file provides compatibility pci bus functionality for the Linux operations.

NOMANUAL
*/

#ifndef _VXWORKS_PCI_H_
#define _VXWORKS_PCI_H_

#if defined(_WRS_CONFIG_VXBUS_LEGACY)
#include <vxBusLib.h> /* for VXB_DEVICE_ID */
#include <hwif/vxbus/vxBus.h>
#include <hwif/vxbus/vxbPciLib.h> /* PCI_HARDWARE */
#include <drv/pci/pciConfigLib.h>
#define VXB_DEV_ID VXB_DEVICE_ID
#else
#include <hwif/vxbus/vxbLib.h> /* for VXB_DEV_ID */
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h> /* PCI_HARDWARE */
#include <hwif/buslib/vxbPciMsi.h>
#include <hwif/util/vxbResourceLib.h>
#include <hwif/buslib/pciDefines.h>
#endif
#include <vxoal/krnl/types.h> /* for resource_size_t, container_of */
#include <vxoal/krnl/device.h> /* for device, device_driver */
#include <vxoal/krnl/mm.h> /* for ioremap, iounmap */
#include <vxoal/krnl/pm.h> /* for pm_message_t */
#include <mod_devicetable.h> /* for pci_device_id, platform_device_id */

#if 0
#define DEBUG_PCI pr_err
#else
#define DEBUG_PCI(...)
#endif

#define PCI_VENDOR_ID_VIA               (0x1106)
#define PCI_VENDOR_ID_SERVERWORKS       (0x1166)
#define PCI_DEVICE_ID_INTEL_82830_HB    (0x3575)
#define PCI_DEVICE_ID_INTEL_82845G_HB   (0x2560)

#define PCI_BASE_ADDRESS_MEM_MASK       (~0x0fUL)
#define PCI_BASE_ADDRESS_IO_MASK        (~0x03UL)

#define PCI_DEVFN(_slot, _func) ((((_slot) & 0x1f) << 3) | ((_func) & 0x07))
#define PCI_SLOT(_devfn)        (((_devfn) >> 3) & 0x1f)
#define PCI_FUNC(_devfn)        ((_devfn) & 0x07)

extern unsigned long pci_mem_start;
#define PCIBIOS_MIN_IO                  0x1000
#define PCIBIOS_MIN_MEM                 (pci_mem_start)

#define to_pci_dev(n) container_of(n, struct pci_dev, dev)

#define pci_resource_start(dev, bar)    ((dev)->resource[(bar)].start)
#define pci_resource_end(dev, bar)      ((dev)->resource[(bar)].end)
#define pci_resource_flags(dev, bar)    ((dev)->resource[(bar)].flags)

#define pci_resource_len(dev,bar)                                              \
    ((pci_resource_start((dev), (bar)) == 0 &&                                 \
    pci_resource_end((dev), (bar)) ==                                          \
    pci_resource_start((dev), (bar))) ? 0 :                                    \
    (pci_resource_end((dev), (bar)) -                                          \
    pci_resource_start((dev), (bar)) + 1))

enum
    {
    /* #0-5: standard PCI resources */
    PCI_STD_RESOURCES,
    PCI_STD_RESOURCE_END = 5,

    /* #6: expansion ROM resource */
    PCI_ROM_RESOURCE,

    /* total resources associated with a PCI device */
    PCI_NUM_RESOURCES,

    /* preserve this for compatibility */
    DEVICE_COUNT_RESOURCE
    };

struct resource
    {
    resource_size_t start;
    resource_size_t end;
    const char *name;
    unsigned long flags;
    };
/*
 * IO resources have these defined flags.
 */
#define IORESOURCE_BITS         0x000000ff      /* Bus-specific bits */

#define IORESOURCE_TYPE_BITS    0x00001f00      /* Resource type */
#define IORESOURCE_IO           0x00000100      /* PCI/ISA I/O ports */
#define IORESOURCE_MEM          0x00000200
#define IORESOURCE_REG          0x00000300      /* Register offsets */
#define IORESOURCE_IRQ          0x00000400
#define IORESOURCE_DMA          0x00000800
#define IORESOURCE_BUS          0x00001000

#define IORESOURCE_PREFETCH     0x00002000      /* No side effects */
#define IORESOURCE_READONLY     0x00004000
#define IORESOURCE_CACHEABLE    0x00008000
#define IORESOURCE_RANGELENGTH  0x00010000
#define IORESOURCE_SHADOWABLE   0x00020000

#define IORESOURCE_SIZEALIGN    0x00040000      /* size indicates alignment */
#define IORESOURCE_STARTALIGN   0x00080000      /* start field is alignment */

#define IORESOURCE_MEM_64       0x00100000
#define IORESOURCE_WINDOW       0x00200000      /* forwarded by bridge */
#define IORESOURCE_MUXED        0x00400000      /* Resource is software muxed */

#define IORESOURCE_EXCLUSIVE    0x08000000      /* Userland may not map this resource */
#define IORESOURCE_DISABLED     0x10000000
#define IORESOURCE_UNSET        0x20000000
#define IORESOURCE_AUTO         0x40000000
#define IORESOURCE_BUSY         0x80000000      /* Driver has marked this resource busy */

/* PnP IRQ specific bits (IORESOURCE_BITS) */
#define IORESOURCE_IRQ_HIGHEDGE         (1<<0)
#define IORESOURCE_IRQ_LOWEDGE          (1<<1)
#define IORESOURCE_IRQ_HIGHLEVEL        (1<<2)
#define IORESOURCE_IRQ_LOWLEVEL         (1<<3)
#define IORESOURCE_IRQ_SHAREABLE        (1<<4)
#define IORESOURCE_IRQ_OPTIONAL         (1<<5)

/* PnP DMA specific bits (IORESOURCE_BITS) */
#define IORESOURCE_DMA_TYPE_MASK        (3<<0)
#define IORESOURCE_DMA_8BIT             (0<<0)
#define IORESOURCE_DMA_8AND16BIT        (1<<0)
#define IORESOURCE_DMA_16BIT            (2<<0)

#define IORESOURCE_DMA_MASTER           (1<<2)
#define IORESOURCE_DMA_BYTE             (1<<3)
#define IORESOURCE_DMA_WORD             (1<<4)

#define IORESOURCE_DMA_SPEED_MASK       (3<<6)
#define IORESOURCE_DMA_COMPATIBLE       (0<<6)
#define IORESOURCE_DMA_TYPEA            (1<<6)
#define IORESOURCE_DMA_TYPEB            (2<<6)
#define IORESOURCE_DMA_TYPEF            (3<<6)

/* PnP memory I/O specific bits (IORESOURCE_BITS) */
#define IORESOURCE_MEM_WRITEABLE        (1<<0)  /* dup: IORESOURCE_READONLY */
#define IORESOURCE_MEM_CACHEABLE        (1<<1)  /* dup: IORESOURCE_CACHEABLE */
#define IORESOURCE_MEM_RANGELENGTH      (1<<2)  /* dup: IORESOURCE_RANGELENGTH */
#define IORESOURCE_MEM_TYPE_MASK        (3<<3)
#define IORESOURCE_MEM_8BIT             (0<<3)
#define IORESOURCE_MEM_16BIT            (1<<3)
#define IORESOURCE_MEM_8AND16BIT        (2<<3)
#define IORESOURCE_MEM_32BIT            (3<<3)
#define IORESOURCE_MEM_SHADOWABLE       (1<<5)  /* dup: IORESOURCE_SHADOWABLE */
#define IORESOURCE_MEM_EXPANSIONROM     (1<<6)

/* PnP I/O specific bits (IORESOURCE_BITS) */
#define IORESOURCE_IO_16BIT_ADDR        (1<<0)
#define IORESOURCE_IO_FIXED             (1<<1)

/* PCI ROM control bits (IORESOURCE_BITS) */
#define IORESOURCE_ROM_ENABLE           (1<<0)  /* ROM is enabled, same as PCI_ROM_ADDRESS_ENABLE */
#define IORESOURCE_ROM_SHADOW           (1<<1)  /* ROM is copy at C000:0 */
#define IORESOURCE_ROM_COPY             (1<<2)  /* ROM is alloc'd copy, resource field overlaid */
#define IORESOURCE_ROM_BIOS_COPY        (1<<3)  /* ROM is BIOS copy, resource field overlaid */

/* PCI control bits.  Shares IORESOURCE_BITS with above PCI ROM.  */
#define IORESOURCE_PCI_FIXED            (1<<4)  /* Do not move resource */

/* This defines the direction arg to the DMA mapping routines. */
#define PCI_DMA_BIDIRECTIONAL   0
#define PCI_DMA_TODEVICE        1
#define PCI_DMA_FROMDEVICE      2
#define PCI_DMA_NONE            3

#define PCI_CLASS_BRIDGE_ISA    0x0601

#define PCI_VENDOR_ID_INTEL     0x8086
#define MAX_PCI_DEVS            65536

#define PCI_CLASS_DISPLAY_VGA           0x0300
#define PCI_COMMAND             0x04    /* 16 bits */
#define PCI_COMMAND_IO          0x1     /* Enable response in I/O space */
#define PCI_COMMAND_MEMORY      0x2     /* Enable response in Memory space */
#define PCI_BRIDGE_CONTROL      0x3e
#define  PCI_BRIDGE_CTL_PARITY  0x01    /* Enable parity detection on secondary interface */
#define  PCI_BRIDGE_CTL_SERR    0x02    /* The same for SERR forwarding */
#define  PCI_BRIDGE_CTL_ISA     0x04    /* Enable ISA mode */
#define  PCI_BRIDGE_CTL_VGA     0x08    /* Forward VGA addresses */
#define  PCI_BRIDGE_CTL_MASTER_ABORT    0x20  /* Report master aborts */
#define  PCI_BRIDGE_CTL_BUS_RESET       0x40    /* Secondary bus reset */
#define  PCI_BRIDGE_CTL_FAST_BACK       0x80    /* Fast Back2Back enabled on secondary interface */

#define  PCI_CAP_ID_AGP         0x02    /* Accelerated Graphics Port */

#define PCI_EXP_LNKCAP          12      /* Link Capabilities */
#define  PCI_EXP_LNKCAP_SLS     0x0000000f /* Supported Link Speeds */
#define  PCI_EXP_LNKCAP_SLS_2_5GB 0x00000001 /* LNKCAP2 SLS Vector bit 0 */
#define  PCI_EXP_LNKCAP_SLS_5_0GB 0x00000002 /* LNKCAP2 SLS Vector bit 1 */

#define PCI_EXP_LNKCAP2         44      /* Link Capabilities 2 */
#define  PCI_EXP_LNKCAP2_SLS_2_5GB      0x00000002 /* Supported Speed 2.5GT/s */
#define  PCI_EXP_LNKCAP2_SLS_5_0GB      0x00000004 /* Supported Speed 5.0GT/s */
#define  PCI_EXP_LNKCAP2_SLS_8_0GB      0x00000008 /* Supported Speed 8.0GT/s */

typedef int pci_power_t;

#define PCI_D0          ((pci_power_t) 0)
#define PCI_D1          ((pci_power_t) 1)
#define PCI_D2          ((pci_power_t) 2)
#define PCI_D3hot       ((pci_power_t) 3)
#define PCI_D3cold      ((pci_power_t) 4)
#define PCI_UNKNOWN     ((pci_power_t) 5)
#define PCI_POWER_ERROR ((pci_power_t) -1)

struct pci_dev;

struct pci_bus
    {
    struct pci_dev     *self;
    unsigned char       number;         /* bus number */
    };

struct device;

/*
 * The pci_dev structure is used to describe PCI devices.
 */
struct pci_dev
    {
    struct pci_bus    * bus;            /* bus this device is on */
    unsigned int        devfn;          /* encoded device & function index */
    unsigned short      vendor;
    unsigned short      device;
    unsigned short      subsystem_vendor;
    unsigned short      subsystem_device;
    unsigned int        class;          /* 3 bytes: (base,sub,prog-if) */
    unsigned char       revision;
    void              * pPciId;
    struct device       dev;            /* Generic device interface */
    unsigned char       irq;
    unsigned int        msi_enabled:1;
    struct resource resource[DEVICE_COUNT_RESOURCE]; /* I/O and memory regions + expansion ROMs */
    int                 vxbPos;
    };

struct pci_device_id;

struct pci_driver
    {
    char *name;
    const struct pci_device_id *id_table;                                   /* must be non-NULL for probe to be called */
    int  (*probe)  (struct pci_dev *dev, const struct pci_device_id *id);   /* New device inserted */
    void (*remove) (struct pci_dev *dev);                                   /* Device removed (NULL if not a hot-plug capable driver) */
    int  (*suspend) (struct pci_dev *dev, pm_message_t state);              /* Device suspended */
    int  (*suspend_late) (struct pci_dev *dev, pm_message_t state);
    int  (*resume_early) (struct pci_dev *dev);
    int  (*resume) (struct pci_dev *dev);                                   /* Device woken up */
    void (*shutdown) (struct pci_dev *dev);
    struct device_driver driver;
    };

#define pci_get_drvdata(pdev) dev_get_drvdata(&((pdev)->dev))
#define pci_set_drvdata(pdev, data) dev_set_drvdata(&((pdev)->dev), (data))
#define pci_get_device(vendor, device, from) ({WARN_DEV; NULL;})
#define pci_dev_get(pdev) ({WARN_DEV; NULL;})
#define pci_dev_put(pdev)

#define pci_enable_device(pdev) (0)
#define pci_disable_device(pdev)
#define pci_name(dev) ("drm")
#define pci_domain_nr(bus) (0)

#define pci_map_rom(pdev, size) ({(pdev)=(pdev);NULL;})
#define pci_unmap_rom(pdev, rom)

#define pci_map_page(pdev, page, offset, size, dir) dma_map_page((pdev), (page), (offset), (size), (dir))
#define pci_unmap_page(pdev, daddr, size, dir) dma_unmap_page((pdev), (daddr), (size), (dir))
#define pci_dma_mapping_error(pdev, daddr) (0)
#define pci_bus_address(pdev, bar) ({(bar)=(bar);(dma_addr_t)0;})

#define pci_set_dma_mask(pdev, mask) (0)
#define pci_set_consistent_dma_mask(pdev, mask)
#define dma_alloc_coherent(dev, size, busaddr, flags) (NULL)
#define dma_free_coherent(dev, size, addr, busaddr)
#define dma_set_coherent_mask(dev, mask)

#define pci_save_state(pdev)
#define pci_set_power_state(pdev, data)

#define request_resource(ires, res) (0)
#define release_resource(res)
#define pci_bus_alloc_resource(bus, res, size, align, min, mask, ares, data) (0)

#define pci_enable_msi(pdev)
#define pci_disable_msi(pdev)
#define pcie_capability_read_dword(dev, pos, val) ((*(val)) = 1)
#define pci_find_capability(pdev, cap) (0)

extern int pci_read_config_byte
    (
    struct pci_dev *dev,
    int        offset,
    UINT8 *    pData
    );

extern int pci_read_config_word
    (
    struct pci_dev *dev,
    UINT32     offset,
    void *     pData
    );

extern int pci_read_config_dword
    (
    struct pci_dev *dev,
    int        offset,
    UINT32 *   pData
    );

extern int pci_write_config_byte
    (
    struct pci_dev *dev,
    int        offset,
    UINT8      data
    );

extern int pci_write_config_word
    (
    struct pci_dev *dev,
    int        offset,
    UINT16     data
    );

extern int pci_write_config_dword
    (
    struct pci_dev *dev,
    int        offset,
    UINT32     data
    );

static inline int pci_bus_read_config_word
    (
    struct pci_bus *bus,
    unsigned int devfn,
    int where,
    UINT16 *val
    )
    {
    struct pci_dev dev;

    dev.bus = bus;
    dev.devfn = devfn;
    dev.vxbPos = 0;

    return pci_read_config_word(&dev, where, val);
    }

extern struct pci_dev *pci_get_bus_and_slot
    (
    unsigned int bus,
    unsigned  int devfn
    );

extern struct pci_dev *pci_get_subsys
    (
    unsigned int vendor,
    unsigned int device,
    unsigned int ss_vendor,
    unsigned int ss_device,
    struct pci_dev *from
    );

extern struct pci_dev *pci_get_class
    (
    unsigned int class,
    struct pci_dev *from
    );

extern void *pci_iomap
    (
    struct pci_dev *dev,
    int bar,
    unsigned long maxlen
    );

#define pci_iounmap(dev, addr) iounmap(addr)

extern void pci_set_master
    (
    struct pci_dev *dev
    );

extern void pcidev_add_entry
    (
    VXB_DEV_ID pVxbDev
    );

extern struct pci_dev *pcidev_find_entry
    (
    VXB_DEV_ID pVxbDev
    );

static inline resource_size_t pcibios_align_resource
    (
    void *data,
    const struct resource *res,
    resource_size_t size,
    resource_size_t align
    )
    {
    return 0;
    }

#endif /* _VXWORKS_PCI_H_ */
