/* evdevDrvTiAm335x.c - TI AM335X touch screen controller routines */

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
15aug13,y_f  created
*/

/*
DESCRIPTION

This is the VxBus driver for TI AM335X touchscreen controller. This driver
provides callback methods to support the evdev library, and the client users
should use the API provided by the evdev library to access the low level
hardware function.

To add the driver to the vxWorks image, add the following component to the
kernel configuration.

\cs
vxprj component add DRV_TOUCH_SCREEN_TI_AM335X
\ce

This driver is bound to device tree, and the device tree node must specify
below parameters:

\is

\i <compatible>
This required parameter specifies the name of the driver. It must be
"ti,am3359-tscadc".

\i <reg>
This required parameter specifies the device address of this module.

\i <interrupt-parent>
This required parameter specifies the offset of interrupt controller.

\i <interrupts>
This required parameter specifies the interrupt number of this module.

\i <clocks>
This required parameter specifies the list of clock.

\i <clock-names>
This required parameter specifies the clock names of this module.

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
    tscadc@44e0d000
        {
        compatible = "ti,am3359-tscadc";
        reg = <0x44e0d000 0x1000>;
        interrupt-parent = <&intc>;
        interrupts = <16>;
        clocks = <&clk 51>,<&clk 50>;
        clock-names = "adc-tsc-ick","adc-tsc-fck";
        display-resolution = <800>,<480>;
        calibration = <5                /@ calibration point count @/
                       800  480         /@ display resolution @/
                       49   59          /@ display point 1 @/
                       749  59          /@ display point 2 @/
                       749  419         /@ display point 3 @/
                       49   419         /@ display point 4 @/
                       399  239         /@ display point 5 @/
                       3744 3352        /@ touch screen point 1 @/
                       326  3370        /@ touch screen point 2 @/
                       331  737         /@ touch screen point 3 @/
                       3764 756         /@ touch screen point 4 @/
                       2040 2067>;      /@ touch screen point 5 @/
        axis-max-val = <4095>,<4095>;
        };
\ce

*/

/* includes */

#include <evdevLib.h>
#include <intLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include "evdevDrvTiAm335x.h"

/* forward declarations */

LOCAL STATUS    fdtTiAm335xTsProbe (VXB_DEV_ID  pDev);
LOCAL STATUS    fdtTiAm335xTsAttach (VXB_DEV_ID pDev);
LOCAL STATUS    am335xTsInit (AM335X_TS_DATA * pCtrl);
LOCAL STATUS    am335xTsOpen (VXB_DEV_ID pInst);
LOCAL STATUS    am335xTsClose (VXB_DEV_ID pInst);
LOCAL STATUS    am335xTsRead (VXB_DEV_ID pInst, TS_POINT * pPoint);
LOCAL STATUS    am335xTsIoctl (VXB_DEV_ID pInst, int request,
                               _Vx_usr_arg_t arg);
LOCAL STATUS    am335xTsIntCheck (AM335X_TS_DATA * pCtrl, TS_POINT * pTsPoint);
LOCAL void      am335xTsStepConfig (AM335X_TS_DATA * pCtrl);

/* locals */

LOCAL VXB_DRV_METHOD    fdtTiAm335xTsMethodList[] =
    {
    { VXB_DEVMETHOD_CALL(vxbDevProbe), fdtTiAm335xTsProbe},
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtTiAm335xTsAttach},
    { 0, NULL }
    };

/* TI AM335X touch screen openfirmware driver */

VXB_DRV vxbFdtTiAm335xTsDrv =
    {
    {NULL},
    "TI AM335X TS",                         /* Name */
    "TI AM335X touch screen Fdt driver",    /* Description */
    VXB_BUSID_FDT,                          /* Class */
    0,                                      /* Flags */
    0,                                      /* Reference count */
    fdtTiAm335xTsMethodList,                /* Method table */
    };

LOCAL const VXB_FDT_DEV_MATCH_ENTRY tiAm335xTsMatch[] =
    {
        {
        AM335X_ADC_TSC_DRIVER_NAME,         /* compatible */
        (void *)NULL,
        },
        {}                                  /* Empty terminated list */
    };

LOCAL TS_CALIB_DATA am335xCalData =
    {
    {AM335X_ADC_TSC_DRIVER_NAME},           /* touch screen name */
    {0, 0, (800 - 1), (480 - 1)},           /* display rect */
    5,

     /* display point */

    {{49, 59}, {749, 59}, {749, 419}, {49, 419}, {399, 239}},

    /* touch screen point */

    {{3744, 3352}, {326, 3370}, {331, 737}, {3764, 756}, {2040, 2067}}
    };

VXB_DRV_DEF(vxbFdtTiAm335xTsDrv)

/* functions */

/*******************************************************************************
*
* fdtTiAm335xTsProbe - probe for device presence at specific address
*
* Check for TI AM335X touch screen contoller (or compatible) device at the
* specified base address. We assume one is present at that address, but we need
* to verify.
*
* RETURNS: OK if probe passes and assumed a valid TI AM335X touch screen
* controller (or compatible) device. ERROR otherwise.
*
*/

LOCAL STATUS fdtTiAm335xTsProbe
    (
    VXB_DEV_ID  pDev
    )
    {
    return vxbFdtDevMatch (pDev, tiAm335xTsMatch, NULL);
    }

/*******************************************************************************
*
* fdtTiAm335xTsAttach - attach TI AM335X touch screen contoller
*
* This is the TI AM335X touch screen initialization routine.
*
* RETURNS: OK or ERROR when initialize failed
*
* ERRNO
*/

LOCAL STATUS fdtTiAm335xTsAttach
    (
    VXB_DEV_ID  pInst
    )
    {
    VXB_FDT_DEV *       pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet(pInst);
    AM335X_TS_DATA *    pCtrl;
    VXB_RESOURCE *      pRegRes  = NULL;
    VXB_RESOURCE_ADR *  pResAdr;
    VXB_RESOURCE *      pIntRes  = NULL;
    UINT32 *            prop;
    int                 len;
    UINT8               i;
    UINT8               pointCnt;

    if (NULL == pFdtDev)
        {
        return ERROR;
        }

    pCtrl = (AM335X_TS_DATA *)vxbMemAlloc (sizeof (AM335X_TS_DATA));
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

    pCtrl->vxbHandle    = pResAdr->pHandle;
    pCtrl->baseAddr     = (void *)pResAdr->virtual;

    /* get interrupt resource */

    pIntRes = vxbResourceAlloc (pInst, VXB_RES_IRQ, 0);
    if (NULL == pIntRes)
       {
       goto error;
       }

    pCtrl->intRes = pIntRes;

    prop = vxFdtPropGet (pFdtDev->offset, "display-resolution", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pCtrl->disWidth     = vxFdt32ToCpu (*prop++);
        pCtrl->disHeight    = vxFdt32ToCpu (*prop);
        }
    else
        {
        pCtrl->disWidth     = AM335X_ADC_TSC_DEFAULT_DIS_WIDTH;
        pCtrl->disHeight    = AM335X_ADC_TSC_DEFAULT_DIS_HEIGHT;
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
        bcopy ((char *)&am335xCalData, (char *)pCtrl->pCalData,
               sizeof (TS_CALIB_DATA));
        }

    (void)snprintf ((char *)pCtrl->pCalData->devName, TS_DEV_NAME_LEN,
                    "%s", AM335X_ADC_TSC_DRIVER_NAME);

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "axis-max-val", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pCtrl->axisXMax = (int)vxFdt32ToCpu (*prop++);
        pCtrl->axisYMax = (int)vxFdt32ToCpu (*prop);
        }
    else
        {
        pCtrl->axisXMax = AM335X_ADC_TSC_X_MAX;
        pCtrl->axisYMax = AM335X_ADC_TSC_Y_MAX;
        }

    if (TASK_ID_ERROR == taskSpawn ("tsDeviceInit", TS_INIT_TASK_PRIO, 0,
                                    TS_INIT_TASK_STACK_SIZE,
                                    (FUNCPTR)am335xTsInit, (_Vx_usr_arg_t)pCtrl,
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

    if (NULL != pIntRes)
        {
        (void)vxbResourceFree (pInst, pIntRes);
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
* am335xTsInit - touch screen driver initialization routine
*
* This routine initializes the touch screen device driver.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*
*/

LOCAL STATUS am335xTsInit
    (
    AM335X_TS_DATA *    pCtrl
    )
    {
    TS_DEVICE   tsDev;

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    if (OK != vxbClkEnableAll (pCtrl->pInst))
        {
        return ERROR;
        }

    tsDev.devAttr                   = TS_DEV_ATTR_INTERRUPT |
                                      TS_DEV_ATTR_POLLING_RELEASE;
    tsDev.releasePeriod             = (AM335X_RELEASE_TIMEOUT *
                                       sysClkRateGet ()) / 1000;
    tsDev.pInst                     = (void *)pCtrl->pInst;
    tsDev.pCalData                  = pCtrl->pCalData;
    tsDev.calDataCount              = 1;
    tsDev.func.open                 = am335xTsOpen;
    tsDev.func.close                = am335xTsClose;
    tsDev.func.read                 = am335xTsRead;
    tsDev.func.ioctl                = am335xTsIoctl;
    tsDev.devData.devCap            = EV_DEV_KEY | EV_DEV_ABS;
    tsDev.devData.pDevName          = AM335X_ADC_TSC_DRIVER_NAME;
    tsDev.devData.devInfo.bustype   = EV_DEV_BUS_HOST;
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
        (void)vxbClkDisableAll (pCtrl->pInst);
        return ERROR;
        }
    else
        {
        return OK;
        }
    }

/*******************************************************************************
*
* am335xTsOpen - open touch screen device
*
* This routine opens touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS am335xTsOpen
    (
    VXB_DEV_ID  pInst
    )
    {
    volatile UINT32     value;
    volatile UINT32     fifoCount;
    UINT32              i;
    AM335X_TS_DATA *    pCtrl = (AM335X_TS_DATA *)vxbDevSoftcGet (pInst);

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    /* connect int */

    if (ERROR == vxbIntConnect (pInst, pCtrl->intRes,
                                (VOIDFUNCPTR)evdevTsDevIsr,
                                (void *)pCtrl->pDevInfo))
        {
        return ERROR;
        }

    if (ERROR == vxbIntEnable (pInst, pCtrl->intRes))
        {
        (void)vxbIntDisconnect (pInst, pCtrl->intRes);
        return ERROR;
        }

    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_CLKDIV,
                    (ADC_TSC_SRC_CLK / ADC_TSC_CLK - 1));

    value = AM335X_ADC_TSC_CTRL_WRITEPROTECT |
            AM335X_ADC_TSC_CTRL_TOUCH_SCREEN_EN |
            AM335X_ADC_TSC_CTRL_STEP_ID |
            AM335X_ADC_TSC_CTRL_4WIRE;
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_CTRL, value);

    value = AM335X_ADC_TSC_IDLECONFIG_YNNSW |
            AM335X_ADC_TSC_IDLECONFIG_YPNSW |
            AM335X_ADC_TSC_IDLECONFIG_SEL_INM |
            AM335X_ADC_TSC_IDLECONFIG_SEL_INP;
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_IDLECONFIG, value);

    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_FIFO1THRESHOLD, Y_MAX_NUM);

    value = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_IRQENABLE_CLR);
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_IRQENABLE_CLR, value);

    value = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_IRQSTATUS);
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_IRQSTATUS, value);

    am335xTsStepConfig (pCtrl);

    value = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_CTRL);
    value |= AM335X_ADC_TSC_CTRL_ENABLE;
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_CTRL, value);

    bzero ((char *)&(pCtrl->point), sizeof (TS_POINT));

    fifoCount = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO0COUNT);

    for (i = 0; i < fifoCount; i++)
        {
        (void)AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO0DATA);
        }

    fifoCount = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO1COUNT);

    for (i = 0; i < fifoCount; i++)
        {
        (void)AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO1DATA);
        }

    pCtrl->errorCount   = 0;
    pCtrl->isFirstOpen  = TRUE;
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_IRQENABLE_SET,
                    AM335X_ADC_TSC_IRQ_FIFO1THRES);

    return OK;
    }

/*******************************************************************************
*
* am335xTsClose - close touch screen device
*
* This routine closes touch screen device.
*
* RETURNS: always return OK
*
* ERRNO: N/A
*/

LOCAL STATUS am335xTsClose
    (
    VXB_DEV_ID  pInst
    )
    {
    volatile UINT32     value;
    volatile UINT32     fifoCount;
    UINT32              i;
    AM335X_TS_DATA *    pCtrl = (AM335X_TS_DATA *)vxbDevSoftcGet (pInst);

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_STEPENABLE, 0x0);

    value = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_IRQENABLE_CLR);
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_IRQENABLE_CLR, value);

    value = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_IRQSTATUS);
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_IRQSTATUS, value);

    fifoCount = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO0COUNT);

    for (i = 0; i < fifoCount; i++)
        {
        (void)AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO0DATA);
        }

    fifoCount = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO1COUNT);

    for (i = 0; i < fifoCount; i++)
        {
        (void)AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO1DATA);
        }

    value = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_CTRL);
    value &= ~(AM335X_ADC_TSC_CTRL_ENABLE |
               AM335X_ADC_TSC_CTRL_TOUCH_SCREEN_EN);
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_CTRL, value);

    (void)vxbIntDisable (pCtrl->pInst, pCtrl->intRes);
    (void)vxbIntDisconnect (pInst, pCtrl->intRes);

    return OK;
    }

/*******************************************************************************
*
* am335xTsRead - read x and y value from touch screen device
*
* This routine reads x and y value from touch screen device.
*
* RETURNS: always return OK
*
* ERRNO: N/A
*/

LOCAL STATUS am335xTsRead
    (
    VXB_DEV_ID  pInst,
    TS_POINT *  pPoint
    )
    {
    AM335X_TS_DATA *    pCtrl = (AM335X_TS_DATA *)vxbDevSoftcGet (pInst);

    memcpy ((void *)pPoint, (const void *)&pCtrl->point, sizeof (TS_POINT));
    return OK;
    }

/*******************************************************************************
*
* am335xTsIoctl - handle IOCTL for touch screen device
*
* This routine handles IOCTL for touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS am335xTsIoctl
    (
    VXB_DEV_ID      pInst,
    int             request,    /* ioctl function */
    _Vx_usr_arg_t   arg         /* function arg */
    )
    {
    STATUS              result = OK;
    AM335X_TS_DATA *    pCtrl = (AM335X_TS_DATA *)vxbDevSoftcGet (pInst);

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    switch (request)
        {
        case TS_CHECK_INT:
            result = am335xTsIntCheck (pCtrl, (TS_POINT *)arg);
            break;

        default:
            errno = S_ioLib_UNKNOWN_REQUEST;
            result = ERROR;
        }

    return result;
    }

/*******************************************************************************
*
* am335xTsIntCheck - check whether is touch screen IRQ
*
* This routine checks whether is touch screen IRQ.
*
* RETURNS: OK, if pressed or released touch screen; ERROR, if not a touch screen
* IRQ
*
* ERRNO: N/A
*/

LOCAL STATUS am335xTsIntCheck
    (
    AM335X_TS_DATA *    pCtrl,
    TS_POINT *          pTsPoint
    )
    {
    UINT32  value;
    UINT32  adcStat;
    UINT32  irqStatRaw;
    UINT32  fifoCount;
    INT16   xData[X_MAX_NUM];
    INT16   yData[Y_MAX_NUM];
    UINT16  i;

    value = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_IRQSTATUS);
    adcStat = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_ADCSTAT);
    irqStatRaw = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_IRQSTATUS_RAW);

    if ((value & AM335X_ADC_TSC_IRQ_FIFO1THRES) != 0)
        {
        fifoCount = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO0COUNT);
        if (fifoCount > (X_MAX_NUM + PRESSURE_MAX_NUM))
            {
            for (i = 0; i < (fifoCount - (X_MAX_NUM + PRESSURE_MAX_NUM)); i++)
                {
                (void)AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO0DATA);
                }
            }

        for (i = 0; i < X_MAX_NUM; i++)
            xData[i] = (INT16)(AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO0DATA)
                               & 0xfff);

        fifoCount = AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO1COUNT);
        if (fifoCount > (Y_MAX_NUM + PRESSURE_MAX_NUM))
            {
            for (i = 0; i < (fifoCount - (Y_MAX_NUM + PRESSURE_MAX_NUM)); i++)
                {
                (void)AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO1DATA);
                }
            }

        for (i = 0; i < Y_MAX_NUM; i++)
            {
            yData[i] = (INT16)(AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO1DATA)
                               & 0xfff);
            }

        (void)AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO0DATA);
        (void)AM335X_READ32 (pCtrl, AM335X_ADC_TSC_FIFO1DATA);

        if (pCtrl->isFirstOpen)
            {
            pCtrl->isFirstOpen = FALSE;
            }
        else
            {
            if (evdevTsEvalPoint (xData, SAMPLE_MAX_DIFF) &&
                evdevTsEvalPoint (yData, SAMPLE_MAX_DIFF))
                {
                /* send coordinate of the point to input service */

                if (pCtrl->point.pressed)
                    {
                    if (((abs(pCtrl->point.y - yData[0]) < SAMPLE_MAX_MOVE) &&
                          (abs(pCtrl->point.x - xData[0]) < SAMPLE_MAX_MOVE)) ||
                         (pCtrl->errorCount > 0))
                        {
                        pCtrl->point.pressed    = TRUE;
                        pCtrl->point.x          = xData[0];
                        pCtrl->point.y          = yData[0];
                        pCtrl->errorCount       = 0;
                        }
                    else
                        {
                        pCtrl->errorCount++;
                        }
                    }
                else
                    {
                    pCtrl->point.pressed    = TRUE;
                    pCtrl->point.x          = xData[0];
                    pCtrl->point.y          = yData[0];
                    pCtrl->errorCount       = 0;
                    }
                }
            }
        }

    /* clear IRQ status */

    value |= AM335X_ADC_TSC_IRQ_PEN_UP | AM335X_ADC_TSC_IRQ_HW_PEN_ASYNC;
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_IRQSTATUS, value);

    /* enable steps */

    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_STEPENABLE,
                    AM335X_ADC_TSC_STEPENABLE_STEPEN);

    return OK;
    }

/*******************************************************************************
*
* am335xTsStepConfig - configure touch screen controller steps
*
* This routine configures touch screen controller steps.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void am335xTsStepConfig
    (
    AM335X_TS_DATA *    pCtrl
    )
    {
    UINT32  delay;
    UINT32  stepConfigX;
    UINT32  stepConfigY;
    UINT32  stepConfigZ1;
    UINT32  stepConfigZ2;
    UINT32  chargeConfig;
    UINT32  i;

    delay = AM335X_ADC_TSC_STEPDELAY_OPENDLY |
            AM335X_ADC_TSC_STEPDELAY_SAMPLEDLY;

    for (i = 1; i <= SAMPLE_MAX_NUM; i++)
        {
        AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_STEPDELAY (i), delay);
        }

    /* configure to calculate X */

    stepConfigX = AM335X_ADC_TSC_STEPCONFIG_MODE |
                  AM335X_ADC_TSC_STEPCONFIG_AVERAGING |
                  AM335X_ADC_TSC_STEPCONFIG_XPP |
                  AM335X_ADC_TSC_STEPCONFIG_INP |
                  AM335X_ADC_TSC_STEPCONFIG_XNN;

    for (i = 1; i <= X_MAX_NUM; i++)
        {
        AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_STEPCONFIG (i), stepConfigX);
        }

    /* configure to calculate Y */

    stepConfigY = AM335X_ADC_TSC_STEPCONFIG_MODE |
                  AM335X_ADC_TSC_STEPCONFIG_AVERAGING |
                  AM335X_ADC_TSC_STEPCONFIG_YNN |
                  AM335X_ADC_TSC_STEPCONFIG_INM |
                  AM335X_ADC_TSC_STEPCONFIG_FIFO |
                  AM335X_ADC_TSC_STEPCONFIG_YPP;

    for (i = (X_MAX_NUM + 1); i <= (X_MAX_NUM + Y_MAX_NUM); i++)
        {
        AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_STEPCONFIG (i), stepConfigY);
        }

    /* configure to calculate pressure */

    stepConfigZ1 = AM335X_ADC_TSC_STEPCONFIG_MODE |
                   AM335X_ADC_TSC_STEPCONFIG_AVERAGING |
                   AM335X_ADC_TSC_STEPCONFIG_XNP |
                   AM335X_ADC_TSC_STEPCONFIG_YPN |
                   AM335X_ADC_TSC_STEPCONFIG_INM;
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_STEPCONFIG (i), stepConfigZ1);


    stepConfigZ2 = stepConfigZ1 |
                   AM335X_ADC_TSC_STEPCONFIG_Z_INP |
                   AM335X_ADC_TSC_STEPCONFIG_FIFO;

    i++;
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_STEPCONFIG (i), stepConfigZ2);

    /* configure to charge */

    chargeConfig = AM335X_ADC_TSC_STEPCONFIG_CHG_INP |
                   AM335X_ADC_TSC_STEPCONFIG_CHG_RFM |
                   AM335X_ADC_TSC_STEPCONFIG_CHG_INM |
                   AM335X_ADC_TSC_STEPCONFIG_RFP |
                   AM335X_ADC_TSC_STEPCONFIG_XPP |
                   AM335X_ADC_TSC_STEPCONFIG_YNN;

    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_CHARGE_STEPCONFIG, chargeConfig);
    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_CHARGE_DELAY,
                    AM335X_ADC_TSC_CHARGE_DELAY_VAL);

    AM335X_WRITE32 (pCtrl, AM335X_ADC_TSC_STEPENABLE,
                    AM335X_ADC_TSC_STEPENABLE_STEPEN);
    }
