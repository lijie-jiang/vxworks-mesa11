/* evdevDrvFslCRTouch.c - Freescale CRTouch screen controller driver */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
18Mar15,c_l  create. (US55213)
*/

/*
DESCRIPTION

This is the VxBus driver for Freescale CRTouch screen controller. This driver
provides callback methods to support the evdev library, and the client users
should use the API provided by the evdev library to access the low level
hardware function.

TWR-LCD-RGB jumper setting:
    jumper J2 shunted on pins 1-2
    jumper J3 shunted on pins 1-2
    jumper J5 shunted on pins 1-2
    jumper J8 shunted on pins 1-2
    jumper J10 shunted on pins 1-2
    
To add the driver to the vxWorks image, add the following component to the
kernel configuration.

\cs
vxprj component add DRV_TOUCH_SCREEN_FSL_CRTOUCH
\ce

This driver is bound to device tree, and the device tree node must specify
below parameters:

\is

\i <compatible>
This required parameter specifies the name of the driver. It must be
"fsl,crtouch".

\i <reg>
This parameter specifies the device address of this module.

\i <pinmux-0>
This optional parameter specifies pin mux configuration for GPIO interrupt.

\i <int-pin>
This optional parameter specifies the number of GPIO interrupt pin. If the
parameter absents, the driver will work at polling mode.

\i <int-level>
This optional parameter specifies the interrupt trigger level. 0 indicates
trigger by low level, and 1 indicates high level. If the parameter
absents, the low level is the trigger condition.

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
    crtouch@4B
        {
        compatible = "fsl,crtouch";
        reg = <0x4B>;
        pinmux-0 = <&crtouch>;
        int-pin = <&gpio0 21>;
        int-level = <0>;
        display-resolution = <480>,<272>;
        calibration = <5                /@ calibration point count @/
                       480  272         /@ display resolution @/
                       16   16          /@ display point 1 @/
                       462  16          /@ display point 2 @/
                       462  254         /@ display point 3 @/
                       16   254         /@ display point 4 @/
                       239  135         /@ display point 5 @/
                       3652 3203        /@ touch screen point 1 @/
                       389  3230        /@ touch screen point 2 @/
                       358  582         /@ touch screen point 3 @/
                       3719 597         /@ touch screen point 4 @/
                       2031 1913>;      /@ touch screen point 5 @/
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
#include <subsys/gpio/vxbGpioLib.h>
#include "evdevDrvFslCRTouch.h"

/* debug macro */

#undef CRTOUCH_DBG_ON
#ifdef  CRTOUCH_DBG_ON

#undef  LOCAL
#define LOCAL

#include <private/kwriteLibP.h> /* _func_kprintf */
#define CRTOUCH_DBG_OFF            0x00000000
#define CRTOUCH_DBG_ISR            0x00000001
#define CRTOUCH_DBG_RW             0x00000002
#define CRTOUCH_DBG_ERR            0x00000004
#define CRTOUCH_DBG_RTN            0x00000008
#define CRTOUCH_DBG_INFO           0x00000010
#define CRTOUCH_DBG_ALL            0xFFFFFFFF
LOCAL UINT32 dbgMask = CRTOUCH_DBG_ALL;

#undef CRTOUCH_DBG
#define CRTOUCH_DBG(mask,...)                          \
do                                                 \
{                                                  \
    if ((dbgMask & (mask)) || ((mask) == CRTOUCH_DBG_ALL)) \
    {                                              \
        if (_func_kprintf != NULL)                 \
        {                                          \
        (* _func_kprintf)("%s,%d, ",__FUNCTION__,__LINE__);\
        (* _func_kprintf)(__VA_ARGS__);            \
        }                                          \
    }                                              \
}while (FALSE)
#else
#define CRTOUCH_DBG(...)
#endif  /* CRTOUCH_DBG_ON */

/* forward declarations */

LOCAL STATUS    fdtFslCRTouchTsProbe (VXB_DEV_ID pDev);
LOCAL STATUS    fdtFslCRTouchTsAttach (VXB_DEV_ID pInst);
LOCAL STATUS    fslCRTouchInit (FSL_CRTOUCH_DATA * pCtrl);
LOCAL STATUS    fslCRTouchOpen (VXB_DEV_ID pInst);
LOCAL STATUS    fslCRTouchClose (VXB_DEV_ID pInst);
LOCAL STATUS    fslCRTouchRead (VXB_DEV_ID pInst, TS_POINT * pPoint);
LOCAL STATUS    fslCRTouchIoctl (VXB_DEV_ID pInst, int request,
                                 _Vx_usr_arg_t arg);
LOCAL STATUS    fslCRTouchI2CRead (FSL_CRTOUCH_DATA * pDev, UINT8 addr,
                                   UINT8 * pData);
LOCAL STATUS    fslCRTouchI2CWrite (FSL_CRTOUCH_DATA * pDev, UINT8 addr,
                                    UINT8 pData);

#define CRTOUCH_REG_READ(pDev,reg,pData)                 \
    fslCRTouchI2CRead ((FSL_CRTOUCH_DATA *)(pDev),(UINT8)(reg),(UINT8 *)(pData))

#define CRTOUCH_REG_WRITE(pDev,reg,data)              \
    fslCRTouchI2CWrite ((FSL_CRTOUCH_DATA *)(pDev),(UINT8)(reg), (UINT8)(data))

/* locals */

LOCAL VXB_DRV_METHOD    fdtFslCRTouchTsMethodList[] =
    {
    { VXB_DEVMETHOD_CALL(vxbDevProbe), fdtFslCRTouchTsProbe},
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtFslCRTouchTsAttach},
    { 0, NULL }
    };

/* Freescale CRTouch screen driver */

VXB_DRV vxbFdtFslCRTouchTsDrv =
    {
        {NULL},
        "CRTouch TS",                           /* Name            */
        "Freescale CRTouch screen Fdt driver",  /* Description     */
        VXB_BUSID_FDT,                          /* Class           */
        0,                                      /* Flags           */
        0,                                      /* Reference count */
        fdtFslCRTouchTsMethodList,              /* Method table    */
    };

LOCAL const VXB_FDT_DEV_MATCH_ENTRY fslCRTouchTsMatch[] =
    {
        {
        FSL_CRTOUCH_DRIVER_NAME,            /* compatible */
        (void *)NULL,
        },
        {}                                  /* Empty terminated list */
    };

VXB_DRV_DEF(vxbFdtFslCRTouchTsDrv)

/* functions */

/*******************************************************************************
*
* fdtFslCRTouchTsProbe - probe for device presence at specific address
*
* Check for Freescale CRTouch screen controller (or compatible) device at the
* specified base address. We assume one is present at that address, but we need
* to verify.
*
* RETURNS: OK if probe passes and assumed a valid Freescale CRTouch screen
* controller (or compatible) device. ERROR otherwise.
*
*/

LOCAL STATUS fdtFslCRTouchTsProbe
    (
    VXB_DEV_ID  pDev
    )
    {
    return vxbFdtDevMatch (pDev, fslCRTouchTsMatch, NULL);
    }

/*******************************************************************************
*
* fdtFslCRTouchTsAttach - attach Freescale CRTouch screen contoller
*
* This is the Freescale CRTouch screen initialization routine.
*
* RETURNS: OK or ERROR when initialize failed
*
* ERRNO
*/

LOCAL STATUS fdtFslCRTouchTsAttach
    (
    VXB_DEV_ID  pInst
    )
    {
    VXB_FDT_DEV      * pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet(pInst);
    FSL_CRTOUCH_DATA * pCtrl   = NULL;
    VXB_RESOURCE     * pRegRes = NULL;
    VXB_RESOURCE_ADR * pResAdr = NULL;
    UINT32           * prop    = NULL;
    UINT8              pointCnt;
    UINT32             pinValue;
    INT32              intPin;
    INT32              len;
    UINT8              i;

    if (NULL == pFdtDev)
        {
        return ERROR;
        }       
        
    pCtrl = (FSL_CRTOUCH_DATA *)vxbMemAlloc (sizeof (FSL_CRTOUCH_DATA));
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
    pCtrl->intMode = FALSE;
    pCtrl->disWidth = FSL_CRTOUCH_DEFAULT_DIS_WIDTH;
    pCtrl->disHeight = FSL_CRTOUCH_DEFAULT_DIS_HEIGHT;

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

    intPin = vxbGpioGetByFdtIndex (pInst, "int-pin", 0);
    if (ERROR != intPin)
        {
        prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "int-level", &len);
        if (NULL != prop)
            {
            pinValue = vxFdt32ToCpu (*prop);
            }
        else
            {
            pinValue = 0;
            }

        pCtrl->intPin = (UINT32)intPin;

        (void)vxbPinMuxEnable (pInst);  /* configure pinmux */
        (void)vxbGpioFree (pCtrl->intPin);
        
        if (ERROR == vxbGpioAlloc (pCtrl->intPin))
            {
            CRTOUCH_DBG (CRTOUCH_DBG_ERR, "vxbGpioAlloc failed!\n",
                         0, 0, 0, 0, 0, 0);
            goto error;
            }

        if (ERROR == vxbGpioSetDir (pCtrl->intPin, GPIO_DIR_INPUT))
            {
            CRTOUCH_DBG (CRTOUCH_DBG_ERR, "vxbGpioSetDir failed!\n",
                         0, 0, 0, 0, 0, 0);
            (void)vxbGpioFree (pCtrl->intPin);
            goto error;
            }

        if (0 == pinValue)
            {
            if (ERROR == vxbGpioIntConfig (pCtrl->intPin, INTR_TRIGGER_LEVEL,
                                           INTR_POLARITY_LOW))
                {
                (void)vxbGpioFree (pCtrl->intPin);
                CRTOUCH_DBG (CRTOUCH_DBG_ERR, "vxbGpioIntConfig failed!\n",
                             0, 0, 0, 0, 0, 0);
                goto error;
                }
            }
        else
            {
            if (ERROR == vxbGpioIntConfig (pCtrl->intPin, INTR_TRIGGER_LEVEL,
                                           INTR_POLARITY_HIGH))
                {
                (void)vxbGpioFree (pCtrl->intPin);
                CRTOUCH_DBG (CRTOUCH_DBG_ERR, "vxbGpioIntConfig failed!\n",
                             0, 0, 0, 0, 0, 0);
                goto error;
                }
            }

        pCtrl->intMode = TRUE;
        }

    prop = vxFdtPropGet (pFdtDev->offset, "display-resolution", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pCtrl->disWidth = vxFdt32ToCpu (*prop++);
        pCtrl->disHeight = vxFdt32ToCpu (*prop);
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
        evdevTsGetCalibQuick (FSL_CRTOUCH_X_MAX, FSL_CRTOUCH_Y_MAX, 0, 0,
                              pCtrl->disWidth, pCtrl->disHeight,
                              pCtrl->pCalData);
        }

    (void)snprintf ((char *)pCtrl->pCalData->devName, TS_DEV_NAME_LEN,
                    "%s", FSL_CRTOUCH_DRIVER_NAME);

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "axis-max-val", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pCtrl->axisXMax = (int)vxFdt32ToCpu (*prop++);
        pCtrl->axisYMax = (int)vxFdt32ToCpu (*prop);
        }
    else
        {
        pCtrl->axisXMax = FSL_CRTOUCH_X_MAX;
        pCtrl->axisYMax = FSL_CRTOUCH_Y_MAX;
        }
    
    if (TASK_ID_ERROR == taskSpawn ("tsDeviceInit", TS_INIT_TASK_PRIO, 0,
                                    TS_INIT_TASK_STACK_SIZE,
                                    (FUNCPTR)fslCRTouchInit,
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
* fslCRTouchInit - touch screen initialization routine
*
* This routine initializes the touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*
*/

LOCAL STATUS fslCRTouchInit
    (
    FSL_CRTOUCH_DATA *   pCtrl
    )
    {
    TS_DEVICE   tsDev;
    UINT8       dataM = 0;
    UINT8       dataL = 0;

    if (NULL == pCtrl)
        {
        return ERROR;
        }

    /*
     * CRTouch configuration register
     *
     * Single resistive touch is enabled, only X and Y coordinate values are desired.
     */

    if (ERROR == CRTOUCH_REG_WRITE (pCtrl, FSL_CRTOUCH_RES_CFG, 0))
        {
        return ERROR;
        }

    /* Disable capacitive touch */

    if (ERROR == CRTOUCH_REG_WRITE (pCtrl, FSL_CRTOUCH_CAP_CFG, 0))
        {
        return ERROR;
        }

    /*
     * FSL_CRTOUCH_RES_TRIGGER
     *
     * bit
     * 7     Touch screen is released
     * 6     Calibration
     * 5     Zoom-in or Zoom out
     * 4     Rotate
     * 3     Slide
     * 2     FIFO watermark
     * 1     Capacitive event
     * 0     Resistive event
     */

    if (ERROR == CRTOUCH_REG_WRITE (pCtrl, FSL_CRTOUCH_RES_TRIGGER,
                                    FSL_CRTOUCH_RTEV | FSL_CRTOUCH_RTREL))
        {
        return ERROR;
        }

    /* Disable FIFO */

    if (ERROR == CRTOUCH_REG_WRITE (pCtrl, FSL_CRTOUCH_RES_FIFO, 0))
        {
        return ERROR;
        }

    /* Sampling rate */

    if (ERROR == CRTOUCH_REG_WRITE (pCtrl, FSL_CRTOUCH_RES_SAMPLING,
                                    FSL_CRTOUCH_RES_DEFAULT_SAMPLING))
        {
        return ERROR;
        }

    /* Display horizontal resolution */

    dataM = (UINT8)((FSL_CRTOUCH_X_MAX & 0xFF00)>> 8);
    dataL = (UINT8)(FSL_CRTOUCH_X_MAX & 0xFF);
    if (ERROR == CRTOUCH_REG_WRITE (pCtrl, FSL_CRTOUCH_HOR_MSB, dataM))
        {
        return ERROR;
        }
    if (ERROR == CRTOUCH_REG_WRITE (pCtrl, FSL_CRTOUCH_HOR_LSB, dataL))
        {
        return ERROR;
        }

    /* Display vertical resolution   */

    dataM = (UINT8)((FSL_CRTOUCH_Y_MAX & 0xFF00)>> 8);
    dataL = (UINT8)(FSL_CRTOUCH_Y_MAX & 0xFF);
    if (ERROR == CRTOUCH_REG_WRITE (pCtrl, FSL_CRTOUCH_VER_MSB, dataM))
        {
        return ERROR;
        }
    if (ERROR == CRTOUCH_REG_WRITE (pCtrl, FSL_CRTOUCH_VER_LSB, dataL))
        {
        return ERROR;
        }

    bzero ((char *)&tsDev, sizeof (TS_DEVICE));

    if (pCtrl->intMode)
        {
        tsDev.devAttr = TS_DEV_ATTR_INTERRUPT;
        }
    else
        {
        tsDev.devAttr = TS_DEV_ATTR_POLLING_PRESS |
                        TS_DEV_ATTR_POLLING_SAMPLE |
                        TS_DEV_ATTR_POLLING_RELEASE;
        tsDev.samplePeriod
            = (FSL_CRTOUCH_SAMPLE_TIMEOUT * sysClkRateGet()) / 1000;
        tsDev.releasePeriod
            = (FSL_CRTOUCH_RELEASE_TIMEOUT * sysClkRateGet()) / 1000;
        }

    tsDev.pInst                     = pCtrl->pInst;
    tsDev.pCalData                  = pCtrl->pCalData;
    tsDev.calDataCount              = 1;
    tsDev.func.open                 = fslCRTouchOpen;
    tsDev.func.close                = fslCRTouchClose;
    tsDev.func.ioctl                = fslCRTouchIoctl;
    tsDev.func.read                 = fslCRTouchRead;
    tsDev.devData.devCap            = EV_DEV_KEY | EV_DEV_ABS | EV_DEV_ABS_MT;
    tsDev.devData.pDevName          = FSL_CRTOUCH_DRIVER_NAME;
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
* fslCRTouchOpen - open touch screen device
*
* This routine opens touch screen device.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS fslCRTouchOpen
    (
    VXB_DEV_ID  pInst
    )
    {
    UINT8              status = 0;
    FSL_CRTOUCH_DATA * pCtrl = (FSL_CRTOUCH_DATA *)vxbDevSoftcGet (pInst);

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
            CRTOUCH_DBG (CRTOUCH_DBG_ERR, "vxbGpioIntConnect failed!\n",
                         0, 0, 0, 0, 0, 0);
            return ERROR;
            }

         /*
         * Clear status register for open interrupt signal.
         * When an event is recorded in the status register the TouchPending
         * signal is asserted, and remains asserted until the status is read.
         */

        if (ERROR == CRTOUCH_REG_READ (pCtrl, FSL_CRTOUCH_RES_STATUS1, &status))
            {
            CRTOUCH_DBG (CRTOUCH_DBG_ERR, "fslCRTouchOpen failed!\n",
                         0, 0, 0, 0, 0, 0);
            return ERROR;
            }

        if (ERROR == vxbGpioIntEnable (pCtrl->intPin,
                                       (VOIDFUNCPTR)evdevTsDevIsr,
                                       (void *)pCtrl->pDevInfo))
            {
            (void)vxbGpioIntDisconnect (pCtrl->intPin,
                                        (VOIDFUNCPTR)evdevTsDevIsr,
                                        (void *)pCtrl->pDevInfo);
            CRTOUCH_DBG (CRTOUCH_DBG_ERR, "vxbGpioIntEnable failed!\n",
                         0, 0, 0, 0, 0, 0);
            return ERROR;
            }

        CRTOUCH_DBG (CRTOUCH_DBG_INFO, "CRTouch: Interrupt mode!\n",
                     0, 0, 0, 0, 0, 0);
        }

    return OK;
    }

/*******************************************************************************
*
* fslCRTouchClose - close touch screen device
*
* This routine closes touch screen device.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS fslCRTouchClose
    (
    VXB_DEV_ID  pInst
    )
    {
    FSL_CRTOUCH_DATA * pCtrl = (FSL_CRTOUCH_DATA *)vxbDevSoftcGet (pInst);

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
* fslCRTouchRead - read x and y value from touch screen device
*
* This routine reads x and y value from touch screen device.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS fslCRTouchRead
    (
    VXB_DEV_ID  pInst,
    TS_POINT *  pPoint
    )
    {
    UINT8               dataM = 0;
    UINT8               dataL = 0;
    UINT8               status = 0;
    STATUS              ret = 0;
    FSL_CRTOUCH_DATA *  pCtrl = (FSL_CRTOUCH_DATA *)vxbDevSoftcGet (pInst);

    if ((NULL == pCtrl) || (NULL == pPoint))
        {
        return ERROR;
        }

    ret = CRTOUCH_REG_READ (pCtrl, FSL_CRTOUCH_RES_STATUS1, &status);
    if (pCtrl->intMode)
        {
        ret |= vxbGpioIntEnable (pCtrl->intPin, (VOIDFUNCPTR)evdevTsDevIsr,
                                 (void *)pCtrl->pDevInfo);
        }

    if (ERROR == ret)
        {
        return ERROR;
        }

    if ((status & FSL_CRTOUCH_RTST) && (status & FSL_CRTOUCH_RTSRDY))
        {
        if (ERROR == CRTOUCH_REG_READ (pCtrl, FSL_CRTOUCH_RES_X_MSB, &dataM))
            {
            return ERROR;
            }

        if (ERROR == CRTOUCH_REG_READ (pCtrl, FSL_CRTOUCH_RES_X_LSB, &dataL))
            {
            return ERROR;
            }
        pPoint->x = (int)((dataM << 8) | dataL);

        if (ERROR == CRTOUCH_REG_READ (pCtrl, FSL_CRTOUCH_RES_Y_MSB, &dataM))
            {
            return ERROR;
            }

        if (ERROR == CRTOUCH_REG_READ (pCtrl, FSL_CRTOUCH_RES_Y_LSB, &dataL))
            {
            return ERROR;
            }
        pPoint->y = (int)((dataM << 8) | dataL);
        pPoint->pressed = TRUE;
        CRTOUCH_DBG (CRTOUCH_DBG_INFO, "X coordinate:%d, Y coordinate:%d\n",
                     pPoint->x, pPoint->y, 0, 0, 0, 0);
        return OK;
        }
    else
        {
        pPoint->pressed = FALSE;
        return OK;
        }

    return OK;
    }

/*******************************************************************************
*
* fslCRTouchIoctl - handle IOCTL for touch screen device
*
* This routine handles IOCTL for touch screen device.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS fslCRTouchIoctl
    (
    VXB_DEV_ID      pInst,
    int             request,    /* ioctl function */
    _Vx_usr_arg_t   arg         /* function arg */
    )
    {
    STATUS             result = OK;
    FSL_CRTOUCH_DATA * pCtrl = (FSL_CRTOUCH_DATA *)vxbDevSoftcGet (pInst);

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
* fslCRTouchI2CRead - read register value from device
*
* This routine reads register value from device.
*
* RETURNS: OK when successfully read; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS fslCRTouchI2CRead
    (
    FSL_CRTOUCH_DATA * pDev,
    UINT8              addr,
    UINT8            * pData
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
    msg[1].len      = 1;

    return vxbI2cDevXfer (pDev->pInst, &msg[0], 2);
    }

/*******************************************************************************
*
* fslCRTouchI2CWrite - write register value to device
*
* This routine writes register value to device.
*
* RETURNS: OK when successfully written; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS fslCRTouchI2CWrite
    (
    FSL_CRTOUCH_DATA * pDev,
    UINT8              addr,
    UINT8              pData
    )
    {
    I2C_MSG msg;
    UINT8   data[2];

    bzero ((char *)&msg, sizeof (I2C_MSG));
    bzero ((char *)data, sizeof (data) / sizeof (UINT8));

    data[0] = addr;
    data[1] = pData;

    msg.addr    = pDev->devAddr;
    msg.buf     = data;
    msg.len     = 2;
    msg.scl     = FAST_MODE;
    msg.wrTime  = 5000; /* Write Cycle Time - 5ms */

    return vxbI2cDevXfer (pDev->pInst, &msg, 1);
    }
