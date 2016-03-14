/* evdevDrvVirtualKbd.c - VxWorks Virtual Keyboard Driver */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
16may14,yat  Add virtual keyboard driver. (US24741)
*/

/*
DESCRIPTION
This file contains code for VxWorks virtual keyboard driver.
*/

/* includes */

#include <evdevLib.h>
#include <evdevLibKbdMap.h>
#include "evdevDrvVirtualKbd.h"

/* typedefs */

/* structure to store the virtualKbd module information */

typedef struct virtualKbdDev
    {
    int                 inputDevHdr;
    BOOL                isEn;
    EV_DEV_DEVICE_DATA  devData;
    EV_DEV_KBD_HANDLE * pKbdHandle;
    } VIRTUAL_KBD_DEV;

/* forward declarations */

LOCAL STATUS    virtualDevIoctl (VIRTUAL_KBD_DEV * pDev, int request, _Vx_usr_arg_t
                                 arg);

/* locals */

LOCAL VIRTUAL_KBD_DEV * pVirtualKbdDev = NULL;

/* The following tables convert the Virtual keyboard message to HID scan code */

LOCAL UINT8 virtualAsciiMap[VIRTUAL_KBD_ASCII_NUM] =
/*          0       1       2       3       4       5       6       7       8       9       A       B       C       D       E       F   */
    {
/* 0x0_ */  0,      0x04,   0x05,   0x06,   0x07,   0x08,   0x09,   0x0A,   0x2A,   0x2B,   0x28,   0x0E,   0x0F,   0x10,   0x11,   0x12,
/* 0x1_ */  0x13,   0x14,   0x15,   0x16,   0x17,   0x18,   0x19,   0x1A,   0x1B,   0x1C,   0x1D,   0x29,   0x32,   0x30,   0x31,   0x33,
/* 0x2_ */  0x2C,   0x1E,   0x34,   0x20,   0x21,   0x22,   0x24,   0x34,   0x26,   0x27,   0x25,   0x2E,   0x36,   0x2D,   0x37,   0x38,
/* 0x3_ */  0x27,   0x1E,   0x1F,   0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x33,   0x33,   0x36,   0x2E,   0x37,   0x38,
/* 0x4_ */  0x1F,   0x04,   0x05,   0x06,   0x07,   0x08,   0x09,   0x0A,   0x0B,   0x0C,   0x0D,   0x0E,   0x0F,   0x10,   0x11,   0x12,
/* 0x5_ */  0x13,   0x14,   0x15,   0x16,   0x17,   0x18,   0x19,   0x1A,   0x1B,   0x1C,   0x1D,   0x2F,   0x31,   0x30,   0x23,   0x2D,
/* 0x6_ */  0x35,   0x04,   0x05,   0x06,   0x07,   0x08,   0x09,   0x0A,   0x0B,   0x0C,   0x0D,   0x0E,   0x0F,   0x10,   0x11,   0x12,
/* 0x7_ */  0x13,   0x14,   0x15,   0x16,   0x17,   0x18,   0x19,   0x1A,   0x1B,   0x1C,   0x1D,   0x2F,   0x31,   0x30,   0x35,   0x4C
    };

LOCAL UINT8 virtualVkMap[VIRTUAL_KBD_VK_NUM] =
/*          0       1       2       3       4       5       6       7       8       9       A       B       C       D   */
    {
/* 0x0_ */  0x4A,   0x4D,   0x49,   0x4B,   0x4E,   0x50,   0x4F,   0x52,   0x51,   0x46,   0x48,   0x39,   0x53,   0x47
    };

LOCAL UINT8 virtualFnMap[VIRTUAL_KBD_FN_NUM] =
/*          0       1       2       3       4       5       6       7       8       9       A       B       C   */
    {
/* 0x0_ */  0,      0x3A,   0x3B,   0x3C,   0x3D,   0x3E,   0x3F,   0x40,   0x41,   0x42,   0x43,   0x44,   0x45
    };

/* functions */

/*******************************************************************************
*
* virtualKbdInit - install the keyboard device driver
*
* This routine installs the keyboard device driver to evdev framework.
*
* RETURNS: OK, or ERROR if the driver cannot be installed
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS virtualKbdInit (void)
    {
    VIRTUAL_KBD_DEV *    pDev = NULL;

    pDev = calloc (1, sizeof (VIRTUAL_KBD_DEV));
    if (NULL == pDev)
        {
        return ERROR;
        }

    pDev->isEn = FALSE;

    pDev->devData.devCap            = EV_DEV_KEY;
    pDev->devData.pDevName          = VIRTUAL_KBD_DRIVER_NAME;
    pDev->devData.ioctl             = (FUNCPTR)virtualDevIoctl;
    pDev->devData.pArg              = (void *)pDev;
    pDev->devData.devInfo.bustype   = EV_DEV_BUS_VIRTUAL;
    pDev->devData.devInfo.vendor    = 0;
    pDev->devData.devInfo.product   = 0;
    pDev->devData.devInfo.version   = 0;
    pDev->pKbdHandle                = evdevKbdReg (&pDev->devData);

    if (NULL == pDev->pKbdHandle)
        {
        goto error;
        }

    pVirtualKbdDev = pDev;

    return OK;

error:

    free (pDev);
    return ERROR;
    }

/*******************************************************************************
*
* virtualDevIoctl - control the virtual keyboard device
*
* This routine controls the virtual keyboard device.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS virtualDevIoctl
    (
    VIRTUAL_KBD_DEV *   pDev,
    int                 request,
    _Vx_usr_arg_t       arg
    )
    {
    STATUS  result = OK;

    /* check arguments */

    if (NULL == pDev)
        {
        errno = EFAULT;
        return ERROR;
        }

    switch (request)
        {
        case EV_DEV_IO_DEV_EN:
            if (!pDev->isEn)
                {
                if (OK == result)
                    {
                    pDev->isEn = TRUE;
                    }
                }
            break;

        case EV_DEV_IO_DEV_DIS:
            if (pDev->isEn)
                {
                if (OK == result)
                    {
                    pDev->isEn = FALSE;
                    }
                }
            break;

        default:
            break;
        }

    return result;
    }

/*******************************************************************************
*
* keyDown - create key down event
*
* This routine may be called from the kernel shell to creates a key down 
* event for the scan code <k>.
*
* RETURNS: OK on success, otherwise ERROR
*           
* ERRNO: N/A
*
* SEE ALSO: keyUp, keyPress
*
*/

STATUS keyDown
    (
    int k
    )
    {
    EV_DEV_KBD_REPORT   report;
    UINT8               keycode;

    if (NULL == pVirtualKbdDev)
        {
        return ERROR;
        }

    bzero ((char *)&report, sizeof (EV_DEV_KBD_REPORT));

    if (VIRTUAL_KBD_FN_MASK == (k & VIRTUAL_KBD_FN_MASK))
        {
        keycode = (UINT8)(k & VIRTUAL_KBD_KEY_MASK);
        if (keycode < VIRTUAL_KBD_FN_NUM)
            {
            report.scanCodes[0] = virtualFnMap[keycode];
            }
        }
    else if (VIRTUAL_KBD_VK_MASK == (k & VIRTUAL_KBD_VK_MASK))
        {
        keycode = (UINT8)(k & VIRTUAL_KBD_KEY_MASK);
        if (keycode < VIRTUAL_KBD_VK_NUM)
            {
            report.scanCodes[0] = virtualVkMap[keycode];
            }
        }
    else
        {
        keycode = (UINT8)(k & VIRTUAL_KBD_KEY_MASK);
        if (keycode < VIRTUAL_KBD_ASCII_NUM)
            {
            report.scanCodes[0] = virtualAsciiMap[keycode];
            }
        }

    pVirtualKbdDev->pKbdHandle->chkTypeMatic  = FALSE;
    pVirtualKbdDev->pKbdHandle->isMapped      = FALSE;
    (void)evdevKbdSendMsg (pVirtualKbdDev->pKbdHandle, &report);

    return OK;
    }

/*************************************************************************
*
* keyUp - create a key up event
*
* This routine may be called from the kernel shell to creates a key up 
* event for the previous keyDown <k>.
*
* RETURNS: OK on success, otherwise ERROR
*           
* ERRNO: N/A
*
* SEE ALSO: keyDown, keyPress
*
*/

STATUS keyUp (void)
    {
    EV_DEV_KBD_REPORT   report;

    if (NULL == pVirtualKbdDev)
        {
        return ERROR;
        }

    bzero ((char *)&report, sizeof (EV_DEV_KBD_REPORT));

    (void)evdevKbdSendMsg (pVirtualKbdDev->pKbdHandle, &report);

    return OK;
    }

/*******************************************************************************
*
* keyPress - create key press event
*
* This routine may be called from the kernel shell to creates a key press 
* event for the scan code <k>. A key press is a key down and key up.
*
* RETURNS: OK on success, otherwise ERROR
*           
* ERRNO: N/A
*
* SEE ALSO: keyDown, keyUp
*
*/

STATUS keyPress
    (
    int k
    )
    {
    if (ERROR == keyDown (k))
        {
        return ERROR;
        }

    return keyUp ();
    }
