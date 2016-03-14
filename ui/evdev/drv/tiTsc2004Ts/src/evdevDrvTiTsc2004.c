/* evdevDrvTiTsc2004.c - TI TSC2004 touch screen controller driver */

/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
14sep15,jnl  support querying size of the touchscreen area. (V7GFX-238)
01sep14,y_f  removed pTsCtrl (V7GFX-208)
20jun14,y_f  updated to support VXBUS GEN2 (US42301)
23jul13,y_f  created
*/

/*
DESCRIPTION

This is the VxBus driver for TI TSC2004 touch screen controller. This driver
provides callback methods to support the evdev library, and the client users
should use the API provided by the evdev library to access the low level
hardware function.

To add the driver to the vxWorks image, add the following component to the
kernel configuration.

\cs
vxprj component add DRV_TOUCH_SCREEN_TI_TSC2004
\ce

This driver is bound to device tree, and the device tree node must specify
below parameters:

\is

\i <compatible>
This required parameter specifies the name of the driver. It must be
"ti,tsc2004".

\i <reg>
This parameter specifies the device address of this module.

\i <display-resolution>
This optional parameter specifies the resolution of the LCD display. The first
is the width. The second is the height. If the parameter absents, the driver
will use the default value.

\i <calibration>
This optional parameter specifies the calibration data that this module use. If
the parameter absents, the driver will use the default value.

\i <axis-max-val>
This optional parameter specifies the max axis value. If the parameter absents, 
the driver will use the default value.

\ie

An example of device node is shown below:

\cs
    tsc2004@48
        {
        compatible = "ti,tsc2004";
        reg = <0x48>;
        display-resolution = <800>,<480>;
        calibration = <5                /@ calibration point count @/
                       800  480         /@ display resolution @/
                       16   16          /@ display point 1 @/
                       782  16          /@ display point 2 @/
                       782  462         /@ display point 3 @/
                       16   462         /@ display point 4 @/
                       399  239         /@ display point 5 @/
                       3906 348         /@ touch screen point 1 @/
                       157  299         /@ touch screen point 2 @/
                       152  3802        /@ touch screen point 3 @/
                       3895 3761        /@ touch screen point 4 @/
                       2016 2076>;      /@ touch screen point 5 @/
        axis-max-val = <4095>,<4095>;
        };
\ce

*/

/* includes */

#include <evdevLib.h>
#include <hwif/vxBus.h>
#include <hwif/vxbus/vxbLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbI2cLib.h>
#include "evdevDrvTiTsc2004.h"

/* forward declarations */

LOCAL STATUS    fdtTiTsc2004TsProbe (VXB_DEV_ID pDev);
LOCAL STATUS    fdtTiTsc2004TsAttach (VXB_DEV_ID pInst);
LOCAL STATUS    tiTsc2004Init (TI_TSC2004_DATA * pCtrl);
LOCAL STATUS    tiTsc2004Open (VXB_DEV_ID pInst);
LOCAL STATUS    tiTsc2004Close (VXB_DEV_ID pInst);
LOCAL STATUS    tiTsc2004Read (VXB_DEV_ID pInst, TS_POINT * pPoint);
LOCAL STATUS    tiTsc2004Ioctl (VXB_DEV_ID pInst, int request, _Vx_usr_arg_t
                                arg);
LOCAL STATUS    tiTsc2004I2CRead (TI_TSC2004_DATA * pDev, UINT8 addr, UINT8 *
                                  pData, UINT32 length);
LOCAL STATUS    tiTsc2004I2CWrite (TI_TSC2004_DATA * pDev, UINT8 * pData, UINT32
                                   length);

/* locals */

LOCAL VXB_DRV_METHOD    fdtTiTsc2004TsMethodList[] =
    {
    { VXB_DEVMETHOD_CALL(vxbDevProbe), fdtTiTsc2004TsProbe},
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtTiTsc2004TsAttach},
    { 0, NULL }
    };

/* TI TSC2004 touch screen openfirmware driver */

VXB_DRV vxbFdtTiTsc2004TsDrv =
    {
    {NULL},
    "TI TSC2004 TS",                        /* Name */
    "TI TSC2004 touch screen Fdt driver",   /* Description */
    VXB_BUSID_FDT,                          /* Class */
    0,                                      /* Flags */
    0,                                      /* Reference count */
    fdtTiTsc2004TsMethodList,               /* Method table */
    };

LOCAL const VXB_FDT_DEV_MATCH_ENTRY tiTsc2004TsMatch[] =
    {
        {
        TI_TSC2004_DRIVER_NAME,             /* compatible */
        (void *)NULL,
        },
        {}                                  /* Empty terminated list */
    };

LOCAL TS_CALIB_DATA tiTsc2004CalData =
    {
        {TI_TSC2004_DRIVER_NAME},       /* touch screen name */
        {0, 0, (800 - 1), (480 - 1)},   /* display rect */
        5,

        /* display point */

        {{16, 16}, {782, 16}, {782, 462}, {16, 462}, {399, 239}},

        /* touch screen point */

        {{3906, 348}, {157, 299}, {152, 3802}, {3895, 3761}, {2016, 2076}}
    };

VXB_DRV_DEF(vxbFdtTiTsc2004TsDrv)

/* functions */

/*******************************************************************************
*
* fdtTiTsc2004TsProbe - probe for device presence at specific address
*
* Check for TI TSC2004 touch screen contoller (or compatible) device at the
* specified base address. We assume one is present at that address, but we need
* to verify.
*
* RETURNS: OK if probe passes and assumed a valid TI TSC2004 touch screen
* controller (or compatible) device. ERROR otherwise.
*
*/

LOCAL STATUS fdtTiTsc2004TsProbe
    (
    VXB_DEV_ID  pDev
    )
    {
    return vxbFdtDevMatch (pDev, tiTsc2004TsMatch, NULL);
    }

/*******************************************************************************
*
* fdtTiTsc2004TsAttach - attach TI TSC2004 touch screen contoller
*
* This is the TI TSC2004 touch screen initialization routine.
*
* RETURNS: OK or ERROR when initialize failed
*
* ERRNO
*/

LOCAL STATUS fdtTiTsc2004TsAttach
    (
    VXB_DEV_ID  pInst
    )
    {
    VXB_FDT_DEV *       pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet(pInst);
    TI_TSC2004_DATA *   pCtrl;
    VXB_RESOURCE *      pRegRes  = NULL;
    VXB_RESOURCE_ADR *  pResAdr;
    UINT32 *            prop;
    int                 len;
    UINT8               pointCnt;
    UINT8               i;

    if (NULL == pFdtDev)
        {
        return ERROR;
        }

    pCtrl = (TI_TSC2004_DATA *)vxbMemAlloc (sizeof (TI_TSC2004_DATA));
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

    pRegRes = vxbResourceAlloc (pInst, VXB_RES_MEMORY, 0);
    if (NULL == pRegRes)
       {
       goto error;
       }

    pResAdr = (VXB_RESOURCE_ADR *)pRegRes->pRes;
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
        pCtrl->disWidth     = TI_TSC2004_DEFAULT_DIS_WIDTH;
        pCtrl->disHeight    = TI_TSC2004_DEFAULT_DIS_HEIGHT;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "calibration", &len);
    if (NULL != prop)
        {
        pointCnt = (UINT8)vxFdt32ToCpu (*prop++);
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
        bcopy ((char *)&tiTsc2004CalData, (char *)pCtrl->pCalData,
               sizeof (TS_CALIB_DATA));
        }

    (void)snprintf ((char *)pCtrl->pCalData->devName, TS_DEV_NAME_LEN,
                    "%s", TI_TSC2004_DRIVER_NAME);

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "axis-max-val", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pCtrl->axisXMax = (int)vxFdt32ToCpu (*prop++);
        pCtrl->axisYMax = (int)vxFdt32ToCpu (*prop);
        }
    else
        {
        pCtrl->axisXMax = TI_TSC2004_TOUCH_X_MAX;
        pCtrl->axisYMax = TI_TSC2004_TOUCH_Y_MAX;
        }

    if (TASK_ID_ERROR == taskSpawn ("tsDeviceInit", TS_INIT_TASK_PRIO, 0,
                                    TS_INIT_TASK_STACK_SIZE,
                                    (FUNCPTR)tiTsc2004Init,
                                    (_Vx_usr_arg_t)pCtrl,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0))
        {
        goto error;
        }

    /* save Drvctrl */

    vxbDevSoftcSet (pInst, (void *)pCtrl);

    return OK;

error:
    if (NULL != pRegRes)
        {
        (void)vxbResourceFree (pInst, pRegRes);
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
* tiTsc2004Init - touch screen initialization routine
*
* This routine initializes the touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*
*/

LOCAL STATUS tiTsc2004Init
    (
    TI_TSC2004_DATA *   pCtrl
    )
    {
    TS_DEVICE   tsDev;
    UINT16      value;
    UINT8       data[4];

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    /*
     * write control 8bits
     * Control Register layout.
     *
     * bit
     * 3:4  get X, Y, Z1 and Z2 coordinates from X, Y, Z1 and Z2 data registers
     * 2    conversion result resolution is 12-bit
     */

    bzero ((char *)data, sizeof (data) / sizeof (UINT8));
    data[0] = TI_TSC2004_CONTROL1 | TI_TSC2004_CONTROL1_RM_VALUE;
    if (ERROR == tiTsc2004I2CWrite (pCtrl, data, 1))
        {
        return ERROR;
        }

    data[0] = TI_TSC2004_CONTROL1 | TI_TSC2004_CONTROL1_RM_VALUE |
              TI_TSC2004_CONTROL1_XYZ_VALUE;
    if (ERROR == tiTsc2004I2CWrite (pCtrl, data, 1))
        {
        return ERROR;
        }

    /*
     * set CFR0 16bits
     * CFR0 Register layout.
     *
     * bit
     * 15    set convert controlled by TSC2004
     * 13    12bit A/D resolution
     * 11:2  1.2V and A/D 2Mhz clock
     * 8:3   stable time
     * 5:3   precharge time
     * 2:3   sense time
     * 1     enable pen touch detection in wait
     * 0     longer sampling mode,when CL01 sets 1.2V, LSM must be set 1
     */

    bzero ((char *)data, sizeof (data) / sizeof (UINT8));
    data[0] = TI_TSC2004_WRITE_CFG0     | TI_TSC2004_CONTROL0_PNG_VALUE;
    value   = TI_TSC2004_CF0_PSM_VALUE  | TI_TSC2004_CF0_RM_VALUE   |
              TI_TSC2004_CL01_VALUE     | TI_TSC2004_PV012_VALUE    |
              TI_TSC2004_PR012_VALUE    | TI_TSC2004_SNS012_VALUE   |
              TI_TSC2004_DTW_VALUE      | TI_TSC2004_LSM_VALUE;

    data[1] = (UINT8)(value >> 8);
    data[2] = (UINT8)(value);

    if (ERROR == tiTsc2004I2CWrite (pCtrl, data, 3))
        {
        return ERROR;
        }

    /* set CFR 1 16bits, config delay time before a sample/conversion */

    bzero ((char *)data, sizeof (data) / sizeof (UINT8));
    data[0] = TI_TSC2004_WRITE_CFG1     | TI_TSC2004_CONTROL0_PNG_VALUE;
    value   = TI_TSC2004_BTD012_VALUE   | TI_TSC2004_TBM0123_VALUE;

    data[1] = (UINT8)(value >> 8);
    data[2] = (UINT8)(value);

    if (ERROR == tiTsc2004I2CWrite (pCtrl, data, 3))
        {
        return ERROR;
        }

    /* set CFR 2 16bits, DAV is output on PINTDAV */

    bzero ((char *)data, sizeof (data) / sizeof (UINT8));
    data[0] = TI_TSC2004_WRITE_CFG2     | TI_TSC2004_CONTROL0_PNG_VALUE;
    value   = TI_TSC2004_M01_VALUE  | TI_TSC2004_W01_VALUE  |
              TI_TSC2004_XYZ_VALUE  | TI_TSC2004_PINTDAV_VALUE_PIO;

    data[1] = (UINT8)(value >> 8);
    data[2] = (UINT8)(value);

    if (ERROR == tiTsc2004I2CWrite (pCtrl, data, 3))
        {
        return ERROR;
        }

    bzero ((char *)&tsDev, sizeof (TS_DEVICE));

    tsDev.devAttr = TS_DEV_ATTR_POLLING_PRESS |
                    TS_DEV_ATTR_POLLING_SAMPLE |
                    TS_DEV_ATTR_POLLING_RELEASE;
    tsDev.samplePeriod
        = (TI_TSC2004_SAMPLE_TIMEOUT * sysClkRateGet()) / 1000;
    tsDev.releasePeriod
        = (TI_TSC2004_RELEASE_TIMEOUT * sysClkRateGet()) / 1000;

    tsDev.pInst                     = pCtrl->pInst;
    tsDev.pCalData                  = pCtrl->pCalData;
    tsDev.calDataCount              = 1;
    tsDev.func.open                 = tiTsc2004Open;
    tsDev.func.close                = tiTsc2004Close;
    tsDev.func.ioctl                = tiTsc2004Ioctl;
    tsDev.func.read                 = tiTsc2004Read;
    tsDev.devData.devCap            = EV_DEV_KEY | EV_DEV_ABS;
    tsDev.devData.pDevName          = TI_TSC2004_DRIVER_NAME;
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
* tiTsc2004Open - open touch screen device
*
* This routine opens touch screen device.
*
* RETURNS: Always OK
*
* ERRNO: N/A
*/

LOCAL STATUS tiTsc2004Open
    (
    VXB_DEV_ID  pInst
    )
    {
    return OK;
    }

/*******************************************************************************
*
* tiTsc2004Close - close touch screen device
*
* This routine close touch screen device.
*
* RETURNS: Always OK
*
* ERRNO: N/A
*/

LOCAL STATUS tiTsc2004Close
    (
    VXB_DEV_ID  pInst
    )
    {
    return OK;
    }

/*******************************************************************************
*
* tiTsc2004Read - read x and y value from touch screen device
*
* This routine reads x and y value from touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiTsc2004Read
    (
    VXB_DEV_ID  pInst,
    TS_POINT *  pPoint
    )
    {
    UINT8               buf[8];
    TI_TSC2004_DATA *   pCtrl = (TI_TSC2004_DATA *)vxbDevSoftcGet (pInst);

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    bzero ((char *)buf, 8);
    if (ERROR == tiTsc2004I2CRead (pCtrl, TI_TSC2004_READ_CFG0, buf, 1))
        {
        return ERROR;
        }

    if (0 != (buf[0] & (TI_TSC2004_CF0_PSM_VALUE >> 8)))
        {
        bzero ((char *)buf, 8);
        if (ERROR == tiTsc2004I2CRead (pCtrl, TI_TSC2004_READ_STATUS, buf, 2))
            {
            return ERROR;
            }

        if (TI_TSC2004_XYZ_STATUS == (buf[0] & TI_TSC2004_XYZ_STATUS))
            {
            bzero ((char *)buf, 8);
            if (ERROR == tiTsc2004I2CRead (pCtrl, TI_TSC2004_DATA_X_ADDR, buf,
                                           8))
                {
                return ERROR;
                }

            pPoint->x = (int)((buf[0] << 8) | buf[1]);
            pPoint->y = (int)((buf[2] << 8) | buf[3]);
            pPoint->pressed = TRUE;
            return OK;
            }
        else
            {
            return ERROR;
            }
        }
    else
        {
        pPoint->pressed = FALSE;
        return OK;
        }
    }

/*******************************************************************************
*
* tiTsc2004Ioctl - handle IOCTL for touch screen device
*
* This routine handles IOCTL for touch screen device.
*
* RETURNS: Always ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiTsc2004Ioctl
    (
    VXB_DEV_ID      pInst,
    int             request,    /* ioctl function */
    _Vx_usr_arg_t   arg         /* function arg */
    )
    {
    return ERROR;
    }

/*******************************************************************************
*
* tiTsc2004I2CRead - read register value from device
*
* This routine reads register value from device.
*
* RETURNS: OK when successfully read; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiTsc2004I2CRead
    (
    TI_TSC2004_DATA *   pDev,
    UINT8               addr,
    UINT8 *             pData,
    UINT32              length
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

/*******************************************************************************
*
* tiTsc2004I2CWrite - write register value to device
*
* This routine writes register value to device.
*
* RETURNS: OK when successfully written; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiTsc2004I2CWrite
    (
    TI_TSC2004_DATA *   pDev,
    UINT8 *             pData,
    UINT32              length
    )
    {
    I2C_MSG msg;

    bzero ((char *)&msg, sizeof (I2C_MSG));
    msg.addr    = pDev->devAddr;
    msg.buf     = pData;
    msg.len     = length;
    msg.scl     = FAST_MODE;
    msg.wrTime  = 5000; /* Write Cycle Time - 5ms */

    return vxbI2cDevXfer (pDev->pInst, &msg, 1);
    }
