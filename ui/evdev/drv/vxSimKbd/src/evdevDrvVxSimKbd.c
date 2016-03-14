/* evdevDrvVxSimKbd.c - VxWorks Simulator Keyboard Driver */

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
27aug13,y_f  created from udIoSimPcKbdDrv.c version 01d and
             udIoSimX11KbdDrv.c version 01c
*/

/*
DESCRIPTION
This file contains code for VxWorks simulator keyboard driver.
*/

/* includes */

#include <wdLib.h>
#include <excLib.h>
#include <intLib.h>
#include <private/vxsimHostLibP.h>
#include <evdevLib.h>
#include <evdevLibKbdMap.h>
#include "evdevDrvVxSimKbd.h"

/* typedefs */

/* structure to store the simKbd module information */

typedef struct vxSimKbdDev
    {
    int                 inputDevHdr;
    WDOG_ID             timerId;
    UINT32              dllDataRead;
    UINT32              dllAnyData;
    UINT32              dllGetEvent;
    BOOL                isEn;
    EV_DEV_DEVICE_DATA  devData;
    EV_DEV_KBD_HANDLE * pKbdHandle;
    } VX_SIM_KBD_DEV;

/* forward declarations */

LOCAL STATUS    vxSimDevIoctl (VX_SIM_KBD_DEV * pDev, int request, _Vx_usr_arg_t
                               arg);
LOCAL void      vxSimChkData (VX_SIM_KBD_DEV * pDev);
LOCAL void      vxSimIntHandler (VX_SIM_KBD_DEV * pDev);

extern void *   gfxSimGetDllDrv (void);

/* locals */

/* The following tables convert the VxSim keyboard message to HID scan code */

LOCAL UINT8 vxSimAsciiMap[SIM_KBD_ASCII_NUM] =
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

LOCAL UINT8 vxSimVkMap[SIM_KBD_VK_NUM] =
/*          0       1       2       3       4       5       6       7       8       9       A       B       C       D   */
    {
/* 0x0_ */  0x4A,   0x4D,   0x49,   0x4B,   0x4E,   0x50,   0x4F,   0x52,   0x51,   0x46,   0x48,   0x39,   0x53,   0x47
    };

LOCAL UINT8 vxSimFnMap[SIM_KBD_FN_NUM] =
/*          0       1       2       3       4       5       6       7       8       9       A       B       C   */
    {
/* 0x0_ */  0,      0x3A,   0x3B,   0x3C,   0x3D,   0x3E,   0x3F,   0x40,   0x41,   0x42,   0x43,   0x44,   0x45
    };

/* functions */

/*******************************************************************************
*
* vxSimKbdInit - install the keyboard device driver
*
* This routine installs the keyboard device driver to evdev framework.
*
* RETURNS: OK, or ERROR if the driver cannot be installed
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS vxSimKbdInit (void)
    {
    VX_SIM_KBD_DEV *    pDev = NULL;

    pDev = calloc (1, sizeof (VX_SIM_KBD_DEV));
    if (NULL == pDev)
        {
        return ERROR;
        }

    pDev->isEn = FALSE;

    /* initialize Dll */

    if (vxsimHostUsrSvcRegister (&pDev->dllDataRead, "uglSimDllKbdDataRead",
                                 VXSIM_HOST_USR_SVC_ON_CPU0) != OK)
        {
        goto error;
        }

#if (_VX_CPU_FAMILY == _VX_SIMLINUX)
    if (vxsimHostUsrSvcRegister (&pDev->dllAnyData, "uglSimDllAnyKbdData",
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

    pDev->devData.devCap            = EV_DEV_KEY;
    pDev->devData.pDevName          = SIM_KBD_DRIVER_NAME;
    pDev->devData.ioctl             = (FUNCPTR)vxSimDevIoctl;
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

#if (_VX_CPU_FAMILY == _VX_SIMNT)
    (void)intConnect ((VOIDFUNCPTR *)(SIM_DLL_KBD_INT_BASE + 0),
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
* vxSimDevIoctl - control the simulator keyboard device
*
* This routine controls the simulator keyboard device.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS vxSimDevIoctl
    (
    VX_SIM_KBD_DEV *    pDev,
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
* vxSimChkData - check the keyboard messages
*
* This rountine checks the keyboard messages.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void vxSimChkData
    (
    VX_SIM_KBD_DEV *    pDev    /* device descriptor */
    )
    {
    BOOL                anyData = 0;
    VX_SIM_DLL_KBD_DATA inBuf;
    STATUS              status;
    STATUS              retVal = OK;
    ssize_t             numBytes;
    void *              pDllDriver;
    EV_DEV_KBD_REPORT   report;
    UINT8               keycode;

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
            (numBytes < sizeof (VX_SIM_DLL_KBD_DATA)))
            {
            return;
            }

        bzero ((char *)&report, sizeof (EV_DEV_KBD_REPORT));

        if (0 != inBuf.isKeyDown)
            {
            if (0 != (inBuf.keyCode & SIM_KBD_LEFT_ALT))
                {
                report.modifiers |= EV_DEV_KBD_LEFT_ALT;
                }
            if (0 != (inBuf.keyCode & SIM_KBD_RIGHT_ALT))
                {
                report.modifiers |= EV_DEV_KBD_RIGHT_ALT;
                }
            if (0 != (inBuf.keyCode & SIM_KBD_LEFT_SHIFT))
                {
                report.modifiers |= EV_DEV_KBD_LEFT_SHIFT;
                }
            if (0 != (inBuf.keyCode & SIM_KBD_RIGHT_SHIFT))
                {
                report.modifiers |= EV_DEV_KBD_RIGHT_SHIFT;
                }
            if (0 != (inBuf.keyCode & SIM_KBD_LEFT_CTRL))
                {
                report.modifiers |= EV_DEV_KBD_LEFT_CTRL;
                }
            if (0 != (inBuf.keyCode & SIM_KBD_RIGHT_CTRL))
                {
                report.modifiers |= EV_DEV_KBD_RIGHT_CTRL;
                }

            if (0 != (inBuf.keyCode & SIM_KBD_NUM_PAD))
                {
                report.modifiers = 0;
                keycode = (UINT8)(inBuf.keyCode & SIM_KBD_KEY_MASK);
                if ((SIM_KBD_ASTERISK == keycode) ||
                    (SIM_KBD_PLUS_SIGN == keycode))
                    {
                    report.modifiers = EV_DEV_KBD_LEFT_SHIFT;
                    }
                }

            if (SIM_KBD_FN_MASK == (inBuf.keyCode & SIM_KBD_FN_MASK))
                {
                keycode = (UINT8)(inBuf.keyCode & SIM_KBD_KEY_MASK);
                if (keycode < SIM_KBD_FN_NUM)
                    {
                    report.scanCodes[0] = vxSimFnMap[keycode];
                    }
                }
            else if (SIM_KBD_VK_MASK == (inBuf.keyCode & SIM_KBD_VK_MASK))
                {
                keycode = (UINT8)(inBuf.keyCode & SIM_KBD_KEY_MASK);
                if (keycode < SIM_KBD_VK_NUM)
                    {
                    report.scanCodes[0] = vxSimVkMap[keycode];
                    }
                }
            else
                {
                keycode = (UINT8)(inBuf.keyCode & SIM_KBD_KEY_MASK);
                if (keycode < SIM_KBD_ASCII_NUM)
                    {
                    report.scanCodes[0] = vxSimAsciiMap[keycode];
                    }
                }
            }

        pDev->pKbdHandle->chkTypeMatic  = FALSE;
        pDev->pKbdHandle->isMapped      = FALSE;
        (void)evdevKbdSendMsg (pDev->pKbdHandle, &report);
        }
    }

/*******************************************************************************
*
* vxSimIntHandler - keyboard interrupt handler
*
* This rountine handle the keyboard interrupt.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void vxSimIntHandler
    (
    VX_SIM_KBD_DEV *    pDev    /* device descriptor */
    )
    {
    (void)excJobAdd ((VOIDFUNCPTR)vxSimChkData, (_Vx_usr_arg_t)pDev,
                     0, 0, 0, 0, 0);

#if (_VX_CPU_FAMILY == _VX_SIMLINUX)
    (void)wdStart (pDev->timerId, 1, (FUNCPTR)vxSimIntHandler,
                   (_Vx_usr_arg_t)pDev);
#endif /* _VX_CPU_FAMILY == _VX_SIMLINUX */
    }
