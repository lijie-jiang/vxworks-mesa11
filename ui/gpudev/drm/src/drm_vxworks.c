/*
 * Copyright (c) 2014-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
20oct15,qsn  Add support for select (US68597)
15jul15,yat  Clean up code (US60452)
22jan15,qsn  Port DRM to VxWorks 7 (US50612)
*/

#include <stdio.h> /* for snprintf */
#include <string.h> /* for memcpy */
#include <ctype.h> /* for isdigit */
#include <iosLib.h> /* for iosDevAdd */
#include <semLib.h> /* for semCCreate */
#include <mod_devicetable.h> /* for pci_device_id */
#include <vxoal/krnl/device.h> /* for file_operations, irq_handler_t */
#include <vxoal/krnl/time.h> /* for gfxDrmTimerShow */
#include <vxoal/krnl/pci.h> /* for pcidev_add_entry, pcidev_find_entry */
#include <vxoal/krnl/list.h> /* for list_for_each_entry_safe */
#include <drm/drm_vxworks.h> /* for struct drm_vxdev */
#include <drm/drmP.h> /* for DRM_MINOR_CNT, drm_driver */
#define _WRS_CONFIG_VXBUS_LEGACY
#if 0
#define DEBUG_DRM_VXWORKS() pr_err("\n")
#else
#define DEBUG_DRM_VXWORKS()
#endif

typedef struct
    {
    VXB_DEV_ID                      vxbDevId;
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    VXB_RESOURCE                   *pResIrq;
#endif
    struct file_operations         *f_op;
    VOIDFUNCPTR                     showDrmMmapFuncPtr;
    VOIDFUNCPTR                     showDrmPciFuncPtr;
    VOIDFUNCPTR                     showDrmTimerFuncPtr;
    VOIDFUNCPTR                     showDrmVxDevFuncPtr;
    VOIDFUNCPTR                     showDrmDevPrivateFuncPtr; 
    struct drm_vxdev                drmVxDevs[DRM_MINOR_CNT];
    } GFX_COMP;

static GFX_COMP gfxComp =
   {
   .vxbDevId = NULL,
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
   .pResIrq = NULL,
#endif
   .f_op = NULL,
   .showDrmPciFuncPtr = NULL,
   .showDrmTimerFuncPtr = NULL,
   .showDrmVxDevFuncPtr = NULL,
   .showDrmDevPrivateFuncPtr = NULL
   };

static int drmDrvNum = ERROR;
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
static STATUS pciDrmDevProbe (VXB_DEV_ID);
static STATUS pciDrmDevAttach (VXB_DEV_ID);
#else
LOCAL BOOL pciDrmDevProbe (VXB_DEV_ID);
#endif

#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
static VXB_DRV_METHOD pciDrmDevMethodList[] =
    {
    /* DEVICE API */
    { VXB_DEVMETHOD_CALL(vxbDevProbe), pciDrmDevProbe },
    { VXB_DEVMETHOD_CALL(vxbDevAttach), pciDrmDevAttach },
    { 0, NULL }
    };

VXB_DRV vxbPciDrmDevDrv =
    {
    { NULL } ,
    "pciDrmDev",                          /* Name */
    "PCI DRM device driver",              /* Description */
    VXB_BUSID_PCI,                        /* Class */
    0,                                    /* Flags */
    0,                                    /* Reference count */
    pciDrmDevMethodList                   /* Method table */
    };

VXB_DRV_DEF(vxbPciDrmDevDrv)
#else
LOCAL struct vxbDeviceMethod drmMethods[] =
   {
   { 0, 0 }
   };

LOCAL void drmInstInit (VXB_DEVICE_ID);
LOCAL void drmInstInit2 (VXB_DEVICE_ID);
LOCAL void drmInstConnect (VXB_DEVICE_ID);

LOCAL struct vxbPciID drmPciDevIDList[] =
    {   
		{ 0xFFFF, 0xFFFF }
    };

LOCAL struct drvBusFuncs drmFuncs =
    {
    drmInstInit,	/* devInstanceInit */
    drmInstInit2,	/* devInstanceInit2 */
    drmInstConnect	/* devConnect */
    };

LOCAL struct vxbPciRegister DrmiDevPciRegistration =
    {
        {
        NULL,			/* pNext */
        VXB_DEVID_DEVICE,	/* devID */
        VXB_BUSID_PCI,		/* busID = PCI */
        VXB_VER_5_0_0,  	/* vxbVersion */
        "pciDrmDev",		/* drvName */
        &drmFuncs,		/* pDrvBusFuncs */
        NULL,		/* pMethods */
        pciDrmDevProbe,			/* devProbe */
        },
    NELEMENTS(drmPciDevIDList),
    &drmPciDevIDList[0],
    };
/*******************************************************************************
*
* drmDevicePciRegister - register sdhcStorage driver
*
* This routine registers the sdhcStorage driver with the vxbus subsystem.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

void drmDevicePciRegister (void)
    {
    vxbDevRegister (&DrmiDevPciRegistration);
    }

/*******************************************************************************
*
* drmInstInit - first level initialization routine of drm device
*
* This routine performs the first level initialization of the drm device.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void drmInstInit
    (
    VXB_DEVICE_ID pInst
    )
    {
    /* get the next available unit number */

    if (pInst->busID == VXB_BUSID_PCI)
        vxbNextUnitGet (pInst);
    }

/*******************************************************************************
*
* drmInstInit2 - second level initialization routine of drm device
*
* This routine performs the second level initialization of the drm device.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void drmInstInit2
    (
    VXB_DEVICE_ID pInst
    )
    { 
    unsigned int i=0;
     for (i = 0; i < VXB_MAXBARS; i++)
        {
        if (pInst->regBaseFlags[i] == VXB_REG_MEM)
            break;
        }
	  if (i == VXB_MAXBARS)
        return;
	
	    
	 gfxComp.vxbDevId = pInst;
 
    return;
    }

/*******************************************************************************
*
* drmInstConnect - third level initialization routine of sdhc device
*
* This routine performs the third level initialization of the sdhc device.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void drmInstConnect
    (
    VXB_DEVICE_ID pInst
    )
    {
	/*pcidev_add_entry (pInst);*/
    return;
    }


#endif

#if !defined(_WRS_CONFIG_VXBUS_LEGACY)

/*******************************************************************************
 *
 * pciDrmDevProbe - probe for PCI DRM device
 *
 * This routine probes for PCI DRM device.
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
static STATUS pciDrmDevProbe
    (
    VXB_DEV_ID      vxbDevId
    )
    {
    const struct pci_device_id *pciidlist;
    PCI_HARDWARE *pPciDev;

    if (vxbDevId == NULL)
        return ERROR;

    pPciDev = (PCI_HARDWARE *)vxbDevIvarsGet(vxbDevId);
    if (pPciDev == NULL)
        return ERROR;

    pciidlist = gfxDrmDevPciList();
    if (pciidlist == NULL)
        return ERROR;

    pcidev_add_entry (vxbDevId);

    while ((pciidlist->vendor != 0) &&
           (pciidlist->device != 0))
        {
        if ((pciidlist->vendor == pPciDev->pciVendId) &&
            (pciidlist->device == pPciDev->pciDevId))
            {
            pr_info("PCI device found: pciVendId=0x%x, pciDevId=0x%x\n", pPciDev->pciVendId, pPciDev->pciDevId);
            return OK;
            }
        pciidlist++;
        }

    return ERROR;
    }

/*******************************************************************************
 *
 * pciDrmDevAttach - attach PCI DRM device
 *
 * This routine attaches for PCI DRM device.
 *
 * RETURNS: OK
 *
 */
static STATUS pciDrmDevAttach
    (
    VXB_DEV_ID      vxbDevId
    )
    {
    gfxComp.vxbDevId = vxbDevId;

    return OK;
    }
#else
LOCAL BOOL pciDrmDevProbe
    (
    VXB_DEVICE_ID pDev
    )
    {
    UINT16 devId = 0;
    UINT16 vendorId = 0;
    UINT32 classValue = 0;
    const struct pci_device_id *pciidlist;
    VXB_PCI_BUS_CFG_READ (pDev, PCI_CFG_VENDOR_ID, 2, vendorId);
    VXB_PCI_BUS_CFG_READ (pDev, PCI_CFG_DEVICE_ID, 2, devId);
    VXB_PCI_BUS_CFG_READ (pDev, PCI_CFG_REVISION, 4, classValue);

    /*AMD*/
    if (pDev == NULL)
        return ERROR;

    pciidlist = gfxDrmDevPciList();
    if (pciidlist == NULL)
        return ERROR;

    pcidev_add_entry (pDev);

    while ((pciidlist->vendor != 0) &&
           (pciidlist->device != 0))
        {
        if ((pciidlist->vendor == vendorId) &&
            (pciidlist->device == devId))
            {
            pr_info("PCI device found: pciVendId=0x%x, pciDevId=0x%x\n", vendorId, devId);
            return TRUE;
            }
        pciidlist++;
        }
      return (FALSE);
    }

#endif

/*******************************************************************************
 *
 * device_add - adds a device
 *
 * This routine adds a device to the VxWorks kernel I/O system.
 *
 * RETURNS: 0 on success; -EIO otherwise.
 *
 */
int device_add
    (
    struct device *dev
    )
    {
    struct drm_minor *minor;
    char *minor_str;
    struct drm_vxdev* pDev;

    DEBUG_DRM_VXWORKS();

    minor = dev_get_drvdata(dev);
    if (!minor)
        {
        return -EIO;
        }

    if (minor->type == DRM_MINOR_CONTROL)
        minor_str = "controlD";
    else if (minor->type == DRM_MINOR_RENDER)
        minor_str = "renderD";
    else
        minor_str = "card";

    pDev = &(gfxComp.drmVxDevs[minor->type]);
    (void) snprintf (pDev->name, VX_DRM_MAX_DEV_NAME,
                     "/dev/dri/%s%d", minor_str, minor->index);

    /* Add the device to the kernel I/O system */
    if (iosDevAdd((DEV_HDR*)pDev, pDev->name, drmDrvNum) != OK)
        {
        pr_err("Unable to add device to ios\n");
        return -EIO;
        }

    pDev->inode.i_rdev = minor->index;
    pDev->dev = minor->dev;
    selWakeupListInit (&(pDev->dev->selWakeupList));

    return 0;
    }

/*******************************************************************************
 *
 * drm_vx_open - open the drm device
 *
 * RETURNS device structure
 *
 */
static void* drm_vx_open
    (
    DEV_HDR *       pDevHdr,
    const char *    name,
    int             flags,
    int             mode
    )
    {
    struct drm_vxdev* pDev = (struct drm_vxdev*)pDevHdr;

    DEBUG_DRM_VXWORKS();

    /* Check arguments */
    if ((pDev == NULL) || (pDev->filp.f_op == NULL))
        {
        errno = EFAULT;
        return (void*)ERROR;;
        }

    pDev->filp.f_op->open(&(pDev->inode), &(pDev->filp));

    errno = 0;
    return (void*)pDev;
    }

/*****************************************************************************
 *
 * drm_vx_close - close the drm device
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
static int drm_vx_close
    (
    void*      pFileDesc
    )
    {
    struct drm_vxdev* pDev = (struct drm_vxdev*) pFileDesc;

    DEBUG_DRM_VXWORKS();
 
    /* Check arguments */
    if (pDev == NULL)
        {
        errno = EFAULT;
        return ERROR;
        }

    errno = 0;
    return OK;
    }

/*******************************************************************************
 *
 * drm_vx_read - handle read
 *
 * RETURNS: The numbers of bytes read if successful, otherwise -1
 *
 */
static ssize_t drm_vx_read
    (
    void*           pFileDesc,
    char*           pBuf,
    size_t          maxBytes
    )
    {
    struct drm_vxdev* pDev = (struct drm_vxdev*) pFileDesc;

    DEBUG_DRM_VXWORKS();

    /* Check arguments */
    if ((pDev == NULL) || (pDev->filp.f_op == NULL))
        {
        errno = EFAULT;
        return ERROR;
        }

    return pDev->filp.f_op->read (&(pDev->filp), pBuf, maxBytes, NULL);
    }

/*******************************************************************************
 *
 * drm_vx_ioctl - perform driver specific operations
 *
 * RETURNS: OK if successful, otherwise ERROR
 *
 */
static int drm_vx_ioctl
    (
    void*           pFileDesc,
    int             request,
    _Vx_ioctl_arg_t arg
    )
    {
    struct drm_vxdev* pDev = (struct drm_vxdev*) pFileDesc;
    struct file *filp;
    struct drm_file *file_priv;

    DEBUG_DRM_VXWORKS();

    /* Check arguments */
    if ((pDev == NULL) || (pDev->filp.f_op == NULL) || (pDev->filp.private_data == NULL))
        {
        errno = EFAULT;
        return ERROR;
        }

    filp = &(pDev->filp);
    file_priv = (struct drm_file *)filp->private_data;
    errno = 0;
    switch (request)
        {
        case FIOSELECT:
            if (NULL == (SEL_WAKEUP_NODE *)arg)
                {
                errno = EINVAL;
                return ERROR;
                }

            /* add node to wakeup list */
            if (ERROR == selNodeAdd (&(pDev->dev->selWakeupList),
                                     (SEL_WAKEUP_NODE *) arg))
                {
                errno = ENOMEM;
                return ERROR;
                }

            if ((selWakeupType ((SEL_WAKEUP_NODE *) arg) == SELREAD)
                && (!list_empty(&file_priv->event_list)))
                {
                /* data available, make sure task does not pend */
                selWakeup ((SEL_WAKEUP_NODE *) arg);
                }
            return OK;

        case FIOUNSELECT:
            if (NULL == (SEL_WAKEUP_NODE *)arg)
                {
                errno = EINVAL;
                return ERROR;
                }

            /* delete node from wakeup list */
            if (ERROR == selNodeDelete (&(pDev->dev->selWakeupList),
                                        (SEL_WAKEUP_NODE *) arg))
                {
                errno = EINVAL;
                return ERROR;;
                }
            return OK;

        default:
            break;
        }

    errno = abs((int)pDev->filp.f_op->unlocked_ioctl (filp, request, arg));
    if (errno != 0)
        {
        return ERROR;
        }
    return OK;
    }

/*******************************************************************************
 *
 * drm_vx_install - install the drm driver
 *
 * RETURNS: OK on success, ERROR otherwise
 *
 */
int drm_vx_install
    (
    struct file_operations *fops
    )
    {
    DEBUG_DRM_VXWORKS();

    gfxComp.f_op = fops;

    drmDrvNum = iosDrvInstall ((DRV_CREATE_PTR)NULL,          /* creat() */
                               (DRV_REMOVE_PTR)NULL,          /* remove() */
                               (DRV_OPEN_PTR)drm_vx_open,     /* open() */
                               (DRV_CLOSE_PTR)drm_vx_close,   /* close() */
                               (DRV_READ_PTR)drm_vx_read,     /* read() */
                               (DRV_WRITE_PTR)NULL,           /* write() */
                               (DRV_IOCTL_PTR)drm_vx_ioctl);  /* ioctl() */
    if (drmDrvNum == ERROR)
        {
        pr_err("No supported device found\n");
        return ERROR;
        }

    return OK;
    }

int pci_register_driver
    (
    struct pci_driver *pci_driver
    )
    {
    struct drm_vxdev *pDev;
    PCI_HARDWARE *pPciDev;
    struct pci_dev *pci_dev;
    int cnt;
    int i;

    DEBUG_DRM_VXWORKS();

    pDev = &(gfxComp.drmVxDevs[0]);
    for (i = 0; i < DRM_MINOR_CNT; i++, pDev++)
        {
        pDev->filp.f_op = gfxComp.f_op;
        pDev->dev = NULL;
        }

    if (pci_driver->probe == NULL) return ERROR;

    if (gfxComp.vxbDevId == NULL) return ERROR;
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    pPciDev = (PCI_HARDWARE *)vxbDevIvarsGet(gfxComp.vxbDevId);
    if (pPciDev == NULL) return ERROR;

    cnt = 0;
    while ((pci_driver->id_table[cnt].vendor != pPciDev->pciVendId) ||
           (pci_driver->id_table[cnt].device != pPciDev->pciDevId))
        cnt++;

    pci_dev = pcidev_find_entry(gfxComp.vxbDevId);
    if (pci_dev == NULL) return ERROR;

    if (pci_driver->probe (pci_dev, &(pci_driver->id_table[cnt])) != 0) return ERROR;
#else
   {
    unsigned short vendor, device;
    VXB_PCI_BUS_CFG_READ (gfxComp.vxbDevId, PCI_CFG_VENDOR_ID, 2, vendor);
    VXB_PCI_BUS_CFG_READ (gfxComp.vxbDevId, PCI_CFG_DEVICE_ID, 2, device);
    cnt = 0;
    while ((pci_driver->id_table[cnt].vendor != vendor) ||
           (pci_driver->id_table[cnt].device != device))
        cnt++;
 }
    
    pci_dev = pcidev_find_entry(gfxComp.vxbDevId);
    if (pci_dev == NULL) return ERROR;

    if (pci_driver->probe (pci_dev, &(pci_driver->id_table[cnt])) != 0) return ERROR;
#endif
    return OK;
    }

void pci_unregister_driver
    (
    struct pci_driver *pci_driver
    )
    {
    }

/*******************************************************************************
 *
 * request_irq - register an interrupt
 *
 * This function assumes one interrupt is registered for one DRM device and
 * there is only one DRM device.
 *
 * RETURNS: 0 on success, -1 otherwise
 *
 */
int request_irq
    (
    unsigned int irq,
    irq_handler_t handler,
    unsigned long flags,
    const char *name,
    void *dev
    )
    {
    DEBUG_DRM_VXWORKS();

    if (!dev ||
        !((struct drm_device *)dev)->driver ||
        !((struct drm_device *)dev)->driver->irq_handler)
        {
        pr_err ("Bad dev\n");
        return -1;
        }
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    gfxComp.pResIrq = vxbResourceAlloc (gfxComp.vxbDevId, VXB_RES_IRQ, 0);
    if (gfxComp.pResIrq == NULL)
        {
        pr_err("vxbResourceAlloc error\n");
        return -1;
        }

    if (ERROR == vxbIntConnect (gfxComp.vxbDevId,
                                gfxComp.pResIrq,
                 (VOIDFUNCPTR)(((struct drm_device *)dev)->driver->irq_handler),
                                (void *)dev))
        {
        pr_err("vxbIntConnect error\n");
        return -1;
        }

    if (ERROR == vxbIntEnable(gfxComp.vxbDevId, gfxComp.pResIrq))
        {
        pr_err("vxbIntEnable error\n");
        return -1;
        }
#else	
   
    if (ERROR == vxbIntConnect (gfxComp.vxbDevId,
                                0,
                 (VOIDFUNCPTR)(((struct drm_device *)dev)->driver->irq_handler),
                                (void *)dev))
        {
        pr_err("vxbIntConnect error\n");
        return -1;
        }

    if (ERROR == vxbIntEnable(gfxComp.vxbDevId,0, (VOIDFUNCPTR)(((struct drm_device *)dev)->driver->irq_handler),
		 (void *)dev))
        {
        pr_err("vxbIntEnable error\n");
        return -1;
        }
#endif
    return 0;
    }

void free_irq
    (
    unsigned int irq,
    void *dev
    )
    {
    DEBUG_DRM_VXWORKS();
#if !defined(_WRS_CONFIG_VXBUS_LEGACY)
    (void)vxbIntDisable(gfxComp.vxbDevId, gfxComp.pResIrq);
    (void)vxbIntDisconnect(gfxComp.vxbDevId, gfxComp.pResIrq);
#else
    (void)vxbIntDisable(gfxComp.vxbDevId, 0,(VOIDFUNCPTR)(((struct drm_device *)dev)->driver->irq_handler),
    (void *)dev);
    (void)vxbIntDisconnect(gfxComp.vxbDevId,0, (VOIDFUNCPTR)(((struct drm_device *)dev)->driver->irq_handler),
		(void *)dev);
#endif
    }

/*******************************************************************************
 *
 * Show routines
 *
 */

void drm_mmap_show_init
    (
    VOIDFUNCPTR func
    )
    {
    gfxComp.showDrmMmapFuncPtr = func;
    }

void drm_pci_show_init
    (
    VOIDFUNCPTR func
    )
    {
    gfxComp.showDrmPciFuncPtr = func;
    }

void drm_timer_show_init
    (
    VOIDFUNCPTR func
    )
    {
    gfxComp.showDrmTimerFuncPtr = func;
    }

void drm_vxdev_show_init
    (
    VOIDFUNCPTR func
    )
    {
    gfxComp.showDrmVxDevFuncPtr = func;
    }

void drm_dev_private_show_init
    (
    VOIDFUNCPTR func
    )
    {
    gfxComp.showDrmDevPrivateFuncPtr = func;
    }

void show_drm_dev_private
    (
    void *cur,
    const char *tab
    )
    {
    if (gfxComp.showDrmDevPrivateFuncPtr)
        {
        gfxComp.showDrmDevPrivateFuncPtr(cur, tab);
        }
    }

int gfxDrmShowRun = 1;

int gfxDrmShowDrmMm = 0; /* set 1 for gfxDrmShow to show_drm_mm only */

void gfxDrmShow
    (
    void
    )
    {
    if (!gfxDrmShowDrmMm)
        {
        if (gfxComp.showDrmMmapFuncPtr)
            {
            gfxComp.showDrmMmapFuncPtr();
            }

        if (gfxComp.showDrmPciFuncPtr)
            {
            gfxComp.showDrmPciFuncPtr();
            }

        if (gfxComp.showDrmTimerFuncPtr)
            {
            gfxComp.showDrmTimerFuncPtr();
           }
        }

    if (gfxComp.showDrmVxDevFuncPtr)
        {
        struct drm_vxdev *cur;
        int i;

        cur = &(gfxComp.drmVxDevs[0]);
        for (i = 0; i < DRM_MINOR_CNT; i++, cur++)
            {
            gfxComp.showDrmVxDevFuncPtr(cur, "");
            }
        }

    gfxDrmShowRun++;
    }
