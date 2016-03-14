/* gfxItlgcDrv.h - Intel Graphics Controller frame buffer driver header file */

/*
 * Copyright (c) 2010-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
22dec14,yat  Rework pmap of frame buffer for RTP restart (US46449)
24jan14,mgc  Modified for VxWorks 7 release
04nov10,m_c Written
*/

#ifndef __INC_gfxItlgcDrv_h
#define __INC_gfxItlgcDrv_h

#include <vxWorks.h>
#include <ioLib.h>
#include <semLib.h>
#include <fbdev.h>

/* Disable Vsync interrupt
#define GFX_VSYNC_VXBUS_IRQ
*/

typedef enum gfxDisp
    {
    GFX_DISP_PORT_SDVO_B,
    GFX_DISP_PORT_SDVO_C,
    GFX_DISP_PORT_LVDS,
    GFX_DISP_PORT_ANALOG,
    GFX_DISP_PORT_PCH_HDMI,
    GFX_DISP_PORT_PCH_LVDS,
    GFX_DISP_PORT_PCH_CRT
    } GFX_DISP;

/* Graphics Translation Table MMU state mask and state */
#define GTT_STATE                   (VM_STATE_VALID |           \
                                     VM_STATE_WRITABLE |        \
                                     VM_STATE_CACHEABLE_NOT |   \
                                     VM_STATE_GLOBAL_NOT)
#define GTT_STATE_MASK              (VM_STATE_MASK_VALID |      \
                                     VM_STATE_MASK_WRITABLE |   \
                                     VM_STATE_MASK_WBACK |      \
                                     VM_STATE_MASK_GLOBAL)

#define GFX_VM_STATE                (VM_STATE_VALID |           \
                                     VM_STATE_WRITABLE |        \
                                     VM_STATE_CACHEABLE_NOT |   \
                                     VM_STATE_GLOBAL)
#define GFX_VM_STATE_MASK           (VM_STATE_MASK_VALID |      \
                                     VM_STATE_MASK_WRITABLE |   \
                                     VM_STATE_MASK_WBACK |      \
                                     VM_STATE_MASK_GLOBAL)

/* PCI registers */
#define PCI_PCICMD                  0x00000004  /* 16 */
#define PCI_REV_ID                  0x00000008  /*  8 */
#define PCI_BAR0                    0x00000010  /* 32 */
#define PCI_BAR1                    0x00000014  /* 32 */
#define PCI_INT_LINE                0x0000003c  /*  8 */

#define PCI_MEM_BASE                0x00000010  /* 32 */
#define PCI_IOBAR                   0x00000014  /* 32 */
#define PCI_GMEM_BASE               0x00000018  /* 32 */
#define PCI_GC                      0x00000052  /* 32 */
#define   PCI_GC_VD                   0x0002
#define   PCI_GC_GMCH_EN              0x0004
#define   PCI_GC_GMS                  0x0070
#define PCI_GTT_BASE                0x0000001c  /* 32 */
#define PCI_BSM                     0x0000005c  /* 32 */
#define PCI_MSAC                    0x00000062  /*  8 */
#define   PCI_MSAC_UAS_MASK           0x03

/* Page Table Entry flags */
#define PTE_VALID_BIT               0x00000001
#define PTE_SNOOP                   0x00000006

/* Basic types */
typedef unsigned char               u8;
typedef unsigned short              u16;
typedef unsigned int                u32;

/* Frame buffer display device */
typedef struct
    {
    DEV_HDR                         header;         /* I/O subsystem header */
    int                             enabled;
    /* Hardware configuration */
    uintptr_t                       mmio;           /* memory mapped registers */
    int                             apertureSize;   /* aperture size */
    u32                             dspAddrReg;     /* registers */
    u32                             dspCntrReg;
    u32                             dspStrideReg;
    u32                             pipeConfReg;
    u32                             pipeStatReg;
    /* Video mode */
    GFX_DISP                        disp;           /* display */
    FB_VIDEO_MODE                   fbMode;         /* video mode */
    FB_VIDEO_MODE*                  fbModesDb;
    ULONG                           fbModesCount;
    ULONG                           fbSize;
#if !defined(GFX_USE_PMAP)
    BOOL                            fbRtpFreeMem;
#endif
    /* Internal data */
    void*                           firstPhysAddr;  /* first buffer address */
    void*                           firstVirtAddr;  /* first buffer address */
    void*                           frontPhysAddr;  /* front buffer address */
    void*                           frontVirtAddr;  /* front buffer address */
#if defined(GFX_VSYNC_VXBUS_IRQ)
    BOOL                            needSwap;
    BOOL                            whenSwap;
    VX_BINARY_SEMAPHORE             (vsync);        /* VBL sync semaphore */
    BOOL                            needVsync;
    VOIDFUNCPTR                     intHandlerVsyncFuncPtr;
#endif
    FUNCPTR                         setVideoModeExFuncPtr;
    FUNCPTR                         setFbAddrFuncPtr;
    char                            deviceName[FB_MAX_STR_CHARS];
    char                            displayName[FB_MAX_STR_CHARS];
    char                            modeStr[FB_MAX_VIDEO_MODE_LEN];
    /* VxBus device ids  */
    VXB_DEVICE_ID                   vxbDev;         /* device */
    } GFX_FBDEV;

static __inline__ u32   inl(u32 reg)
    {
    return (*((const volatile u32*)reg));
    }

static __inline__ void  outl(u32 reg, u32 val)
    {
    *((volatile u32*)reg) = val;
    }

#endif /* __INC_gfxItlgcDrv_h */
