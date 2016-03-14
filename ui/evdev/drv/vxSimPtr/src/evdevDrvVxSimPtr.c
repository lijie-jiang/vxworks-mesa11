/* evdevDrvVxSimPtr.c - VxWorks Simulator Pointer Driver */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
12aug14,y_f  removed compiler warning (V7GFX-204)
26aug13,y_f  created from udIoSimPcPtrDrv.c version 01d and
             udIoSimX11PtrDrv.c version 01d
*/

/*
DESCRIPTION
This file contains code for VxWorks simulator pointer driver.
*/

/* includes */

#include <wdLib.h>
#include <excLib.h>
#include <intLib.h>
#include <private/vxsimHostLibP.h>
#include <evdevLib.h>
#include "evdevDrvVxSimPtr.h"

/* typedefs */

/* structure to store the simPtr module information */

typedef struct vxSimPtrDev
    {
    int                 inputDevHdr;
    UINT32              buttonState;
    WDOG_ID             timerId;
    UINT32              dllDataRead;
    UINT32              dllAnyData;
    UINT32              dllGetEvent;
    BOOL                isEn;
    EV_DEV_DEVICE_DATA  devData;
    EV_DEV_HANDLE *     pEvdev;
    } VX_SIM_PTR_DEV;

/* forward declarations */

LOCAL STATUS    vxSimDevIoctl (VX_SIM_PTR_DEV * pDev, int request, _Vx_usr_arg_t
                               arg);
LOCAL void      vxSimChkData (VX_SIM_PTR_DEV * pDev);
LOCAL void      vxSimIntHandler (VX_SIM_PTR_DEV * pDev);

extern void *   gfxSimGetDllDrv (void);

/* functions */

/*******************************************************************************
*
* vxSimPtrInit - install the pointer device driver
*
* This routine installs the pointer device driver to evdev framework.
*
* RETURNS: OK, or ERROR if the driver cannot be installed
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS vxSimPtrInit (void)
    {
    VX_SIM_PTR_DEV *    pDev = NULL;

    pDev = calloc (1, sizeof (VX_SIM_PTR_DEV));
    if (NULL == pDev)
        {
        return ERROR;
        }

    pDev->isEn = FALSE;

    /* initialize Dll */

    if (vxsimHostUsrSvcRegister (&pDev->dllDataRead, "uglSimDllPtrDataRead",
                                 VXSIM_HOST_USR_SVC_ON_CPU0) != OK)
        {
        goto error;
        }

#if (_VX_CPU_FAMILY == _VX_SIMLINUX)
    if (vxsimHostUsrSvcRegister (&pDev->dllAnyData, "uglSimDllAnyPtrData",
                                 VXSIM_HOST_USR_SVC_ON_CPU0) != OK)
        {
        goto error;
        }

    if (vxsimHostUsrSvcRegister (&pDev->dllGetEvent, "uglSimDllX11GetEvent",
                                 VXSIM_HOST_USR_SVC_ON_CPU0) != OK)
        {
        goto error;
        }

    pDev->timerId = (WDOG_ID)wdCreate ();
    if (NULL == pDev->timerId)
        {
        goto error;
        }
#endif /* _VX_CPU_FAMILY == _VX_SIMLINUX */

    pDev->devData.devCap             = EV_DEV_KEY | EV_DEV_ABS;
    pDev->devData.pDevName           = SIM_PTR_DRIVER_NAME;
    pDev->devData.ioctl              = (FUNCPTR)vxSimDevIoctl;
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

#if (_VX_CPU_FAMILY == _VX_SIMNT)
    (void)intConnect ((VOIDFUNCPTR *)(SIM_DLL_PTR_INT_BASE + 0),
                      (VOIDFUNCPTR)vxSimIntHandler, (_Vx_usr_arg_t)pDev);
#endif /* _VX_CPU_FAMILY == _VX_SIMNT */

    return OK;

error:

#if (_VX_CPU_FAMILY == _VX_SIMLINUX)
    if (NULL != pDev->timerId)
        {
        (void)wdDelete (pDev->timerId);
        }
#endif /* _VX_CPU_FAMILY == _VX_SIMLINUX */

    free (pDev);
    return ERROR;
    }

/*******************************************************************************
*
* vxSimDevIoctl - control the simulator pointer device
*
* This routine controls the simulator pointer device.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS vxSimDevIoctl
    (
    VX_SIM_PTR_DEV *    pDev,
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
#if (_VX_CPU_FAMILY == _VX_SIMLINUX)
                result = wdStart (pDev->timerId, 1, (FUNCPTR)vxSimIntHandler,
                                  (_Vx_usr_arg_t)pDev);
#endif /* _VX_CPU_FAMILY == _VX_SIMLINUX */
                if (OK == result)
                    {
                    pDev->isEn = TRUE;
                    }
                }
            break;

        case EV_DEV_IO_DEV_DIS:
            if (pDev->isEn)
                {
#if (_VX_CPU_FAMILY == _VX_SIMLINUX)
                result = wdCancel (pDev->timerId);
#endif /* _VX_CPU_FAMILY == _VX_SIMLINUX */
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
* vxSimChkData - check the pointer messages
*
* This rountine checks the pointer messages.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void vxSimChkData
    (
    VX_SIM_PTR_DEV *    pDev    /* device descriptor */
    )
    {
    BOOL                anyData = 0;
    VX_SIM_DLL_PTR_DATA inBuf;
    STATUS              status;
    STATUS              retVal = OK;
    int                 numBytes;
    void *              pDllDriver;
    EV_DEV_PTR_DATA     ptrData;

    pDllDriver = gfxSimGetDllDrv ();
    if (NULL == pDllDriver)
        {
        return;
        }

#if (_VX_CPU_FAMILY == _VX_SIMLINUX)
    (void)vxsimHostUsrSvcCall ((ULONG *)&retVal, pDev->dllGetEvent,
                               (ULONG)pDllDriver, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    (void)vxsimHostUsrSvcCall ((ULONG *)&retVal, pDev->dllAnyData,
                               (ULONG)pDllDriver, (ULONG)&anyData,
                               0, 0, 0, 0, 0, 0, 0, 0);
#else
    anyData = TRUE;
#endif /* _VX_CPU_FAMILY == _VX_SIMLINUX */

    if (anyData)
        {
        status = vxsimHostUsrSvcCall ((ULONG *)&retVal, pDev->dllDataRead,
                                       (ULONG)pDllDriver, (ULONG)&inBuf,
                                       (ULONG)&numBytes, 0, 0, 0, 0, 0, 0, 0);

        if ((ERROR == status) || (ERROR == retVal) ||
            (numBytes < sizeof (VX_SIM_DLL_PTR_DATA)))
            {
            return;
            }

        bzero ((char *)&ptrData, sizeof (EV_DEV_PTR_DATA));
        ptrData.type        = EV_DEV_ABS;
        ptrData.position.x  = inBuf.x;
        ptrData.position.y  = inBuf.y;

        /* get button state */

        if (inBuf.buttonState & 1)
            {
            ptrData.buttonState |= EV_DEV_PTR_BTN_LEFT_BIT;
            }
        if (inBuf.buttonState & 2)
            {
            ptrData.buttonState |= EV_DEV_PTR_BTN_MIDDLE_BIT;
            }
        if (inBuf.buttonState & 4)
            {
            ptrData.buttonState |= EV_DEV_PTR_BTN_RIGHT_BIT;
            }

        ptrData.buttonChange = pDev->buttonState ^ ptrData.buttonState;

        pDev->buttonState = ptrData.buttonState;

        (void)evdevPtrSendMsg (pDev->pEvdev, &ptrData);
        }
    }

/*******************************************************************************
*
* vxSimIntHandler - pointer interrupt handler
*
* This rountine handle the pointer interrupt.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void vxSimIntHandler
    (
    VX_SIM_PTR_DEV *    pDev    /* device descriptor */
    )
    {
    (void)excJobAdd ((VOIDFUNCPTR)vxSimChkData, (_Vx_usr_arg_t)pDev,
                     0, 0, 0, 0, 0);

#if (_VX_CPU_FAMILY == _VX_SIMLINUX)
    (void)wdStart (pDev->timerId, 1, (FUNCPTR)vxSimIntHandler,
                   (_Vx_usr_arg_t)pDev);
#endif /* _VX_CPU_FAMILY == _VX_SIMLINUX */
    }
