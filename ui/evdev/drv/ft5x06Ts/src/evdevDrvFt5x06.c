/* evdevDrvFt5x06.c - FocalTech Systems FT5X06 Multi-touch Controller Driver */

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
01aug14,y_f  created (US41404)
*/

/*
DESCRIPTION

This is the VxBus driver for FocalTech Systems FT5X06 multi-touch controller.
This driver provides callback methods to support the evdev library, and the
client users should use the API provided by the evdev library to access the low
level hardware function.

To add the driver to the vxWorks image, add the following component to the
kernel configuration.

\cs
vxprj component add DRV_TOUCH_SCREEN_FT_5X06
\ce

This driver is bound to device tree, and the device tree node must specify
below parameters:

\is

\i <compatible>
This required parameter specifies the name of the driver. It must be "ft,5x06".

\i <reg>
This required parameter specifies the device address of this module.

\i <pinmux-0>
This optional parameter specifies pin mux configuration for GPIO interrupt.

\i <int-pin>
This optional parameter specifies the number of GPIO interrupt pin. If the
parameter absents, the driver will work at polling mode.

\i <int-level>
This optional parameter specifies the interrupt trigger level. 0 indicates
trigger by falling edge, and 1 indicates rising edge. If the parameter
absents, the falling edge is the trigger condition.

\i <xy-swap>
This optional parameter specifies whether enable swapping X and Y value. 1
indicates enable, 0 indicates disable. If the parameter absents, the driver will
disable swapping.

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
    ft5x06@38
        {
        compatible = "ft,5x06";
        reg = <0x38>;
        pinmux-0 = <&ft5x06_pad>;
        int-pin = <&gpio1 17>;
        int-level = <0>;
        xy-swap = <1>;
        display-resolution = <1024>,<600>;
        calibration = <5                /@ calibration point count @/
                       1024 600         /@ display resolution @/
                       0    0           /@ display point 1 @/
                       1023 0           /@ display point 2 @/
                       1023 599         /@ display point 3 @/
                       0    599         /@ display point 4 @/
                       511  299         /@ display point 5 @/
                       1023 767         /@ touch screen point 1 @/
                       0    767         /@ touch screen point 2 @/
                       0    0           /@ touch screen point 3 @/
                       1023 0           /@ touch screen point 4 @/
                       511  383>;       /@ touch screen point 5 @/
        axis-max-val = <1023>,<767>;
        };
\ce

*/

/* includes */

#include <evdevLib.h>
#include <hwif/vxBus.h>
#include <hwif/vxbus/vxbLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbI2cLib.h>
#include <subsys/gpio/vxbGpioLib.h>
#include <subsys/pinmux/vxbPinMuxLib.h>
#include "evdevDrvFt5x06.h"

/* forward declarations */

LOCAL STATUS    fdtFt5x06TsProbe (VXB_DEV_ID pDev);
LOCAL STATUS    fdtFt5x06TsAttach (VXB_DEV_ID pInst);
LOCAL STATUS    ft5x06Init (FT5X06_DATA * pCtrl);
LOCAL STATUS    ft5x06Open (VXB_DEV_ID pInst);
LOCAL STATUS    ft5x06Close (VXB_DEV_ID pInst);
LOCAL STATUS    ft5x06Read (VXB_DEV_ID pInst, EV_DEV_PTR_MT_DATA * pMtData);
LOCAL STATUS    ft5x06Ioctl (VXB_DEV_ID pInst, int request, _Vx_usr_arg_t
                             arg);
LOCAL STATUS    ft5x06I2CRead (FT5X06_DATA * pDev, UINT8 addr, UINT8 * pData,
                               UINT32 length);

/* locals */

LOCAL VXB_DRV_METHOD    fdtFt5x06TsMethodList[] =
    {
    { VXB_DEVMETHOD_CALL(vxbDevProbe), fdtFt5x06TsProbe},
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtFt5x06TsAttach},
    { 0, NULL }
    };

/* FT5X06 multi-touch openfirmware driver */

VXB_DRV vxbFdtFt5x06TsDrv =
    {
    {NULL},
    "FT5X06 TS",                            /* Name */
    "FT5X06 multi-touch Fdt driver",        /* Description */
    VXB_BUSID_FDT,                          /* Class */
    0,                                      /* Flags */
    0,                                      /* Reference count */
    fdtFt5x06TsMethodList,                  /* Method table */
    };

LOCAL const VXB_FDT_DEV_MATCH_ENTRY ft5x06TsMatch[] =
    {
        {
        FT5X06_DRIVER_NAME,                 /* compatible */
        (void *)NULL,
        },
        {}                                  /* Empty terminated list */
    };

VXB_DRV_DEF(vxbFdtFt5x06TsDrv)

/* functions */

/*******************************************************************************
*
* fdtFt5x06TsProbe - probe for device presence at specific address
*
* Check for FT5X06 touch screen contoller (or compatible) device at the
* specified base address. We assume one is present at that address, but we need
* to verify.
*
* RETURNS: OK if probe passes and assumed a valid FT5X06 touch screen
* controller (or compatible) device. ERROR otherwise.
*
*/

LOCAL STATUS fdtFt5x06TsProbe
    (
    VXB_DEV_ID  pDev
    )
    {
    return vxbFdtDevMatch (pDev, ft5x06TsMatch, NULL);
    }

/*******************************************************************************
*
* fdtFt5x06TsAttach - attach FT5X06 touch screen contoller
*
* This is the FT5X06 touch screen initialization routine.
*
* RETURNS: OK or ERROR when initialize failed
*
* ERRNO
*/

LOCAL STATUS fdtFt5x06TsAttach
    (
    VXB_DEV_ID  pInst
    )
    {
    VXB_FDT_DEV *       pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet(pInst);
    FT5X06_DATA *       pCtrl;
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

    pCtrl = (FT5X06_DATA *)vxbMemAlloc (sizeof (FT5X06_DATA));
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

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "xy-swap", &len);
    if (NULL != prop)
        {
        pCtrl->xySwap   = (BOOL)vxFdt32ToCpu (*prop);
        }
    else
        {
        pCtrl->xySwap   = FALSE;
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
            if (ERROR == vxbGpioIntConfig (pCtrl->intPin, INTR_TRIGGER_EDGE,
                                           INTR_POLARITY_LOW))
                {
                (void)vxbGpioFree (pCtrl->intPin);
                goto error;
                }
            }
        else
            {
            if (ERROR == vxbGpioIntConfig (pCtrl->intPin, INTR_TRIGGER_EDGE,
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
        if ((5 == pointCnt) || (9 == pointCnt))
            {
            pCtrl->pCalData->calibPtCount   = pointCnt;
            pCtrl->pCalData->disRect.right  = (int)vxFdt32ToCpu (*prop++) - 1;
            pCtrl->pCalData->disRect.bottom = (int)vxFdt32ToCpu (*prop++) - 1;

            for (i = 0; i < pointCnt; i++)
                {
                pCtrl->pCalData->disPoint[i].x  = (int)vxFdt32ToCpu (*prop++);
                pCtrl->pCalData->disPoint[i].y  = (int)vxFdt32ToCpu (*prop++);
                }

            for (i = 0; i < pointCnt; i++)
                {
                pCtrl->pCalData->tsPoint[i].x   = (int)vxFdt32ToCpu (*prop++);
                pCtrl->pCalData->tsPoint[i].y   = (int)vxFdt32ToCpu (*prop++);
                }
            }
        }

    if (0 == pCtrl->pCalData->calibPtCount)
        {
        pCtrl->pCalData->calibPtCount = 5;
        evdevTsGetCalibQuick (FT5X06_TOUCH_X_MAX, FT5X06_TOUCH_Y_MAX, 0, 0,
                              pCtrl->disWidth, pCtrl->disHeight,
                              pCtrl->pCalData);
        }

    (void)snprintf ((char *)pCtrl->pCalData->devName, TS_DEV_NAME_LEN,
                    "%s", FT5X06_DRIVER_NAME);

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "axis-max-val", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pCtrl->axisXMax = (int)vxFdt32ToCpu (*prop++);
        pCtrl->axisYMax = (int)vxFdt32ToCpu (*prop);
        }
    else
        {
        pCtrl->axisXMax = FT5X06_TOUCH_X_MAX;
        pCtrl->axisYMax = FT5X06_TOUCH_Y_MAX;
        }

    if (TASK_ID_ERROR == taskSpawn ("tsDeviceInit", TS_INIT_TASK_PRIO, 0,
                                    TS_INIT_TASK_STACK_SIZE,
                                    (FUNCPTR)ft5x06Init, (_Vx_usr_arg_t)pCtrl,
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
* ft5x06Init - touch screen initialization routine
*
* This routine initializes the touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*
*/

LOCAL STATUS ft5x06Init
    (
    FT5X06_DATA *   pCtrl
    )
    {
    TS_DEVICE   tsDev;
    UINT8       buf[FT5X06_TOUCH_REG_LEN];

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    if (ERROR == ft5x06I2CRead (pCtrl, 0x0, buf, FT5X06_TOUCH_REG_LEN))
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
        tsDev.samplePeriod = (FT5X06_SAMPLE_PERIOD * sysClkRateGet()) / 1000;
        }

    tsDev.pInst                     = pCtrl->pInst;
    tsDev.pCalData                  = pCtrl->pCalData;
    tsDev.calDataCount              = 1;
    tsDev.func.open                 = ft5x06Open;
    tsDev.func.close                = ft5x06Close;
    tsDev.func.ioctl                = ft5x06Ioctl;
    tsDev.func.read                 = ft5x06Read;
    tsDev.devData.devCap            = EV_DEV_KEY | EV_DEV_ABS | EV_DEV_ABS_MT;
    tsDev.devData.pDevName          = FT5X06_DRIVER_NAME;
    tsDev.devData.devInfo.bustype   = EV_DEV_BUS_I2C;
    tsDev.devData.devInfo.vendor    = 0;
    tsDev.devData.devInfo.product   = 0;
    tsDev.devData.devInfo.version   = 0;
    tsDev.defDisRect.right          = (int)pCtrl->disWidth - 1;
    tsDev.defDisRect.bottom         = (int)pCtrl->disHeight - 1;
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
* ft5x06Open - open touch screen device
*
* This routine opens touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS ft5x06Open
    (
    VXB_DEV_ID  pInst
    )
    {
    FT5X06_DATA *   pCtrl = (FT5X06_DATA *)vxbDevSoftcGet (pInst);

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
* ft5x06Close - close touch screen device
*
* This routine close touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS ft5x06Close
    (
    VXB_DEV_ID  pInst
    )
    {
    FT5X06_DATA *   pCtrl = (FT5X06_DATA *)vxbDevSoftcGet (pInst);

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
* ft5x06Read - read x and y value from touch screen device
*
* This routine reads x and y value from touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS ft5x06Read
    (
    VXB_DEV_ID              pInst,
    EV_DEV_PTR_MT_DATA *    pMtData
    )
    {
    UINT8           buf[FT5X06_TOUCH_REG_LEN];
    FT5X06_DATA *   pCtrl = (FT5X06_DATA *)vxbDevSoftcGet (pInst);
    UINT8           i;
    UINT8           id;
    int             x;
    int             y;
    STATUS          ret;

    if ((NULL == pCtrl) || (NULL == pMtData))
        {
        return ERROR;
        }

    bzero ((char *)buf, FT5X06_TOUCH_REG_LEN);
    ret = ft5x06I2CRead (pCtrl, 0x0, buf, FT5X06_TOUCH_REG_LEN);

    if (pCtrl->intMode)
        {
        ret |= vxbGpioIntEnable (pCtrl->intPin, (VOIDFUNCPTR)evdevTsDevIsr,
                                 (void *)pCtrl->pDevInfo);
        }

    if (ERROR == ret)
        {
        return ERROR;
        }

    pMtData->count = FT5X06_TOUCH_POINTS_MAX;
    if (buf[FT5X06_TOUCH_POINTS] > 0)
        {
        for (i = 0; i < FT5X06_TOUCH_POINTS_MAX; i++)
            {
            id  = (buf[FT5X06_TOUCH1_YH + i * 6] & 0xF0) >> 4;
            x   = (int)(((buf[FT5X06_TOUCH1_XH + i * 6] & 0xF) << 8) |
                        buf[FT5X06_TOUCH1_XL + i * 6]);
            y   = (int)(((buf[FT5X06_TOUCH1_YH + i * 6] & 0xF) << 8) |
                        buf[FT5X06_TOUCH1_YL + i * 6]);

            if ((buf[FT5X06_TOUCH1_XH + i * 6] & 0xC0) == 0x80)
                {
                pMtData->points[id].pressed = TRUE;
                }

            if (pCtrl->xySwap)
                {
                pMtData->points[id].x   = y;
                pMtData->points[id].y   = x;
                }
            else
                {
                pMtData->points[id].x   = x;
                pMtData->points[id].y   = y;
                }
            }
        }
    return OK;
    }

/*******************************************************************************
*
* ft5x06Ioctl - handle IOCTL for touch screen device
*
* This routine handles IOCTL for touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS ft5x06Ioctl
    (
    VXB_DEV_ID      pInst,
    int             request,    /* ioctl function */
    _Vx_usr_arg_t   arg         /* function arg */
    )
    {
    STATUS          result = OK;
    FT5X06_DATA *   pCtrl = (FT5X06_DATA *)vxbDevSoftcGet (pInst);

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
* ft5x06I2CRead - read register value from device
*
* This routine reads register value from device.
*
* RETURNS: OK when successfully read; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS ft5x06I2CRead
    (
    FT5X06_DATA *   pDev,
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
