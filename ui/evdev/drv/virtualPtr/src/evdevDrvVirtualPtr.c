/* evdevDrvVirtualPtr.c - VxWorks Virtual Pointer Driver */

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
16may14,yat  Add virtual pointer driver. (US24741)
*/

/*
DESCRIPTION
This file contains code for VxWorks virtual pointer driver.
*/

/* includes */

#include <evdevLib.h>
#include "evdevDrvVirtualPtr.h"

/* typedefs */

/* structure to store the virtualPtr module information */

typedef struct virtualPtrDev
    {
    int                 inputDevHdr;
    UINT32              buttonState;
    BOOL                isEn;
    EV_DEV_DEVICE_DATA  devData;
    EV_DEV_HANDLE *     pEvdev;
    } VIRTUAL_PTR_DEV;

/* forward declarations */

LOCAL STATUS    virtualDevIoctl (VIRTUAL_PTR_DEV * pDev, int request, _Vx_usr_arg_t
                                 arg);

IMPORT int sysClkRateGet (void);

/* locals */

LOCAL VIRTUAL_PTR_DEV * pVirtualPtrDev = NULL;

/* functions */

/*******************************************************************************
*
* virtualPtrInit - install the pointer device driver
*
* This routine installs the pointer device driver to evdev framework.
*
* RETURNS: OK, or ERROR if the driver cannot be installed
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS virtualPtrInit (void)
    {
    VIRTUAL_PTR_DEV *    pDev = NULL;

    pDev = calloc (1, sizeof (VIRTUAL_PTR_DEV));
    if (NULL == pDev)
        {
        return ERROR;
        }

    pDev->isEn = FALSE;

    pDev->devData.devCap             = EV_DEV_KEY | EV_DEV_ABS;
    pDev->devData.pDevName           = VIRTUAL_PTR_DRIVER_NAME;
    pDev->devData.ioctl              = (FUNCPTR)virtualDevIoctl;
    pDev->devData.pArg               = (void *)pDev;
    pDev->devData.devInfo.bustype    = EV_DEV_BUS_VIRTUAL;
    pDev->devData.devInfo.vendor     = 0;
    pDev->devData.devInfo.product    = 0;
    pDev->devData.devInfo.version    = 0;

    pDev->pEvdev = evdevPtrReg (&pDev->devData);
    if (NULL == pDev->pEvdev)
        {
        goto error;
        }

    pVirtualPtrDev = pDev;

    return OK;

error:

    free (pDev);
    return ERROR;
    }

/*******************************************************************************
*
* virtualDevIoctl - control the virtual pointer device
*
* This routine controls the virtual pointer device.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS virtualDevIoctl
    (
    VIRTUAL_PTR_DEV *   pDev,
    int                 request,    /* ioctl function */
    _Vx_usr_arg_t       arg         /* function arg */
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
* mouse - simulate pointer move event with button press
*
* This routine may be called from the kernel shell to simulate a pointer
* move event, with button press information. 
*
* The destination coordinate is specified by <x> and <y>. 
* 
* The button state is specified by the <buttonState>. This is a bitmask, 
* where the binary button state is specified by the least significant 3 
* bits. A value of one in a particular bit position indicates that the 
* button is down. Bit position zero controls button1, bit position one 
* controls button2, and bit position two controls button 3.
* 
* In this example, button 1 is up, while buttons 2 and 3 are down, where 
* the value 6 is the bitwise OR of the value 2 and 4.
*
* /cs
* mouse(10, 10, 6)
* /ce
* /ce
*
* Note, once marked as down, the button state remains down until it is
* marked as up. 
*
* RETURNS: OK on success, otherwise ERROR
*           
* ERRNO: N/A
*
* SEE ALSO: mouseMoveTo, mouseClick, mouseDrag
*
*/

STATUS mouse
    (
    int x,                      /* X coordinate */
    int y,                      /* Y coordinate */
    int buttonState             /* button state */
    )
    {
    EV_DEV_PTR_DATA     ptrData;

    if (NULL == pVirtualPtrDev)
        {
        return ERROR;
        }

    bzero ((char *)&ptrData, sizeof (EV_DEV_PTR_DATA));

    ptrData.type        = EV_DEV_ABS;
    ptrData.position.x  = x;
    ptrData.position.y  = y;

    /* get button state */

    if (buttonState & 1)
        {
        ptrData.buttonState |= EV_DEV_PTR_BTN_LEFT_BIT;
        }
    if (buttonState & 2)
        {
        ptrData.buttonState |= EV_DEV_PTR_BTN_RIGHT_BIT;
        }
    if (buttonState & 4)
        {
        ptrData.buttonState |= EV_DEV_PTR_BTN_MIDDLE_BIT;
        }

    ptrData.buttonChange = pVirtualPtrDev->buttonState ^ ptrData.buttonState;

    pVirtualPtrDev->buttonState = ptrData.buttonState;

    (void)evdevPtrSendMsg (pVirtualPtrDev->pEvdev, &ptrData);

    return OK;
    }

/*******************************************************************************
*
* mouseMoveTo - simulate pointer move event
*
* This routine may be called from the kernel shell to simulate a pointer
* move event. 
*
* The destination coordinate is specified by <x> and <y>. 
* 
* RETURNS: OK on success, otherwise ERROR
*           
* ERRNO: N/A
*
* SEE ALSO: mouse, mouseClick, mouseDrag
*
*/

STATUS mouseMoveTo
    (
    int x,                   /* X Coordinate */
    int y                    /* Y Coordinate */
    )
    {
    return mouse (x, y, 0);
    }

/*******************************************************************************
*
* mouseClick - simulate pointer move event with button click
*
* This routine may be called from the kernel shell to simulate a pointer
* move event, with button click information. The major difference between
* this routine and <mouse> is that this performs a down and up event on
* the specified button(s). All buttons end in the up position.
*
* The destination coordinate is specified by <x> and <y>. 
* 
* The button state is specified by the <buttonState>. This is a bitmask, 
* where the binary button state is specified by the least significant 3 
* bits. A value of one in a particular bit position indicates that the 
* button is down. Bit position zero controls button1, bit position one 
* controls button2, and bit position two controls button 3.
* 
* In this example, button 1 is up, while buttons 2 and 3 are down, where 
* the value 6 is the bitwise OR of the value 2 and 4.
*
* /cs
* mouseClick(10, 10, 6, 0)
* /ce
*
* RETURNS: OK on success, otherwise ERROR
*           
* ERRNO: N/A
*
* SEE ALSO: mouse, mouseMoveTo, mouseDrag
*
*/

STATUS mouseClick
    (
    int x,
    int y,
    int buttonState
    )
    {
    if (ERROR == mouse (x, y, buttonState))
        {
        return ERROR;
        }

    (void)taskDelay (sysClkRateGet());

    return mouse (x, y, 0);
    }

/*******************************************************************************
*
* mouseDrag - simulate pointer drag event
*
* This routine may be called from the kernel shell to simulate a pointer
* drag event, with button information. A drag event will move a pointer
* from one coordinate to another, with button click information as
* specified by the user. All buttons end in the up position.
*
* The initial coordinate is specified by <x0> and <y0>. The destination
* coordinate is specified by <x1> and <y1>.
* 
* The button state is specified by the <buttonState>. This is a bitmask, 
* where the binary button state is specified by the least significant 3 
* bits. A value of one in a particular bit position indicates that the 
* button is down. Bit position zero controls button1, bit position one 
* controls button2, and bit position two controls button 3.
* 
* In this example, button 1 is up, while buttons 2 and 3 are down, where 
* the value 6 is the bitwise OR of the value 2 and 4.
*
* /cs
* mouseDrag(10, 10, 20, 20, 6, 0)
* /ce
*
* RETURNS: OK on success, otherwise ERROR
*           
* ERRNO: N/A
*
* SEE ALSO: mouse, mouseMoveTo, mouseClick
*
*/

STATUS mouseDrag
    (
    int x0,
    int y0,
    int x1,
    int y1,
    int buttonState
    )
    {
    if (ERROR == mouse (x0, y0, buttonState))
        {
        return ERROR;
        }

    (void)taskDelay (sysClkRateGet());

    return mouse (x1, y1, 0);
    }
