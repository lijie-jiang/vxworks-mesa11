/* config.h - itl_sandybridge configuration header */

/*
 * Copyright (c) 2010-2012 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01l,08nov12,cfm  WIND00375416 - Added I8253 for non-vxprj build.
01k,25may12,j_l  WIND00351919 - Modify virtual wire mode DRV_TIMER_LOAPIC
                                comment
                 Adhere to coding standards more closely
01j,27mar12,wyt  WIND00340694 - Remove ATA related code.
01i,17mar12,jlv  added 1st-3rd min/max PCI bus numbers. (CQ:288091)
01h,29feb12,wyt  change to new AHCI and ICH components
01g,11jan12,jjk  WIND00203743 - Fix AuxClock failures
01f,04oct11,jjk  WIND00308690 - Increment BSP revision
01e,17aug11,jjk  WIND00263692, Multi-stage boot support
01d,09mar11,j_z  cold clean.
01c,28apr11,j_z  update BSP_REV for 6.9.1 release.
01b,07mar11,j_z  sync to itl_nehalem/config.h 01o version for 6.9 release.
01a,14dec10,j_z  initial creation based on itl_nehalem version 01a
*/

/*
DESCRIPTION
This module contains the configuration parameters for the
Intel Sandy Bridge platform.
*/

#ifndef INCconfigh
#define INCconfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* BSP version/revision identification, before configAll.h */

#define BSP_VERSION "6.9"   /* vxWorks 6.x BSP */
#define BSP_REV     "/3"    /* increment by whole numbers */

#include <vsbConfig.h>
#include "configAll.h"
#include "pc.h"

/* BSP specific prototypes that must be in config.h */

#ifndef _ASMLANGUAGE
IMPORT void sysHwInit0 (void);
IMPORT UINT8 *sysInumTbl;      /* IRQ vs intNum table */
#endif /* !_ASMLANGUAGE */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) && \
     defined (INCLUDE_BOOT_APP)

/*
 * MP Table boot options (select only one)
 * Definitions below apply only to bootApp images.
 * Other images use an MP table left behind in memory by the bootApp
 * Defaults can only be set for non-vxprj images,
 * Otherwise, these settings would override any components
 * configured by vxprj.
 */

#undef INCLUDE_ACPI_BOOT_OP     /* define if bootrom uses ACPI for MP table */
#define INCLUDE_USR_BOOT_OP     /* user defined MP table */
#if ((!defined INCLUDE_ACPI_BOOT_OP) && (!defined INCLUDE_USR_BOOT_OP))
#define  INCLUDE_MPTABLE_BOOT_OP /* bios provides MP table */
#endif /* ((!defined INCLUDE_ACPI_BOOT_OP) && */
       /* (!defined INCLUDE_USR_BOOT_OP))     */

#if (defined INCLUDE_ACPI_BOOT_OP) && (!defined INCLUDE_BOOT_RAM_IMAGE)
#define INCLUDE_ACPI_MPAPIC  /* ACPI loads MP apic table */
#define INCLUDE_ACPI_TABLE_MAP /* map ACPI tables into vxWorks memory */
#endif /* (defined INCLUDE_ACPI_BOOT_OP) && */
       /* (!defined INCLUDE_BOOT_RAM_IMAGE) */

#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) &&
           defined INCLUDE_BOOT_APP */

/*
 * Define this to view ACPI log and error messages during the boot sequence.
 * NOTE: This is a debug tool and is not formally supported.
 */

#undef INCLUDE_ACPI_DEBUG_PRINT


/* define this to enable early formatted print statments. */

#undef INCLUDE_EARLY_PRINT

/* BSP specific initialization (before cacheLibInit() is called) */

#define INCLUDE_SYS_HW_INIT_0
#define SYS_HW_INIT_0()         (sysHwInit0())

/* power management definitions */

#if (!defined INCLUDE_BOOT_APP) && (!defined _WRS_CONFIG_SMP)
#undef INCLUDE_CPU_PERFORMANCE_MGMT /* component not default for now */
#endif /* (!defined INCLUDE_BOOT_APP) && (!defined _WRS_CONFIG_SMP) */

#ifdef INCLUDE_CPU_PERFORMANCE_MGMT
#define INCLUDE_ACPI_CPU_CONFIG /* query ACPI to get PWR configuration */
#define SELECT_PRIORITY_PERF   TRUE /* priority performance */
#define ISR_PSTATE          0
#define NUM_PERF_ENTRIES    4
#define NUM_PERF_PROFILES   4
#define DEF_PROFILE_INDEX   2   /* just ISRs get P0 */
#define PERF_PROFILE        sysPerfProfile
#define PERF_MGR_PROF0      "0,0"
#define PERF_MGR_PROF1      "0,0, 1,1, 32,3, 64,7"
#define PERF_MGR_PROF2      "0,1, 16,3, 32,5, 64,8"
#define PERF_MGR_PROF3      "0,8"
#define INCLUDE_ERF
#else  /* INCLUDE_CPU_PERFORMANCE_MGMT */
#define INCLUDE_CPU_LIGHT_PWR_MGR   /* default for VIP */
#endif /* INCLUDE_CPU_PERFORMANCE_MGMT */

#ifdef INCLUDE_ACPI_CPU_CONFIG
#define INCLUDE_ACPI_TABLE_MAP /* map ACPI tables into vxWorks memory */
#define INCLUDE_ACPI_SHOW
#define INCLUDE_ACPI
#endif /* INCLUDE_ACPI_CPU_CONFIG */


#ifdef INCLUDE_ACPI
#define INCLUDE_HPET            /* ACPI uses HPET */
#endif /* INCLUDE_ACPI */

#ifdef _WRS_VX_AMP
#ifndef INCLUDE_AMP_CPU
#define INCLUDE_AMP_CPU         /* Required */
#endif /* INCLUDE_AMP_CPU */
#endif /* _WRS_VX_AMP */

#if defined(_WRS_CONFIG_SMP) || defined(INCLUDE_AMP_CPU)

/* STRICTLY SMP BASED BSP, one must be built with _WRS_CONFIG_SMP defined!!! */

#define INCLUDE_VXIPI

/* SMP AP specific defines... */

/*
 * NOTE: if sysInitCpuStartup changes the value of CPU_STARTUP_SIZE
 *       will need to be recalculated...
 */

#define CPU_STARTUP_SIZE    0x00A8  /* size of sysInitCpuStartup */

#define CPU_ENTRY_POINT     0xC000  /* cold start code location for APs */
#define CPU_SCRATCH_START   0x0100  /* start of scratch memory */

#define CPU_AP_AREA         0x200   /* work area for APs */

/* offset for scratch memory */

#define CPU_SCRATCH_POINT   (CPU_ENTRY_POINT + CPU_SCRATCH_START)

#define NUM_GDT_ENTRIES     0x0A

#define CPU_SCRATCH_GDT     0x2E
#define CPU_SCRATCH_IDT     0x3A

#define LIM_GDT_OFFSET      0x2C
#define BASE_GDT_OFFSET     0x30
#define CR3_OFFSET          0x34
#define LIM_IDT_OFFSET      0x38
#define BASE_IDT_OFFSET     0x3C
#define AP_STK_OFFSET       0x40

#define CPU_INIT_START_ADR  (MP_ANCHOR_ADRS + 0x10)

#endif /* defined(_WRS_CONFIG_SMP) || defined(INCLUDE_AMP_CPU) */

/* Default boot line */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)
#define DEFAULT_BOOT_LINE \
    "gei(0,0)host:vxWorks h=90.0.0.3 e=90.0.0.50 u=target"
#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */

/* Warm boot (reboot) devices and parameters */

#define SYS_WARM_BIOS       0   /* warm start from BIOS */
#define SYS_WARM_FD         1   /* warm start from FD */
#define SYS_WARM_TFFS       2   /* warm start from DiskOnChip */
#define SYS_WARM_USB        3   /* warm start from USB */
#define SYS_WARM_ICH        4   /* warm start from ICH(IDE) */
#define SYS_WARM_AHCI       5   /* warm start from AHCI */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)

/* Warm boot (reboot) device and filename strings */

/*
 * BOOTROM_DIR is the device name for the device containing
 * the bootrom file. This string is used in sysToMonitor, sysLib.c
 * in dosFsDevCreate().
 */

#define BOOTROM_DIR  "/bd0"

/*
 * BOOTROM_BIN is the default path and file name to either a binary
 * bootrom file or an A.OUT file with its 32 byte header stripped.
 * Note that the first part of this string must match BOOTROM_DIR
 * The "bootrom.sys" file name will work with VxLd 1.5.
 */

#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */

#define BOOTROM_BIN  BOOTROM_DIR "/bootrom.sys"

/*
 * BOOTROM_AOUT is that default path and file name of an A.OUT bootrom
 * _still containing_ its 32byte A.OUT header.   This is legacy code.
 * Note that the first part of this string must match BOOTROM_DIR
 * The "bootrom.dat" file name does not work with VxLd 1.5.
 */

#define BOOTROM_AOUT BOOTROM_DIR "/bootrom.dat"

/* IDT entry type options */

#define SYS_INT_TRAPGATE    0x0000ef00  /* trap gate */
#define SYS_INT_INTGATE     0x0000ee00  /* int gate */

#if defined(_WRS_CONFIG_SMP) || defined(INCLUDE_AMP_CPU)

/* SMP must define INCLUDE_SYMMETRIC_IO_MODE */

#define INCLUDE_SYMMETRIC_IO_MODE       /* Interrupt Mode: Symmetric IO Mode */
#undef  INCLUDE_VIRTUAL_WIRE_MODE       /* Virtual Wire Mode NOT Supported */
                                        /* on SMP                          */
#define INCLUDE_HPET            /* SMP requires HPET */
#define USE_HPET_FOR_TIMESTAMP  /* SMP images use HPET for timestamp timer */
#elif !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)

/*
 * The following defaults can only be set for non-vxprj builds.
 * Otherwise, these settings would override any components
 * configured by vxprj.
 */

/* optional settings under UP... */

#undef  INCLUDE_VIRTUAL_WIRE_MODE   /* Interrupt Mode: Virtual Wire Mode */
#define INCLUDE_SYMMETRIC_IO_MODE   /* Interrupt Mode: Symmetric IO Mode */
#undef  INCLUDE_HPET                /* Optional component if supported by HW */
#endif  /* defined(_WRS_CONFIG_SMP) || defined(INCLUDE_AMP_CPU) */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)
#   if defined(INCLUDE_SYMMETRIC_IO_MODE)
#      undef SYS_MODEL
#      define SYS_MODEL "Intel(R) Sandy Bridge Processor SYMMETRIC IO"
#   elif  defined (INCLUDE_VIRTUAL_WIRE_MODE)
#      undef SYS_MODEL
#      define SYS_MODEL "Intel(R) Sandy Bridge Processor VIRTUAL WIRE"
#   else
#      define SYS_MODEL "Intel(R) Sandy Bridge Processor"
#   endif /* INCLUDE_SYMMETRIC_IO_MODE */
#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */

#undef INCLUDE_CPU_TURBO       /* overclock UP VIP images - off by default */

#if (defined INCLUDE_CPU_TURBO) || (defined INCLUDE_CPU_PERFORMANCE_MGMT)
#define INCLUDE_CPU_PWR_ARCH
#endif /* (defined INCLUDE_CPU_TURBO) ||         */
       /* (defined INCLUDE_CPU_PERFORMANCE_MGMT) */

#if defined (INCLUDE_SYMMETRIC_IO_MODE) || defined (INCLUDE_VIRTUAL_WIRE_MODE)
#define INCLUDE_USR_MPAPIC      /* MPAPIC table initialzation */
#endif /* defined (INCLUDE_SYMMETRIC_IO_MODE) || */
       /* defined (INCLUDE_VIRTUAL_WIRE_MODE)    */

#ifndef _WRS_CONFIG_SMP
#undef  USE_HPET_FOR_TIMESTAMP  /* for UP, timestamp driver does not use HPET */
#endif  /* !_WRS_CONFIG_SMP */

/* driver and file system options */

#undef INCLUDE_FD              /* include floppy disk driver */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)
#   undef  INCLUDE_DRV_STORAGE_PIIX  /* vxbus version of ATA disk driver */
#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */
#ifdef  INCLUDE_DRV_STORAGE_PIIX
#   ifdef INCLUDE_BOOT_APP
#      define INCLUDE_BOOT_FILESYSTEMS
#   endif /* INCLUDE_BOOT_APP */
#endif /* INCLUDE_DRV_STORAGE_PIIX */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)
#   undef  INCLUDE_DRV_STORAGE_AHCI
#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */
#ifdef  INCLUDE_DRV_STORAGE_AHCI
#   ifdef INCLUDE_BOOT_APP
#      define INCLUDE_BOOT_FILESYSTEMS
#   endif /* INCLUDE_BOOT_APP */
#endif /* INCLUDE_DRV_STORAGE_AHCI */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)

/*
 * Define INCLUDE_USB_BOOT_NOTBOOTAPP to support
 * USB nvram.txt access from bootrom.bin
 */

#undef INCLUDE_USB_BOOT_NOTBOOTAPP
#ifdef INCLUDE_USB_BOOT_NOTBOOTAPP

/*
 * This block of defines enables USB and USB based nvram.txt
 * file support for bootrom/bootrom.bin
 */

#define INCLUDE_USB
#define INCLUDE_USB_INIT
#define INCLUDE_EHCI
#define INCLUDE_EHCI_INIT
#define INCLUDE_UHCI
#define INCLUDE_UHCI_INIT
#define INCLUDE_USB_MS_BULKONLY
#define INCLUDE_USB_MS_BULKONLY_INIT
#if defined (INCLUDE_BOOT_APP)
#define INCLUDE_BOOT_USB_FS_LOADER
#endif /* INCLUDE_BOOT_APP */
#endif /* INCLUDE_USB_BOOT_NOTBOOTAPP */
#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)
#if defined (INCLUDE_USB)
#define SYS_WARM_TYPE      SYS_WARM_USB   /* USB warm start device */
#else
#define SYS_WARM_TYPE      SYS_WARM_BIOS  /* BIOS warm start device */
#endif /* INCLUDE_USB */
#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */

#if defined(INCLUDE_USB) || defined(INCLUDE_TFFS) || \
     defined(INCLUDE_FD) || defined(INCLUDE_DRV_STORAGE_PIIX) ||  \
     defined(INCLUDE_DRV_STORAGE_AHCI)
#   define INCLUDE_RAWFS           /* include raw FS */
#   define INCLUDE_DOSFS           /* dosFs file system */
#   define INCLUDE_DOSFS_MAIN      /* DOSFS2 file system primary module */
#   define INCLUDE_DOSFS_FAT       /* DOS file system FAT12/16/32 handler */
#   define INCLUDE_DOSFS_DIR_FIXED /* DOS File system old directory format */
                                   /* handler                              */
#   define INCLUDE_DOSFS_DIR_VFAT  /* DOS file system VFAT directory handler */
#   define INCLUDE_DOSFS_CHKDSK    /* DOS file system consistency checker */
#   define INCLUDE_DOSFS_FMT       /* DOS file system volume formatter */
#   define INCLUDE_FS_MONITOR
#   define INCLUDE_FS_EVENT_UTIL   /* include file event utility */
#   define INCLUDE_ERF
#   define INCLUDE_XBD
#   define INCLUDE_XBD_BLK_DEV
#   define INCLUDE_DEVICE_MANAGER
#   define INCLUDE_XBD_PART_LIB
#   undef  INCLUDE_HRFS            /* include HRFS file system */
#endif /* INCLUDE_USB || INCLUDE_DRV_STORAGE_PIIX */

#undef	INCLUDE_LPT             /* include parallel port driver */

#undef	INCLUDE_TFFS            /* include TrueFFS driver for Flash */
#undef	INCLUDE_PCMCIA          /* include PCMCIA driver */

/* SCSI driver options */

#undef	INCLUDE_SCSI            /* include SCSI driver */
#undef	INCLUDE_AIC_7880        /* include AIC 7880 SCSI driver */
#undef	INCLUDE_SCSI_BOOT       /* include ability to boot from SCSI */
#undef	INCLUDE_CDROMFS         /* file system to be used */
#undef	INCLUDE_SCSI2           /* select SCSI2 not SCSI1 */

/* VxBus configuration */

#define INCLUDE_VXBUS

#ifdef INCLUDE_VXBUS

#define INCLUDE_HWCONF  /* usrDepend.c includes hwconf.c */

/* VxBus util */

#define VXBUS_TABLE_CONFIG
#define INCLUDE_VXB_CMDLINE
#define INCLUDE_SIO_UTILS
#define INCLUDE_HWMEM_ALLOC
#define HWMEM_POOL_SIZE 100000
#define INCLUDE_PARAM_SYS
#define INCLUDE_DMA_SYS
#define INCLUDE_NON_VOLATILE_RAM

/* VxBus bus types */

#define INCLUDE_PLB_BUS
#define INCLUDE_PCI_BUS
#define INCLUDE_PCI

#if defined (INCLUDE_VIRTUAL_WIRE_MODE) || defined (INCLUDE_SYMMETRIC_IO_MODE)

/* PCI_IO_ADRS must not overlap APIC... */

#ifdef PCI_IO_ADRS
#undef PCI_IO_ADRS
#endif /* PCI_IO_ADRS */

#ifdef PCI_IO_SIZE
#undef PCI_IO_SIZE
#endif /* PCI_IO_SIZE */

#define PCI_IO_ADRS 0xa0000000
#define PCI_IO_SIZE 0x58000000

#else  /* INCLUDE_VIRTUAL_WIRE_MODE || INCLUDE_SYMMETRIC_IO_MODE */

#ifndef PCI_IO_ADRS
#define PCI_IO_ADRS 0xa0000000
#define PCI_IO_SIZE 0x5ff80000
#endif /* PCI_IO_ADRS */

#endif /* INCLUDE_VIRTUAL_WIRE_MODE || INCLUDE_SYMMETRIC_IO_MODE */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)

/*
 * On shumway board, the first PCI bus controller starts from bus 0 and
 * the second PCI bus controller starts from bus 128 but they are not bridge
 * connected.  In order to search the bus 128 or above, we create the second
 * VxBus PCI bus controller device instance with the start bus number 128.
 *
 * The sub-class of the PCI bus controller, that bridges to bus 130, on
 * the bus 128 is not configured as a PCI-to-PCI bridge on some shumway boards.
 * To search the bus 130 or above, we create another VxBus PCI bus controller
 * device instance with the start bus number 130.
 *
 * Note:
 * The PCI_BUS_CTRL_NUM configuration currently supported is 1, 2 or 3 only.
 */

#if defined (INCLUDE_SHUMWAY)
  #define SYS_PCI_BUS_CTRL_NUM	3
#else /* INCLUDE_SHUMWAY */
  #define SYS_PCI_BUS_CTRL_NUM  1
#endif /* INCLUDE_SHUMWAY */

/* the minimum and max bus numbers on the 1st PCI bus controller */

#define SYS_FIRST_PCI_MIN_BUS	0		/* start bus on 1st ctrl */

#if	(SYS_PCI_BUS_CTRL_NUM == 1)
# define SYS_FIRST_PCI_MAX_BUS	PCI_MAX_BUS	/* end bus on 1st ctrl */
#else   /* SYS_PCI_BUS_CTRL_NUM == 1 */
# define SYS_FIRST_PCI_MAX_BUS	127		/* end bus on 1st ctrl */

/* the minimum and max bus numbers on the 2nd PCI bus controller */

# define SYS_SECOND_PCI_MIN_BUS	128		/* start bus on 2nd ctrl */

# if	(SYS_PCI_BUS_CTRL_NUM == 2)
#  define SYS_SECOND_PCI_MAX_BUS PCI_MAX_BUS	/* end bus on 2nd ctrl */
# else  /* SYS_PCI_BUS_CTRL_NUM == 2 */
#  define SYS_SECOND_PCI_MAX_BUS 129		/* end bus on 2nd ctrl */

/* the minimum and max bus numbers on the 3rd PCI bus controller */

#  define SYS_THIRD_PCI_MIN_BUS	130		/* start bus on 3rd ctrl */
#  define SYS_THIRD_PCI_MAX_BUS	PCI_MAX_BUS	/* end bus on 3rd ctrl */
# endif	/* SYS_PCI_BUS_CTRL_NUM == 2 */

#endif	/* SYS_PCI_BUS_CTRL_NUM == 1 */

#endif  /* #if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */

/* VxBus drivers */

#define INCLUDE_PENTIUM_PCI
#define INCLUDE_PCI_OLD_CONFIG_ROUTINES
#define DRV_SIO_NS16550
#define DRV_NVRAM_FILE

/* Network driver options: VxBus drivers */

#undef  INCLUDE_AM79C97X_VXB_END
#undef  INCLUDE_AN983_VXB_END
#define INCLUDE_FEI8255X_VXB_END
#define INCLUDE_GEI825XX_VXB_END
#undef  INCLUDE_MVYUKONII_VXB_END
#undef  INCLUDE_MVYUKON_VXB_END
#undef  INCLUDE_NS8381X_VXB_END
#undef  INCLUDE_RTL8139_VXB_END
#undef  INCLUDE_RTL8169_VXB_END
#undef  INCLUDE_TC3C905_VXB_END
#undef  INCLUDE_NE2000_VXB_END

/* PHY and MII bus support */

#define INCLUDE_MII_BUS
#define INCLUDE_GENERICPHY
#undef  INCLUDE_DM9191PHY
#undef  INCLUDE_LXT972PHY
#undef  INCLUDE_MV88E1X11PHY
#undef  INCLUDE_RTL8201PHY
#undef  INCLUDE_RTL8169PHY
#undef  INCLUDE_VSC82XXPHY

#endif /* INCLUDE_VXBUS */

#define INCLUDE_END             /* Enhanced Network Driver Support */

#undef  INCLUDE_DEC21X40_END    /* (END) DEC 21x4x PCI interface */
#undef  INCLUDE_EL_3C90X_END    /* (END) 3Com Fast EtherLink XL PCI */
#undef  INCLUDE_ELT_3C509_END   /* (END) 3Com EtherLink III interface */
#undef  INCLUDE_ENE_END         /* (END) Eagle/Novell NE2000 interface */
#undef	INCLUDE_ULTRA_END       /* (END) SMC Elite16 Ultra interface */
#undef  INCLUDE_GEI8254X_END    /* (END) Intel 82543/82544 PCI interface */
#undef  INCLUDE_LN_97X_END      /* (END) AMD 79C97x PCI interface */

#undef	INCLUDE_BSD             /* BSD / Netif Driver Support (Deprecated) */

#undef	INCLUDE_EEX             /* (BSD) Intel EtherExpress interface */
#undef	INCLUDE_EEX32           /* (BSD) Intel EtherExpress flash 32 */
#undef	INCLUDE_ELC             /* (BSD) SMC Elite16 interface */
#undef	INCLUDE_ESMC            /* (BSD) SMC 91c9x Ethernet interface */
#undef	INCLUDE_AR521X_END      /* Atheros AR521X WLAN Support */

#ifndef INCLUDE_VXBUS
#define INCLUDE_FEI_END         /* (END) Intel 8255[7/8/9] PCI interface */
#endif /* !INCLUDE_VXBUS */

/* PCMCIA driver options */

#ifdef  INCLUDE_PCMCIA

#   define INCLUDE_SRAM         /* include SRAM driver */
#   undef INCLUDE_TFFS          /* include TFFS driver */

#   ifdef INCLUDE_NETWORK
#       define INCLUDE_BSD      /* include BSD / Netif Driver Support */
#   endif /* INCLUDE_NETWORK */

#endif  /* INCLUDE_PCMCIA */

/* default MMU options and PHYS_MEM_DESC type state constants */

#define INCLUDE_MMU_BASIC       /* bundled MMU support */
#undef	VM_PAGE_SIZE            /* page size */
#define VM_PAGE_SIZE        PAGE_SIZE_4KB   /* default page size */

#define VM_STATE_MASK_FOR_ALL \
    VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | VM_STATE_MASK_CACHEABLE
#define VM_STATE_FOR_IO \
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT
#define VM_STATE_FOR_MEM_OS \
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE
#define VM_STATE_FOR_MEM_APPLICATION \
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE
#define VM_STATE_FOR_PCI \
    VM_STATE_VALID | VM_STATE_WRITABLE | VM_STATE_CACHEABLE_NOT

/* CPU family/type-specific macros and options */

#undef	INCLUDE_SW_FP           /* Sandy Bridge has hardware FPP */
#undef	USER_D_CACHE_MODE       /* Sandy Bridge write-back data cache support */
#define USER_D_CACHE_MODE   (CACHE_COPYBACK | CACHE_SNOOP_ENABLE)
#define INCLUDE_MTRR_GET        /* get MTRR to sysMtrr[] */
#define INCLUDE_PMC             /* include PMC */

#define INCLUDE_EARLY_I_CACHE_ENABLE /* enable instruction cache early in */
                                     /* sysHwInit                         */
#define INCLUDE_EARLY_D_CACHE_ENABLE /* enable data cache early in sysHwInit */

#if defined (INCLUDE_VIRTUAL_WIRE_MODE) || defined (INCLUDE_SYMMETRIC_IO_MODE)
#define DRV_INTCTLR_MPAPIC
#define DRV_INTCTLR_LOAPIC

/* MSI: define INCLUDE_INTCTLR_DYNAMIC_LIB */

#define INCLUDE_INTCTLR_DYNAMIC_LIB
#if defined (INCLUDE_SYMMETRIC_IO_MODE)
#define DRV_INTCTLR_IOAPIC
#endif /* defined (INCLUDE_SYMMETRIC_IO_MODE) */
#endif /* defined (INCLUDE_VIRTUAL_WIRE_MODE) || (INCLUDE_SYMMETRIC_IO_MODE) */

#define INCLUDE_MMU_P6_32BIT    /* include 32bit MMU for Sandy Bridge */

#if defined (INCLUDE_MMU_P6_32BIT) || defined (INCLUDE_MMU_P6_36BIT)
#   undef  VM_STATE_MASK_FOR_ALL
#   undef  VM_STATE_FOR_IO
#   undef  VM_STATE_FOR_MEM_OS
#   undef  VM_STATE_FOR_MEM_APPLICATION
#   undef  VM_STATE_FOR_PCI
#   define VM_STATE_MASK_FOR_ALL \
       VM_STATE_MASK_VALID | VM_STATE_MASK_WRITABLE | \
       VM_STATE_MASK_WBACK | VM_STATE_MASK_GLOBAL
#   define VM_STATE_FOR_IO \
       VM_STATE_VALID | VM_STATE_WRITABLE | \
       VM_STATE_CACHEABLE_NOT | VM_STATE_GLOBAL_NOT
#   define VM_STATE_FOR_MEM_OS \
       VM_STATE_VALID | VM_STATE_WRITABLE | \
       VM_STATE_WBACK | VM_STATE_GLOBAL_NOT
#   define VM_STATE_FOR_MEM_APPLICATION \
       VM_STATE_VALID | VM_STATE_WRITABLE | \
       VM_STATE_WBACK | VM_STATE_GLOBAL_NOT
#   define VM_STATE_FOR_PCI \
       VM_STATE_VALID | VM_STATE_WRITABLE | \
       VM_STATE_CACHEABLE_NOT | VM_STATE_GLOBAL_NOT
#endif  /* defined (INCLUDE_MMU_P6_32BIT) || defined (INCLUDE_MMU_P6_36BIT) */

#if (defined (INCLUDE_VIRTUAL_WIRE_MODE) || defined (INCLUDE_SYMMETRIC_IO_MODE))
#   undef  INCLUDE_DEBUG_STORE      /* Debug Store (BTS/PEBS) */
#   ifdef  INCLUDE_DEBUG_STORE
#       define DS_SYS_MODE   FALSE  /* TRUE system mode, FALSE task mode */
#       define BTS_ENABLED   TRUE   /* BTS TRUE enable, FALSE disable */
#       define BTS_INT_MODE  TRUE   /* BTS TRUE int mode, FALSE circular */
#       define BTS_BUF_MODE  TRUE   /* BTS TRUE buffer mode, FALSE bus */
#       define PEBS_ENABLED  TRUE   /* PEBS TRUE enable, FALSE disable */
#       define PEBS_EVENT    PEBS_REPLAY        /* PEBS event */
#       define PEBS_METRIC   PEBS_2NDL_CACHE_LOAD_MISS  /* PEBS metric */
#       define PEBS_OS       TRUE   /* PEBS TRUE supervisor, FALSE usr */
#       define PEBS_RESET    -1LL   /* PEBS default reset counter value */
#   endif /* INCLUDE_DEBUG_STORE */
#endif  /* (defined (INCLUDE_VIRTUAL_WIRE_MODE) || */
        /* defined (INCLUDE_SYMMETRIC_IO_MODE))    */

/* vxbus timer specific macros */

#define INCLUDE_TIMER_SYS     /* timer drv control via vxBus */

#if defined (INCLUDE_SYMMETRIC_IO_MODE)
#define DRV_TIMER_LOAPIC        /* SMP TIMER DRV */
#define DRV_TIMER_IA_TIMESTAMP  /* SMP will need HPET for vxbDelay */
#undef  INCLUDE_AUX_CLK         /* SMP does not support AUX Clock */
#elif  defined (INCLUDE_VIRTUAL_WIRE_MODE)
#define DRV_TIMER_LOAPIC        /* MODE requires use of Local APIC as well */
#elif !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)

/*
 * The following defaults can only be set for non-vxprj builds.
 * Otherwise, these settings would override any components
 * configured by vxprj.
 */

#undef  DRV_TIMER_LOAPIC        /* APICS Not initialized in UP */
#endif /* defined (INCLUDE_SYMMETRIC_IO_MODE) */

#ifdef  INCLUDE_TIMESTAMP
#   define DRV_TIMER_IA_TIMESTAMP
#endif  /* INCLUDE_TIMESTAMP */

#define DRV_TIMER_I8253         /* UP TIMER DRV */

#ifdef  INCLUDE_AUX_CLK
#   define DRV_TIMER_MC146818
#endif  /* INCLUDE_AUX_CLK */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)

/*
 * default system and auxiliary clock constants
 *
 * Among other things, SYS_CLK_RATE_MAX depends upon the CPU and application
 * work load.  The default value, chosen in order to pass the internal test
 * suite, could go up to PIT_CLOCK.
 */

#define SYS_CLK_RATE_MIN    (19)   /* minimum system clock rate */
#define SYS_CLK_RATE_MAX    (5000) /* maximum system clock rate */

#ifdef DRV_TIMER_LOAPIC
#define AUX_CLK_RATE_MIN    SYS_CLK_RATE_MIN /* min auxiliary clock rate */
#define AUX_CLK_RATE_MAX    SYS_CLK_RATE_MAX /* max auxiliary clock rate */
#else  /* DRV_TIMER_LOAPIC */
#define AUX_CLK_RATE_MIN    (2)    /* minimum auxiliary clock rate */
#define AUX_CLK_RATE_MAX    (8192)/* maximum auxiliary clock rate */
#endif /* DRV_TIMER_LOAPIC */

#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */

#define IO_ADRS_ELC         0x240
#define INT_LVL_ELC         0x0b
#define MEM_ADRS_ELC        0xc8000
#define MEM_SIZE_ELC        0x4000
#define CONFIG_ELC          0   /* 0=EEPROM 1=RJ45+AUI 2=RJ45+BNC */

#define IO_ADRS_ULTRA       0x240
#define INT_LVL_ULTRA       0x0b
#define MEM_ADRS_ULTRA      0xc8000
#define MEM_SIZE_ULTRA      0x4000
#define CONFIG_ULTRA        0   /* 0=EEPROM 1=RJ45+AUI 2=RJ45+BNC */

#define IO_ADRS_EEX         0x240
#define INT_LVL_EEX         0x0b
#define NTFDS_EEX           0x00
#define CONFIG_EEX          0   /* 0=EEPROM  1=AUI  2=BNC  3=RJ45          */
                                /* Auto-detect is not supported, so choose */
                                /* the right one you're going to use       */

#define IO_ADRS_ELT         0x240
#define INT_LVL_ELT         0x0b
#define NRF_ELT             0x00
#define CONFIG_ELT          0   /* 0=EEPROM 1=AUI  2=BNC  3=RJ45 */

#define IO_ADRS_ENE         0x300
#define INT_LVL_ENE         0x05 /* Hardware jumper is used to set          */
                                 /* RJ45(Twisted Pair) AUI(Thick) BNC(Thin) */

#define IO_ADRS_ESMC        0x300
#define INT_LVL_ESMC        0x0b
#define CONFIG_ESMC         0   /* 0=EEPROM 1=AUI  2=BNC 3=RJ45 */
#define RX_MODE_ESMC        0   /* 0=interrupt level 1=task level */

#ifdef  INCLUDE_EEX32
#   define INCLUDE_EI               /* include 82596 driver */
#   define INT_LVL_EI       0x0b
#   define EI_SYSBUS        0x44    /* 82596 SYSBUS value */
#   define EI_POOL_ADRS NONE    /* memory allocated from system memory */
#endif  /* INCLUDE_EEX32 */

/* console definitions  */

#undef	NUM_TTY
#define NUM_TTY       (N_UART_CHANNELS)  /* number of tty channels */

#undef INCLUDE_PC_CONSOLE                /* PC keyboard and VGA console */
#ifdef INCLUDE_PC_CONSOLE
#   define PC_CONSOLE           (0)      /* console number */
#   define N_VIRTUAL_CONSOLES   (2)      /* shell / application */
#   ifdef INCLUDE_VXBUS
#       define DRV_KBD_I8042
#       define DRV_VGA_M6845
#   endif /* INCLUDE_VXBUS */
#endif /* INCLUDE_PC_CONSOLE */

/* PS/2 101-key default keyboard type (use PC_XT_83_KBD for 83-key) */

#define PC_KBD_TYPE   (PC_PS2_101_KBD)

/* memory addresses, offsets, and size constants */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)
#if (SYS_WARM_TYPE == SYS_WARM_BIOS)            /* non-volatile RAM size */
#   define NV_RAM_SIZE          (NONE)
#else /* SYS_WARM_TYPE == SYS_WARM_BIOS */
#   define NV_RAM_SIZE          (0x1000)
#endif /* SYS_WARM_TYPE == SYS_WARM_BIOS */
#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */

#define DEFAULT_LOCAL_MEM_LOCAL_ADRS  (0x00100000)    /* on-board memory base */

#define USER_RESERVED_MEM       (0)             /* user reserved memory */

/*
 * LOCAL_MEM_SIZE is the offset from the start of on-board memory to the
 * top of memory.  If the page size is 2MB or 4MB, write-protected pages
 * for the MMU directory tables and <globalPageBlock> array are also a
 * multiple of 2MB or 4MB.  Thus, LOCAL_MEM_SIZE should be big enough to
 * hold them.
 */

#define SYSTEM_RAM_SIZE      (0x40000000) /* Sandy bridge minimum system RAM */

#define DEFAULT_LOCAL_MEM_SIZE  (SYSTEM_RAM_SIZE - DEFAULT_LOCAL_MEM_LOCAL_ADRS)

#if !defined(_WRS_CONFIG_SMP) && !defined(INCLUDE_AMP_CPU)

#undef LOCAL_MEM_LOCAL_ADRS
#define LOCAL_MEM_LOCAL_ADRS DEFAULT_LOCAL_MEM_LOCAL_ADRS

#undef LOCAL_MEM_SIZE
#define LOCAL_MEM_SIZE          (SYSTEM_RAM_SIZE - LOCAL_MEM_LOCAL_ADRS)
#endif  /* !defined(_WRS_CONFIG_SMP) && !defined(INCLUDE_AMP_CPU) */

#ifdef INCLUDE_AMP_CPU
/* Setup MPAPIC Data Area address */

#define MPAPIC_DATA_START (LOCAL_MEM_LOCAL_ADRS + 0x2000)
#endif /* INCLUDE_AMP_CPU */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)

/* BIOS E820 memory auto-sizing is supported when this option is defined */

#ifndef INCLUDE_AMP_CPU
#define INCLUDE_BIOS_E820_MEM_AUTOSIZE
#else   /* INCLUDE_AMP_CPU */
#undef  INCLUDE_BIOS_E820_MEM_AUTOSIZE
#endif  /* INCLUDE_AMP_CPU */
#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */

/*
 * The following parameters are defined here and in the BSP Makefile.
 * They must be kept synchronized.  Any changes made here must be made
 * in the Makefile and vice versa.
 */

#ifdef  BOOTCODE_IN_RAM
#   undef	ROMSTART_BOOT_CLEAR
#endif  /* BOOTCODE_IN_RAM */

#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)
#ifdef  BOOTCODE_IN_RAM
#ifdef INCLUDE_MULTI_STAGE_BOOT
#   define ROM_BASE_ADRS        (0x00408000)    /* base address of ROM */
#   define ROM_SIZE             (0x00200000)    /* size of ROM */
#else /* INCLUDE_MULTI_STAGE_BOOT */
#   define ROM_BASE_ADRS        (0x00008000)    /* base address of ROM */
#   define ROM_SIZE             (0x00090000)    /* size of ROM */
#endif /* INCLUDE_MULTI_STAGE_BOOT */
#   define ROM_TEXT_ADRS        (ROM_BASE_ADRS) /* booting from A: or C: */
#else
#   define ROM_BASE_ADRS        (0xfff20000)    /* base address of ROM */
#   define ROM_TEXT_ADRS        (ROM_BASE_ADRS) /* booting from EPROM */
#   define ROM_SIZE             (0x0007fe00)    /* size of ROM */
#endif /* BOOTCODE_IN_RAM */
#endif /* !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD) */

#define FIRST_TEXT_ADRS         (0x00008000)

#ifdef RAM_LOW_ADRS
#define SYS_RAM_LOW_ADRS        RAM_LOW_ADRS    /* VxWorks image entry point */
#else /* RAM_LOW_ADRS */
#define SYS_RAM_LOW_ADRS        (0x00408000)    /* VxWorks image entry point */
#endif /* RAM_LOW_ADRS */

#ifdef RAM_HIGH_ADRS
#define SYS_RAM_HIGH_ADRS       RAM_HIGH_ADRS   /* Boot image entry point */
#else /* RAM_HIGH_ADRS */
#define SYS_RAM_HIGH_ADRS       (0x10008000)    /* Boot image entry point */
#endif /* RAM_HIGH_ADRS */

#define BOOT_IMAGE_ADRS_OFFSET       (BOOT_MULTI_STAGE_DATA_OFFSET)
#define BOOT_IMAGE_ADRS_OFFSET_SIZE  4
#define BOOT_IMAGE_SIZE_OFFSET       (BOOT_IMAGE_ADRS_OFFSET +          \
                                      BOOT_IMAGE_ADRS_OFFSET_SIZE)
#define BOOT_IMAGE_SIZE_OFFSET_SIZE  4
#define BOOT_MULTI_STAGE_DATA_SIZE   (BOOT_IMAGE_SIZE_OFFSET +          \
                                      BOOT_IMAGE_SIZE_OFFSET_SIZE       \
                                      - BOOT_MULTI_STAGE_DATA_OFFSET)
#define BOOT_MULTI_STAGE_DATA        (LOCAL_MEM_LOCAL_ADRS +            \
                                      BOOT_MULTI_STAGE_DATA_OFFSET)
#define BOOT_IMAGE_ADRS              (LOCAL_MEM_LOCAL_ADRS +    \
                                      BOOT_IMAGE_ADRS_OFFSET)
#define BOOT_IMAGE_SIZE              (LOCAL_MEM_LOCAL_ADRS +    \
                                      BOOT_IMAGE_SIZE_OFFSET)

/* interrupt mode/number definitions */

#include "configInum.h"

#define _WRS_BSP_VM_PAGE_OFFSET (VM_PAGE_SIZE)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* INCconfigh */

#if defined(PRJ_BUILD)
#   include "prjParams.h"
#endif /* defined(PRJ_BUILD) */

