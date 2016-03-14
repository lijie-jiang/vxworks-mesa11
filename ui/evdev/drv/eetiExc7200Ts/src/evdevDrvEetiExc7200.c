/* evdevDrvEetiExc7200.c - EETI EXC7200 Multi-touch Controller Driver */

/*
 * Copyright (c) 2014-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
14sep15,jnl  support querying size of the touchscreen area. (V7GFX-238)
01dec14,y_f  created (US50218)
*/

/*
DESCRIPTION

This is the VxBus driver for EETI EXC7200 multi-touch controller. This driver
provides callback methods to support the evdev library, and the client users
should use the API provided by the evdev library to access the low level
hardware function.

To add the driver to the vxWorks image, add the following component to the
kernel configuration.

\cs
vxprj component add DRV_TOUCH_SCREEN_EETI_EXC7200
\ce

This driver is bound to device tree, and the device tree node must specify
below parameters:

\is

\i <compatible>
This required parameter specifies the name of the driver. It must be
"eeti,exc7200".

\i <reg>
This required parameter specifies the device address of this module.

\i <pinmux-0>
This optional parameter specifies pin mux configuration for GPIO interrupt.

\i <int-pin>
This optional parameter specifies the number of GPIO interrupt pin. If the
parameter absents, the driver will work at polling mode.

\i <int-level>
This optional parameter specifies the interrupt trigger level. 0 indicates
trigger by low level, and 1 indicates high level. If the parameter absents, the
low level is the trigger condition.

\i <fingers>
This optional parameter specifies the maximum number of fingers. If the
parameter absents, the driver will use the default value.

\i <display-resolution>
This required parameter specifies the resolution of the LCD display. The first
is the width. The second is the height.

\i <calibration>
This optional parameter specifies the calibration data that this module use. If
the parameter absents, the driver will use the default value.

\i <axis-max-val>
This optional parameter specifies the max axis value. If the parameter absents, 
the driver will use the default value.

\ie

An example of device node is shown below:

\cs

exc7200@4
    {
    compatible = "eeti,exc7200";
    reg = <0x04>;
    pinmux-0 = <&exc7200_2>;
    int-pin = <&gpio6 7>;
    int-level = <1>;
    fingers = <2>;
    display-resolution = <1024>,<768>;
    calibration = <5                /@ calibration point count @/
                   1024 768         /@ display resolution @/
                   0    0           /@ display point 1 @/
                   1023 0           /@ display point 2 @/
                   1023 767         /@ display point 3 @/
                   0    767         /@ display point 4 @/
                   511  383         /@ display point 5 @/
                   0    0           /@ touch screen point 1 @/
                   2047 0           /@ touch screen point 2 @/
                   2047 2047        /@ touch screen point 3 @/
                   0    2047        /@ touch screen point 4 @/
                   1024 1024>;      /@ touch screen point 5 @/
    axis-max-val = <2047>,<2047>;
    };

\ce

INCLUDE FILES: evdevLib.h vxBus.h vxbLib.h vxbFdtLib.h vxbI2cLib.h vxbGpioLib.h
*/

/* includes */

#include <evdevLib.h>
#include <hwif/vxBus.h>
#include <hwif/vxbus/vxbLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbI2cLib.h>
#include <subsys/gpio/vxbGpioLib.h>
#include <subsys/pinmux/vxbPinMuxLib.h>
#include "evdevDrvEetiExc7200.h"

/* forward declarations */

LOCAL STATUS    fdtExc7200TsProbe (VXB_DEV_ID pDev);
LOCAL STATUS    fdtExc7200TsAttach (VXB_DEV_ID pInst);
LOCAL STATUS    exc7200Init (EXC7200_DATA * pCtrl);
LOCAL STATUS    exc7200Open (VXB_DEV_ID pInst);
LOCAL STATUS    exc7200Close (VXB_DEV_ID pInst);
LOCAL STATUS    exc7200Read (VXB_DEV_ID pInst, EV_DEV_PTR_MT_DATA * pMtData);
LOCAL STATUS    exc7200Ioctl (VXB_DEV_ID pInst, int request, _Vx_usr_arg_t
                              arg);
LOCAL STATUS    exc7200I2CRead (EXC7200_DATA * pDev, UINT8 addr, UINT8 * pData,
                                UINT32 length);

/* locals */

LOCAL VXB_DRV_METHOD    fdtExc7200TsMethodList[] =
    {
    { VXB_DEVMETHOD_CALL(vxbDevProbe), fdtExc7200TsProbe},
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtExc7200TsAttach},
    { 0, NULL }
    };

/* EXC7200 multi-touch openfirmware driver */

VXB_DRV vxbFdtExc7200TsDrv =
    {
    {NULL},
    "exc7200",                              /* Name */
    "EXC7200 multi-touch Fdt driver",       /* Description */
    VXB_BUSID_FDT,                          /* Class */
    0,                                      /* Flags */
    0,                                      /* Reference count */
    fdtExc7200TsMethodList,                 /* Method table */
    };

LOCAL const VXB_FDT_DEV_MATCH_ENTRY exc7200TsMatch[] =
    {
        {
        EXC7200_DRIVER_NAME,                /* compatible */
        (void *)NULL,
        },
        {}                                  /* Empty terminated list */
    };

VXB_DRV_DEF(vxbFdtExc7200TsDrv)

/* functions */

/*******************************************************************************
*
* fdtExc7200TsProbe - probe for device presence at specific address
*
* Check for EXC7200 touch screen controller (or compatible) device at the
* specified base address. We assume one is present at that address, but we need
* to verify.
*
* RETURNS: OK if probe passes and assumed a valid EXC7200 touch screen
* controller (or compatible) device. ERROR otherwise.
*
*/

LOCAL STATUS fdtExc7200TsProbe
    (
    VXB_DEV_ID  pDev
    )
    {
    VXB_FDT_DEV_MATCH_ENTRY *   pMatch;

    if (vxbFdtDevMatch (pDev, exc7200TsMatch, &pMatch) == ERROR)
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* fdtExc7200TsAttach - attach EXC7200 touch screen controller
*
* This is the EXC7200 touch screen initialization routine.
*
* RETURNS: OK or ERROR when initialize failed
*
* ERRNO
*/

LOCAL STATUS fdtExc7200TsAttach
    (
    VXB_DEV_ID  pInst
    )
    {
    VXB_FDT_DEV *       pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet(pInst);
    EXC7200_DATA *      pCtrl;
    VXB_RESOURCE *      pRes    = NULL;
    VXB_RESOURCE_ADR *  pResAdr;
    UINT32 *            prop;
    int                 len;
    UINT8               pointCnt;
    UINT8               i;
    int                 intPin;
    UINT32              pinValue;

    if (NULL == pFdtDev)
        {
        return ERROR;
        }

    pCtrl = (EXC7200_DATA *)vxbMemAlloc (sizeof (EXC7200_DATA));
    if (NULL == pCtrl)
        {
        return ERROR;
        }

    pCtrl->pCalData = (TS_CALIB_DATA *)vxbMemAlloc (sizeof (TS_CALIB_DATA));
    if (NULL == pCtrl->pCalData)
        {
        goto error;
        }

    pCtrl->pInst = pInst;

    /* get resources */

    pRes = vxbResourceAlloc (pInst, VXB_RES_MEMORY, 0);
    if (NULL == pRes)
        {
        goto error;
        }

    pResAdr = (VXB_RESOURCE_ADR *)pRes->pRes;
    if (NULL == pResAdr)
        {
        goto error;
        }

    pCtrl->devAddr = (UINT8)pResAdr->virtual;

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "display-resolution", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pCtrl->disWidth     = vxFdt32ToCpu (*prop++);
        pCtrl->disHeight    = vxFdt32ToCpu (*prop);
        }
    else
        {
        goto error;
        }

    intPin = vxbGpioGetByFdtIndex (pInst, "int-pin", 0);
    if (ERROR != intPin)
        {
        prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "int-level", &len);
        if (NULL != prop)
            {
            pinValue    = vxFdt32ToCpu (*prop);
            }
        else
            {
            pinValue    = 0;
            }

        pCtrl->intPin   = (UINT32)intPin;

        (void)vxbPinMuxEnable (pInst);

        if (ERROR == vxbGpioAlloc (pCtrl->intPin))
            {
            goto error;
            }

        if (ERROR == vxbGpioSetDir (pCtrl->intPin, GPIO_DIR_INPUT))
            {
            (void)vxbGpioFree (pCtrl->intPin);
            goto error;
            }

        if (0 == pinValue)
            {
            if (ERROR == vxbGpioIntConfig (pCtrl->intPin, INTR_TRIGGER_LEVEL,
                                           INTR_POLARITY_LOW))
                {
                (void)vxbGpioFree (pCtrl->intPin);
                goto error;
                }
            }
        else
            {
            if (ERROR == vxbGpioIntConfig (pCtrl->intPin, INTR_TRIGGER_LEVEL,
                                           INTR_POLARITY_HIGH))
                {
                (void)vxbGpioFree (pCtrl->intPin);
                goto error;
                }
            }

        pCtrl->intMode = TRUE;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "calibration", &len);
    if (NULL != prop)
        {
        pointCnt = (UINT8)vxFdt32ToCpu (*prop++);

        /* only support 5 and 9 points calibration */

        if ((5 == pointCnt) || (9 == pointCnt))
            {
            pCtrl->pCalData->calibPtCount = pointCnt;
            pCtrl->pCalData->disRect.right = (int)vxFdt32ToCpu (*prop++) - 1;
            pCtrl->pCalData->disRect.bottom = (int)vxFdt32ToCpu (*prop++) - 1;

            for (i = 0; i < pointCnt; i++)
                {
                pCtrl->pCalData->disPoint[i].x = (int)vxFdt32ToCpu (*prop++);
                pCtrl->pCalData->disPoint[i].y = (int)vxFdt32ToCpu (*prop++);
                }

            for (i = 0; i < pointCnt; i++)
                {
                pCtrl->pCalData->tsPoint[i].x = (int)vxFdt32ToCpu (*prop++);
                pCtrl->pCalData->tsPoint[i].y = (int)vxFdt32ToCpu (*prop++);
                }
            }
        }

    if (0 == pCtrl->pCalData->calibPtCount)
        {
        pCtrl->pCalData->calibPtCount = 5;
        evdevTsGetCalibQuick (0, 0, EXC7200_TOUCH_X_MAX, EXC7200_TOUCH_Y_MAX,
                              pCtrl->disWidth, pCtrl->disHeight,
                              pCtrl->pCalData);
        }

    (void)snprintf ((char *)pCtrl->pCalData->devName, TS_DEV_NAME_LEN,
                    "%s", EXC7200_DRIVER_NAME);

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "fingers", &len);
    if (NULL != prop)
        {
        pCtrl->fingers  = (UINT8)vxFdt32ToCpu (*prop);
        }
    else
        {
        pCtrl->fingers  = EXC7200_TOUCH_DEFAULT_FINGERS;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "axis-max-val", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pCtrl->axisXMax = (int)vxFdt32ToCpu (*prop++);
        pCtrl->axisYMax = (int)vxFdt32ToCpu (*prop);
        }
    else
        {
        pCtrl->axisXMax = EXC7200_TOUCH_X_MAX;
        pCtrl->axisYMax = EXC7200_TOUCH_Y_MAX;
        }

    if (TASK_ID_ERROR == taskSpawn ("tsDeviceInit", TS_INIT_TASK_PRIO, 0,
                                    TS_INIT_TASK_STACK_SIZE,
                                    (FUNCPTR)exc7200Init, (_Vx_usr_arg_t)pCtrl,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0))
        {
        goto error;
        }

    /* save Drvctrl */

    vxbDevSoftcSet (pInst, (void *)pCtrl);

    return OK;

error:
    if (NULL != pRes)
        {
        (void)vxbResourceFree (pInst, pRes);
        }

    if (NULL != pCtrl->pCalData)
        {
        vxbMemFree (pCtrl->pCalData);
        }
    vxbMemFree (pCtrl);
    return ERROR;
    }

/*******************************************************************************
*
* exc7200Init - touch screen initialization routine
*
* This routine initializes the touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*
*/

LOCAL STATUS exc7200Init
    (
    EXC7200_DATA *  pCtrl
    )
    {
    TS_DEVICE   tsDev;
    UINT8       buf[EXC7200_TOUCH_REG_LEN];

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    if (ERROR == exc7200I2CRead (pCtrl, 0x0, buf, EXC7200_TOUCH_REG_LEN))
        {
        return ERROR;
        }

    bzero ((char *)&tsDev, sizeof (TS_DEVICE));

    if (pCtrl->intMode)
        {
        tsDev.devAttr = TS_DEV_ATTR_INTERRUPT | TS_DEV_ATTR_MULTITOUCH;
        }
    else
        {
        tsDev.devAttr = TS_DEV_ATTR_POLLING_PRESS   |
                        TS_DEV_ATTR_POLLING_SAMPLE  |
                        TS_DEV_ATTR_MULTITOUCH;
        tsDev.samplePeriod = (EXC7200_SAMPLE_PERIOD * sysClkRateGet()) / 1000;
        }

    tsDev.pInst                     = pCtrl->pInst;
    tsDev.pCalData                  = pCtrl->pCalData;
    tsDev.calDataCount              = 1;
    tsDev.func.open                 = exc7200Open;
    tsDev.func.close                = exc7200Close;
    tsDev.func.ioctl                = exc7200Ioctl;
    tsDev.func.read                 = exc7200Read;
    tsDev.devData.devCap            = EV_DEV_KEY | EV_DEV_ABS | EV_DEV_ABS_MT;
    tsDev.devData.pDevName          = EXC7200_DRIVER_NAME;
    tsDev.devData.devInfo.bustype   = EV_DEV_BUS_I2C;
    tsDev.devData.devInfo.vendor    = 0;
    tsDev.devData.devInfo.product   = 0;
    tsDev.devData.devInfo.version   = 0;
    tsDev.defDisRect.right          = pCtrl->disWidth - 1;
    tsDev.defDisRect.bottom         = pCtrl->disHeight - 1;
    tsDev.defDisRect.left           = 0;
    tsDev.defDisRect.top            = 0;

    tsDev.devAxisVal[0].axisIndex   = 0;
    tsDev.devAxisVal[0].minVal      = 0;
    tsDev.devAxisVal[0].maxVal      = pCtrl->axisXMax;

    tsDev.devAxisVal[1].axisIndex   = 1;
    tsDev.devAxisVal[1].minVal      = 0;
    tsDev.devAxisVal[1].maxVal      = pCtrl->axisYMax;

    pCtrl->pDevInfo = evdevTsReg (&tsDev);
    if (NULL == pCtrl->pDevInfo)
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* exc7200Open - open touch screen device
*
* This routine opens touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS exc7200Open
    (
    VXB_DEV_ID  pInst
    )
    {
    EXC7200_DATA *  pCtrl = (EXC7200_DATA *)vxbDevSoftcGet (pInst);

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    if (pCtrl->intMode)
        {
        if (ERROR == vxbGpioIntConnect (pCtrl->intPin,
                                        (VOIDFUNCPTR)evdevTsDevIsr,
                                        (void *)pCtrl->pDevInfo))
            {
            return ERROR;
            }

        if (ERROR == vxbGpioIntEnable (pCtrl->intPin,
                                       (VOIDFUNCPTR)evdevTsDevIsr,
                                       (void *)pCtrl->pDevInfo))
            {
            (void)vxbGpioIntDisconnect (pCtrl->intPin,
                                        (VOIDFUNCPTR)evdevTsDevIsr,
                                        (void *)pCtrl->pDevInfo);
            return ERROR;
            }
        }

    return OK;
    }

/*******************************************************************************
*
* exc7200Close - close touch screen device
*
* This routine close touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS exc7200Close
    (
    VXB_DEV_ID  pInst
    )
    {
    EXC7200_DATA *  pCtrl = (EXC7200_DATA *)vxbDevSoftcGet (pInst);

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    if (pCtrl->intMode)
        {
        if (ERROR == vxbGpioIntDisable (pCtrl->intPin,
                                        (VOIDFUNCPTR)evdevTsDevIsr,
                                        (void *)pCtrl->pDevInfo))
            {
            return ERROR;
            }

        if (ERROR == vxbGpioIntDisconnect (pCtrl->intPin,
                                           (VOIDFUNCPTR)evdevTsDevIsr,
                                           (void *)pCtrl->pDevInfo))
            {
            return ERROR;
            }
        }

    return OK;
    }

/*******************************************************************************
*
* exc7200Read - read x and y value from touch screen device
*
* This routine reads x and y value from touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS exc7200Read
    (
    VXB_DEV_ID              pInst,
    EV_DEV_PTR_MT_DATA *    pMtData
    )
    {
    UINT8           buf[EXC7200_TOUCH_REG_LEN];
    EXC7200_DATA *  pCtrl = (EXC7200_DATA *)vxbDevSoftcGet (pInst);
    UINT8           id;
    int             x;
    int             y;
    STATUS          ret;
    BOOL            pressed;

    if ((NULL == pCtrl) || (NULL == pMtData))
        {
        return ERROR;
        }

    bzero ((char *)buf, EXC7200_TOUCH_REG_LEN);
    ret = exc7200I2CRead (pCtrl, 0x0, buf, EXC7200_TOUCH_REG_LEN);

    if (pCtrl->intMode)
        {
        ret |= vxbGpioIntEnable (pCtrl->intPin, (VOIDFUNCPTR)evdevTsDevIsr,
                                 (void *)pCtrl->pDevInfo);
        }

    if (ERROR == ret)
        {
        return ERROR;
        }

    if (EXC7200_TOUCH_REPORT_ID == buf[0])
        {
        id  = (buf[EXC7200_TOUCH_STATUS] >> EXC7200_TOUCH_ID_OFFSET) &
              EXC7200_TOUCH_ID_MASK;
        x   = (int)(((buf[EXC7200_TOUCH_XL] >> EXC7200_TOUCH_XL_OFFSET) &
                     EXC7200_TOUCH_XL_MASK) |
                    (buf[EXC7200_TOUCH_XH] << EXC7200_TOUCH_XH_OFFSET));
        y   = (int)(((buf[EXC7200_TOUCH_YL] >> EXC7200_TOUCH_YL_OFFSET) &
                     EXC7200_TOUCH_YL_MASK) |
                    (buf[EXC7200_TOUCH_YH] << EXC7200_TOUCH_YH_OFFSET));

        if (0x1 == ((buf[EXC7200_TOUCH_STATUS] >> EXC7200_TOUCH_DOWN_OFFSET) &
                    EXC7200_TOUCH_DOWN_MASK))
            {
            pressed = TRUE;
            }
        else
            {
            pressed = FALSE;
            }

        pCtrl->lastMtData.count                 = pCtrl->fingers;
        pCtrl->lastMtData.points[id].x          = x;
        pCtrl->lastMtData.points[id].y          = y;
        pCtrl->lastMtData.points[id].pressed    = pressed;

        bcopy ((char *)&pCtrl->lastMtData, (char *)pMtData,
               sizeof (EV_DEV_PTR_MT_DATA));
        return OK;
        }
    else
        {
        return ERROR;
        }
    }

/*******************************************************************************
*
* exc7200Ioctl - handle IOCTL for touch screen device
*
* This routine handles IOCTL for touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS exc7200Ioctl
    (
    VXB_DEV_ID      pInst,
    int             request,    /* ioctl function */
    _Vx_usr_arg_t   arg         /* function arg */
    )
    {
    STATUS          result = OK;
    EXC7200_DATA *  pCtrl = (EXC7200_DATA *)vxbDevSoftcGet (pInst);

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    switch (request)
        {
        case TS_CHECK_INT:
        if (pCtrl->intMode)
            {
            result = vxbGpioIntDisable (pCtrl->intPin,
                                        (VOIDFUNCPTR)evdevTsDevIsr,
                                        (void *)pCtrl->pDevInfo);
            }
            break;

        default:
            errno = S_ioLib_UNKNOWN_REQUEST;
            result = ERROR;
        }

    return result;
    }

/*******************************************************************************
*
* exc7200I2CRead - read register value from device
*
* This routine reads register value from device.
*
* RETURNS: OK when successfully read; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS exc7200I2CRead
    (
    EXC7200_DATA *  pDev,
    UINT8           addr,
    UINT8 *         pData,
    UINT32          length
    )
    {
    I2C_MSG msg[2];

    bzero ((char *)msg, sizeof (I2C_MSG) * 2);

    /* write offset */

    msg[0].addr = pDev->devAddr;
    msg[0].scl  = FAST_MODE;
    msg[0].buf  = &addr;
    msg[0].len  = 1;

    /* read data */

    msg[1].addr     = pDev->devAddr;
    msg[1].scl      = FAST_MODE;
    msg[1].flags    = I2C_M_RD;
    msg[1].buf      = pData;
    msg[1].len      = length;

    return vxbI2cDevXfer (pDev->pInst, &msg[0], 2);
    }
