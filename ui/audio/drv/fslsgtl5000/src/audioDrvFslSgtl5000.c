/* audioDrvFslSgtl5000.c - Freescale SGTL5000 audio codec driver */

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
02sep14,y_f  removed pSgtl5000Data (V7GFX-208)
30jul14,y_f  fixed start cycle error on i.MX6 boards. (V7GFX-196)
24jul14,y_f  fixed I2C write/read error sometimes (V7GFX-191)
22jul14,y_f  fixed setting 32k error (V7GFX-190)
05jun14,y_f  written (US41080)
*/

/*
DESCRIPTION

This is the VxBus driver for Freescale SGTL5000 which supplies audio playback
and record functionality. This driver provides callback methods to support the
audio library, and the client users should use the API provided by the audio
library to access the low level hardware function.

To add the driver to the vxWorks image, add the following component to the
kernel configuration.

\cs
vxprj component add DRV_AUDIO_FSL_SGTL5000
\ce

This driver is bound to device tree, and the device tree node must specify
below parameters:

\is

\i <compatible>
This required parameter specifies the name of the driver. It must be
"fsl,sgtl5000".

\i <reg>
This required parameter specifies the device address of this module.

\i <clocks>
This optional parameter specifies the list of clocks that this module use.

\i <clock-names>
This optional parameter specifies the list of clocks' name that this module use.

\i <clk-rate>
This required parameter specifies the list of clocks' rate of this module. The
first is used for 8KHz, 16KHz, 24KHz, 32KHz and 48KHz. The second is used for
11.025KHz, 22.05KHz and 44.1KHz.

\i <pinmux-0>
This optional parameter specifies the PinMux configuration of this module.

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
    sgtl5000@0a
        {
        compatible = "fsl,sgtl5000";
        reg = <0x0a>;
        clocks = <&clk 69>;
        clock-names = "clko2";
        clk-rate = <24576000>,<22579200>;
        pinmux-0 = <&sgtl5000>;
        master-enable = <1>;
        codec-unit = <0>;
        avail-paths = <0x20004>;
        def-paths = <0x20004>;
        };
\ce

\tb "SGTL5000RM, Rev 5"
*/

/* includes */

#include <audioLibCore.h>
#include <hwif/vxBus.h>
#include <hwif/vxbus/vxbLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbI2cLib.h>
#include <subsys/clk/vxbClkLib.h>
#include <subsys/pinmux/vxbPinMuxLib.h>
#include "audioDrvFslSgtl5000.h"

/* typedefs */

/* structure to store the audio module information */

typedef struct sgtl5000AudData
    {
    VXB_DEV_ID  pInst;
    UINT16      devAddr;
    UINT32      isMaster;
    UINT32      devClk0;        /* clock for 8k, 16k, 24k, 32k, 48k */
    UINT32      devClk1;        /* clock for 11.025k, 22.05k, 44.1k */
    AUDIO_DEV   audioDev;
    UINT32      curPaths;
    }SGTL5000_AUD_DATA;

/* forward declarations */

LOCAL STATUS    fdtSgtl5000AudProbe (VXB_DEV_ID pDev);
LOCAL STATUS    fdtSgtl5000AudAttach (VXB_DEV_ID pInst);
LOCAL STATUS    sgtl5000Init (SGTL5000_AUD_DATA * pDev);
LOCAL int       sgtl5000Open (SGTL5000_AUD_DATA * pDev);
LOCAL int       sgtl5000Close (SGTL5000_AUD_DATA * pDev);
LOCAL STATUS    sgtl5000Ioctl (SGTL5000_AUD_DATA * pDev, AUDIO_IO_CTRL function,
                               AUDIO_IOCTL_ARG * pIoctlArg);
LOCAL STATUS    sgtl5000I2CRead (SGTL5000_AUD_DATA * pDev, UINT16 regAddr,
                                 UINT16 * pRegData);
LOCAL STATUS    sgtl5000I2CWrite (SGTL5000_AUD_DATA * pDev, UINT16 regAddr,
                                  UINT16 regData);
LOCAL STATUS    sgtl5000Reset (SGTL5000_AUD_DATA * pDev);
LOCAL STATUS    sgtl5000Config (SGTL5000_AUD_DATA * pDev, UINT32 sampleBits,
                                UINT32 sampleRate);
LOCAL STATUS    sgtl5000PlayEn (SGTL5000_AUD_DATA * pDev, BOOL isEn);
LOCAL STATUS    sgtl5000RecordEn (SGTL5000_AUD_DATA * pDev, BOOL isEn);
LOCAL STATUS    sgtl5000PlayVolSet (SGTL5000_AUD_DATA * pDev, UINT32 path,UINT8
                                    left, UINT8 right);
LOCAL STATUS    sgtl5000RecordVolSet (SGTL5000_AUD_DATA * pDev, UINT32 path,
                                      UINT8 left, UINT8 right);

/* locals */

LOCAL VXB_DRV_METHOD    fdtSgtl5000AudMethodList[] =
    {
    { VXB_DEVMETHOD_CALL(vxbDevProbe), fdtSgtl5000AudProbe},
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtSgtl5000AudAttach},
    { 0, NULL }
    };

LOCAL const VXB_FDT_DEV_MATCH_ENTRY sgtl5000AudMatch[] =
    {
        {
        SGTL5000_AUD_DRIVER_NAME,               /* compatible */
        (void *)NULL,
        },
        {}                                      /* Empty terminated list */
    };

/* globals */

/* Freescale SGTL5000 audio codec openfirmware driver */

VXB_DRV vxbFdtSgtl5000AudDrv =
    {
    {NULL},
    "Freescale SGTL5000",                       /* Name */
    "Freescale SGTL5000 Fdt driver",            /* Description */
    VXB_BUSID_FDT,                              /* Class */
    0,                                          /* Flags */
    0,                                          /* Reference count */
    fdtSgtl5000AudMethodList,                   /* Method table */
    };

VXB_DRV_DEF(vxbFdtSgtl5000AudDrv)

/* functions */

/*******************************************************************************
*
* fdtSgtl5000AudProbe - probe for device presence at specific address
*
* Check for Freescale SGTL5000 audio codec (or compatible) device at the
* specified base address. We assume one is present at that address, but we need
* to verify.
*
* RETURNS: OK if probe passes and assumed a valid Freescale SGTL5000 audio codec
* (or compatible) device. ERROR otherwise.
*
* ERRNO: N/A
*/

LOCAL STATUS fdtSgtl5000AudProbe
    (
    VXB_DEV_ID  pDev
    )
    {
    return vxbFdtDevMatch (pDev, sgtl5000AudMatch, NULL);
    }

/*******************************************************************************
*
* fdtSgtl5000AudAttach - attach Freescale SGTL5000 audio codec
*
* This is the Freescale SGTL5000 audio codec initialization routine.
*
* RETURNS: OK or ERROR when initialize failed
*
* ERRNO: N/A
*/

LOCAL STATUS fdtSgtl5000AudAttach
    (
    VXB_DEV_ID  pInst
    )
    {
    VXB_FDT_DEV *       pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet (pInst);
    SGTL5000_AUD_DATA * pDev;
    VXB_RESOURCE *      pRegRes = NULL;
    VXB_RESOURCE_ADR *  pResAdr;
    int                 len;
    UINT32 *            prop;
    TASK_ID             takId   = TASK_ID_ERROR;

    if (NULL == pFdtDev)
        {
        return ERROR;
        }

    pDev = (SGTL5000_AUD_DATA *)vxbMemAlloc (sizeof (SGTL5000_AUD_DATA));
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

    if ((pDev->devClk0 > SYS_MCLK_MAX) || (pDev->devClk0 < SYS_MCLK_MIN) ||
        (pDev->devClk1 > SYS_MCLK_MAX) || (pDev->devClk1 < SYS_MCLK_MIN))
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

    (void)snprintf ((char *)pDev->audioDev.devInfo.name, AUDIO_DEV_NAME_LEN,
                    SGTL5000_AUD_DRIVER_NAME);

    pDev->audioDev.open         = sgtl5000Open;
    pDev->audioDev.close        = sgtl5000Close;
    pDev->audioDev.ioctl        = sgtl5000Ioctl;
    pDev->audioDev.extension    = (void *)pDev;
    pDev->pInst                 = pInst;

    takId = taskSpawn ("audDevInit", AUDIO_DEV_INIT_TASK_PRIO, 0,
                       AUDIO_DEV_INIT_TASK_STACK_SIZE, (FUNCPTR)sgtl5000Init,
                       (_Vx_usr_arg_t)pDev, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (TASK_ID_ERROR == takId)
        {
        goto error;
        }

    if (ERROR == audioCoreRegCodec (&pDev->audioDev))
        {
        goto error;
        }

    /* save Drvctrl */

    vxbDevSoftcSet (pInst, (void *)pDev);

    return OK;

error:
    if (TASK_ID_ERROR != takId)
        {
        (void)taskDelete (takId);
        }

    if (NULL != pRegRes)
        {
        (void)vxbResourceFree (pInst, pRegRes);
        }

    vxbMemFree (pDev);
    return ERROR;
    }

/*******************************************************************************
*
* sgtl5000Init - initialize SGTL5000
*
* This routine initializes SGTL5000.
*
* RETURNS: OK when device successfully initialized; otherwise ERROR
*
* ERRNO: N/A
*
*/

LOCAL STATUS sgtl5000Init
    (
    SGTL5000_AUD_DATA * pDev
    )
    {
    UINT16  value;

    if (NULL == pDev)
        {
        return ERROR;
        }

    (void)vxbClkEnableAll (pDev->pInst);
    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_REF_CTRL,
                                   CHIP_REF_CTRL_REG_VALUE))
        {
        return ERROR;
        }

    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_LINE_OUT_CTRL,
                                   CHIP_LINE_OUT_CTRL_REG_VALUE))
        {
        return ERROR;
        }

    value = (0x1 << CHIP_ANA_POWER_DAC_MONO) |
            (0x1 << CHIP_ANA_POWER_LINREG_SIMPLE) |
            (0x1 << CHIP_ANA_POWER_STARTUP) |
            (0x1 << CHIP_ANA_POWER_LINREG_D) |
            (0x1 << CHIP_ANA_POWER_ADC_MONO) |
            (0x1 << CHIP_ANA_POWER_REFTOP) |
            (0x1 << CHIP_ANA_POWER_HEADPHONE) |
            (0x1 << CHIP_ANA_POWER_LINEOUT);
    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_POWER, value))
        {
        return ERROR;
        }

    value |= (0x1 << CHIP_ANA_POWER_VAG);
    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_POWER, value))
        {
        return ERROR;
        }

    (void)taskDelay (sysClkRateGet () / 2);

    value = (0x1 << CHIP_ANA_CTRL_MUTE_LO) |    /* Line out Mute */
            (0x0 << CHIP_ANA_CTRL_SELECT_HP) |  /* DAC-> headphone */
            (0x1 << CHIP_ANA_CTRL_MUTE_HP) |    /* Headphone Mute */
            (0x1 << CHIP_ANA_CTRL_MUTE_ADC);    /* ADC Mute */

    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_CTRL, value))
        {
        return ERROR;
        }

    value = (0x1 << CHIP_SSS_CTRL_DAC_SELECT) | /* I2S_IN->DAC */
            (0x0 << CHIP_SSS_CTRL_I2S_SELECT);  /* ADC->I2S_DOUT */
    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_SSS_CTRL, value))
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* sgtl5000Open - open SGTL5000
*
* This routine opens SGTL5000.
*
* RETURNS: OK when device successfully opened; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL int sgtl5000Open
    (
    SGTL5000_AUD_DATA * pDev
    )
    {
    return sgtl5000Reset (pDev);
    }

/*******************************************************************************
*
* sgtl5000Close - disable SGTL5000
*
* This routine disables SGTL5000.
*
* RETURNS: OK when device successfully closed; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL int sgtl5000Close
    (
    SGTL5000_AUD_DATA * pDev
    )
    {
    return sgtl5000Reset (pDev);
    }

/*******************************************************************************
*
* sgtl5000Ioctl - handle ioctls for SGTL5000
*
* This routine handles ioctls for SGTL5000.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS sgtl5000Ioctl
    (
    SGTL5000_AUD_DATA * pDev,
    AUDIO_IO_CTRL       function,   /* function to perform */
    AUDIO_IOCTL_ARG *   pIoctlArg   /* function argument */
    )
    {
    STATUS  status  = OK;

    switch (function)
        {
        case AUDIO_SET_DATA_INFO:
            status = sgtl5000Config (pDev, pIoctlArg->dataInfo.sampleBits,
                                     pIoctlArg->dataInfo.sampleRate);
            break;

        case AUDIO_SET_PATH:
            pDev->curPaths = pIoctlArg->path;
            break;

        case AUDIO_SET_VOLUME:
            if (0 != (pIoctlArg->volume.path & (UINT32)AUDIO_OUT_MASK))
                {
                status = sgtl5000PlayVolSet (pDev, pIoctlArg->volume.path,
                                             pIoctlArg->volume.left,
                                             pIoctlArg->volume.right);
                }

            if (0 != (pIoctlArg->volume.path & (UINT32)AUDIO_IN_MASK))
                {
                status |= sgtl5000RecordVolSet (pDev, pIoctlArg->volume.path,
                                                pIoctlArg->volume.left,
                                                pIoctlArg->volume.right);
                }
            break;

        case AUDIO_DEV_ENABLE:
            if (0 != (pDev->curPaths & (UINT32)AUDIO_OUT_MASK))
                {
                status = sgtl5000PlayEn (pDev, TRUE);
                }

            if (0 != (pDev->curPaths & (UINT32)AUDIO_IN_MASK))
                {
                status |= sgtl5000RecordEn (pDev, TRUE);
                }
            break;

        case AUDIO_DEV_DISABLE:
            if (0 != (pDev->curPaths & (UINT32)AUDIO_OUT_MASK))
                {
                status = sgtl5000PlayEn (pDev, FALSE);
                }

            if (0 != (pDev->curPaths & (UINT32)AUDIO_IN_MASK))
                {
                status |= sgtl5000RecordEn (pDev, FALSE);
                }
            break;

        default:
            break;
        }

    return status;
    }

/*******************************************************************************
*
* sgtl5000I2CRead - read register value from device
*
* This routine reads register value from device.
*
* RETURNS: OK when successfully read; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS sgtl5000I2CRead
    (
    SGTL5000_AUD_DATA * pDev,
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
* sgtl5000I2CWrite - write register value to device
*
* This routine writes register value to device.
*
* RETURNS: OK when successfully written; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS sgtl5000I2CWrite
    (
    SGTL5000_AUD_DATA * pDev,
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
* sgtl5000Reset - reset SGTL5000
*
* This routine resets SGTL5000.
*
* RETURNS: OK when device successfully reset; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS sgtl5000Reset
    (
    SGTL5000_AUD_DATA * pDev
    )
    {
    UINT16  value;

    if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_CTRL, &value))
        {
        return ERROR;
        }

    value |= (0x1 << CHIP_ANA_CTRL_MUTE_LO) |    /* Line out Mute */
             (0x1 << CHIP_ANA_CTRL_MUTE_HP) |    /* Headphone Mute */
             (0x1 << CHIP_ANA_CTRL_MUTE_ADC);    /* ADC Mute */

    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_CTRL, value))
        {
        return ERROR;
        }

    if (ERROR == sgtl5000I2CRead (pDev, CHIP_ADCDAC_CTRL, &value))
        {
        return ERROR;
        }

    value |= (0x1 << CHIP_ADCDAC_CTRL_DAC_MUTE_RIGHT) |
             (0x1 << CHIP_ADCDAC_CTRL_DAC_MUTE_LEFT);
    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ADCDAC_CTRL, value))
        {
        return ERROR;
        }

    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_I2S_CTRL, 0x0))
        {
        return ERROR;
        }

    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_CLK_CTRL, 0x0))
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* sgtl5000Config - set sample bits and sample rate
*
* This routine sets sample bits and sample rate.
*
* RETURNS: OK when device successfully configured; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS sgtl5000Config
    (
    SGTL5000_AUD_DATA * pDev,
    UINT32              sampleBits,
    UINT32              sampleRate
    )
    {
    UINT16  value = 0;
    UINT16  intDivisor;
    UINT16  fracDivisor;
    UINT64  divisor;
    UINT32  clkRate;
    UINT8   mclkFreq;
    UINT32  mclkFreqDiv;
    UINT32  sysFsList[4] = {32000, 44100, 48000, 96000};
    UINT8   sysFs = 0xFF;
    UINT8   rateModeList[4] = {1, 2, 4, 6};
    UINT8   rateMode = 0xFF;
    UINT8   i;
    UINT8   j;

    switch (sampleRate)
        {
        case 8000:
        case 11025:
        case 12000:
        case 16000:
        case 22050:
        case 24000:
        case 32000:
        case 44100:
        case 48000:
        case 96000:
            break;

        default:
            return ERROR;
        }

    switch (sampleBits)
        {
        case 16:
        case 24:
            break;

        default:
            return ERROR;
        }

    if (0 == (sampleRate % 1000))
        {
        clkRate = pDev->devClk0;
        }
    else
        {
        clkRate = pDev->devClk1;
        }

    if (0 == (clkRate % sampleRate))
        {
        for (i = 0; i < 4; i++)
            {
            if (0 == (sysFsList[i] % sampleRate))
                {
                for (j = 0; j < 4; j++)
                    {
                    if (rateModeList[j] == (sysFsList[i] / sampleRate))
                        {
                        mclkFreqDiv = clkRate / sysFsList[i];

                        if (256 == mclkFreqDiv)
                            {
                            mclkFreq = 0x0;
                            }
                        else if (384 == mclkFreqDiv)
                            {
                            mclkFreq = 0x1;
                            }
                        else if (512 == mclkFreqDiv)
                            {
                            mclkFreq = 0x2;
                            }
                        else
                            {
                            continue;
                            }

                        sysFs    = i;
                        rateMode = j;
                        break;
                        }
                    }

                if (0xFF != sysFs)
                    {
                    break;
                    }
                }
            }

        if (0xFF == sysFs)
            {
            mclkFreq = 0x3;
            }
        }
    else
        {
        mclkFreq = 0x3;
        }

    if ((0x3 == mclkFreq) && (0x0 == pDev->isMaster))
        {
        /*
         * NOTE: If the PLL is used (CHIP_CLK_CTRL->MCLK_FREQ==0x3), the
         * SGTL5000 must be a master of the I2S port (MS==1)
         */

        return ERROR;
        }

    (void)taskDelay (sysClkRateGet () / 20);

    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_CLK_CTRL, 0))
        {
        return ERROR;
        }

    if (0x3 == mclkFreq)
        {
        sysFs    = 0xFF;
        rateMode = 0xFF;

        for (i = 0; i < 4; i++)
            {
            if (0 == (sysFsList[i] % sampleRate))
                {
                for (j = 0; j < 4; j++)
                    {
                    if (rateModeList[j] == (sysFsList[i] / sampleRate))
                        {
                        sysFs    = i;
                        rateMode = j;
                        break;
                        }
                    }

                if (0xFF != sysFs)
                    {
                    break;
                    }
                }
            }

        if (0xFF == sysFs)
            {
            return ERROR;
            }

        if (clkRate <= SYS_MCLK_MEDIAN)
            {
            value = 0x0 << CHIP_CLK_TOP_CTRL_INPUT_FREQ_DIV2;
            }
        else if (clkRate <= SYS_MCLK_MAX)
            {
            clkRate = clkRate / 2;
            value = 0x1 << CHIP_CLK_TOP_CTRL_INPUT_FREQ_DIV2;
            }
        else
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_CLK_TOP_CTRL, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_POWER, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~((0x1 << CHIP_ANA_POWER_PLL) |
                   (0x1 << CHIP_ANA_POWER_VCOAMP)));
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_POWER, value))
            {
            return ERROR;
            }

        if (0 == (sampleRate % 1000))
            {
            divisor = ((UINT64)PLL_FREQ_NORMAL * 2048) / (UINT64)clkRate;
            }
        else
            {
            divisor = ((UINT64)PLL_FREQ_44K * 2048) / (UINT64)clkRate;
            }

        intDivisor = (UINT16)((divisor / 2048) & 0x1F);
        fracDivisor = (UINT16)((divisor - (UINT64)intDivisor * 2048) & 0x7FF);

        value = (UINT16)((intDivisor << CHIP_PLL_CTRL_INT) |
                         (fracDivisor << CHIP_PLL_CTRL_FRAC));

        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_PLL_CTRL, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_POWER, &value))
            {
            return ERROR;
            }

        value |= (0x1 << CHIP_ANA_POWER_PLL) | (0x1 << CHIP_ANA_POWER_VCOAMP);
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_POWER, value))
            {
            return ERROR;
            }
        }

    value = (UINT16)((mclkFreq << CHIP_CLK_CTRL_MCLK_FREQ) |
                     (sysFs << CHIP_CLK_CTRL_SYS_FS) |
                     (rateMode << CHIP_CLK_CTRL_RATE_MODE));
    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_CLK_CTRL, value))
        {
        return ERROR;
        }

    value = (0x0 << CHIP_I2S_CTRL_SCLK_INV) |
            (0x0 << CHIP_I2S_CTRL_I2S_MODE) |
            (0x0 << CHIP_I2S_CTRL_LRALIGN) |
            (0x0 << CHIP_I2S_CTRL_LRPOL);

    if (0 != pDev->isMaster)
        {
        value |= (0x1 << CHIP_I2S_CTRL_MS);
        }

    switch (sampleBits)
        {
        case 16:
            value |= (0x3 << CHIP_I2S_CTRL_DLEN) |
                     (0x1 << CHIP_I2S_CTRL_SCLKFREQ);
            break;

        case 24:
            value |= (0x1 << CHIP_I2S_CTRL_DLEN) |
                     (0x0 << CHIP_I2S_CTRL_SCLKFREQ);
            break;

        default:
            return ERROR;
        }

    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_I2S_CTRL, value))
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* sgtl5000PlayEn - enable or disable playback function
*
* This routine enables or disables playback function.
*
* RETURNS: OK when device successfully configured; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS sgtl5000PlayEn
    (
    SGTL5000_AUD_DATA * pDev,
    BOOL                isEn
    )
    {
    UINT16  value;

    if (isEn)
        {
        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ADCDAC_CTRL, &value))
            {
            return ERROR;
            }

        value |= (0x1 << CHIP_ADCDAC_CTRL_DAC_MUTE_RIGHT) |
                 (0x1 << CHIP_ADCDAC_CTRL_DAC_MUTE_LEFT);
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ADCDAC_CTRL, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_POWER, &value))
            {
            return ERROR;
            }

        value |= (0x1 << CHIP_ANA_POWER_DAC);
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_POWER, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_DIG_POWER, &value))
            {
            return ERROR;
            }

        value |= (0x1 << CHIP_DIG_POWER_DAC_POWERUP) |
                 (0x1 << CHIP_DIG_POWER_I2S_IN_POWERUP);
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_DIG_POWER, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ADCDAC_CTRL, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~((0x1 << CHIP_ADCDAC_CTRL_DAC_MUTE_RIGHT) |
                   (0x1 << CHIP_ADCDAC_CTRL_DAC_MUTE_LEFT)));
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ADCDAC_CTRL, value))
            {
            return ERROR;
            }
        }
    else
        {
        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ADCDAC_CTRL, &value))
            {
            return ERROR;
            }

        value |= (0x1 << CHIP_ADCDAC_CTRL_DAC_MUTE_RIGHT) |
                 (0x1 << CHIP_ADCDAC_CTRL_DAC_MUTE_LEFT);
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ADCDAC_CTRL, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_POWER, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~(0x1 << CHIP_ANA_POWER_DAC));
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_POWER, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_DIG_POWER, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~((0x1 << CHIP_DIG_POWER_DAC_POWERUP) |
                   (0x1 << CHIP_DIG_POWER_I2S_IN_POWERUP)));
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_DIG_POWER, value))
            {
            return ERROR;
            }
        }

    return OK;
    }

/*******************************************************************************
*
* sgtl5000RecordEn - enable or disable record function
*
* This routine enables or disables record function.
*
* RETURNS: OK when device successfully configured; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS sgtl5000RecordEn
    (
    SGTL5000_AUD_DATA * pDev,
    BOOL                isEn
    )
    {
    UINT16  value;

    if (isEn)
        {
        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_CTRL, &value))
            {
            return ERROR;
            }

        value |= (0x1 << CHIP_ANA_CTRL_MUTE_ADC);    /* ADC Mute */
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_CTRL, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_CTRL, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~(0x1 << CHIP_ANA_CTRL_SELECT_ADC));
        if (0 != (pDev->curPaths & (UINT32)AUDIO_MIC_IN))
            {
            value |= (0x0 << CHIP_ANA_CTRL_SELECT_ADC);  /* Microphone-> ADC */
            }
        else
            {
            value |= (0x1 << CHIP_ANA_CTRL_SELECT_ADC);  /* Line in-> ADC */
            }

        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_CTRL, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_POWER, &value))
            {
            return ERROR;
            }

        value |= (0x1 << CHIP_ANA_POWER_ADC);
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_POWER, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_DIG_POWER, &value))
            {
            return ERROR;
            }

        value |= (0x1 << CHIP_DIG_POWER_ADC_POWERUP) |
                 (0x1 << CHIP_DIG_POWER_I2S_OUT_POWERUP);
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_DIG_POWER, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_MIC_CTRL, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~(0x3 << CHIP_MIC_CTRL_BIAS_RESISTOR));
        value |= (MIC_BIAS_IMPEDANCE_2KOHM << CHIP_MIC_CTRL_BIAS_RESISTOR);
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_MIC_CTRL, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_CTRL, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~(0x1 << CHIP_ANA_CTRL_MUTE_ADC));    /* ADC Unmute */
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_CTRL, value))
            {
            return ERROR;
            }
        }
    else
        {
        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_CTRL, &value))
            {
            return ERROR;
            }

        value |= (0x1 << CHIP_ANA_CTRL_MUTE_ADC);    /* ADC Mute */
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_CTRL, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_POWER, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~(0x1 << CHIP_ANA_POWER_ADC));
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_POWER, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_DIG_POWER, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~((0x1 << CHIP_DIG_POWER_ADC_POWERUP) |
                   (0x1 << CHIP_DIG_POWER_I2S_OUT_POWERUP)));
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_DIG_POWER, value))
            {
            return ERROR;
            }

        if (ERROR == sgtl5000I2CRead (pDev, CHIP_MIC_CTRL, &value))
            {
            return ERROR;
            }

        value &= (UINT16)(~(0x3 << CHIP_MIC_CTRL_BIAS_RESISTOR));
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_MIC_CTRL, value))
            {
            return ERROR;
            }
        }

    return OK;
    }

/*******************************************************************************
*
* sgtl5000PlayVolSet - set play volume
*
* This routine sets play volume.
*
* RETURNS: OK when device successfully configured; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS sgtl5000PlayVolSet
    (
    SGTL5000_AUD_DATA * pDev,
    UINT32              path,
    UINT8               left,
    UINT8               right
    )
    {
    UINT16  value;
    UINT16  volume;
    UINT16  leftVol;
    UINT16  rightVol;

    if ((left > 100) || (right > 100))
        {
        return ERROR;
        }

    if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_CTRL, &value))
        {
        return ERROR;
        }

    if (0 != (path & (UINT32)AUDIO_HP_OUT))
        {
        if ((left != 0) || (right != 0))
            {
            leftVol     = (UINT16)((CHIP_ANA_HP_MIN_VOL * (100 - left)) / 100);
            rightVol    = (UINT16)((CHIP_ANA_HP_MIN_VOL * (100 - right)) / 100);
            volume      = (UINT16)((leftVol << CHIP_ANA_HP_CTRL_VOL_LEFT) |
                          (rightVol << CHIP_ANA_HP_CTRL_VOL_RIGHT));
            if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_HP_CTRL, volume))
                {
                return ERROR;
                }

            value &= (UINT16)(~(0x1 << CHIP_ANA_CTRL_MUTE_HP));
            }
        else
            {
            value |= (0x1 << CHIP_ANA_CTRL_MUTE_HP);
            }
        }

    if (0 != (path & (UINT32)AUDIO_LINE_OUT))
        {
        if ((left != 0) || (right != 0))
            {
            leftVol     = (UINT16)(((CHIP_DAC_MIN_VOL - CHIP_DAC_MAX_VOL) *
                           (100 - left)) / 100 + CHIP_DAC_MAX_VOL);
            rightVol    = (UINT16)(((CHIP_DAC_MIN_VOL - CHIP_DAC_MAX_VOL) *
                           (100 - right)) / 100 + CHIP_DAC_MAX_VOL);
            volume      = (UINT16)((leftVol << CHIP_DAC_VOL_LEFT) |
                          (rightVol << CHIP_DAC_VOL_RIGHT));
            if (ERROR == sgtl5000I2CWrite (pDev, CHIP_DAC_VOL, volume))
                {
                return ERROR;
                }

            value &= (UINT16)(~(0x1 << CHIP_ANA_CTRL_MUTE_LO));
            }
        else
            {
            value |= (0x1 << CHIP_ANA_CTRL_MUTE_LO);
            }
        }

    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_CTRL, value))
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* sgtl5000RecordVolSet - set record volume
*
* This routine sets record volume.
*
* RETURNS: OK when device successfully configured; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS sgtl5000RecordVolSet
    (
    SGTL5000_AUD_DATA * pDev,
    UINT32              path,
    UINT8               left,
    UINT8               right
    )
    {
    UINT16  value;
    UINT16  leftVol;
    UINT16  rightVol;
    UINT16  volume;

    if ((left > 100) || (right > 100))
        {
        return ERROR;
        }

    if (ERROR == sgtl5000I2CRead (pDev, CHIP_ANA_CTRL, &value))
        {
        return ERROR;
        }

    if ((left != 0) || (right != 0))
        {
        leftVol     = (UINT16)((CHIP_ANA_ADC_MAX_VOL * left) / 100);
        rightVol    = (UINT16)((CHIP_ANA_ADC_MAX_VOL * right) / 100);
        volume      = (UINT16)((leftVol << CHIP_ANA_ADC_CTRL_VOL_LEFT) |
                      (rightVol << CHIP_ANA_ADC_CTRL_VOL_RIGHT));
        if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_ADC_CTRL, volume))
            {
            return ERROR;
            }

        value &= (UINT16)(~(0x1 << CHIP_ANA_CTRL_MUTE_ADC));
        }
    else
        {
        value |= (0x1 << CHIP_ANA_CTRL_MUTE_ADC);
        }

    if (ERROR == sgtl5000I2CWrite (pDev, CHIP_ANA_CTRL, value))
        {
        return ERROR;
        }

    return OK;
    }
