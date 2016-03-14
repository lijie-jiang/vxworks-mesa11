/* gfxItlgcDrv.c - Intel(R) Graphics Controller driver */

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
24jan14,mgc  Modified for VxWorks 7 release
04nov10,m_c  Written
*/

/*
DESCRIPTION
This driver provides basic functionality to initialize and control
various Intel graphics chipsets.
*/

/* includes */
#include <vxWorks.h>
#include <vxBusLib.h>
#include <hwif/vxbus/vxbPciLib.h>

#include <gfxItlgcDrv.h>

IMPORT STATUS gfxItlgcIosDrvInstall (void);
IMPORT STATUS gfxItlgcIosDevAdd (VXB_DEVICE_ID pVxbDev);
IMPORT void gfxItlgcIosInstInit0 (VXB_DEVICE_ID pVxbDev);

BOOL gfxItlgcVxbInitialized = FALSE;

/* List of supported devices */
LOCAL struct vxbPciID gfxItlgcPciDevIDList [] = {
#if 0
    /* Nehalem QM57 */
    {0x0046, 0x8086}, /* Intel GMA HD */
    
    /* Sandy Bridge */
    {0x0102, 0x8086}, /* Intel HD Graphics SNB */
    {0x0106, 0x8086}, /* Intel HD Graphics SNB */
    {0x010a, 0x8086}, /* Intel HD Graphics SNB */
    {0x010b, 0x8086}, /* Intel HD Graphics SNB */
    {0x010e, 0x8086}, /* Intel HD Graphics SNB */
    {0x0112, 0x8086}, /* Intel HD Graphics SNB */
    {0x0116, 0x8086}, /* Intel HD Graphics SNB */
    {0x0122, 0x8086}, /* Intel HD Graphics SNB */
    {0x0126, 0x8086}, /* Intel HD Graphics SNB */
    
    /* Atom D2xxx/N2xxx (Cedarview, SGX545) */
    {0x0be0, 0x8086}, /* Intel GMA3600 */
    {0x0be1, 0x8086}, /* Intel GMA3600 */
    {0x0be2, 0x8086}, /* Intel GMA3600 */
    {0x0be3, 0x8086}, /* Intel GMA3600 */
    {0x0be4, 0x8086}, /* Intel GMA3600 */
    {0x0be5, 0x8086}, /* Intel GMA3600 */
    {0x0be6, 0x8086}, /* Intel GMA3600 */
    {0x0be7, 0x8086}, /* Intel GMA3600 */
    {0x0be8, 0x8086}, /* Intel GMA3600 */
    {0x0be9, 0x8086}, /* Intel GMA3600 */
    {0x0bea, 0x8086}, /* Intel GMA3600 */
    {0x0beb, 0x8086}, /* Intel GMA3600 */
    {0x0bec, 0x8086}, /* Intel GMA3600 */
    {0x0bed, 0x8086}, /* Intel GMA3600 */
    {0x0bee, 0x8086}, /* Intel GMA3600 */
    {0x0bef, 0x8086}, /* Intel GMA3600 */
    
    /* Mobile 945 Family (Navy Pier 945GSE) */
    {0x27a2, 0x8086}, /* Intel 945GM/GMS */
    {0x27a6, 0x8086}, /* Intel 945GM/GMS */
    {0x27ae, 0x8086}, /* Intel 945GM/GMS */
    
    /* Bearlake (Q33, Q35, G31, G33) */
    {0x29b2, 0x8086}, /* Intel GMA3100 */
    {0x29b3, 0x8086}, /* Intel GMA3100 */
    {0x29c2, 0x8086}, /* Intel GMA3100 */
    {0x29c3, 0x8086}, /* Intel GMA3100 */
    {0x29d2, 0x8086}, /* Intel GMA3100 */
    {0x29d3, 0x8086}, /* Intel GMA3100 */
    
    /* i855G */
    {0x3582, 0x8086}, /* Intel 855GM/GME */
    
    /* Atom E6xx (Tunnel Creek, SGX535) */
    {0x4108, 0x8086}, /* Intel GMA600 */
    {0x4109, 0x8086}, /* Intel GMA600 */
    {0x410a, 0x8086}, /* Intel GMA600 */
    {0x410b, 0x8086}, /* Intel GMA600 */
    {0x410c, 0x8086}, /* Intel GMA600 */
    {0x410d, 0x8086}, /* Intel GMA600 */
    {0x410e, 0x8086}, /* Intel GMA600 */
    {0x410f, 0x8086}, /* Intel GMA600 */
#endif
    /* Atom Z5xx (Menlow US15W/L "Poulsbo", SGX535) */
    {0x8108, 0x8086}, /* Intel GMA500 */
    {0x8109, 0x8086}, /* Intel GMA500 */
#if 0
    /* Atom D410, D510, D525, N4x0, N550 (Pineview) */
    {0xa001, 0x8086}, /* Intel GMA3150 N10 */
    {0xa002, 0x8086}, /* Intel GMA3150 N10 */
    {0xa011, 0x8086}, /* Intel GMA3150 N10 */
    {0xa012, 0x8086}, /* Intel GMA3150 N10 */
#endif
    {0,0}
};

/*******************************************************************************
*
* gfxItlgcInstInit0 - Initialize the Intel graphics controller
* 
* This routine is called by the vxbus framework after the driver is registered
*
* RETURNS: N/A
*/
LOCAL void gfxItlgcInstInit0
    (
    VXB_DEVICE_ID pVxbDev
    )
    {
    gfxItlgcIosInstInit0 (pVxbDev);
    }

/*******************************************************************************
*
* gfxItlgcInstInit2n - Second level of graphics controller initialization
*
* RETURNS: N/A
*/
LOCAL void gfxItlgcInstInit2n
    (
    VXB_DEVICE_ID pDev
    )
    {
    }

/*******************************************************************************
*
* gfxItlgcInstConnect - Third level of graphics controller initialization
*
* RETURNS: N/A
*/
LOCAL void gfxItlgcInstConnect
    (
    VXB_DEVICE_ID pDev
    )
    {
    gfxItlgcVxbInitialized = TRUE;
    }

/*******************************************************************************
*
* gfxItlgcDrvInstall - Install the Intel Graphics IOS driver
*
* RETURNS: N/A
*/
LOCAL STATUS gfxItlgcDrvInstall
    (
    VXB_DEVICE_ID pVxbDev,
    void * unused
    )
    {
    return gfxItlgcIosDrvInstall();
    }


/*******************************************************************************
*
* gfxItlgcDevAdd - Add the Intel Graphics IOS driver
*
* RETURNS: N/A
*/
LOCAL STATUS gfxItlgcDevAdd
    (
    VXB_DEVICE_ID pVxbDev,
    void * unused
    )
    {
    return gfxItlgcIosDevAdd(pVxbDev);
    }

/*******************************************************************************
*
* gfxItlgcInstUnlink - Uninstall Intel graphics VxBus driver
*
* RETURNS: N/A
*/
LOCAL STATUS gfxItlgcInstUnlink
    (
    VXB_DEVICE_ID pDev,
    void * unused
    )
    {
    return OK;
    }

/* Vxbus driver initialization routines */
LOCAL struct drvBusFuncs gfxItlgcVxBusFuncs0 =
    {
    gfxItlgcInstInit0,      /* devInstanceInit */
    gfxItlgcInstInit2n,     /* devInstanceInit2 */
    gfxItlgcInstConnect    /* devConnect */
    };

/* Graphics IOS installation routines */
DEVMETHOD_DEF(gfxItlgcVxbDrvInstall, "Intel Graphics IOS driver installation");
DEVMETHOD_DEF(gfxItlgcVxbDevAdd, "Intel graphics IOS device add");
DEVMETHOD_DEF(gfxItlgcVxbUnlink, "Unregister graphics vxbus driver");

LOCAL struct vxbDeviceMethod gfxItlgcVxBusMethods[] =
   {
   DEVMETHOD(gfxItlgcVxbDrvInstall, gfxItlgcDrvInstall),
   DEVMETHOD(gfxItlgcVxbDevAdd, gfxItlgcDevAdd),
   DEVMETHOD(gfxItlgcVxbUnlink, gfxItlgcInstUnlink),
   { 0, 0 }
   };

/* Vxbus driver structure */
struct vxbPciRegister gfxItlgcPciDevRegistration0 =
    {
        {
        NULL,                    /* pNext */
        VXB_DEVID_DEVICE,        /* devID */
        VXB_BUSID_PCI,           /* busID = PCI */
        VXB_VER_5_0_0,           /* vxbVersion */
        "Intel_Graphics_Driver", /* drvName */
        &gfxItlgcVxBusFuncs0,        /* pDrvBusFuncs */
        gfxItlgcVxBusMethods,       /* pMethods */
        NULL,                    /* devProbe */
        NULL                     /* pParamDefaults */
        },
    NELEMENTS(gfxItlgcPciDevIDList) - 1,
    gfxItlgcPciDevIDList
    };
