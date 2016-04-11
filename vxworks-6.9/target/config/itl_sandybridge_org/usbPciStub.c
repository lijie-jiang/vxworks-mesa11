/* usbPciStub.c - System-specific PCI Functions */

/*
 * Copyright (c) 2011 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
Modification history
--------------------
01a,11apr11,j_z  initial creation based on itl_nehalem version 01q 
*/

/*
DESCRIPTION

This file defines platform-independent functions accessing PCI bus 
capabilities.  These functions allow PCI device drivers to be written 
independent of the underlying O/S's PCI access mechanisms.

The name of each function in this group begins with "usb" to represent
"Device Driver Services."
*/

/* Includes */

#include "vxWorks.h"

#include "string.h"

#include "sysLib.h"
#include "cacheLib.h"

#include "iv.h"
#include "intLib.h"
#include "sysLib.h"

#include "drv/pci/pciConfigLib.h"   /* VxWorks PCI funcs */

/* PLATFORM-DEPENDENT INCLUDES/DEFINITIONS */


/* Pentium (e.g, PC Pentium) */

#include "drv/pci/pciIntLib.h"


/* Mappings for I/O and memory address translation */

#define PCI_IO_OFFSET	    0
#define PCI_MEM_OFFSET	    0
#define PCI_MEMIO_OFFSET    0

/* Interrupt enable/disable */

/*
 * The old pci routines are only available when this macro is defined.
 * The old PCI routines are used by the peripheral stack as it is not
 * vxBusified. It is mandatory to define this macro incase using USB
 * Peripheral Stack.
 * The host stack will not use these routines anymore.
 */

#ifdef INCLUDE_PCI_OLD_CONFIG_ROUTINES

#define INTERRUPT_BASE	0x20
#define IVEC(irq) 	INUM_TO_IVEC (INT_NUM_GET(irq))

#define INT_CONNECT(intNo, func, param) \
    pciIntConnect (IVEC (intNo), (VOIDFUNCPTR) func, (int) param)

#define INT_DISCONNECT(intNo, func, param) \
    pciIntDisconnect (IVEC (intNo), (VOIDFUNCPTR) func)

#define INT_ENABLE(i)	sysIntEnablePIC (i)
#define INT_DISABLE(i)	sysIntDisablePIC (i)


/* more includes */

#include "usb/usbPlatform.h"	    /* Basic definitions */
#include "usb/usbPciLib.h"	    /* Our API */


/* Map I/O functions to underlying system functions. */

#define OUR_PCI_IN_BYTE(a)	sysInByte ((a) + PCI_IO_OFFSET)
#define OUR_PCI_IN_WORD(a)	sysInWord ((a) + PCI_IO_OFFSET)
#define OUR_PCI_IN_DWORD(a)	sysInLong ((a) + PCI_IO_OFFSET)
#define OUR_PCI_OUT_BYTE(a,v)	sysOutByte ((a) + PCI_IO_OFFSET, (v))
#define OUR_PCI_OUT_WORD(a,v)	sysOutWord ((a) + PCI_IO_OFFSET, (v))
#define OUR_PCI_OUT_DWORD(a,v)	sysOutLong ((a) + PCI_IO_OFFSET, (v))


/* code tracks usage count for interrupts 0..MAX_INT_NO-1. */

#define MAX_INT_NO		256

/* locals */

LOCAL UINT16 intUsage [MAX_INT_NO] = {0};


/***************************************************************************
*
* usbPciByteGet - Returns a UINT8 configuration value
*
* This function returns the UINT8 value at offset <regOffset> from 
* the PCI configuration space of the device identified by <busNo>, 
* <deviceNo>, and <funcNo>.
*
* RETURNS: UINT8 value read from device configuration space
*/

UINT8 usbPciByteGet 
    (
    UINT8 busNo,		/* Bus number of device */
    UINT8 deviceNo,		/* Device number of device */
    UINT8 funcNo,		/* Function number of device */
    UINT16 regOffset		/* Offset into PCI config space */
    )
    {
    UINT8 value;

    if (pciConfigInByte (busNo, deviceNo, funcNo, regOffset, &value) != OK)
	return 0;

    return value;
    }


/***************************************************************************
*
* usbPciWordGet - Returns a UINT16 configuration value
*
* This function returns the UINT16 value at offset <regOffset> from 
* the PCI configuration space of the device identified by <busNo>, 
* <deviceNo>, and <funcNo>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: UINT16 value read from device configuration space
*/

UINT32 usbPciWordGet
    (
    UINT8 busNo,		/* Bus number of device */
    UINT8 deviceNo,		/* Device number of device */
    UINT8 funcNo,		/* Function number of device */
    UINT16 regOffset		/* Offset into PCI config space */
    )
    {
    UINT16 value;

    if (pciConfigInWord (busNo, deviceNo, funcNo, regOffset, &value) != OK)
	return 0;

    return value;
    }


/***************************************************************************
*
* usbPciDwordGet - Returns a UINT32 configuration value
*
* This function returns the UINT32 value at offset <regOffset> from 
* the PCI configuration space of the device identified by <busNo>, 
* <deviceNo>, and <funcNo>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: UINT32 value read from device configuration space
*/

UINT32 usbPciDwordGet
    (
    UINT8 busNo,		/* Bus number of device */
    UINT8 deviceNo,		/* Device number of device */
    UINT8 funcNo,		/* Function number of device */
    UINT16 regOffset		/* Offset into PCI config space */
    )
    {
    UINT32 value;

    if (pciConfigInLong (busNo, deviceNo, funcNo, regOffset, &value) != OK)
	return 0;

    return value;
    }


/***************************************************************************
*
* usbPciConfigHeaderGet - Reads a device's PCI configuration header
*
* This function reads the PCI configuration header for the device
* identified by <busNo>, <deviceNo>, and <funcNo>.  The configuration
* header is stored in the <pCfgHdr> buffer provided by the caller.
*
* This function initializes the <pCfgHdr> structure to zeros.  Any 
* fields which cannot be read from the device's configuration header 
* will remain zero upon return.  This function does not attempt to read
* fields defined as "reserved" in the PCI configuration header.
*
* RETURNS: N/A
*/

VOID usbPciConfigHeaderGet
    (
    UINT8 busNo,		/* Bus number of device */
    UINT8 deviceNo,		/* Device number of device */
    UINT8 funcNo,		/* Function number of device */
    pPCI_CFG_HEADER pCfgHdr	/* Buffer provided by caller */
    )
    {
    int i;

    /* Do nothing if CfgHdr is NULL */

    if (pCfgHdr == NULL)
	return;


    /* Initialize the buffer to zeros. */

    memset (pCfgHdr, 0, sizeof (*pCfgHdr));


    /* Read and store each field in the PCI configuration header. */

    pCfgHdr->vendorId	= usbPciWordGet (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID);
    pCfgHdr->deviceId	= usbPciWordGet (busNo, deviceNo, funcNo, PCI_CFG_DEVICE_ID);
    pCfgHdr->command	= usbPciWordGet (busNo, deviceNo, funcNo, PCI_CFG_COMMAND);
    pCfgHdr->status	= usbPciWordGet (busNo, deviceNo, funcNo, PCI_CFG_STATUS);
    pCfgHdr->revisionId = usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_REVISION);
    pCfgHdr->pgmIf	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_PROGRAMMING_IF);
    pCfgHdr->subClass	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_SUBCLASS);
    pCfgHdr->pciClass	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_CLASS);
    pCfgHdr->cacheLineSize = usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_CACHE_LINE_SIZE);
    pCfgHdr->latencyTimer = usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_LATENCY_TIMER);
    pCfgHdr->headerType = usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE);
    pCfgHdr->bist	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_BIST);

    for (i = 0; i < PCI_CFG_NUM_BASE_REG; i++)
	pCfgHdr->baseReg [i] = usbPciDwordGet (busNo, deviceNo, funcNo, 
	    PCI_CFG_BASE_ADDRESS_0 + i * sizeof (UINT32));

    pCfgHdr->romBase	= usbPciDwordGet (busNo, deviceNo, funcNo, PCI_CFG_EXPANSION_ROM);
    pCfgHdr->intLine	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_DEV_INT_LINE);
    pCfgHdr->intPin	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_DEV_INT_PIN);
    pCfgHdr->minGrant	= usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_MIN_GRANT);
    pCfgHdr->maxLatency = usbPciByteGet (busNo, deviceNo, funcNo, PCI_CFG_MAX_LATENCY);
    }


/***************************************************************************
*
* usbPciByteIn - input a byte from PCI I/O space
*
* Inputs a byte from a PCI I/O address <address>.
*
* RETURNS: byte input from i/o address
*/

UINT8 usbPciByteIn
    (
    UINT32 address		/* PCI I/O address */
    )

    {
    return OUR_PCI_IN_BYTE (address);
    }


/***************************************************************************
*
* usbPciWordIn - input a word from PCI I/O space
*
* Inputs a word from a PCI I/O address <address>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: word input from i/o address
*/

UINT16 usbPciWordIn
    (
    UINT32 address		/* PCI I/O address */
    )

    {
    UINT16 w = OUR_PCI_IN_WORD (address);
    return FROM_LITTLEW (w);
    }


/***************************************************************************
*
* usbPciDwordIn - input a dword from PCI I/O space
*
* Inputs a dword from a PCI I/O address <address>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: dword input from i/o address
*/

UINT32 usbPciDwordIn
    (
    UINT32 address		/* PCI I/O address */
    )

    {
    UINT32 l = OUR_PCI_IN_DWORD (address);
    return FROM_LITTLEL (l);
    }


/***************************************************************************
*
* usbPciByteOut - output a byte to PCI I/O space
*
* Outputs <value> to the PCI I/O address <address>.
*
* RETURNS: N/A
*/

VOID usbPciByteOut
    (
    UINT32 address,		/* PCI I/O address */
    UINT8 value 		/* value */
    )

    {
    OUR_PCI_OUT_BYTE (address, value);
    CACHE_PIPE_FLUSH ();
    }


/***************************************************************************
*
* usbPciWordOut - outputs a word to PCI I/O space
*
* Outputs <value> to the PCI I/O address <address>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: N/A
*/

VOID usbPciWordOut
    (
    UINT32 address,		/* PCI I/O address */
    UINT16 value		/* value */
    )

    {
    UINT16 w = TO_LITTLEW (value);
    OUR_PCI_OUT_WORD (address, w);
    CACHE_PIPE_FLUSH ();
    }


/***************************************************************************
*
* usbPciDwordOut - outputs a dword to PCI I/O space
*
* Outputs <value> to the PCI I/O address <address>.
*
* NOTE: This function adjusts for big vs. little endian environments.
*
* RETURNS: N/A
*/

VOID usbPciDwordOut
    (
    UINT32 address,		/* PCI I/O address */
    UINT32 value		/* value */
    )

    {
    UINT32 l = TO_LITTLEL (value);
    OUR_PCI_OUT_DWORD (address, l);
    CACHE_PIPE_FLUSH ();
    }


/***************************************************************************
*
* usbPciMemioOffset - Return PCI MEMIO to CPU MEMIO offset
*
* For memory-mapped I/O, the CPU's view of a memory address may not be the
* same as that programmed into the base address register of a PCI adapter.
* The CPU should add the value returned by this function to the BAR in order
* to produce the correct CPU-visible memory address.
*
* RETURNS: PCI_MEMIO_OFFSET
*/

UINT32 usbPciMemioOffset (void)
    {
    return PCI_MEMIO_OFFSET;
    }

/***************************************************************************
*
* usbPciIntConnect - Connect to a interrupt vector
*
* Connects the <func> to the interrupt number <intNo>.	<param> is an
* application-specific value which will be passed to <func> each time
* the interrupt handler is invoked.  
*
* RETURNS: OK, or ERROR if unable to connect/enable interrupt
*/

STATUS usbPciIntConnect
    (
    INT_HANDLER_PROTOTYPE func,     /* new interrupt handler */
    pVOID param,		    /* parameter for int handler */
    UINT16 intNo		    /* interrupt vector number */
    )

    {
    if (INT_CONNECT (intNo, func, param) != OK)
	return ERROR;

    if (INT_ENABLE (intNo) != OK)
	{
	INT_DISCONNECT (intNo, func, param);
	return ERROR;
	}

    if (intNo < MAX_INT_NO) 
	intUsage [intNo]++;

    return OK;
    }


/***************************************************************************
*
* usbPciIntDisconnect - Removes an interrupt handler
*
* Removes an interrupt handler installed by usbPciIntConnect().  <func>,
* <param>, and <intNo> must match the corresponding parameters from an earlier 
* call to usbPciIntConnect().
*
* RETURNS: N/A
*/

VOID usbPciIntRestore
    (
    INT_HANDLER_PROTOTYPE func,     /* int handler to be removed */
    pVOID param,		    /* parameter for int handler */
    UINT16 intNo		    /* interrupt vector number */
    )

    {
    if (intNo >= MAX_INT_NO || 
	(intUsage [intNo] != 0 && --intUsage [intNo] == 0))
	{
	INT_DISABLE (intNo);
	}

    INT_DISCONNECT (intNo, func, param);
    }

#endif /* INCLUDE_PCI_OLD_CONFIG_ROUTINES */

/***************************************************************************
*
* usbMemToPci - Convert a memory address to a PCI-reachable memory address
*
* Converts <pMem> to an equivalent 32-bit memory address visible from the 
* PCI bus.  This conversion is necessary to allow PCI bus masters to address
* the same memory viewed by the processor.
*
* RETURNS: converted memory address
*/

UINT32 usbMemToPci
    (
    pVOID pMem			/* memory address to convert */
    )

    {
    pVOID pPhys;
    
    /* The conversion is a two-step process.  First, we need to convert the
     * logical processor address (virtual) to a physical address.  Then, we
     * convert the physical address to one which can be seen from the PCI
     * bus.
     */

    pPhys = (pMem == NULL) ? NULL :
			     CACHE_DRV_VIRT_TO_PHYS (&cacheUserFuncs, pMem);

    return ((UINT32) pPhys) + PCI_MEM_OFFSET;

    }


/***************************************************************************
*
* usbPciToMem - Convert a PCI-reachable address to a CPU-reachable pointer
*
* Converts <pciAdrs> to an equivalent CPU memory address.  
*
* RETURNS: pointer to PCI memory
*/

pVOID usbPciToMem
    (
    UINT32 pciAdrs		/* 32-bit PCI address to be converted */
    )
    {
    void *	pVirt;

    pVirt = CACHE_DRV_PHYS_TO_VIRT (&cacheUserFuncs,
				   (void *) (pciAdrs - PCI_MEM_OFFSET));
    return (pVirt);
    }


/***************************************************************************
*
* usbPciMemInvalidate - Invalidate cache for a region of memory
*
* When another bus master, such as a PCI bus master, writes to memory, the
* cache may need to be invalidated for that region of memory.
*
* NOTE: Returns immediately if size == 0.
*
* RETURNS: N/A
*/

VOID usbPciMemInvalidate
    (
    pVOID pMem, 		/* base of memory region to invalidate */
    UINT32 size 		/* size of region to invalidate */
    )

    {
    if (size != 0)
	CACHE_USER_INVALIDATE (pMem, size);
    }


/***************************************************************************
*
* usbPciMemFlush - Flush a region of memory through the cache
*
* In systems which implement a non-write-thru cache, the processor may have
* written data to memory which has not yet been flushed to the actual system
* memory.  Before other bus masters may interrogate this memory, it may be
* necessary to flush the cache.
*
* NOTE: Returns immediately if size == 0.
*
* RETURNS: N/A
*/

VOID usbPciMemFlush
    (
    pVOID pMem, 		/* base of memory region to invalidate */
    UINT32 size 		/* size of region to invalidate */
    )

    {
    if (size != 0)
	CACHE_USER_FLUSH (pMem, size);
    }

/* End of file. */



