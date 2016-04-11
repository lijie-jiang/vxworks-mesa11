/* pciCfgStub.c - Sandy Bridge BSP stub for PCI configuration */

/*
 * Copyright (c) 2011 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */
 
/*
modification history
--------------------
01a,11apr11,j_z  initial creation based on itl_nehalem version 01f
*/

/*
These macros customize the module pciConfigLib for the generic pc BSP.
They are all set to their nominal default values.  For a specific PC
platform it may be necessary to modify these values.  Be sure to modify
them in the BSP specific stub file and not the global stub file in 
target/config/comps/src.

Note: we assume here that system I/O addresses and PCI IO addresses map one to
one with each other.
*/


/* defines */

/* Macros used to access PCI config space registers */

#undef POPEYE
#undef  PCI_OUT_BYTE
#define PCI_OUT_BYTE(addr, data) \
	sysOutByte (addr, data)

#undef  PCI_OUT_WORD
#define PCI_OUT_WORD(addr, data) \
	sysOutWord (addr, data)

#undef  PCI_OUT_LONG
#define PCI_OUT_LONG(addr, data) \
	sysOutLong (addr, data)

#undef  PCI_IN_BYTE
#define PCI_IN_BYTE(addr) \
	sysInByte (addr)

#undef  PCI_IN_WORD
#define PCI_IN_WORD(addr) \
	sysInWord (addr)

#undef  PCI_IN_LONG
#define PCI_IN_LONG(addr) \
	sysInLong (addr)

/* Offsets of PCI resources in host address space */
 
#define CPU_PCI_IO_ADRS      (0)     /* PCI IO base address */
#define CPU_PCI_MEM_ADRS     (0)     /* PCI memory base address */
#define PCI2DRAM_BASE_ADRS   (0)     /* DRAM base address from PCI */
#define CPU_PCI_ISA_IO_ADRS  (0)     /* ISA IO space */
 
#define PCI_DEV_MMU_MSK      (~(VM_PAGE_SIZE - 1))  /* MMU page mask */
#define PCI_DEV_ADRS_SIZE    (VM_PAGE_SIZE)         /* size of one page */


/* PCI (non-prefetchable) memory address to CPU address */

#define PCI_MEMIO2LOCAL(x)   ((x) + CPU_PCI_MEM_ADRS)

/* PCI IO memory address to CPU address */

#define PCI_IO2LOCAL(x)      ((x) + CPU_PCI_IO_ADRS)

/* 24-bit PCI network class ethernet subclass and prog. I/F code */

#define PCI_NET_ETHERNET_CLASS \
    ((PCI_CLASS_NETWORK_CTLR << 16) | (PCI_SUBCLASS_NET_ETHERNET << 8))

#define BOARD_TYPE_UNKNOWN   (-1)    /* unknown or unsupported board type */


/* typedefs */

typedef struct pciBoardResource      /* PCI_BOARD_RESOURCE */
    {
    UINT32        pciBus;            /* PCI Bus number */
    UINT32        pciDevice;         /* PCI Device number */
    UINT32        pciFunc;           /* PCI Function number */

    UINT32        vendorID;          /* PCI Vendor ID */
    UINT32        deviceID;          /* PCI Device ID */
    UINT8         revisionID;        /* PCI Revision ID */
    UINT32        boardType;         /* BSP-specific board type ID */

    UINT8         irq;               /* Interrupt Request Level */
    UINT32        irqvec;            /* Interrupt Request vector */

    UINT32        bar [6];           /* PCI Base Address Registers */

    void * const  pExtended;         /* pointer to extended device info */

    } PCI_BOARD_RESOURCE;


/* imports */
#ifndef INCLUDE_VXBUS
IMPORT STATUS pciConfigLibInit (int, ULONG, ULONG, ULONG);
#endif

IMPORT int    ffsLsb (UINT32 i);


#ifndef INCLUDE_VXBUS
/***************************************************************************
*
* sysPciCfgInit - pci configuration and initialization
*
* Modify this routine as needed for special host bridge initialization if
* needed.  This is the first access to PCI in a normal system.
*
* RETURNS: N/A
*/
VOID sysPciCfgInit
    (
    int	   mechanism,	/* mechanism 1 or 2 */
    UINT32 addr1,	/* address 1 */
    UINT32 addr2,	/* address 2 */
    UINT32 addr3	/* address 3 */
    )
    {

    /*
     * TODO: Any special host bridge initialization should be done here. Or
     * it can be in a separate component, if it is done before
     * INCLUDE_PCI_CFG_LIB
     */

    /* Initialize PCI driver library. */

    if (pciConfigLibInit (mechanism, addr1, addr2, addr3) != OK)
        {
	sysExcMsg += sprintf (sysExcMsg, "pciCfgStub.c: PCI configuration failure\n");
        sysToMonitor (BOOT_NO_AUTOBOOT);
        }

    }
#endif
