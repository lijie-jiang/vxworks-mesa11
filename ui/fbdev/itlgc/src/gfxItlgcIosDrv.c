/* gfxItlgcIosDrv.c - Intel Graphics Controller frame buffer io driver */

/*
 * Copyright (c) 2010-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
01apr15,yat  Resolve defects from static analysis run (US50633)
01oct14,yat  Resolve defects from static analysis run
24jan14,mgc  Modified for VxWorks 7 release
07oct11,m_c  Added a system to script registers' setting
26may11,m_c  Fixed setting the SLC register on non-Poulsbo chipsets. Added a
             check to avoid a crash in the VxBus library
10may11,m_c  Added support for the GMA3150
03feb11,m_c  Updated handling of splash screen
27jan11,m_c  Fixed the timing and programming of registers for LVDS displays
17jan11,m_c  Changed the method used to determine the GTT size. Added
             initialization of unused PTEs. Added code to detect the
             primary display type (sDVO or LVDS) and handle both types of
             outputs
13jan11,m_c  Fixed flickering and loss of frame problem caused by
             double-buffered registers. Added 800x600. Improved
             mode setting
16dec10,m_c  Fixed GTT initialization and added interrupt support
04nov10,m_c  Written
*/

/* includes */

#include <vxWorks.h>
#include <vxBusLib.h>
#include <hwif/vxbus/vxBus.h>
#include <hwif/vxbus/vxbPciLib.h>

#include <gfxItlgcDrv.h>
#include <gfxFbIosDrv.inl>
#include <gfxItlgc.h>

#define cacheflush()                __asm("wbinvd")

#define KILOBYTE                    (1 * 1024)
#define MEGABYTE                    (1024 * 1024)

IMPORT void sysUsDelay (int);
IMPORT STATUS gfxItlgcGetCfg0 (size_t*, char**, char**, char**, unsigned int*, unsigned int*, FUNCPTR*, FUNCPTR*);

/* List of video modes known to the driver */
LOCAL FB_VIDEO_MODE fbModesDb[] =
    {
        {"800x600",
        60, 800, 600, 32, 4*800,
        0,
        0, 0, 0, 0,
        0, 0,
        0,
        FB_VMODE_NONINTERLACED,
        _800X600X32_60,
        0,
        3},
        {"640x480",
        60, 640, 480, 32, 4*640,
        0,
        0, 0, 0, 0,
        0, 0,
        0,
        FB_VMODE_NONINTERLACED,
        _640X480X32_60,
        0,
        3},
    };

/* Graphics Mode Select conversion table */
LOCAL const char gms[8] = {0, 1, 4, 8, 16, 32, 48, 64};

/*******************************************************************************
 *
 * disable - Disable the display device
 *
 * RETURNS: N/A
 *
 */
LOCAL void disable
    (
    GFX_FBDEV*      pDev
    )
    {
    LOCK (&gfxComp);

    if (pDev->enabled)
        {
        /* Disable both A and B pipes */
        outl(pDev->mmio + PIPEACONF, inl(pDev->mmio + PIPEACONF) & ~BIT31);
        outl(pDev->mmio + PIPEBCONF, inl(pDev->mmio + PIPEBCONF) & ~BIT31);

        /* Mark device as disabled */
        pDev->enabled = FALSE;
        }

    UNLOCK (&gfxComp);
    }

/*******************************************************************************
 *
 * enable - Enable the display device
 *
 * RETURNS: N/A
 *
 */
LOCAL void enable
    (
    GFX_FBDEV*      pDev
    )
    {
    LOCK (&gfxComp);

    if (!pDev->enabled)
        {
        /* Enable pipe */
        outl(pDev->pipeConfReg, inl(pDev->pipeConfReg) | BIT31);

        /* Mark device as enabled */
        pDev->enabled = TRUE;
        }

    UNLOCK (&gfxComp);
    }

/*******************************************************************************
 *
 * gfxItlgcSetVideoModeEx - set the display mode
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
LOCAL STATUS gfxItlgcSetVideoModeEx
    (
    GFX_FBDEV*      pDev,
    FB_VIDEO_MODE*  pFbMode
    )
    {
    int             i;
    u32             mask;
    u32             pipe;           /* display pipe */
    u32*            regs;           /* registers set */
    u32*            ringBuffer;     /* ring buffer */
    u32             slc;            /* Scan Line Count register */
    u32             y0, y1;         /* scan line numbers */
    u32             op, val, cond;  /* register operation */

    /* Identify the proper registers set */
    regs = (u32*)pFbMode->flag;
    while (*regs++ != pDev->disp)
        {
        while (1)
            {
            op = *regs++;
            if (op == OP_END)
                break;
            else if (op == OP_WR_IMM)
                regs += 2;
            else if (op == OP_WAIT)
                regs += 1;
            else if (op == OP_UPD)
                regs += 3;
            else if (op == OP_CMP_EQ)
                regs += 3;
            else if (op == OP_CMP_NE)
                regs += 3;
            else if (op == OP_WR)
                regs += 1;
            }
        if (*regs == 0xffffffff)
            return ERROR;
        }

    /* Disable the device */
    disable(pDev);

    /* Initialize the base registers */
    if (pDev->disp != GFX_DISP_PORT_LVDS)
        {
        pDev->dspAddrReg = pDev->mmio + DSPAADDR;
        pDev->dspCntrReg = pDev->mmio + DSPACNTR;
        pDev->dspStrideReg = pDev->mmio + DSPASTRIDE;
        pDev->pipeConfReg = pDev->mmio + PIPEACONF;
        pDev->pipeStatReg = pDev->mmio + PIPEASTAT;
        pipe = MI_LOAD_SCAN_LINES_PIPEA;
        slc = PIPEA_SLC;
        }
    else
        {
        pDev->dspAddrReg = pDev->mmio + DSPBADDR;
        pDev->dspCntrReg = pDev->mmio + DSPBCNTR;
        pDev->dspStrideReg = pDev->mmio + DSPBSTRIDE;
        pDev->pipeConfReg = pDev->mmio + PIPEBCONF;
        pDev->pipeStatReg = pDev->mmio + PIPEBSTAT;
        pipe = MI_LOAD_SCAN_LINES_PIPEB;
        slc = PIPEB_SLC;
        }

    /* Set all video registers */
    i = 0;
    val = 0, cond = TRUE;
    while (1)
        {
        op = regs[i];
        if (op == OP_END)
            break;
        else if (op == OP_WR_IMM)
            {
            if (cond)
                outl(pDev->mmio + regs[i + 1], regs[i + 2]);
            else
                cond = TRUE;
            i += 3;
            }
        else if (op == OP_WAIT)
            {
            if (cond)
                sysUsDelay(regs[i + 1]);
            else
                cond = TRUE;
            i += 2;
            }
        else if (op == OP_UPD)
            {
            if (cond)
                {
                val = inl(pDev->mmio + regs[i + 1]);
                val &= regs[i + 2];
                val |= regs[i + 3];
                outl(pDev->mmio + regs[i + 1], val);
                }
            else
                cond = TRUE;
            i += 4;
            }
        else if (op == OP_CMP_EQ)
            {
            val = inl(pDev->mmio + regs[i + 1]);
            cond = (val & regs[i + 2]) == regs[i + 3];
            i += 4;
            }
        else if (op == OP_CMP_NE)
            {
            val = inl(pDev->mmio + regs[i + 1]);
            cond = (val & regs[i + 2]) != regs[i + 3];
            i += 4;
            }
        else if (op == OP_WR)
            {
            if (cond)
                outl(pDev->mmio + regs[i + 1], val);
            else
                cond = TRUE;
            i += 2;
            }
        }

    /* Update the start address */
    outl(pDev->dspAddrReg, pDev->frontVirtAddr - pDev->firstVirtAddr);

    /* Set the "Scan Line Count Range Compare" register and... */
    y0 = y1 = pFbMode->yres - 2;
    outl(pDev->mmio + slc, BIT31 | (y0 << 16) | y1);
    /* ...verify that it worked */
    if (inl(pDev->mmio + slc) == 0x00000000)
        {
        /* That did not work, so this is not a Poulsbo chipset. Try to do it
         * using the command stream processor */
        ringBuffer = pDev->firstVirtAddr + pDev->fbSize - GFX_PAGE_SIZE;
        if (ringBuffer == NULL)
            {
            return ERROR;
            }
        bzero ((char *)ringBuffer, GFX_PAGE_SIZE);

        /* Disable the ring buffer */
        outl(pDev->mmio + PRB0_CTL, 0x00000000);

        /* Initialize the ring buffer (the documentation says that the
         * LOAD_SCAN_LINES commands always come in pairs) */
        i = 0;
        ringBuffer[i++] = MI_LOAD_SCAN_LINES_INCL | pipe;
        ringBuffer[i++] = (y0 << 16) + y1;
        ringBuffer[i++] = MI_LOAD_SCAN_LINES_INCL | pipe;
        ringBuffer[i++] = (y0 << 16) + y1;
        ringBuffer[i++] = MI_FLUSH;
        ringBuffer[i++] = MI_NOOP;

        /* Enable the primary ring buffer */
        outl(pDev->mmio + PRB0_TAIL, i * 4);
        outl(pDev->mmio + PRB0_HEAD, 0);
        outl(pDev->mmio + PRB0_START, (u32)ringBuffer - (u32)pDev->firstVirtAddr);
        outl(pDev->mmio + PRB0_CTL, (0 << 12) | /* Buffer Length */
                                    (1 << 0));  /* Ring Buffer Enable */

        /* Busy wait until done */
        i = 10000;
        while (i-- > 0)
            {
            if (inl(pDev->mmio + PRB0_HEAD) == inl(pDev->mmio + PRB0_TAIL))
                break;
            }

        /* Disable the ring buffer */
        outl(pDev->mmio + PRB0_CTL, 0x00000000);

        /* Verify that it worked */
        if (inl(pDev->mmio + slc) != (BIT31 | (y0 << 16) | y1))
            {
            return ERROR;
            }
        }

    /* Setup interrupts */
    mask = BIT19 |  /* MSVDX */
           BIT18 |  /* Thalia */
           BIT17 |  /* display */
           BIT15 |  /* master error */
           BIT11 |  /* display plane A flip pending */
           BIT10 |  /* display plane B flip pending */
           BIT9  |  /* overlay flip pending */
           BIT7  |  /* display pipe A VBLank */
           BIT5  |  /* display pipe B VBLank */
           BIT0;    /* ASLE */
    outl(pDev->mmio + HWSTAM, mask);
    outl(pDev->mmio + IMR,  mask);
    outl(pDev->mmio + IER, BIT6 |   /* display pipe A event */
                           BIT4);   /* display pipe B event */
    outl(pDev->mmio + EIR, 0);
    outl(pDev->mmio + EMR, BIT4);   /* page table error */

    /* Enable the device */
    enable(pDev);

    return OK;
    }

/*******************************************************************************
 *
 * gfxItlgcSetFbAddr - set frame buffer address
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
LOCAL STATUS gfxItlgcSetFbAddr
    (
    GFX_FBDEV*      pDev
    )
    {
    outl(pDev->dspStrideReg, pDev->fbMode.stride);
    outl(pDev->dspAddrReg,
    pDev->frontVirtAddr - pDev->firstVirtAddr);
    outl(pDev->dspCntrReg, inl(pDev->dspCntrReg));
    outl(pDev->dspAddrReg, inl(pDev->dspAddrReg));
    if (!pDev->fbMode.sync)
        (void)taskDelay (1);

    return OK;
    }

#if defined(GFX_VSYNC_VXBUS_IRQ)
/*******************************************************************************
 *
 * gfxItlgcIntHandlerVsync - Display interrupt handler
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
LOCAL void gfxItlgcIntHandlerVsync
    (
    GFX_FBDEV*      pDev
    )
    {
    u32             iir;    /* Interrupt Identity Register */
    u32             status; /* display pipe status */

    /* Load the IIR (only keep the events we can handle) */
    iir = inl(pDev->mmio + IIR) & (BIT6 | BIT4);

    /* Check for Line Compare event */
    if (iir != 0)
        {
        /* Clear the interrupt */
        status = inl(pDev->pipeStatReg) & ~PIPESTAT_MASK;
        outl(pDev->pipeStatReg, status | BIT8);
        inl(pDev->pipeStatReg);
        outl(pDev->mmio + IIR, iir);

        /* Swap frame buffer as needed */
        if (pDev->needSwap)
            {
            pDev->whenSwap--;
            if (pDev->whenSwap <= 0)
                {
                pDev->needSwap = FALSE;
                pDev->setFbAddrFuncPtr (pDev);
                (void)semGive ((SEM_ID)pDev->vsync);
                }
            }
        else if (pDev->needVsync)
            {
            pDev->needVsync = FALSE;
            (void)semGive ((SEM_ID)pDev->vsync);
            }
        }
    }
#endif

/*******************************************************************************
 *
 * gfxItlgcIosDrvInstall - Install the display device driver
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
STATUS gfxItlgcIosDrvInstall
    (
    void
    )
    {
    return drvInstall();
    }

/*******************************************************************************
 *
 * gfxItlgcIosDevAdd - Initialize the display device
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
STATUS gfxItlgcIosDevAdd
    (
    VXB_DEVICE_ID   pVxbDev
    )
    {
    GFX_FBDEV*      pDev = (GFX_FBDEV*)(pVxbDev->pDrvCtrl);

    if (ERROR == devAdd (pDev))
        {
        (void)fprintf (stderr, "Unable to add driver\n");
        drvCleanup (pDev);
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
 *
 * gfxItlgcIosInstInit0 - Initialize the Intel graphics controller
 *
 * This routine is called by the vxbus framework after the driver is registered
 *
 * RETURNS: N/A
 *
 */
void gfxItlgcIosInstInit0
    (
    VXB_DEVICE_ID pVxbDev
    )
    {
    int                 apertureSize = 0; /* aperture size */
    int                 display = 0;      /* display port */
    u16                 gc = 0;           /* Graphics Control register */
    u32                 gmem = 0;         /* graphis memory base address */
    u32                 gtt = 0;          /* GTT virtual address */
    u32                 gttSize = 0;      /* size of the GTT */
    int                 i = 0;
    UINT32              mmio = 0;         /* MMIO base */
    u8                  msac = 0;         /* Multi Size Aperture Control */
    int                 n = 0;            /* number of pages in the video memory */
    GFX_FBDEV*          pDev = NULL;      /* device structure */
    u32                 pte = 0;          /* Page Table Entry */
    u32                 smem = 0;         /* base address of stolen memory */
    u32                 smemSize = 0;     /* size of stolen memory */
    u32                 sysGtt = 0;       /* physical address of the GTT */
    PCI_HARDWARE*       pPciHw = NULL;

    pPciHw = pVxbDev->pBusSpecificDevInfo;
#ifdef DEBUG
    kprintf ("PCI Information\n" \
                   "       Device Id:          0x%x\n" \
                   "       Vendor Id:          0x%x\n" \
                   "       Bus Number:         0x%x\n" \
                   "       Device Number:      0x%x\n" \
                   "       Function number:    0x%x\n",
            pPciHw->pciDevId, pPciHw->pciVendId, pPciHw->pciBus, pPciHw->pciDev, pPciHw->pciFunc);
#endif
    /*
     * Read the Graphics Control Register from the PCI header to determine if graphics chip is enabled.
     * This is a read only bit
     */
    VXB_PCI_BUS_CFG_READ(pVxbDev, PCI_GC, 2, gc);

    if ((gc & PCI_GC_VD) != 0)
        {
        (void)fprintf (stderr, "Video controller is disabled\n");
        return;
        }

    /*
     * Enable I/O space, Memory Space, and Bus mastering
     */
    VXB_PCI_BUS_CFG_WRITE(pVxbDev, PCI_PCICMD, 2, 0x0007);

    /*
     * Figure out the H/W configuration - MMIO base address
     * This is where the Intel Graphics Media Adapter
     * registers are located. Note that we are only reading
     * this register which assumes the bios or os has already
     * set it up.
     */
    VXB_PCI_BUS_CFG_READ(pVxbDev, PCI_MEM_BASE, 4, mmio);

    /*
     * The base address is aligned to 1 MB (the base address
     * is contained in bits 19-31) and it is 512 KB
     * in size.
     */
    mmio &= 0xfff80000;

    /*
     * Read the MSAC register - Multi Size Aperture Control
     * This register determines the size of the graphics memory
     * aperture. According the docs, typically the bios sets
     * this up
     */
    VXB_PCI_BUS_CFG_READ(pVxbDev, PCI_MSAC, 1, msac);

    if ((msac & PCI_MSAC_UAS_MASK) == 2)
        apertureSize = 256 * MEGABYTE;
    else
        apertureSize = 128 * MEGABYTE;

    /*
     * Read the GMEM_BASE register which provides the
     * base address of the memory aperture in MMIO space
     * (this seems to be broken on Poulsbo)
     */
    VXB_PCI_BUS_CFG_READ(pVxbDev, PCI_GMEM_BASE, 4, gmem);

    /*
     * Read the BSM register to determine the base address
     * of stolen memory. The size of stolen memory comes
     * from bits 4 - 6 of the GC
     */
    VXB_PCI_BUS_CFG_READ(pVxbDev, PCI_BSM, 4, smem);

    smem &= 0xfffffff0;
    smemSize = gms[(gc & 0x0070) >> 4] * MEGABYTE;
#ifdef DEBUG
    kprintf (" mmio:          0x%08x\n" \
                   " aperture:      %d MiB\n", mmio, apertureSize / MEGABYTE);
    kprintf (" stolen memory: %d MiB @ 0x%08x\n", smemSize / MEGABYTE, smem);
#endif
    /* Use the current state to determine the type of output */
    /* - north display */
    if (inl(mmio + SDVOB_PORT_CTRL) & BIT31)
        {
        display = GFX_DISP_PORT_SDVO_B;
        }
    else if (inl(mmio + SDVOC_PORT_CTRL) & BIT31)
        {
        display = GFX_DISP_PORT_SDVO_C;
        }
    else if (inl(mmio + LVDS_PORT_CTRL) & BIT31)
        {
        display = GFX_DISP_PORT_LVDS;
        }
    else if (inl(mmio + ADPA_PORT_CTRL) & BIT31)
        {
        display = GFX_DISP_PORT_ANALOG;
        }
    /* - south display */
    else if (inl(mmio + PCH_HDMI_CTL) & BIT31)
        {
        display = GFX_DISP_PORT_PCH_HDMI;
        }
    else if (inl(mmio + PCH_LVDS) & BIT31)
        {
        display = GFX_DISP_PORT_PCH_LVDS;
        }
    else if (inl(mmio + PCH_DAC_CTL) & BIT31)
        {
        display = GFX_DISP_PORT_PCH_CRT;
        }
    else
        {
        (void)fprintf (stderr, "Unable to identify display port\n");
        return;
        }

    /* Initialize the GTT (Graphics Translation Table) */
    /* - get the size and addresses */
    VXB_PCI_BUS_CFG_READ(pVxbDev, PCI_GTT_BASE, 4, gtt);
    VXB_PCI_BUS_CFG_WRITE(pVxbDev, PCI_GMEM_BASE, 4, 0xffffffff);
    VXB_PCI_BUS_CFG_READ(pVxbDev, PCI_GMEM_BASE, 4, gttSize);
    VXB_PCI_BUS_CFG_WRITE(pVxbDev, PCI_GMEM_BASE, 4, gmem);

    gttSize = (~(gttSize & 0xfffffff0) + 1) / KILOBYTE;
    sysGtt = inl(mmio + PGTBL_CTL) & GFX_PAGE_MASK;
    if (sysGtt == 0x00000000)
        {
        if (gttSize <= smemSize)
            sysGtt = smem;
        else
            {
            sysGtt = (u32)memalign(GFX_PAGE_SIZE, gttSize);
            if (sysGtt == 0x00000000)
                {
                return;
                }
            }
        }

    /* - map/remap */
    if (ERROR == vmPageMap (0, sysGtt, sysGtt, gttSize, GTT_STATE_MASK, GTT_STATE))
        {
        (void)fprintf (stderr, "Unable to map memory\n");
        return;
        }
#ifdef DEBUG
    kprintf (" GTT:           %d KiB @ 0x%08x\n", gttSize / KILOBYTE, gtt);
    kprintf ("  can map %d MiB\n" \
                "  in system RAM @ 0x%08x\n", gttSize / KILOBYTE, sysGtt);
#endif
    pDev = drvInit (gfxItlgcGetCfg0, apertureSize);
    if (pDev == NULL)
        return;

    if (OK == gfxStrcmp (pDev->displayName, "default"))
        pDev->disp = display;
    else
        {
        (void)fprintf (stderr, "Invalid display %s\n", pDev->displayName);
        drvCleanup (pDev);
        return;
        }

    /* - enable */
    outl(mmio + PGTBL_CTL, sysGtt | 0x00000001);
    /* - initialize the PTEs */
    n = pDev->fbSize / GFX_PAGE_SIZE;

    pte = (unsigned int)pDev->firstVirtAddr;
    for (i = 0; i < n; i++)
        {
        outl(gtt, pte | PTE_VALID_BIT);
        gtt += sizeof(u32);
        pte += GFX_PAGE_SIZE;
        }
    for (i = (gttSize / 4) - n; i > 0; i--)
        {
        outl(gtt, 0x00000000);
        gtt += sizeof(u32);
        }
    cacheflush();

    (void) vmStateSet (NULL, (VIRT_ADDR)pDev->firstVirtAddr, pDev->fbSize,
                       GFX_VM_STATE_MASK, GFX_VM_STATE);

    /* Setup the device */
    pDev->mmio = mmio;
    pDev->apertureSize = apertureSize;

    pDev->fbModesDb = fbModesDb;
    pDev->fbModesCount = NELEMENTS(fbModesDb);
    pDev->setVideoModeExFuncPtr = gfxItlgcSetVideoModeEx;
    pDev->setFbAddrFuncPtr = gfxItlgcSetFbAddr;
#if defined(GFX_VSYNC_VXBUS_IRQ)
    pDev->intHandlerVsyncFuncPtr = gfxItlgcIntHandlerVsync;
#endif
    pDev->vxbDev = pVxbDev;
    pVxbDev->pDrvCtrl = (void *)pDev;
    }
