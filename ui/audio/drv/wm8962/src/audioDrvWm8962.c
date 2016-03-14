/* audioDrvWm8962.c - Wolfson Microelectronics 8962 audio codec driver */

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
13nov15,jnl  fixed warnings.
02sep14,y_f  removed pWm8962Data (V7GFX-208)
20may14,y_f  removed unsupported sample bits. (V7GFX-154)
17feb14,y_f  written (US24784)
*/

/*
DESCRIPTION

This is the VxBus driver for Wolfson Microelectronics 8962 (WM8962) which
supplies audio playback and record functionality. This driver provides callback
methods to support the audio library, and the client users should use the API
provided by the audio library to access the low level hardware function.

To add the driver to the vxWorks image, add the following component to the
kernel configuration.

\cs
vxprj component add DRV_AUDIO_WM8962
\ce

This driver is bound to device tree, and the device tree node must specify
below parameters:

\is

\i <compatible>
This required parameter specifies the name of the driver. It must be
"wlf,wm8962".

\i <reg>
This required parameter specifies the device address of this module.

\i <clk-rate>
This required parameter specifies the list of clocks' rate of this module. The
first is used for 8KHz, 16KHz, 24KHz, 32KHz and 48KHz. The second is used for
11.025KHz, 22.05KHz and 44.1KHz.

\i <pinmux-0>
This optional parameter specifies the PinMux configuration of this module.

\i <power-pin>
This optional parameter specifies the power pin configuration of this module.
The first is the number of GPIO pin. The second is the output value for power on
the device.

\i <master-enable>
This required parameter specifies the work mode of this module. '1' indicates
working at master mode. '0' indicates working at slave mode.

\i <codec-unit>
This required parameter specifies the unit number of this module. If there is
only one device on the board, it should be '0'. Otherwise, it should be the
index of the devices.

\i <avail-paths>
This required parameter specifies the supported paths of this module. The paths
are defined in audioLibCore.h.

\i <def-paths>
This required parameter specifies the default paths of this module. The paths
are defined in audioLibCore.h.

\ie

An example of device node is shown below:

\cs
    wm8962@1a
        {
        compatible = "wlf,wm8962";
        reg = <0x1a>;
        clk-rate = <24576000>,<22579200>;
        pinmux-0 = <&wm8962>;
        power-pin = <106>,<1>;
        master-enable = <0>;
        codec-unit = <0>;
        avail-paths = <0x20004>;
        def-paths = <0x20004>;
        };
\ce

\tb "WM8962B, Rev 4.1"
*/

/* includes */

#include <audioLibCore.h>
#include <hwif/vxBus.h>
#include <hwif/vxbus/vxbLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbI2cLib.h>
#include <subsys/gpio/vxbGpioLib.h>
#include <subsys/pinmux/vxbPinMuxLib.h>
#include "audioDrvWm8962.h"

/* typedefs */

/* structure to store the audio module information */

typedef struct wm8962AudData
    {
    VXB_DEV_ID  pInst;
    UINT16      devAddr;
    UINT32      isMaster;
    UINT32      devClk0;        /* clock for 8k, 16k, 24k, 32k, 48k */
    UINT32      devClk1;        /* clock for 11.025k, 22.05k, 44.1k */
    AUDIO_DEV   audioDev;
    UINT32      curPaths;
    }WM8962_AUD_DATA;

/* forward declarations */

LOCAL STATUS    fdtWm8962AudProbe (VXB_DEV_ID pDev);
LOCAL STATUS    fdtWm8962AudAttach (VXB_DEV_ID pInst);
LOCAL int       wm8962Open (WM8962_AUD_DATA * pDev);
LOCAL int       wm8962Close (WM8962_AUD_DATA * pDev);
LOCAL STATUS    wm8962Ioctl (WM8962_AUD_DATA * pDev, AUDIO_IO_CTRL function,
                             AUDIO_IOCTL_ARG * pIoctlArg);
LOCAL STATUS    wm8962I2CRead (WM8962_AUD_DATA * pDev, UINT16 regAddr, UINT16 *
                               pRegData);
LOCAL STATUS    wm8962I2CWrite (WM8962_AUD_DATA * pDev, UINT16 regAddr, UINT16
                                regData);
LOCAL STATUS    wm8962Config (WM8962_AUD_DATA * pDev, UINT32 sampleBits, UINT32
                              sampleRate);
LOCAL STATUS    wm8962PlayEn (WM8962_AUD_DATA * pDev, BOOL isEn);
LOCAL STATUS    wm8962RecordEn (WM8962_AUD_DATA * pDev, BOOL isEn);
LOCAL STATUS    wm8962PlayVolSet (WM8962_AUD_DATA * pDev, UINT8 left, UINT8
                                  right);
LOCAL STATUS    wm8962RecordVolSet (WM8962_AUD_DATA * pDev, UINT32 left, UINT32
                                    right);

/* locals */

LOCAL VXB_DRV_METHOD    fdtWm8962AudMethodList[] =
    {
    { VXB_DEVMETHOD_CALL(vxbDevProbe), fdtWm8962AudProbe},
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtWm8962AudAttach},
    { 0, NULL }
    };

/* Wolfson Microelectronics 8962 audio codec openfirmware driver */

VXB_DRV vxbFdtWm8962AudDrv =
    {
    {NULL},
    "Wolfson Microelectronics 8962",            /* Name */
    "Wolfson Microelectronics 8962 Fdt driver", /* Description */
    VXB_BUSID_FDT,                              /* Class */
    0,                                          /* Flags */
    0,                                          /* Reference count */
    fdtWm8962AudMethodList,                     /* Method table */
    };

LOCAL const VXB_FDT_DEV_MATCH_ENTRY wm8962AudMatch[] =
    {
        {
        WM8962_AUD_DRIVER_NAME,                 /* compatible */
        (void *)NULL,
        },
        {}                                      /* Empty terminated list */
    };

VXB_DRV_DEF(vxbFdtWm8962AudDrv)

/* functions */

/*******************************************************************************
*
* fdtWm8962AudProbe - probe for device presence at specific address
*
* Check for Wolfson Microelectronics 8962 audio codec (or compatible) device at
* the specified base address. We assume one is present at that address, but we
* need to verify.
*
* RETURNS: OK if probe passes and assumed a valid Wolfson Microelectronics 8962
* audio codec (or compatible) device. ERROR otherwise.
*
*/

LOCAL STATUS fdtWm8962AudProbe
    (
    VXB_DEV_ID  pDev
    )
    {
    return vxbFdtDevMatch (pDev, wm8962AudMatch, NULL);
    }

/*******************************************************************************
*
* fdtWm8962AudAttach - attach Wolfson Microelectronics 8962 audio codec
*
* This is the Wolfson Microelectronics 8962 audio codec initialization routine.
*
* RETURNS: OK or ERROR when initialize failed
*
* ERRNO
*/

LOCAL STATUS fdtWm8962AudAttach
    (
    VXB_DEV_ID  pInst
    )
    {
    VXB_FDT_DEV *       pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet (pInst);
    WM8962_AUD_DATA *   pDev;
    VXB_RESOURCE *      pRegRes = NULL;
    VXB_RESOURCE_ADR *  pResAdr;
    int                 len;
    UINT32 *            prop;
    UINT32              pinId;
    UINT32              pinValue;

    if (NULL == pFdtDev)
        {
        return ERROR;
        }

    pDev = (WM8962_AUD_DATA *)vxbMemAlloc (sizeof (WM8962_AUD_DATA));
    if (NULL == pDev)
        {
        return ERROR;
        }

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

    pDev->devAddr   = (UINT16)pResAdr->virtual;

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "codec-unit", &len);
    if ((NULL != prop) && (sizeof (UINT32) == len))
        {
        pDev->audioDev.unit = vxFdt32ToCpu (*prop);
        }
    else
        {
        goto error;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "master-enable", &len);
    if ((NULL != prop) && (sizeof (UINT32) == len))
        {
        pDev->isMaster = vxFdt32ToCpu (*prop);
        }
    else
        {
        goto error;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "clk-rate", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pDev->devClk0   = vxFdt32ToCpu (*prop++);
        pDev->devClk1   = vxFdt32ToCpu (*prop);
        }
    else
        {
        goto error;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "avail-paths", &len);
    if ((NULL != prop) && (sizeof (UINT32) == len))
        {
        pDev->audioDev.devInfo.availPaths = vxFdt32ToCpu (*prop);
        }
    else
        {
        goto error;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "def-paths", &len);
    if ((NULL != prop) && (sizeof (UINT32) == len))
        {
        pDev->audioDev.devInfo.defPaths = vxFdt32ToCpu (*prop);
        }
    else
        {
        goto error;
        }

    (void)vxbPinMuxEnable (pInst);

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "power-pin", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pinId       = vxFdt32ToCpu (*prop++);
        pinValue    = vxFdt32ToCpu (*prop);

        if (OK == vxbGpioAlloc (pinId))
            {
            if (ERROR == vxbGpioSetDir (pinId, GPIO_DIR_OUTPUT))
                {
                goto error;
                }

            if (1 == pinValue)
                {
                if(ERROR == vxbGpioSetValue (pinId, GPIO_VALUE_HIGH))
                    {
                    goto error;
                    }
                }
            else
                {
                if(ERROR == vxbGpioSetValue (pinId, GPIO_VALUE_LOW))
                    {
                    goto error;
                    }
                }
            }
        }

    (void)snprintf ((char *)pDev->audioDev.devInfo.name, AUDIO_DEV_NAME_LEN,
                    WM8962_AUD_DRIVER_NAME);

    pDev->audioDev.open         = wm8962Open;
    pDev->audioDev.close        = wm8962Close;
    pDev->audioDev.ioctl        = wm8962Ioctl;
    pDev->audioDev.extension    = (void *)pDev;
    pDev->pInst                 = pInst;

    if (ERROR == audioCoreRegCodec (&pDev->audioDev))
        {
        goto error;
        }

    /* save Drvctrl */

    vxbDevSoftcSet (pInst, (void *)pDev);

    return OK;

error:
    if (NULL != pRegRes)
        {
        (void)vxbResourceFree (pInst, pRegRes);
        }

    vxbMemFree (pDev);
    return ERROR;
    }

/*******************************************************************************
*
* wm8962Open - open wm8962
*
* This routine opens wm8962.
*
* RETURNS: OK when device successfully opened; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL int wm8962Open
    (
    WM8962_AUD_DATA *   pDev
    )
    {
    UINT16  value = 0;

    if (ERROR == wm8962I2CWrite (pDev, WM8962_SOFTWARE_RESET, WM8962_CHIP_ID))
        {
        return ERROR;
        }

    if (ERROR == wm8962I2CWrite (pDev, WM8962_PLL_SOFTWARE_RESET, 0x0))
        {
        return ERROR;
        }

    if (ERROR == wm8962I2CRead (pDev, WM8962_SOFTWARE_RESET, &value))
        {
        return ERROR;
        }

    if (WM8962_CHIP_ID != value)
        {
        return ERROR;
        }

    if (ERROR == wm8962I2CWrite (pDev, WM8962_CLOCKING2,
                                 WM8962_CLOCKING2_RESET_VALUE))
        {
        return ERROR;
        }

    if (ERROR == wm8962I2CWrite (pDev, WM8962_PLL2, 0x1))
        {
        return ERROR;
        }

    if (pDev->isMaster)
        {
        if (ERROR == wm8962I2CRead (pDev, WM8962_AUDIO_INTERFACE_0, &value))
            {
            return ERROR;
            }

        value |= (1 << WM8962_AUDIO_INTERFACE_0_MSTR);

        if (ERROR == wm8962I2CWrite (pDev, WM8962_AUDIO_INTERFACE_0, value))
            {
            return ERROR;
            }
        }

    return OK;
    }

/*******************************************************************************
*
* wm8962Close - disable wm8962
*
* This routine disables wm8962.
*
* RETURNS: OK when device successfully closed; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL int wm8962Close
    (
    WM8962_AUD_DATA *   pDev
    )
    {
    UINT16  value = 0;

    if (ERROR == wm8962I2CWrite (pDev, WM8962_SOFTWARE_RESET, WM8962_CHIP_ID))
        {
        return ERROR;
        }

    if (ERROR == wm8962I2CWrite (pDev, WM8962_PLL_SOFTWARE_RESET, 0x0))
        {
        return ERROR;
        }

    if (ERROR == wm8962I2CRead (pDev, WM8962_SOFTWARE_RESET, &value))
        {
        return ERROR;
        }

    if (WM8962_CHIP_ID != value)
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* wm8962Ioctl - handle ioctls for wm8962
*
* This routine handles ioctls for wm8962.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS wm8962Ioctl
    (
    WM8962_AUD_DATA *   pDev,
    AUDIO_IO_CTRL       function,   /* function to perform */
    AUDIO_IOCTL_ARG *   pIoctlArg   /* function argument */
    )
    {
    STATUS  status  = OK;

    switch (function)
        {
        case AUDIO_SET_DATA_INFO:
            status = wm8962Config (pDev, pIoctlArg->dataInfo.sampleBits,
                                   pIoctlArg->dataInfo.sampleRate);
            break;

        case AUDIO_SET_PATH:
            pDev->curPaths = pIoctlArg->path;
            break;

        case AUDIO_SET_VOLUME:
            if (0 != (pIoctlArg->volume.path & (UINT32)AUDIO_OUT_MASK))
                {
                status = wm8962PlayVolSet (pDev, pIoctlArg->volume.left,
                                           pIoctlArg->volume.right);
                }

            if (0 != (pIoctlArg->volume.path & (UINT32)AUDIO_IN_MASK))
                {
                status |= wm8962RecordVolSet (pDev, pIoctlArg->volume.left,
                                              pIoctlArg->volume.right);
                }
            break;

        case AUDIO_DEV_ENABLE:
            if (0 != (pDev->curPaths & (UINT32)AUDIO_OUT_MASK))
                {
                status = wm8962PlayEn (pDev, TRUE);
                }

            if (0 != (pDev->curPaths & (UINT32)AUDIO_IN_MASK))
                {
                status |= wm8962RecordEn (pDev, TRUE);
                }
            break;

        case AUDIO_DEV_DISABLE:
            if (0 != (pDev->curPaths & (UINT32)AUDIO_OUT_MASK))
                {
                status = wm8962PlayEn (pDev, FALSE);
                }

            if (0 != (pDev->curPaths & (UINT32)AUDIO_IN_MASK))
                {
                status |= wm8962RecordEn (pDev, FALSE);
                }
            break;

        default:
            break;
        }

    return status;
    }

/*******************************************************************************
*
* wm8962I2CRead - read register value from device
*
* This routine reads register value from device.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS wm8962I2CRead
    (
    WM8962_AUD_DATA *   pDev,
    UINT16              regAddr,
    UINT16 *            pRegData
    )
    {
    UINT8   data[2];
    UINT8   regData[2];
    I2C_MSG msg[2];

    bzero ((char *)msg, sizeof (I2C_MSG) * 2);

    /* write offset */

    regData[0]  = (UINT8)((regAddr & 0xFF00) >> 8);
    regData[1]  = (UINT8)(regAddr & 0x00FF);
    msg[0].addr = pDev->devAddr;
    msg[0].scl  = FAST_MODE;
    msg[0].buf  = regData;
    msg[0].len  = 2;

    /* read data */

    bzero ((char *)data, sizeof (UINT8) * 2);
    msg[1].addr     = pDev->devAddr;
    msg[1].scl      = FAST_MODE;
    msg[1].flags    = I2C_M_RD;
    msg[1].buf      = data;
    msg[1].len      = 2;

    if (OK == vxbI2cDevXfer (pDev->pInst, &msg[0], 2))
        {
        if (NULL != pRegData)
            {
            *pRegData = (UINT16)((data[0] << 8) | data[1]);
            }

        return OK;
        }
    else
        {
        return ERROR;
        }
    }

/*******************************************************************************
*
* wm8962I2CWrite - write register value to device
*
* This routine writes register value to device.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS wm8962I2CWrite
    (
    WM8962_AUD_DATA *   pDev,
    UINT16              regAddr,
    UINT16              regData
    )
    {
    UINT8   data[4];
    I2C_MSG msg;

    data[0] = (UINT8)((regAddr & 0xFF00) >> 8);
    data[1] = (UINT8)(regAddr & 0x00FF);
    data[2] = (UINT8)((regData & 0xFF00) >> 8);
    data[3] = (UINT8)(regData & 0x00FF);

    bzero ((char *)&msg, sizeof (I2C_MSG));
    msg.addr    = pDev->devAddr;
    msg.buf     = data;
    msg.scl     = FAST_MODE;
    msg.len     = 4;
    msg.wrTime  = 5000; /* Write Cycle Time - 5ms */

    return vxbI2cDevXfer (pDev->pInst, &msg, 1);
    }

/*******************************************************************************
*
* wm8962Config - set sample bits and sample rate
*
* This routine sets sample bits and sample rate.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS wm8962Config
    (
    WM8962_AUD_DATA *   pDev,
    UINT32              sampleBits,
    UINT32              sampleRate
    )
    {
    UINT16  value;
    UINT8   dspClkDiv;
    UINT32  dspClk;
    UINT32  bitClk;
    UINT16  frameLen;
    UINT16  aifRate;
    UINT8   i;
    UINT8   bitClkDivArr[]      = {1, 2, 3, 4, 6, 8, 12, 16, 24, 32};
    UINT8   bitClkDivValArr[]   = {0, 2, 3, 4, 6, 7, 9, 10, 11, 13};
    UINT32  clkRate;

    if (ERROR == wm8962I2CRead (pDev, WM8962_AUDIO_INTERFACE_0, &value))
        {
        return ERROR;
        }

    value &= (UINT16)(~(0x3 << WM8962_AUDIO_INTERFACE_0_WL));
    switch (sampleBits)
        {
        case 16:
            value |= (0x0 << WM8962_AUDIO_INTERFACE_0_WL);
            break;

        case 24:
            value |= (0x2 << WM8962_AUDIO_INTERFACE_0_WL);
            break;

        default:
            return ERROR;
        }

    if (ERROR == wm8962I2CWrite (pDev, WM8962_AUDIO_INTERFACE_0, value))
        {
        return ERROR;
        }

    switch (sampleRate)
        {
        case 8000:
            value = (0x1 << WM8962_ADDITIONAL_CONTROL_3_MODE) | 0x5;
            break;

        case 11025:
            value = (0x0 << WM8962_ADDITIONAL_CONTROL_3_MODE) | 0x4;
            break;

        case 12000:
            value = (0x1 << WM8962_ADDITIONAL_CONTROL_3_MODE) | 0x4;
            break;

        case 16000:
            value = (0x1 << WM8962_ADDITIONAL_CONTROL_3_MODE) | 0x3;
            break;

        case 22050:
            value = (0x0 << WM8962_ADDITIONAL_CONTROL_3_MODE) | 0x2;
            break;

        case 24000:
            value = (0x1 << WM8962_ADDITIONAL_CONTROL_3_MODE) | 0x2;
            break;

        case 32000:
            value = (0x1 << WM8962_ADDITIONAL_CONTROL_3_MODE) | 0x1;
            break;

        case 44100:
            value = (0x0 << WM8962_ADDITIONAL_CONTROL_3_MODE) | 0x0;
            break;

        case 48000:
            value = (0x1 << WM8962_ADDITIONAL_CONTROL_3_MODE) | 0x0;
            break;

        case 96000:
            value = (0x1 << WM8962_ADDITIONAL_CONTROL_3_MODE) | 0x6;
            break;

        default:
            return ERROR;
        }

    if (ERROR == wm8962I2CWrite (pDev, WM8962_ADDITIONAL_CONTROL_3, value))
        {
        return ERROR;
        }

    if (pDev->isMaster)
        {
        if (0 == (sampleRate % 1000))
            {
            clkRate = pDev->devClk0;
            }
        else
            {
            clkRate = pDev->devClk1;
            }

        if (ERROR == wm8962I2CRead (pDev, WM8962_CLOCKING1, &value))
            {
            return ERROR;
            }

        dspClkDiv = (value >> WM8962_CLOCKING1_DSPCLK_DIV) & 0x3;
        switch (dspClkDiv)
            {
            case 0:
                dspClk = clkRate;
                break;

            case 1:
                dspClk = clkRate / 2;
                break;

            case 2:
                dspClk = clkRate / 4;
                break;

            default:
                return ERROR;
            }

        frameLen = (UINT16)(sampleBits * 2);
        for (i = 0; i < NELEMENTS (bitClkDivArr); i++)
            {
            bitClk  = dspClk / bitClkDivArr[i];
            if (bitClk > WM8962_BITCLK_RATE_MAX)
                {
                continue;
                }

            aifRate = (UINT16)((bitClk * 10 / sampleRate + 5) / 10);
            if (aifRate < frameLen)
                {
                return ERROR;
                }

            if (aifRate < WM8962_AIF_RATE_MAX)
                {
                break;
                }

            if (aifRate > WM8962_AIF_RATE_MAX)
                {
                continue;
                }
            }

        if (i >= NELEMENTS (bitClkDivArr))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CRead (pDev, WM8962_CLOCKING2, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~0xF);
        value |= bitClkDivValArr[i];
        if (ERROR == wm8962I2CWrite (pDev, WM8962_CLOCKING2, value))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_AUDIO_INTERFACE_2, aifRate))
            {
            return ERROR;
            }
        }

    if (ERROR == wm8962I2CRead (pDev, WM8962_CLOCKING2, &value))
        {
        return ERROR;
        }

    value |= (0x1 << WM8962_CLOCKING2_SYSCLK_ENA);

    if (ERROR == wm8962I2CWrite (pDev, WM8962_CLOCKING2, value))
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* wm8962PlayEn - enable or disable playback function
*
* This routine enables or disables playback function.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS wm8962PlayEn
    (
    WM8962_AUD_DATA *   pDev,
    BOOL                isEn
    )
    {
    UINT16  value;

    if (isEn)
        {
        /* mute DAC*/

        if (ERROR == wm8962I2CRead (pDev, WM8962_ADC_DAC_CONTROL_1, &value))
            {
            return ERROR;
            }

        value |= (0x1 << WM8962_ADC_DAC_CONTROL_1_DAC_MUTE);
        if (ERROR == wm8962I2CWrite (pDev, WM8962_ADC_DAC_CONTROL_1, value))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_ANTI_POP, 0x18))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CRead (pDev, WM8962_PWR_MGMT_1, &value))
            {
            return ERROR;
            }

        value |= (0x3 << WM8962_PWR_MGMT_1_VMID_SEL) |
                 (0x1 << WM8962_PWR_MGMT_1_BIAS_ENA);

        if (ERROR == wm8962I2CWrite (pDev, WM8962_PWR_MGMT_1, value))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_CHARGE_PUMP_1, 0x1))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_PWR_MGMT_2, 0x1E0))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_ANALOGUE_HP_0, 0x11))
            {
            return ERROR;
            }

        (void)taskDelay (sysClkRateGet () / 20); /* wait 50ms */

        if (ERROR == wm8962I2CWrite (pDev, WM8962_ANALOGUE_HP_0, 0x33))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_DC_SERVO_1, 0xCC))
            {
            return ERROR;
            }

        (void)taskDelay (sysClkRateGet () / 20); /* wait 50ms */

        if (ERROR == wm8962I2CWrite (pDev, WM8962_ANALOGUE_HP_0, 0x77))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_ANALOGUE_HP_0, 0xFF))
            {
            return ERROR;
            }

        /* un-mute DAC*/

        if (ERROR == wm8962I2CRead (pDev, WM8962_ADC_DAC_CONTROL_1, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~(0x1 << WM8962_ADC_DAC_CONTROL_1_DAC_MUTE));

        if (ERROR == wm8962I2CWrite (pDev, WM8962_ADC_DAC_CONTROL_1, value))
            {
            return ERROR;
            }
        }

    return OK;
    }

/*******************************************************************************
*
* wm8962RecordEn - enable or disable record function
*
* This routine enables or disables record function.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS wm8962RecordEn
    (
    WM8962_AUD_DATA *   pDev,
    BOOL                isEn
    )
    {
    UINT16  value;

    if (isEn)
        {
        if (ERROR == wm8962I2CRead (pDev, WM8962_LEFT_INPUT_MIXER_VOLUME,
                                    &value))
            {
            return ERROR;
            }

        value |= 0x7;
        if (ERROR == wm8962I2CWrite (pDev, WM8962_LEFT_INPUT_MIXER_VOLUME,
                                     value))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CRead (pDev, WM8962_RIGHT_INPUT_MIXER_VOLUME,
                                    &value))
            {
            return ERROR;
            }

        value |= 0x7;
        if (ERROR == wm8962I2CWrite (pDev, WM8962_RIGHT_INPUT_MIXER_VOLUME,
                                     value))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CRead (pDev, WM8962_PWR_MGMT_1, &value))
            {
            return ERROR;
            }

        value |= (0x1 << WM8962_PWR_MGMT_1_INL_ENA) |
                 (0x1 << WM8962_PWR_MGMT_1_INR_ENA) |
                 (0x1 << WM8962_PWR_MGMT_1_MICBIAS_ENA) ;
        if (ERROR == wm8962I2CWrite (pDev, WM8962_PWR_MGMT_1, value))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CRead (pDev, WM8962_DC_SERVO_0, &value))
            {
            return ERROR;
            }

        value |= (0x1 << WM8962_DC_SERVO_0_INL_DCS_ENA) |
                 (0x1 << WM8962_DC_SERVO_0_INL_DCS_STARTUP) |
                 (0x1 << WM8962_DC_SERVO_0_INR_DCS_ENA) |
                 (0x1 << WM8962_DC_SERVO_0_INR_DCS_STARTUP);
        if (ERROR == wm8962I2CWrite (pDev, WM8962_DC_SERVO_0,
                                     value))
            {
            return ERROR;
            }

        (void)taskDelay (sysClkRateGet () / 20); /* wait 50ms */

        if (ERROR == wm8962I2CRead (pDev, WM8962_PWR_MGMT_1, &value))
            {
            return ERROR;
            }

        value |= (0x1 << WM8962_PWR_MGMT_1_ADCL_ENA) |
                 (0x1 << WM8962_PWR_MGMT_1_ADCR_ENA) ;
        if (ERROR == wm8962I2CWrite (pDev, WM8962_PWR_MGMT_1, value))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_THREED1,
                                     (0x1 << WM8962_THREED1_ADC_MONOMIX)))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_LEFT_INPUT_PGA_CONTROL, 0x1F))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_RIGHT_INPUT_PGA_CONTROL,
                                     0x1F))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_INPUT_MIXER_CONTROL_2, 0x1F))
            {
            return ERROR;
            }

        if (ERROR == wm8962I2CWrite (pDev, WM8962_INPUT_MIXER_CONTROL_1, 0x03))
            {
            return ERROR;
            }
        }
    else
        {
        if (ERROR == wm8962I2CRead (pDev, WM8962_PWR_MGMT_1, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~((0x1 << WM8962_PWR_MGMT_1_INL_ENA) |
                           (0x1 << WM8962_PWR_MGMT_1_INR_ENA) |
                           (0x1 << WM8962_PWR_MGMT_1_MICBIAS_ENA) |
                           (0x1 << WM8962_PWR_MGMT_1_ADCL_ENA) |
                           (0x1 << WM8962_PWR_MGMT_1_ADCR_ENA)));
        if (ERROR == wm8962I2CWrite (pDev, WM8962_PWR_MGMT_1, value))
            {
            return ERROR;
            }
        }

    return OK;
    }

/*******************************************************************************
*
* wm8962PlayVolSet - set play volume
*
* This routine sets play volume.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS wm8962PlayVolSet
    (
    WM8962_AUD_DATA *   pDev,
    UINT8               left,
    UINT8               right
    )
    {
    UINT16  leftVol;
    UINT16  rightVol;

    if ((left > 100) || (right > 100))
        {
        return ERROR;
        }

    leftVol     = (UINT16)(((WM8962_HP_MAX_VOL - WM8962_HP_MIN_VOL) * left) / 100 +
                  WM8962_HP_MIN_VOL);
    rightVol    = (UINT8)(((WM8962_HP_MAX_VOL - WM8962_HP_MIN_VOL) * right) / 100 +
                  WM8962_HP_MIN_VOL);
    rightVol   |= (0x1 << WM8962_HPOUTR_VOLUME_HPOUT_VU);
    if (ERROR == wm8962I2CWrite (pDev, WM8962_HPOUTL_VOLUME, leftVol))
        {
        return ERROR;
        }

    if (ERROR == wm8962I2CWrite (pDev, WM8962_HPOUTR_VOLUME, rightVol))
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* wm8962RecordVolSet - set record volume
*
* This routine sets record volume.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS wm8962RecordVolSet
    (
    WM8962_AUD_DATA *   pDev,
    UINT32              left,
    UINT32              right
    )
    {
    UINT16  leftVol;
    UINT16  rightVol;

    if ((left > 100) || (right > 100))
        {
        return ERROR;
        }

    leftVol     = (UINT16)((WM8962_ADC_MAX_VOL * left) / 100);
    rightVol    = (UINT16)((WM8962_ADC_MAX_VOL * right) / 100);
    rightVol   |= (UINT16)(0x1 << WM8962_RIGHT_ADC_VOLUME_ADC_VU);

    if (ERROR == wm8962I2CWrite (pDev, WM8962_LEFT_ADC_VOLUME, leftVol))
        {
        return ERROR;
        }

    if (ERROR == wm8962I2CWrite (pDev, WM8962_RIGHT_ADC_VOLUME, rightVol))
        {
        return ERROR;
        }

    leftVol     = (UINT16)((WM8962_PGA_MAX_VOL * left) / 100);
    rightVol    = (UINT16)((WM8962_PGA_MAX_VOL * right) / 100);
    rightVol   |= (UINT16)(0x1 << WM8962_RIGHT_INPUT_VOLUME_IN_VU);

    if (ERROR == wm8962I2CWrite (pDev, WM8962_LEFT_INPUT_VOLUME, leftVol))
        {
        return ERROR;
        }

    if (ERROR == wm8962I2CWrite (pDev, WM8962_RIGHT_INPUT_VOLUME, rightVol))
        {
        return ERROR;
        }

    return OK;
    }
