/* pciCfgIntStub.c - BSP stub for PCI shared interrupts */

/*
 * Copyright (c) 2011-2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01d,19jul13,jjk  WIND00427114 - Fixing prevent defects
01c,20feb13,scm  WIND00397381 IA BSPs need to configure interrupt lines behind
                 PCI bridge...
01b,18mar12,jlv  added minimum bus number option. (CQ:288091)
                 added multiple bus ctrl support.
01a,11apr11,j_z  initial creation based on itl_nehalem version 01z
*/

/*
For the pciIntLib module, the following macros customize how the code is
compiled.  For the generic pc platform, all these macros take on their
default values.  For special hardware, just edit these macros in this BSP
specific stub file as needed. (Please do not edit the global stub file in
target/config/comps/src).

.IP PCI_INT_LINES
This macro represents the number of PCI interrupt vectors that may be shared.
The default number is 32.  At startup time a linked list is created for each
possible shared vector.

.IP PCI_INT_BASE
This macro represents the base vector number for the first shared PCI vector.
It is used in the default mapping of system vector numbers to shared PCI vector
numbers.  The default value is the standard macro INT_NUM_IRQ0, or zero
if that macro is not defined. This macro is used with the PCI_INT_VECTOR_TO_IRQ
macro described below.

.IP PCI_INT_VECTOR_TO_IRQ(vector)
This macro accepts a system vector number as input and returns a PCI irq number
as output.  This value is used as the index into the list of shared interrupts.
The resulting PCI irq number must lie in the range of zero to
(PCI_INT_LINES - 1).  By default, the macro evaluates to:
.CS
        (IVEC_TO_INUM(vector) - PCI_INT_BASE)
.CE

.IP PCI_INT_HANDLER_BIND(vector, rtn, arg, pResult)
This macro is used by the module to bind the master PCI interrupt handling
routine to the actual system vector.  By default it is mapped to:
.CS
        *pResult = intConnect (vector, rtn, arg);
.CE

.IP PCI_INT_HANDLER_UNBIND(vector, rtn, arg, pResult)
This macro is used by the module to unbind the master PCI interrupt handling
routine pciInt() from the actual system vector.  This will only happen when
the last shared interrupt handler is disconnected from a shared interrupt.
The default mapping is a no-op function that returns OK.
Note: without a functional mapping, a small memory leak will exist if PCI
interrupts are connected and disconnected repeatedly.  If present, the leak
involves the code stub generated as part of the default binding routine, 
intConnect().
*/

#include <hwif/util/hwMemLib.h>
#include <hwif/vxbus/vxBus.h>
#include <hwif/vxbus/vxbPlbLib.h>
#include <hwif/vxbus/hwConf.h>

#include <hwif/vxbus/vxbPciLib.h>

IMPORT VXB_DEVICE_ID globalBusCtrlID;

/* macros */

/* 
 * PCI_INT_BASE: PCI base IRQ number (not intNum) is
 * - IRQ 0 in the PIC or VIRTUAL_WIRE mode
 * - IRQ 0 in the SYMMETRIC_IO mode
 */

#define PCI_INT_BASE            (0)

/*
 * Provide intConnect via a macro so that an alternate interrupt binding
 * mechanism can be specified
 */

#define PCI_INT_HANDLER_BIND(vector, routine, param, pResult)           \
    do {                                                                \
    IMPORT STATUS intConnect();                                         \
    *pResult = intConnect ( (vector),(routine), (int)(param) );         \
    } while ((0))

/*
 * Provide an unbind macro to remove an interrupt binding.  The default 
 * is a no-op.  This can result in a memory leak if there
 * is a lot of pciIntConnect, pciIntDisconnect activity.
 */

#define PCI_INT_HANDLER_UNBIND(vector, routine, param, pResult)         \
    do { *pResult = OK; } while ((0))

/* imports */

#ifndef INCLUDE_VXBUS
IMPORT STATUS   pciIntLibInit (void);           /* pci/pciIntLib.c */
#endif
 
IMPORT UINT8    *sysInumTbl;                    /* IRQ vs intNum table */
IMPORT UINT32   sysInumTblNumEnt;               /* number of IRQs */

#ifdef INCLUDE_USR_MPAPIC
IMPORT STATUS usrMpapicInit (BOOL earlyInit, char * pBuf);
#endif /* INCLUDE_USR_MPAPIC */


/* forward declarations */

#ifndef INCLUDE_VXBUS
/***********************************************************************
*
* sysPciIntInit - PCI interrupt library init
*
* Modify this routine as needed for any special host bridge initialization
* related to interrupt handling.
*/

VOID sysPciIntInit (void)
    {

    /* TODO: add any special pre-initialization code here */

    if (pciIntLibInit () == ERROR)
        {
        sprintf (sysExcMsg, "pciCfgIntStub.c: pciIntLibInit() failed\n");
        sysToMonitor (BOOT_NO_AUTOBOOT);
        }
    }
#endif

/*******************************************************************************
*
* sysPciIvecToIrq - get an IRQ(PIC or IOAPIC) number from vector address
*
* This routine gets an IRQ(PIC or IOAPIC) number from vector address.
* Assumptions are following:
*   - IRQ number is 0 - 15 in PIC or VIRTUAL_WIRE mode
*   - IRQ number is 0 - 23 in SYMMETRIC_IO mode
*
* RETURNS: IRQ 0 - 15/23, or -1 if we failed to get it.
*
*/

int sysPciIvecToIrq 
    (
    _Vx_usr_arg_t vector        /* vector address */
    )
    {
    UINT32 irq;
    UINT32 intNum = IVEC_TO_INUM (vector);

    /* walk through the sysInumTbl[] to get the match */

    for (irq = PCI_INT_BASE; irq < sysInumTblNumEnt; irq++)
        {
        if (sysInumTbl[irq] == intNum)
            return (irq);
        }

    return (ERROR);
    }

/* typedefs */

#if defined (INCLUDE_SYMMETRIC_IO_MODE)

/* defines */

#define ICH_LPC_PCI_BUSNO      0x0     /* LPC PCI BusNo */
#define ICH_LPC_PCI_DEVNO      0x1f    /* LPC PCI DevNo */
#define ICH_LPC_PCI_FUNCNO     0x0     /* LPC PCI FuncNo */

#define ICH_GEN_CNTRL          0xd0    /* 6300ESB ICH General Control Reg */
#define ICH_RCBA_REG           0xf0    /* ICH7 Root Complex Base Addr Reg */

/* ICH (LPC I/F - D31:F0) PIRQ[n]_ROUT - PIRQ[n] Routing Control */
                                                                                
#define ICH_LPC_PIRQA          0x60    /* offset PIRQA */
#define ICH_LPC_PIRQB          0x61    /* offset PIRQB */
#define ICH_LPC_PIRQC          0x62    /* offset PIRQC */
#define ICH_LPC_PIRQD          0x63    /* offset PIRQD */
#define ICH_LPC_PIRQE          0x68    /* offset PIRQE */
#define ICH_LPC_PIRQF          0x69    /* offset PIRQF */
#define ICH_LPC_PIRQG          0x6a    /* offset PIRQG */
#define ICH_LPC_PIRQH          0x6b    /* offset PIRQH */

#define ICH_IRQ_DIS            0x80    /* ISA IRQ routing disable */
#define ICH_IRQ_MASK           0x0f    /* ISA IRQ routing mask */

/* externs */

IMPORT UINT32 ioApicIntrDrvInitializedCount;

IMPORT UINT8  glbMpApicIoBaseId; /* base IO Apic Id */

IMPORT UINT32 glbMpApicNioint;  /* number of io interrupts (MP Table) */
IMPORT UINT32 glbMpApicNloint;  /* number of local interrupts (MP Table) */

IMPORT MP_INTERRUPT **glbMpApicInterruptTable;

IMPORT UINT8 ioApicRedNumEntries;

/* globals */

typedef struct _PIRQ_ENABLE_ARG
    {
    BOOL enable;
    } PIRQ_ENABLE_ARG;

VOID sysPciPirqEnable (BOOL enable);

/* locals */

/*
 * These globals (glbMpApicNioint, glbMpApicNloint, glbMpApicInterruptTable) 
 * are used in pciConfigIntStub.c, they avoid calling
 * through vxbus and reduces overhead, potential spinLock nesting...
 *
 * They are only present for INCLUDE_SYMMETRIC_IO_MODE, used with IO APIC.
 */

/* store PCI bridge device information */

struct  pciBridgeDevice
    {
    int bus;      /* bridge bus number */
    int dev;
    int func;
    int priBus;   /* priBus bus number */
    int secBus;   /* child bsu number*/
    int pin;      /* bridge's int pin */
    int type;     /* P2P  or Card Bus */
    struct  pciBridgeDevice *pNext;  /* next bridge pointer */
    };

/* pci bridge device list pointer */

struct pciBridgeDevice * pPciBridgeList = NULL ;

/*******************************************************************************
*
* sysPciLookMpEntry - find an interrupt entry from MP table
*
* This function is responsible for find a interrupt entry for specific entry.
*
* RETURNS: return ERROR/irq number
*
*/

int sysPciFindMpEntry
    (
    VXB_DEVICE_ID ctrl,
    int bus,
    int dev,
    int func,
    int pin,
    MP_INTERRUPT *pMpEntry
    )
    {
    UINT32 i;
    UINT8 sourceBusId, sourceBusIrq;
    int irq = 0;
    MP_INTERRUPT *pMpApicInterruptTable;

    pMpApicInterruptTable = (MP_INTERRUPT *) glbMpApicInterruptTable;

    for (i = 0; i < (glbMpApicNioint + glbMpApicNloint); i++)
        {
        irq = pMpApicInterruptTable->destApicIntIn;

        sourceBusId = pMpApicInterruptTable->sourceBusId;
        sourceBusIrq = pMpApicInterruptTable->sourceBusIrq;

        if ((bus == sourceBusId) &&
            (dev == (sourceBusIrq >> 2)) &&
            (pin == (1 + (sourceBusIrq & 0x3))))
            {

            /* adjustment for multiple IO APIC support, needs to sync with 
             * vxbIoApicIntr.c 
             */

            if ((pMpApicInterruptTable->destApicId > glbMpApicIoBaseId) &&
                (pMpApicInterruptTable->destApicId <
                (glbMpApicIoBaseId + ioApicIntrDrvInitializedCount)))
                {
                irq += (ioApicRedNumEntries * 
                        (pMpApicInterruptTable->destApicId -
                         glbMpApicIoBaseId));
                }
            bcopy ((char *)pMpApicInterruptTable, 
                   (char *)pMpEntry, 
                   sizeof (MP_INTERRUPT));
            return irq;
            }

        pMpApicInterruptTable ++;
        }

    return ERROR;
    }

/*******************************************************************************
*
* sysPciProgIrq - program a interrupt line register
*
* This function is responsible for write interrupt line register in symmetric IO mode.
*
* RETURNS: return ERROR/irq number
*
*/

void sysPciProgIrq
    (
    VXB_DEVICE_ID ctrl,
    int bus,
    int dev,
    int func,
    void * arg,
    int irq
    )
    {
    PIRQ_ENABLE_ARG * pArg = arg;
    UINT8 offset, irqroute;

    INT32 pciBusLpc     = ICH_LPC_PCI_BUSNO;   /* bus# of LPC */
    INT32 pciDevLpc     = ICH_LPC_PCI_DEVNO;   /* dev# of LPC */
    INT32 pciFuncLpc    = ICH_LPC_PCI_FUNCNO;  /* func# of LPC */

    if (irq > 0xff)
        return;

    if (pArg->enable)
        {
        vxbPciConfigOutByte (ctrl, bus, dev, func, PCI_CFG_DEV_INT_LINE,
                             irq);
        }

    switch (irq)
        {
        case IOAPIC_PIRQA_INT_LVL:
        case IOAPIC_PIRQB_INT_LVL:
        case IOAPIC_PIRQC_INT_LVL:
        case IOAPIC_PIRQD_INT_LVL:
            offset = ICH_LPC_PIRQA;
            offset += irq;
            offset -= IOAPIC_PIRQA_INT_LVL;
            break;

        case IOAPIC_PIRQE_INT_LVL:
        case IOAPIC_PIRQF_INT_LVL:
        case IOAPIC_PIRQG_INT_LVL:
        case IOAPIC_PIRQH_INT_LVL:
            offset = ICH_LPC_PIRQE;
            offset += irq;
            offset -= IOAPIC_PIRQE_INT_LVL;
            break;

        default:
            offset = ICH_LPC_PIRQA;
        }

        vxbPciConfigInByte (ctrl, (int) pciBusLpc, (int) pciDevLpc,
                           (int) pciFuncLpc, offset, (UINT8 *)&irqroute);

        if (pArg->enable)
            irqroute = irqroute | ICH_IRQ_DIS;
        else
            irqroute = irqroute & ~ICH_IRQ_DIS;

        vxbPciConfigOutByte (ctrl, (int) pciBusLpc, (int) pciDevLpc,
                            (int) pciFuncLpc, offset, irqroute);
    }

/*******************************************************************************
 *
 * sysPciFindBridge - scan and store the bridge device information
 *
 * This function is responsible for find bridge
 *
 * RETURNS: OK
 *
 */

STATUS sysPciFindBridge
    (
    VXB_DEVICE_ID ctrl,
    int bus,
    int dev,
    int func,
    void * arg
    )
    {
    UINT16 pciClass;        /* Class field of function */
    UINT8 priBus, secBus, subBus, pin;
    struct pciBridgeDevice *pList;
    static struct pciBridgeDevice *pListPri = NULL;


    pciConfigInWord (bus, dev, func, PCI_CFG_SUBCLASS, &pciClass);

    if ((pciClass == ((PCI_CLASS_BRIDGE_CTLR << 8) | 
                       PCI_SUBCLASS_P2P_BRIDGE)) ||
        (pciClass == ((PCI_CLASS_BRIDGE_CTLR << 8) | 
                       PCI_SUBCLASS_CARDBUS_BRIDGE)))
        {

        pList = 
         (struct pciBridgeDevice *)hwMemAlloc(sizeof(struct pciBridgeDevice));

        if (pList != NULL)
            {
            if (pListPri != NULL)
                {
                pListPri->pNext = pList;
                }
            }
        else
            {
            return ERROR;
            }

        vxbPciConfigInByte (ctrl, bus, dev, func, PCI_CFG_PRIMARY_BUS,
                         (UINT8 *)&priBus);
        vxbPciConfigInByte (ctrl, bus, dev, func, PCI_CFG_SECONDARY_BUS,
                         (UINT8 *)&secBus);
        vxbPciConfigInByte (ctrl, bus, dev, func, PCI_CFG_SUBORDINATE_BUS,
                         (UINT8 *)&subBus);
        vxbPciConfigInByte (ctrl, bus, dev, func, PCI_CFG_BRG_INT_PIN,
                         (UINT8 *)&pin);

        pList->bus = bus;
        pList->dev = dev;
        pList->func = func;
        pList->priBus = priBus;
        pList->secBus = secBus;
        pList->pNext = NULL;
        pList->pin = pin;
        pList->type = pciClass;

        pListPri = pList;

        if (pPciBridgeList == NULL)
            pPciBridgeList = pList;

        }

    return OK;
    }

/*******************************************************************************
*
* sysPciPirqEnable2  - scan mptable and configure device interrupt line
*
* This function is responsible configure device interrupt line base on mptable
*
* RETURNS: OK
*
*/

LOCAL STATUS sysPciPirqEnable2
    (
    VXB_DEVICE_ID ctrl,
    int           bus,
    int           dev,
    int           func,
    void        * arg
    )
    {
    UINT8  pin, pinPri;
    int irq = 0, copyBus, copyDev, copyPin;
    struct pciBridgeDevice * pList = pPciBridgeList ;
    MP_INTERRUPT mpEntry;

    vxbPciConfigInByte (ctrl, bus, dev, func, PCI_CFG_DEV_INT_PIN,
                             (UINT8 *)&pin);

    if ((pin > 4) || (pin == 0))
        return OK;

    irq = sysPciFindMpEntry (ctrl, bus, dev, func, pin, &mpEntry);

    if (irq != ERROR)
        {
        sysPciProgIrq (ctrl, bus, dev, func, arg, irq);
        return OK;
        }
    else
        {
        copyBus = bus;
        copyDev = dev;
        copyPin = pin;

        /* try to use  PCI specification  method  */

        while (pList)
            {

            /* TODO, if ARI enable dev = 0 */

            pinPri = (copyDev + (copyPin -1)) % 4;

            while (pList)
                {
                if (pList->secBus == copyBus)
                    break;
                pList = pList->pNext;
                }
            if (pList == NULL)
                return OK;

            if ((pList->type == ((PCI_CLASS_BRIDGE_CTLR << 8) |
                PCI_SUBCLASS_CARDBUS_BRIDGE)) &&
                (pList->pin != 0))
                pinPri = pList->pin;

            irq = sysPciFindMpEntry (ctrl, pList->bus, pList->dev, pList->func,
                pinPri + 1, &mpEntry);

            if (irq != ERROR)
                {
                sysPciProgIrq (ctrl, bus, dev, func, arg, irq);
                return OK;
                }

            if ((pList->bus == 0) &&
                (pList->priBus == 0))
                return OK;

            pList = pPciBridgeList;

            copyPin = pinPri;
            copyDev = pList->dev;
            copyBus = pList->bus;
            }
        }
    return OK;
    }

/***********************************************************************
*
* sysPciPirqEnable - enable or disbable PCI PIRQ direct handling
*
* This routine enables or disbales the PCI PIRQ direct handling.
*
* RETURNS: N/A
*/

VOID sysPciPirqEnable
    (
    BOOL enable         /* TRUE to enable, FALSE to disable */
    )
    {

    /* Can't use pciConfigInLong() because it depends upon the
       software device lists which haven't been initialized yet. */

    PIRQ_ENABLE_ARG arg;
    VXB_DEVICE_ID ctrl = globalBusCtrlID;

#ifdef INCLUDE_USR_MPAPIC
    INT32 oldLevel = intCpuLock();  /* LOCK INTERRUPTS */

    usrMpapicInit (FALSE, (char *) pMpApicData);

    intCpuUnlock(oldLevel);         /* UNLOCK INTERRUPTS */
#endif /* INCLUDE_USR_MPAPIC */
    arg.enable = enable;

    /* Take action based on each and every device/function/pin combination */

    vxbPciConfigForeachFunc (ctrl, 0, TRUE,
                             (VXB_PCI_FOREACH_FUNC)sysPciFindBridge, &arg);

    vxbPciConfigForeachFunc (ctrl, 0, TRUE,
                             (VXB_PCI_FOREACH_FUNC)sysPciPirqEnable2, &arg);
    }

/*******************************************************************************
 *
 * sysPciShowIrq - scan the configured PCI routing information
 *
 * This routine shows the PCI PIRQ[A-H] to IRQ[0-15] routing status
 *
 * RETURNS: N/A
 */

int sysPciShowIrq
    (
    VXB_DEVICE_ID ctrl,
    int bus,
    int dev,
    int func,
    void * arg
    )
    {
    UINT8  pin, pinPri;
    int irq = 0, copyBus, copyDev, copyPin;
    struct pciBridgeDevice * pList = pPciBridgeList ;
    MP_INTERRUPT mpEntry;

    vxbPciConfigInByte (ctrl, bus, dev, func, PCI_CFG_DEV_INT_PIN,
                       (UINT8 *)&pin);

    if ((pin > 4) || (pin == 0))
        return OK;

    irq = sysPciFindMpEntry (ctrl, bus, dev, func, pin, &mpEntry);

    if (irq != ERROR)
        {
        printf("  %2d       %2d        %2d                    %3d     %3d\r\n",
            bus, dev, pin, mpEntry.destApicId, irq);
        return OK;
        }
    else
        {
        copyBus = bus;
        copyDev = dev;
        copyPin = pin;

        /* try to use  PCI specification  method  */

        while (pList)
            {

            /* TODO, if ARI enable dev = 0 */

            pinPri = (copyDev + (copyPin -1)) % 4;

            while (pList)
                {
                if (pList->secBus == copyBus)
                    break;
                pList = pList->pNext;
                }
            if (pList == NULL)
                return OK;

            if ((pList->type == ((PCI_CLASS_BRIDGE_CTLR << 8) |
                PCI_SUBCLASS_CARDBUS_BRIDGE)) && (pList->pin != 0))
                pinPri = pList->pin;

            irq = sysPciFindMpEntry (ctrl, pList->bus, pList->dev,
                pList->func, pinPri + 1, &mpEntry);

            if (irq != ERROR)
                {
                printf(
                "  %2d       %2d        %2d                    %3d     %3d\r\n",
                bus, 
                dev, 
                pin, 
                mpEntry.destApicId, 
                irq);
                return OK;
                }

            if ((pList->bus == 0) &&
                (pList->priBus == 0))
                return OK;

            pList = pPciBridgeList;

            copyPin = pinPri;
            copyDev = pList->dev;
            copyBus = pList->bus;
            }
        }
    return OK;
    }

/*******************************************************************************
*
* sysPciPirqShow - show the PCI PIRQ[A-H] to IRQ[0-15] routing status
*
* This routine shows the PCI PIRQ[A-H] to IRQ[0-15] routing status
*
* RETURNS: N/A
*/

void sysPciPirqShow (void)
    {
    PIRQ_ENABLE_ARG arg;
    VXB_DEVICE_ID ctrl = globalBusCtrlID;

    printf("Bus_ID  Device_ID   Int#/Pin#(on the bus)ApicId ");
    printf("ApicInt(dest irq)\r\n");
    printf("======  =========   ====                 ====== ");
    printf("=======          \r\n");

    vxbPciConfigForeachFunc (ctrl, 0, TRUE,
             (VXB_PCI_FOREACH_FUNC)sysPciShowIrq, &arg);

    }

#endif  /* INCLUDE_SYMMETRIC_IO_MODE */
