/* sysWindML.c - WindML BSP specific routines  */

/*
 * Copyright (c) 2011, 2012 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01b,18mar12,jlv  added minimum bus number option. (CQ:288091)
                 added multiple bus ctrl support.
01a,11apr11,j_z  initial creation based on itl_nehalem version 01g
*/

/*
DESCRIPTION
This library provides board-specific routines to support WindML.  

This API is designed in a general fashion, such that it is applicable to all
processor types and is bus independent.  It provides the support equally for
graphics devices that are integral to a processor (such as with the PPC823) 
and graphics devices that are present on a PCI bus.

The API provides support for configurations where a system has multiple
graphics devices and input devices.

A data structure allocated within the BSP provides information describing the 
graphics, input (keyboard and pointer), and audio devices.  A serial pointer
that connects to a standard serial port (such as, /tyCo/0) is not covered by
this API.  Those devices use the standard serial drivers.

The data structure to define the graphics device is as follows:

\cs
 typedef struct windml_device
     {
     UINT32        vendorID,        /@ PCI vendor ID @/
     UINT32        deviceID,        /@ PCI device ID @/
     UINT32        instance,        /@ device instance @/
     UINT32        devType,         /@ WindML device type @/
     UINT32        busType;         /@ bus type @/
     UINT32        regDelta;        /@ distance between adjacent registers @/
     UINT32        intLevel;        /@ device interrupt level @/
     VOIDFUNCPTR * intVector;       /@ device interrupt vector @/
     void *        pPhysBaseAdrs0;  /@ PCI base address 0 @/
     void *        pPhysBaseAdrs1;  /@ PCI base address 1 @/
     void *        pPhysBaseAdrs2;  /@ PCI base address 2 @/
     void *        pPhysBaseAdrs3;  /@ PCI base address 3 @/
     void *        pPhysBaseAdrs4;  /@ PCI base address 4 @/
     void *        pPhysBaseAdrs5;  /@ PCI base address 5 @/
     void *        pRegBase;        /@ register space base address @/

     } WINDML_DEVICE;
\ce

The <vendorID> and the <deviceID> are based upon the PCI bus identifiers.  In
this case, these identifiers are extended to include the mapping for non-PCI
devices.  The file sysWindML.h provides the identifier for supported vendor
and device identifiers.
 
The above structure provides space for up to six memory segments that are
used to access the device (for example, one segment for the frame buffer
and another for the memory mapped registers).  Typically, a device 
will only have a single memory segment.  The size field identifies the number
of bytes present within the memory segment.

The <pRegBase> field identifies the base address to use to access the
I/O ports.  This is typically the base of the ISA space.  For X86 type
processors, this field would be set to 0.  For powerPC processors, this
field would be set according to the memory model used (PRep or CHRP).

INCLUDE FILES: sysWindML.h
*/



/* includes */

#include <vxWorks.h>
#include <memPartLib.h>
#include <ugl/sysWindML.h>
#include <drv/pci/pciConfigLib.h>
#include <hwif/util/vxbParamSys.h>
#include "config.h"



/* defines */

#define FRAME_BUFF_ADDR_VGA  ((void *)(0xa0000))  /*default VGA frame buffer*/
#define WINDML_MAX_DEV       (32)


/* local configuration options */

#undef  SYS_WINDML_PCI_SHOW
#define SYS_WINDML_STATIC_MEM_POOL


#ifdef    SYS_WINDML_STATIC_MEM_POOL

#include <bufLib.h>

LOCAL BUF_POOL        windMlBufPool;
LOCAL WINDML_DEVICE   windMlDevPool  [WINDML_MAX_DEV];

#endif /* SYS_WINDML_STATIC_MEM_POOL */



/* locals */

/* store handles to allocated WINDML_DEVICE PCI device instances */

LOCAL WINDML_DEVICE * pciDisplayDevs [WINDML_MAX_DEV];
LOCAL WINDML_DEVICE * pciMmAudioDevs [WINDML_MAX_DEV];

/* specifies the number of allocated WINDML_DEVICE PCI device instances */

LOCAL int pciDisplayDevNo = 0;
LOCAL int pciMmAudioDevNo = 0;


/* forward declarations */

LOCAL STATUS sysWindMlPciDevMap (WINDML_DEVICE *, UINT32, UINT32, UINT32);
LOCAL STATUS sysWindMlPciInit (UINT32, UINT32, UINT32, void *);





/*******************************************************************************
*
* sysWindMlDescAlloc - allocate a WINDML_DEVICE descriptor
*
* This routine allocates a WINDML_DEVICE descriptor from a memory pool for
* use by the system.
*
* INTERNAL
* This interface facilitates changing the underlying implementation to suit
* system requirements.  The current implementation uses a simple static pool
* of WINDML_DEVICE descriptors.  This makes it safe to use from sysHwInit().
* However, future versions may require allocation out of the system memory
* partition (kernel heap in AE) or a dedicated non-system memory partition.
*
* RETURNS: A pointer to the allocated descriptor, or a null pointer if
* there is an error.
*
* NOMANUAL
*/
__inline__ static WINDML_DEVICE * sysWindMlDescAlloc (void)
    {
#ifdef SYS_WINDML_STATIC_MEM_POOL

    return (WINDML_DEVICE *) bufAlloc (&windMlBufPool);

#else

    return (WINDML_DEVICE *) KHEAP_ALLOC (sizeof (WINDML_DEVICE));

#endif /* SYS_WINDML_STATIC_MEM_POOL */
    }

/*******************************************************************************
*
* sysWindMlDescFree - free a WINDML_DEVICE descriptor
*
* This routine releases a previously allocated WINDML_DEVICE descriptor
* back to the memory pool from which it was allocated via a call to
* sysWindMlDescAlloc().
*
* INTERNAL
* This interface facilitates changing the underlying implementation to suit
* system requirements.  The current implementation uses a simple static pool
* of WINDML_DEVICE descriptors.  This makes it safe to use from sysHwInit().
* However, future versions may require allocation out of the system memory
* partition (kernel heap in AE) or a dedicated non-system memory partition.
*
* RETURNS: N/A
*
* NOMANUAL
*/
__inline__ static void sysWindMlDescFree
    (
    WINDML_DEVICE * pWindMlDev    /* pointer to the descriptor to free */
    )
    {
#ifdef SYS_WINDML_STATIC_MEM_POOL

    bufFree (&windMlBufPool, (char *) pWindMlDev);

#else

    KHEAP_FREE ((char *) pWindMlDev);

#endif /* SYS_WINDML_STATIC_MEM_POOL */
    }

/*******************************************************************************
*
* sysWindMLDevGet - configures the device
*
* This routine will determine the presence of a specified WindML <devType>
* device and perform any device configuration required.  The behavior of
* this function varies depending on the type of device configuration (such
* as a PCI device, integral, device controller, etc.).  A device
* configuration data structure of type WINDML_DEVICE is created for the
* specified device.  This configuration data defines, among other things,
* the access mechanism for the device.    
*
* The <vendorID> and <deviceID> identify the vendor and device identifiers
* in the PCI environment.  In the case of non-PCI type devices, these
* identifiers provide identifiers of the device outside of the range of PCI
* identifiers.  If these values are set to zero, then the <instance>
* occurrence of a <devType> device will be returned.
* 
* The returned data structure provides miscellaneous data items that
* describe the manner in which to access the device.  
*
* RETURNS:
* The address of a WINDML_DEVICE descriptor, else NULL when a device
* configuration cannot be obtained.
*/
WINDML_DEVICE * sysWindMLDevGet 
    (
    UINT32     devType,     /* WindML device type */
    UINT32     instance,    /* instance of WindML device type */
    UINT32     vendorID,    /* the PCI Vendor ID for the device */
    UINT32     deviceID     /* the PCI Device ID for the device */
    )
    {
    LOCAL BOOL vgaCreated = FALSE;

    WINDML_DEVICE * pDev  = NULL;


    switch (devType)
        {
        case WINDML_GRAPHICS_DEVICE:
            {
            if ((vendorID == 0) && (deviceID == 0))
                {
                /* If there are no PCI display class devices, create one
                 * VGA device (i80X86/PentiumX processor families only).
                 */

                if (pciDisplayDevNo == 0)
                    {
                    if ((vgaCreated == FALSE) &&
                        ((pDev = sysWindMlDescAlloc ()) != NULL))
                        {
                        bzero ((char *) pDev, sizeof (WINDML_DEVICE)); 

                        pDev->vendorID       = VENDOR_GENERIC_VGA;
                        pDev->deviceID       = DEVICE_VGA;
                        pDev->instance       = instance;
                        pDev->devType        = devType;
                        pDev->pPhysBaseAdrs0 = FRAME_BUFF_ADDR_VGA;
                        pDev->regDelta       = 0;
                        pDev->intLevel       = 0;
                        pDev->intVector      = NULL;
                        pDev->pRegBase       = 0;
                        vgaCreated           = TRUE;
                        }
                    }
                else if ((instance >= 0) && (instance < pciDisplayDevNo))
                    {
                    return (pciDisplayDevs[instance]);
                    }
 
                return (pDev);
                }
            else if (deviceID == 0)
                {
                /* Find the specified <instance> of the specified PCI
                 * <vendorID> value.
                 */

                int i;

                for (i = 0; i < pciDisplayDevNo; ++i)
                    {
                    if (((pciDisplayDevs[i])->vendorID == vendorID) &&
                        (instance-- == 0))
                        {
                        return (pciDisplayDevs[i]);
                        }
                    }
                }
            else
                {
                /* Find the specified <instance> of the specified PCI
                 * <vendorID> and <deviceID> values.
                 */

                int i;

                for (i = 0; i < pciDisplayDevNo; ++i)
                    {
                    if (((pciDisplayDevs[i])->vendorID == vendorID) &&
                        ((pciDisplayDevs[i])->deviceID == deviceID) &&
                        (instance-- == 0))
                        {
                        return (pciDisplayDevs[i]);
                        }
                    }
                }

            break;
            }

        case WINDML_KEYBOARD_DEVICE:
            {
            if ((pDev = sysWindMlDescAlloc ()) != NULL)
                {
                bzero ((char *) pDev, sizeof (WINDML_DEVICE));

                pDev->instance  = instance;
                pDev->devType   = devType;
                pDev->regDelta  = 4;
                pDev->intLevel  = KBD_INT_LVL;
                pDev->intVector = (VOIDFUNCPTR *)
                                  (INUM_TO_IVEC (INT_NUM_GET (KBD_INT_LVL)));
                pDev->pRegBase  = (void *)(DATA_8042);
                }

            return (pDev);
            }

        case WINDML_POINTER_DEVICE :
            {
            if ((pDev = sysWindMlDescAlloc ()) != NULL)
                {
                bzero ((char *) pDev, sizeof (WINDML_DEVICE));

                pDev->instance  = instance;
                pDev->devType   = devType;
                pDev->regDelta  = 4;
                pDev->intLevel  = MSE_INT_LVL;
                pDev->intVector = (VOIDFUNCPTR *)
                                  (INUM_TO_IVEC (INT_NUM_GET (MSE_INT_LVL)));
                pDev->pRegBase  = (void *)(DATA_8042);
                }

            return (pDev);
            }

        case WINDML_AUDIO_DEVICE:
            {
            if ((vendorID == 0) && (deviceID == 0))
                {
                /* If there are no PCI audio class devices ... */

                if (pciMmAudioDevNo == 0)
                    {
                    return (pDev);
                    }
                else if ((instance >= 0) && (instance < pciMmAudioDevNo))
                    {
                    return (pciMmAudioDevs[instance]);
                    }
                }
            else if (deviceID == 0)
                {
                /* Find the specified <instance> of the specified PCI
                 * <vendorID> value.
                 */

                int i;

                for (i = 0; i < pciMmAudioDevNo; ++i)
                    {
                    if (((pciMmAudioDevs[i])->vendorID == vendorID) &&
                        (instance-- == 0))
                        {
                        return (pciMmAudioDevs[i]);
                        }
                    }
                }
            else
                {
                /* Find the specified <instance> of the specified PCI
                 * <vendorID> and <deviceID> values.
                 */

                int i;

                for (i = 0; i < pciMmAudioDevNo; ++i)
                    {
                    if (((pciMmAudioDevs[i])->vendorID == vendorID) &&
                        ((pciMmAudioDevs[i])->deviceID == deviceID) &&
                        (instance-- == 0))
                        {
                        return (pciMmAudioDevs[i]);
                        }
                    }
                }

            break;
            }
        }


    return (pDev);
    }

/*******************************************************************************
*
* sysWindMLDevCtrl - special control of the device mode
*
* This routine provides special control features for the device.  This
* function is essentially a catch all to provide control of the device
* where the functionality is provided within a PCI configuration header or
* by board specific registers which may be shared by other functions
* implemented on the target board.
*
* The <function> argument defines the type of function that is to be
* performed and the <pArg> parameter provides the details relating to the
* function. 
*
* The values for <function> and the interpretation of the <pArg> parameters
* are:
*
*\is
*\i WINDML_ACCESS_MODE_SET
*       Sets the device's access mode as to whether it is to respond to I/O
*       cycles of memory mapped cycles or both.  The accessibility is
*       provided by the <pArg> parameter which is bit mapped containing the
*       flags WINDML_MEM_ENABLE (enable memory mapped access) and
*       WINDML_IO_ENABLE (enable I/O access).
*\i WINDML_LCD_MODE_SET
*       Sets the control mode for an LCD that is controllable by an on board
*       register rather than a graphics device register. The mode information
*       is passed through <pArg>. The flags available are WINDML_LCD_ON
*       WINDML_LCD_OFF, WINDML_BACKLIGHT_ON, WINDML_BACKLIGHT_OFF.
*\i WINDML_BUSWIDTH_SET
*       Some boards allow the LCD bus width to be changed dynamically via
*       an FPGA or other configurable logic. This can be done in a board
*       specific manner. The actual bus width will be passed through <pArg>.
*\i WINDML_PCI_MEMBASE_GET
*       Obtain the base address of CPU memory as seen by PCI devices.  
*\ie
*
* RETURNS: OK for successful control operations, else ERROR.
*/
STATUS sysWindMLDevCtrl 
    (
    WINDML_DEVICE * pDev,        /* Device to control */
    int             function,    /* Type of operation to perform */
    int *           pArg         /* Control mode */
    )
    {
    STATUS status = ERROR;

    if (pDev == NULL)
        {
        return (status);
        }

    switch (function)
        {
        /* Conrol the PCI access mode, the command byte */

        case WINDML_ACCESS_MODE_SET:
            {
            int busno, devno, funcno;

            if (pciFindDevice (pDev->vendorID,
                               pDev->deviceID,
                               pDev->instance, 
                               &busno, &devno, &funcno) != OK)
                {
                return (ERROR);
                }

            status = pciConfigOutWord (busno, devno, funcno, 
                                      PCI_CFG_COMMAND, *pArg);
            break;
            }


        /* Obtain the CPU memory base address as seen by PCI device */

        case WINDML_PCI_MEMBASE_GET:

            /* PCI memory base is same as CPU, so set to 0 */

            *pArg = 0;
            break;
        }


    return (status);
    }

/*******************************************************************************
*
* sysWindMLDevRelease - release a device configuration
*
* This routine will release any resources that were allocated when a  
* device was configured using the sysWindMLDevGet() function.  This 
* function will free the memory that was allocated for the WINDML_DEVICE 
* data structure if it was dynamically allocated.  If the data structure
* was not dynamically allocated, this function will usually be simply a
* stub.
*
* INTERNAL
* When a static memory pool configuration is used, <pDev> descriptors
* for PCI devices are not released back to the static pool once they
* are successfully allocated and initialized at the conclusion of the
* sysWindMLHwInit() routine.
*
* RETURNS: OK for a successful release operation, else ERROR.
*/
STATUS sysWindMLDevRelease 
    (
    WINDML_DEVICE * pDev         /* Device to release */
    )
    {
#ifdef    SYS_WINDML_STATIC_MEM_POOL

    if ((pDev != NULL) && (pDev->busType != BUS_TYPE_PCI))
        {
        sysWindMlDescFree (pDev);
        }

#else

    if (pDev != NULL)
        {
        sysWindMlDescFree (pDev);
        }

#endif /* SYS_WINDML_STATIC_MEM_POOL */

    return (OK);
    }

/*******************************************************************************
*
* sysWindMLIntConnect - Connect the device interrupt
*
* This routine connects a routine to the interrupt.
*
* RETURNS: OK or ERROR.
*/
STATUS sysWindMLIntConnect
    (
    WINDML_DEVICE * pDev,        /* Graphics device to control */
    VOIDFUNCPTR     routine,     /* routine to be called */
    int             parameter    /* parameter to be passed */
    )
    {
    STATUS status = ERROR;

    if ((pDev != NULL) && (pDev->intVector != NULL) && (routine != NULL))
        {
        if (pDev->busType == BUS_TYPE_PCI)
            status = pciIntConnect (pDev->intVector, routine, parameter);
        else
            status = intConnect (pDev->intVector, routine, parameter);
        }

    return (status);
    }

/*******************************************************************************
*
* sysWindMLIntEnable - Enable interrupts
*
* This routine enables the interrupt.
*
* RETURNS: OK or ERROR.
*/
STATUS sysWindMLIntEnable
    (
    WINDML_DEVICE * pDev         /* Device to control */
    )
    {
    return ((pDev != NULL) ? (sysIntEnablePIC (pDev->intLevel)) : (ERROR));
    }

/*******************************************************************************
*
* sysWindMLIntDisable - Disable interrupts
*
* This routine disables the interrupt.
*
* RETURNS: OK or ERROR.
*/
STATUS sysWindMLIntDisable
    (
    WINDML_DEVICE * pDev         /* Device to control */
    )
    {
    return ((pDev != NULL) ? (sysIntDisablePIC (pDev->intLevel)) : (ERROR));
    }

/*******************************************************************************
*
* sysWindMlPciDevMap - map a WindML device into the host address space
*
* This routine initializes the <pPhysBaseAdrs(n)> fields of the specified
* WINDML_DEVICE descriptor with the PCI base address register (BAR) values
* from a PCI device specified by <bus>, <dev>, and <func>.  
*
* CAVEATS
* As of PCI 2.2, decoders may be implemented in any of the base address
* register (BAR) positions.  If more than one decoder is implemented,
* there may be holes.  Therefore, inspect all six of the possible BAR
* positions in the device header to determine which registers are actually
* implemented.
*
* This routine will not correctly detect and handle 64-bit base address
* registers.
**
*
* RETURNS:
* OK.
*
* NOMANUAL
*/
LOCAL STATUS sysWindMlPciDevMap
    (
    WINDML_DEVICE * pDev,       /* WindML device descriptor */
    UINT32          bus,        /* device's PCI bus number */
    UINT32          dev,        /* device's PCI device number */
    UINT32          func        /* device's PCI function number */
    )
    {
    UINT16          cmdSave;    /* saves 16-bit PCI command word register */
    UINT32          barSave;    /* saves 32-bit PCI base address register */
    STATUS          retVal = OK;



    /* save PCI device command word register & disable memory decode */

    pciConfigInWord  (bus, dev, func, PCI_CFG_COMMAND, &cmdSave);
    pciConfigOutWord (bus, dev, func, PCI_CFG_COMMAND,
                      cmdSave & ((~PCI_CMD_MEM_ENABLE) | (~PCI_CMD_IO_ENABLE)));


    /* save the BARs and determine whether memory decoders are implemented */

    pciConfigInLong  (bus, dev, func, PCI_CFG_BASE_ADDRESS_0, &barSave);
    pDev->pPhysBaseAdrs0 = (void *) barSave;

    pciConfigInLong  (bus, dev, func, PCI_CFG_BASE_ADDRESS_1, &barSave);
    pDev->pPhysBaseAdrs1 = (void *) barSave;

    pciConfigInLong  (bus, dev, func, PCI_CFG_BASE_ADDRESS_2, &barSave);
    pDev->pPhysBaseAdrs2 = (void *) barSave;

    pciConfigInLong  (bus, dev, func, PCI_CFG_BASE_ADDRESS_3, &barSave);
    pDev->pPhysBaseAdrs3 = (void *) barSave;

    pciConfigInLong  (bus, dev, func, PCI_CFG_BASE_ADDRESS_4, &barSave);
    pDev->pPhysBaseAdrs4 = (void *) barSave;

    pciConfigInLong  (bus, dev, func, PCI_CFG_BASE_ADDRESS_5, &barSave);
    pDev->pPhysBaseAdrs5 = (void *) barSave;


    /* restore PCI device command word register */

    pciConfigOutWord (bus, dev, func, PCI_CFG_COMMAND, cmdSave);

    return retVal;
    }

/*******************************************************************************
*
* sysWindMlPciInit - initialize PCI display devices
*
* This routine allocates and initializes WINDML_DEVICE descriptors for PCI
* display and multimedia class devices.  The interface is constructed such
* that this function can be invoked via pciConfigForeachFunc().
*
* For each PCI device specified by the <bus>, <dev>, and <func> parameters,
* this routine tests the 24-bit PCI class code value (class/subclass/prog_if).
* If WindML supports the specified function, a WINDML_DEVICE descriptor will
* be allocated and initialzed with the device attributes.  
*
* The <devType> field of a new WINDML_DEVICE descriptor will be set to
* one of the following constants indicating the WindML device type:
*
* \is
* \i WINDML_GRAPHICS_DEVICE
*        All PCI display controller class devices.
*
* \i WINDML_AUDIO_DEVICE
*        All PCI multimedia class and audio sub-class devices.
* \ie
*
* INTERNAL
* This callback will terminate pciConfigForeachFunc() if it returns ERROR.
* So, this routine returns OK always such that a failure to config one
* device will not prevent attempts to initialize other devices.
*
* This routine could be extended to create and configure WINDML_DEVICE
* descriptors for mouse and keyboard input PCI device classes.
*
* WindML supports an Epson display device which does not identify the
* device function in the PCI class code register.  Specifically, the class
* code register is set to 0xff which, according to the PCI spec, should be
* used when the device does not fit any of the defined class codes.  These
* special-case Epson display devices are identified by their PCI Device ID,
* 0x1300, along with the Epson PCI Vendor ID.
*
* In the case of PCI devices, the <instance> field in a WINDML_DEVICE
* descriptor specifies the ordinal value of a particular kind of device, as
* specified by PCI Vendor and device IDs, installed on the system.
*
* RETURNS: OK always.
*
* NOMANUAL
*/
LOCAL STATUS sysWindMlPciInit
    (
    UINT32     bus,              /* store a PCI bus number */
    UINT32     dev,              /* store a PCI device number */
    UINT32     func,             /* store a PCI function number */
    void *     pArg              /* reserved argument */
    )
    {
    UINT32     vendorId;         /* store a PCI vendor ID */
    UINT32     deviceId;         /* store a PCI device ID */

    UINT32     classCode;        /* store a PCI class code value */
    UINT32     subClassCode;     /* store a PCI sub-class code value */

    UINT32     devType = 0;   /* store a WindML device type */


    pciConfigInLong (bus, dev, func, PCI_CFG_VENDOR_ID, &vendorId);

    deviceId = ((vendorId >> 16) & 0x0000ffff);
    vendorId = (vendorId & 0x0000ffff);


    pciConfigInLong (bus, dev, func, PCI_CFG_REVISION, &classCode);

    subClassCode = (classCode >> 16) & 0x000000ff;
    classCode    = (classCode >> 24);


    if ((classCode == PCI_CLASS_DISPLAY_CTLR) ||
        ((vendorId == VENDOR_PCI_EPSON) &&
         (deviceId == DEVICE_ID_EPSON_13XXX)))
        {
        devType = WINDML_GRAPHICS_DEVICE;
        }
    else if ((classCode == PCI_CLASS_MMEDIA_DEVICE) &&
             (subClassCode == PCI_SUBCLASS_MMEDIA_AUDIO))
        {
        devType = WINDML_AUDIO_DEVICE;
        }


    if ((devType == WINDML_GRAPHICS_DEVICE) || (devType == WINDML_AUDIO_DEVICE))
        {
        UINT8  intLine;       /* store a PCI interrupt line value */

        /* allocate a WINDML_DEVICE descriptor for the device */

        WINDML_DEVICE * const pDev = sysWindMlDescAlloc ();

        if (pDev == NULL)
            {
            return OK;
            }

        /* initialize the new WINDML_DEVICE attributes */

        bzero ((char *) pDev, sizeof (WINDML_DEVICE));

        pDev->vendorID = vendorId;
        pDev->deviceID = deviceId;
        pDev->devType  = devType;
        pDev->busType  = BUS_TYPE_PCI;

        pciConfigInByte (bus, dev, func, PCI_CFG_DEV_INT_LINE, &intLine);

        pDev->intLevel  = intLine;
        pDev->intVector = (VOIDFUNCPTR *)(INUM_TO_IVEC (INT_NUM_GET (intLine)));

        /* get base addresses */

        if (sysWindMlPciDevMap (pDev, bus, dev, func) == ERROR)
            {
            sysWindMlDescFree (pDev);
            return OK;
            }

        /* initialized the device - move on to the next one */

        if (devType == WINDML_GRAPHICS_DEVICE)
            {
            int i = 0;
            int j = 0;

            for (; i < pciDisplayDevNo; ++i)
                {
                if (((pciDisplayDevs[i])->vendorID == vendorId) &&
                    ((pciDisplayDevs[i])->deviceID == deviceId))
                    ++j;
                }

            pDev->instance = j;
            pciDisplayDevs [pciDisplayDevNo++] = pDev;
            }
        else if (devType == WINDML_AUDIO_DEVICE)
            {
            int i = 0;
            int j = 0;

            for (;i < pciMmAudioDevNo; ++i)
                {
                if (((pciMmAudioDevs[i])->vendorID == vendorId) &&
                    ((pciMmAudioDevs[i])->deviceID == deviceId))
                    ++j;
                }

            pDev->instance = j;
            pciMmAudioDevs [pciMmAudioDevNo++] = pDev;
            }

        return OK;
        }

    return OK;
    }

/*******************************************************************************
*
* sysWindMLHwInit - Initialize unique multimedia components
*
* This routine initializes specific multimedia hardware devices that
* are not normally initialized by the BSP.
*
* RETURNS: OK or ERROR.
*/

STATUS sysWindMLHwInit (void)
    {
    FUNCPTR		method;
    STATUS		status;
    int			i;
    struct vxbPciConfig *pPciConfig = NULL;
    VXB_DEVICE_ID	ctrl;

#ifdef    SYS_WINDML_STATIC_MEM_POOL

    bufPoolInit (&windMlBufPool, (char *) &windMlDevPool[0],
                 WINDML_MAX_DEV, sizeof (WINDML_DEVICE));

#endif /* SYS_WINDML_STATIC_MEM_POOL */

    for (i = 0; i < PCI_BUS_CTRL_NUM; i++)
        {
        /* get the PCI bus controller dev ID */

        ctrl = vxbInstByNameFind ("pentiumPci", i);

        if (ctrl == NULL)
            {
            /* this should not happen */

            continue;
            }

        method = vxbDevMethodGet (ctrl, PCI_CONTROLLER_METHOD_CFG_INFO);

        if ((method == NULL) ||
            ((status = (*method) (ctrl, &pPciConfig)) == ERROR))
            {
            /* this should not happen */

            continue;
            }

        /* initialize PCI display controllers starting from pciMinBus */

        vxbPciConfigForeachFunc (ctrl, pPciConfig->pciMinBus, TRUE,
                                 (PCI_FOREACH_FUNC) sysWindMlPciInit, NULL);
        }

    return (OK);
    }


#ifdef SYS_WINDML_PCI_SHOW

/*******************************************************************************
*
* sysWindMlPciShow - show the contents of the PCI device table
*
* This routine prints a formatted detail of the WindML PCI device table
* contents to the standard output device.
*
* RETURNS: N/A
*
* NOMANUAL
*/
void sysWindMlPciShow (void)
    {
    WINDML_DEVICE const * pDev;

    int i;

    for (i = 0; i < pciDisplayDevNo; ++i)
        {
        pDev = pciDisplayDevs[i];

        puts ("\n______________");

        printf ("\nWINDML_GRAPHICS_DEVICE, instance %d\n", pDev->instance);

        printf ("\nPCI Vendor ID 0x%x", pDev->vendorID);
        printf ("\nPCI Device ID 0x%x", pDev->deviceID);

        printf ("\n\nInterrupt level %u, vector %p\n",
                pDev->intLevel, pDev->intVector);

        printf ("\n PCI BAR 0 %p", pDev->pPhysBaseAdrs0);
        printf ("\n PCI BAR 1 %p", pDev->pPhysBaseAdrs1);
        printf ("\n PCI BAR 2 %p", pDev->pPhysBaseAdrs2);
        printf ("\n PCI BAR 3 %p", pDev->pPhysBaseAdrs3);
        printf ("\n PCI BAR 4 %p", pDev->pPhysBaseAdrs4);
        printf ("\n PCI BAR 5 %p", pDev->pPhysBaseAdrs5);
        }

    for (i = 0; i < pciMmAudioDevNo; ++i)
        {
        pDev = pciMmAudioDevs[i];

        puts ("\n______________");

        printf ("\nWINDML_AUDIO_DEVICE, instance %d\n", pDev->instance);

        printf ("\nPCI Vendor ID 0x%x", pDev->vendorID);
        printf ("\nPCI Device ID 0x%x", pDev->deviceID);

        printf ("\n\nInterrupt level %u, vector %p\n",
                pDev->intLevel, pDev->intVector);

        printf ("\n PCI BAR 0 %p", pDev->pPhysBaseAdrs0);
        printf ("\n PCI BAR 1 %p", pDev->pPhysBaseAdrs1);
        printf ("\n PCI BAR 2 %p", pDev->pPhysBaseAdrs2);
        printf ("\n PCI BAR 3 %p", pDev->pPhysBaseAdrs3);
        printf ("\n PCI BAR 4 %p", pDev->pPhysBaseAdrs4);
        printf ("\n PCI BAR 5 %p", pDev->pPhysBaseAdrs5);
        }

    putchar ('\n');
    }

#endif  /* SYS_WINDML_PCI_SHOW */
