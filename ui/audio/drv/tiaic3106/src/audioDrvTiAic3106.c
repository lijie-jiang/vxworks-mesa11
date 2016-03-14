/* audioDrvTiAic3106.c - TI TLV320AIC3106 audio codec driver */

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
02sep14,y_f  removed pAic3106Data (V7GFX-208)
21mar14,y_f  written (US34862)
*/

/*
DESCRIPTION

This is the VxBus driver for TI AIC3106 (TLV320AIC3106) which supplies audio
playback and record functionality. This driver provides callback methods to
support the audio library, and the client users should use the API provided by
the audio library to access the low level hardware function.

To add the driver to the vxWorks image, add the following component to the
kernel configuration.

\cs
vxprj component add DRV_AUDIO_TI_AIC3106
\ce

This driver is bound to device tree, and the device tree node must specify
below parameters:

\is

\i <compatible>
This required parameter specifies the name of the driver. It must be
"ti,aic3106".

\i <reg>
This required parameter specifies the device address of this module.

\i <clk-rate>
This required parameter specifies the clocks' rate of this module.

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
    aic3106@1b
        {
        compatible = "ti,aic3106";
        reg = <0x1b>;
        clk-rate = <12000000>;
        master-enable = <1>;
        codec-unit = <0>;
        avail-paths = <0x10004>;
        def-paths = <0x10004>;
        };
\ce

\tb "TLV320AIC3106"
*/

/* includes */

#include <audioLibCore.h>
#include <hwif/vxBus.h>
#include <hwif/vxbus/vxbLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbI2cLib.h>
#include "audioDrvTiAic3106.h"

/* typedefs */

/* structure to store the audio module information */

typedef struct aic3106AudData
    {
    VXB_DEV_ID  pInst;
    UINT32      clkRate;
    UINT16      devAddr;
    AUDIO_DEV   audioDev;
    UINT32      curPaths;
    UINT32      isMaster;
    }AIC3106_AUD_DATA;

/* forward declarations */

LOCAL STATUS    fdtTiAic3106AudProbe (VXB_DEV_ID pDev);
LOCAL STATUS    fdtTiAic3106AudAttach (VXB_DEV_ID pInst);
LOCAL STATUS    tiAic3106Init (AIC3106_AUD_DATA * pDev);
LOCAL int       tiAic3106Open (AIC3106_AUD_DATA * pDev);
LOCAL int       tiAic3106Close (AIC3106_AUD_DATA * pDev);
LOCAL STATUS    tiAic3106Ioctl (AIC3106_AUD_DATA * pDev, AUDIO_IO_CTRL function,
                                AUDIO_IOCTL_ARG * pIoctlArg);
LOCAL STATUS    tiAic3106I2CRead (AIC3106_AUD_DATA * pDev, UINT8 regAddr,
                                  UINT8 * pRegData);
LOCAL STATUS    tiAic3106I2CWrite (AIC3106_AUD_DATA * pDev, UINT8 regAddr, UINT8
                                   regData);
LOCAL STATUS    tiAic3106Config (AIC3106_AUD_DATA * pDev, UINT32 sampleBits,
                                 UINT32 sampleRate);
LOCAL STATUS    tiAic3106PlayEn (AIC3106_AUD_DATA * pDev, BOOL isEn);
LOCAL STATUS    tiAic3106RecordEn (AIC3106_AUD_DATA * pDev, BOOL isEn);
LOCAL STATUS    tiAic3106PlayVolSet (AIC3106_AUD_DATA * pDev, UINT8 left, UINT8
                                     right);
LOCAL STATUS    tiAic3106RecordVolSet (AIC3106_AUD_DATA * pDev, UINT8 left,
                                       UINT8 right);

/* locals */

LOCAL VXB_DRV_METHOD    fdtTiAic3106AudMethodList[] =
    {
    { VXB_DEVMETHOD_CALL(vxbDevProbe), fdtTiAic3106AudProbe},
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtTiAic3106AudAttach},
    { 0, NULL }
    };

/* TI TLV320AIC3106 audio codec openfirmware driver */

VXB_DRV vxbFdtTiAic3106AudDrv =
    {
    {NULL},
    "TI TLV320AIC3106",                         /* Name */
    "TI TLV320AIC3106 Fdt driver",              /* Description */
    VXB_BUSID_FDT,                              /* Class */
    0,                                          /* Flags */
    0,                                          /* Reference count */
    fdtTiAic3106AudMethodList,                  /* Method table */
    };

LOCAL const VXB_FDT_DEV_MATCH_ENTRY tiAic3106AudMatch[] =
    {
        {
        AIC3106_AUD_DRIVER_NAME,                /* compatible */
        (void *)NULL,
        },
        {}                                      /* Empty terminated list */
    };

VXB_DRV_DEF(vxbFdtTiAic3106AudDrv)

/* functions */

/*******************************************************************************
*
* fdtTiAic3106AudProbe - probe for device presence at specific address
*
* Check for TI TLV320AIC3106 audio codec (or compatible) device at the specified
* base address. We assume one is present at that address, but we need to verify.
*
* RETURNS: OK if probe passes and assumed a valid TI TLV320AIC3106 audio codec
* (or compatible) device. ERROR otherwise.
*
*/

LOCAL STATUS fdtTiAic3106AudProbe
    (
    VXB_DEV_ID  pDev
    )
    {
    return vxbFdtDevMatch (pDev, tiAic3106AudMatch, NULL);
    }

/*******************************************************************************
*
* fdtTiAic3106AudAttach - attach TI TLV320AIC3106 audio codec
*
* This is the TI TLV320AIC3106 audio codec initialization routine.
*
* RETURNS: OK or ERROR when initialize failed
*
* ERRNO
*/

LOCAL STATUS fdtTiAic3106AudAttach
    (
    VXB_DEV_ID  pInst
    )
    {
    VXB_FDT_DEV *       pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet (pInst);
    AIC3106_AUD_DATA *  pDev;
    VXB_RESOURCE *      pRegRes = NULL;
    VXB_RESOURCE_ADR *  pResAdr;
    int                 len;
    UINT32 *            prop;
    TASK_ID             takId   = TASK_ID_ERROR;

    if (NULL == pFdtDev)
        {
        return ERROR;
        }

    pDev = (AIC3106_AUD_DATA *)vxbMemAlloc (sizeof (AIC3106_AUD_DATA));
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

    pDev->devAddr = (UINT16)pResAdr->virtual;

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "codec-unit", &len);
    if ((NULL != prop) && (sizeof (UINT32) == len))
        {
        pDev->audioDev.unit = vxFdt32ToCpu (*prop);
        }
    else
        {
        goto error;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "clk-rate", &len);
    if ((NULL != prop) && (sizeof (UINT32) == len))
        {
        pDev->clkRate = vxFdt32ToCpu (*prop);
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

    (void)snprintf ((char *)pDev->audioDev.devInfo.name, AUDIO_DEV_NAME_LEN,
                    AIC3106_AUD_DRIVER_NAME);

    pDev->audioDev.open         = tiAic3106Open;
    pDev->audioDev.close        = tiAic3106Close;
    pDev->audioDev.ioctl        = tiAic3106Ioctl;
    pDev->audioDev.extension    = (void *)pDev;
    pDev->pInst                 = pInst;

    takId = taskSpawn ("audDevInit", AUDIO_DEV_INIT_TASK_PRIO, 0,
                       AUDIO_DEV_INIT_TASK_STACK_SIZE, (FUNCPTR)tiAic3106Init,
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
* tiAic3106Init - initialize AIC3106
*
* This routine initializes AIC3106.
*
* RETURNS: OK when device successfully initialized; otherwise ERROR
*
* ERRNO: N/A
*
*/

LOCAL STATUS tiAic3106Init
    (
    AIC3106_AUD_DATA *  pDev
    )
    {
    if (NULL == pDev)
        {
        return ERROR;
        }

    /* avoid pop-noise */

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_PAGE_SELECT, PAGE_0))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_SOFTWARE_RESET, SOFT_RESET))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_POP_REDUCTION_CTRL,
                                    POWER_ON_400MS))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_HPLOUT_LVL_CTRL, HP_FULL_PW))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_HPROUT_LVL_CTRL, HP_FULL_PW))
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* tiAic3106Open - reset and initialize the audio codec
*
* This routine resets and initializes the audio codec.
*
* RETURNS: OK when device successfully opened; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL int tiAic3106Open
    (
    AIC3106_AUD_DATA *  pDev
    )
    {
    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_PAGE_SELECT, PAGE_0))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_SOFTWARE_RESET, SOFT_RESET))
        {
        return ERROR;
        }

    if (1 == pDev->isMaster)
        {
        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_IF_CTRL_A,
                                        (BIT_CLOCK | WORD_CLOCK)))
            {
            return ERROR;
            }
        }
    else
        {
        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_IF_CTRL_A, 0))
            {
            return ERROR;
            }
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_DIGITAL_FILTER, 0))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_OVERFLOW_FLG, PLL_R(0x1)))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_POP_REDUCTION_CTRL,
                                    POWER_ON_1MS))
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* tiAic3106Close - disable the audio codec
*
* This routine disables the audio codec.
*
* RETURNS: OK when device successfully closed; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL int tiAic3106Close
    (
    AIC3106_AUD_DATA *   pDev
    )
    {
    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_SOFTWARE_RESET, SOFT_RESET))
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* tiAic3106Ioctl - handle ioctls for the audio codec
*
* This routine handles ioctls for the audio codec.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiAic3106Ioctl
    (
    AIC3106_AUD_DATA *  pDev,
    AUDIO_IO_CTRL       function,   /* function to perform */
    AUDIO_IOCTL_ARG *   pIoctlArg   /* function argument */
    )
    {
    STATUS  status  = OK;

    switch (function)
        {
        case AUDIO_SET_DATA_INFO:
            status = tiAic3106Config (pDev, pIoctlArg->dataInfo.sampleBits,
                                      pIoctlArg->dataInfo.sampleRate);
            break;

        case AUDIO_SET_PATH:
            pDev->curPaths = pIoctlArg->path;
            break;

        case AUDIO_SET_VOLUME:
            if (0 != (pIoctlArg->volume.path & (UINT32)AUDIO_OUT_MASK))
                {
                status = tiAic3106PlayVolSet (pDev, pIoctlArg->volume.left,
                                              pIoctlArg->volume.right);
                }

            if (0 != (pIoctlArg->volume.path & (UINT32)AUDIO_IN_MASK))
                {
                status |= tiAic3106RecordVolSet (pDev, pIoctlArg->volume.left,
                                                 pIoctlArg->volume.right);
                }
            break;

        case AUDIO_DEV_ENABLE:
            if (0 != (pDev->curPaths & (UINT32)AUDIO_OUT_MASK))
                {
                status = tiAic3106PlayEn (pDev, TRUE);
                (void)taskDelay (sysClkRateGet () / 10);
                }

            if (0 != (pDev->curPaths & (UINT32)AUDIO_IN_MASK))
                {
                status |= tiAic3106RecordEn (pDev, TRUE);
                }
            break;

        case AUDIO_DEV_DISABLE:
            if (0 != (pDev->curPaths & (UINT32)AUDIO_OUT_MASK))
                {
                status = tiAic3106PlayEn (pDev, FALSE);
                }

            if (0 != (pDev->curPaths & (UINT32)AUDIO_IN_MASK))
                {
                status |= tiAic3106RecordEn (pDev, FALSE);
                }
            break;

        default:
            break;
        }

    return status;
    }

/*******************************************************************************
*
* tiAic3106I2CRead - read register value from device
*
* This routine reads register value from device.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiAic3106I2CRead
    (
    AIC3106_AUD_DATA *   pDev,
    UINT8                regAddr,
    UINT8 *              pRegData
    )
    {
    UINT8   data[1];
    UINT8   regData[1];
    I2C_MSG msg[2];

    bzero ((char *)msg, sizeof (I2C_MSG) * 2);

    /* write offset */

    regData[0]  = regAddr;
    msg[0].addr = pDev->devAddr;
    msg[0].scl  = FAST_MODE;
    msg[0].buf  = regData;
    msg[0].len  = 1;

    /* read data */

    bzero ((char *)data, sizeof (UINT8));
    msg[1].addr     = pDev->devAddr;
    msg[1].scl      = FAST_MODE;
    msg[1].flags    = I2C_M_RD;
    msg[1].buf      = data;
    msg[1].len      = 1;

    if (OK == vxbI2cDevXfer (pDev->pInst, &msg[0], 2))
        {
        if (NULL != pRegData)
            {
            *pRegData = data[0];
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
* tiAic3106I2CWrite - write register value to device
*
* This routine writes register value to device.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiAic3106I2CWrite
    (
    AIC3106_AUD_DATA *   pDev,
    UINT8                regAddr,
    UINT8                regData
    )
    {
    UINT8   data[2];
    I2C_MSG msg;

    data[0] = regAddr;
    data[1] = regData;

    bzero ((char *)&msg, sizeof (I2C_MSG));
    msg.addr    = pDev->devAddr;
    msg.buf     = data;
    msg.scl     = FAST_MODE;
    msg.len     = 2;
    msg.wrTime  = 5000; /* Write Cycle Time - 5ms */

    return vxbI2cDevXfer (pDev->pInst, &msg, 1);
    }

/*******************************************************************************
*
* tiAic3106Config - set sample bits and sample rate
*
* This routine sets sample bits and sample rate.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiAic3106Config
    (
    AIC3106_AUD_DATA *  pDev,
    UINT32              sampleBits,
    UINT32              sampleRate
    )
    {
    UINT8   pllPVal;
    UINT8   pllJVal;
    UINT8   pllDHiVal;
    UINT8   pllDLwVal;
    UINT8   dataPath;
    UINT32  div;
    UINT8   dataBits;

    switch (sampleBits)
        {
        case 16:
            dataBits = DATA_16;
            break;

        case 24:
            dataBits = DATA_24;
            break;

        case 32:
            dataBits = DATA_32;
            break;

         default:
            return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_IF_CTRL_B,
                                    (UINT8)((TRANS_MODE (I2S_MODE) |
                                     DATA_WORD_LEN (dataBits)))))
        {
        return ERROR;
        }

    pllPVal = (UINT8)(pDev->clkRate / AIC3106_BASE_CLK_RATE);
    if (0 == pllPVal)
        {
        return ERROR;
        }

    dataPath = (LDAC_DPATH (STEREO_DATA) | RDAC_DPATH (STEREO_DATA));
    switch (sampleRate)
        {
        case 96000:
            div         = FS_DIV_1;
            pllDLwVal   = 0x1e;
            pllDHiVal   = 0x00;
            pllJVal     = 0x8;
            dataPath   |= (ADC_DUALR | DAC_DUALR);
            break;

        case 88200:
            div         = FS_DIV_1;
            pllDLwVal   = 0x52;
            pllDHiVal   = 0x10;
            pllJVal     = 0x7;
            dataPath   |= FS_HLZ (1) | (ADC_DUALR | DAC_DUALR);
            break;

        case 48000:
            div         = FS_DIV_1;
            pllDLwVal   = 0x1e;
            pllDHiVal   = 0x00;
            pllJVal     = 0x8;
            break;

        case 44100:
            div         = FS_DIV_1;
            pllDLwVal   = 0x52;
            pllDHiVal   = 0x10;
            pllJVal     = 0x7;
            dataPath   |= FS_HLZ (1);
            break;

        case 32000:
            div         = FS_DIV_1_5;
            pllDLwVal   = 0x1e;
            pllDHiVal   = 0x00;
            pllJVal     = 0x8;
            break;

        case 24000:
            div         = FS_DIV_2;
            pllDLwVal   = 0x1e;
            pllDHiVal   = 0x00;
            pllJVal     = 0x8;
            break;

        case 22050:
            div         = FS_DIV_2;
            pllDLwVal   = 0x52;
            pllDHiVal   = 0x10;
            pllJVal     = 0x7;
            dataPath   |= FS_HLZ (1);
            break;

        case 16000:
            div         = FS_DIV_3;
            pllDLwVal   = 0x1e;
            pllDHiVal   = 0x00;
            pllJVal     = 0x8;
            break;

        case 12000:
            div         = FS_DIV_4;
            pllDLwVal   = 0x1e;
            pllDHiVal   = 0x00;
            pllJVal     = 0x8;
            break;

        case 11025:
            div         = FS_DIV_4;
            pllDLwVal   = 0x52;
            pllDHiVal   = 0x10;
            pllJVal     = 0x7;
            dataPath   |= FS_HLZ (1);
            break;

        case 8000:
            div         = FS_DIV_6;
            pllDLwVal   = 0x1e;
            pllDHiVal   = 0x00;
            pllJVal     = 0x8;
            break;

         default:
            return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_SAMPLE_RATE,
                                    (UINT8)((ADC_SAMPLE (div) | DAC_SAMPLE (div)))))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_DATA_PATH, dataPath))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_PLL_PRG_A,
                                    (PLL_Q (0x2) | PLL_P (pllPVal))))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_PLL_PRG_B, (UINT8)PLL_J (pllJVal)))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_PLL_PRG_C,
                                    PLL_D_LW (pllDLwVal)))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_PLL_PRG_D,
                                    (UINT8)PLL_D_HI (pllDHiVal)))
        {
        return ERROR;
        }

    if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_PLL_PRG_A,
                          (PLL_ENABLE | PLL_Q (0x2) | PLL_P (pllPVal))))
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* tiAic3106PlayEn - enable or disable playback function
*
* This routine enables or disables the playback function.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiAic3106PlayEn
    (
    AIC3106_AUD_DATA *  pDev,
    BOOL                isEn
    )
    {
    if (isEn)
        {
        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_DAC_PWR_AND_DRIVER_CTRL,
                                        (LDAC_PW_ON | RDAC_PW_ON)))
            {
            return ERROR;
            }

        /* DAC default volume and mute */

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LDAC_VOL_CTRL,
                                        (DAC_MUTE_ENABLE | 0x7F)))
            {
            return ERROR;
            }

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_RDAC_VOL_CTRL,
                                        (DAC_MUTE_ENABLE | 0x7F)))
            {
            return ERROR;
            }

        if (0 != (AUDIO_LINE_OUT & pDev->curPaths))
            {
            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_DACL1_LLOPM_VOL_CTRL,
                                            0x80))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_DACR1_RLOPM_VOL_CTRL,
                                            0x80))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LLOPM_LVL_CTRL,
                    (LOPM_MUTE_DISABLE | LOPM_FULL_PW | LOPM_OUTPUT_LEVEL)))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_RLOPM_LVL_CTRL,
                    (LOPM_MUTE_DISABLE | LOPM_FULL_PW | LOPM_OUTPUT_LEVEL)))
                {
                return ERROR;
                }
            }

        if (0 != (AUDIO_HP_OUT & pDev->curPaths))
            {
            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_HPLOUT_LVL_CTRL,
                                            (HP_MUTE_DISABLE | HP_FULL_PW)))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_HPROUT_LVL_CTRL,
                                            (HP_MUTE_DISABLE | HP_FULL_PW)))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_DACL1_HPLOUT_VOL_CTRL,
                                            (DAC_L1_TO_HPOUT | HPOUT_DEF_VOL)))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_DACR1_HPLOUT_VOL_CTRL,
                                            (DAC_L1_TO_HPOUT | HPOUT_DEF_VOL)))
                {
                return ERROR;
                }
            }
        }

    return OK;
    }

/*******************************************************************************
*
* tiAic3106RecordEn - enable or disable record function
*
* This routine enables or disables record function.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiAic3106RecordEn
    (
    AIC3106_AUD_DATA *  pDev,
    BOOL                isEn
    )
    {
    UINT8   regData;

    if (isEn)
        {
        if (0 != (AUDIO_LINE_IN & pDev->curPaths))
            {
            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LINE1_LADC_CTRL,
                                            ADC_POWER_ON))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LINE1_RADC_CTRL,
                                            ADC_POWER_ON))
                {
                return ERROR;
                }
            }

        if (0 != (AUDIO_MIC_IN & pDev->curPaths))
            {
            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_MIC3LL_CTRL, 0x0F))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_MIC3LR_CTRL, 0xF0))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_MICBASIC_CTRL,
                                            MIC_OUT_AVDD))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CRead (pDev, AIC3106_LINE1_LADC_CTRL,
                                           &regData))
                {
                return ERROR;
                }

            regData |= ADC_POWER_ON;
            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LINE1_LADC_CTRL,
                                            regData))
                {
                return ERROR;
                }

            if (ERROR == tiAic3106I2CRead (pDev, AIC3106_LINE1_RADC_CTRL,
                                           &regData))
                {
                return ERROR;
                }

            regData |= ADC_POWER_ON;
            if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LINE1_RADC_CTRL,
                                            regData))
                {
                return ERROR;
                }
            }
        }
    else
        {
        /* disable line in */

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LINE1_LADC_CTRL,
                                        LINE1_INPUT_LVL (0xF)))
            {
            return ERROR;
            }

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LINE1_RADC_CTRL,
                                        LINE1_INPUT_LVL (0xF)))
            {
            return ERROR;
            }

        /* disable MIC in */

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_MIC3LL_CTRL, 0xFF))
            {
            return ERROR;
            }

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_MIC3LR_CTRL, 0xFF))
            {
            return ERROR;
            }

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_MICBASIC_CTRL, 0))
            {
            return ERROR;
            }
        }

    return OK;
    }

/*******************************************************************************
*
* tiAic3106PlayVolSet - set play volume
*
* This routine sets play volume.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiAic3106PlayVolSet
    (
    AIC3106_AUD_DATA *  pDev,
    UINT8               left,
    UINT8               right
    )
    {
    UINT8   leftVol;
    UINT8   rightVol;

    if ((left > 100) || (right > 100))
        {
        return ERROR;
        }

    if ((0 == right) || (0 == left))    /* mute */
        {
        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LDAC_VOL_CTRL,
                                        DAC_MUTE_ENABLE))
            {
            return ERROR;
            }

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_RDAC_VOL_CTRL,
                                        DAC_MUTE_ENABLE))
            {
            return ERROR;
            }
        }
    else
        {
        leftVol     = (UINT8)((AIC3106_DAC_MIN_VOL * (100 - left)) / 100);
        rightVol    = (UINT8)((AIC3106_DAC_MIN_VOL * (100 - right)) / 100);

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LDAC_VOL_CTRL, leftVol))
            {
            return ERROR;
            }

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_RDAC_VOL_CTRL, rightVol))
            {
            return ERROR;
            }
        }

    return OK;
    }

/*******************************************************************************
*
* tiAic3106RecordVolSet - set record volume
*
* This routine sets record volume.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tiAic3106RecordVolSet
    (
    AIC3106_AUD_DATA *  pDev,
    UINT8               left,
    UINT8               right
    )
    {
    UINT8   leftVol;
    UINT8   rightVol;

    if ((left > 100) || (right > 100))
        {
        return ERROR;
        }

    if ((0 == left) || (0 == right))    /* mute */
        {
        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LADC_PGA_CTRL,
                                        PGA_MUTE_ENABLE))
            {
            return ERROR;
            }

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_RADC_PGA_CTRL,
                                        PGA_MUTE_ENABLE))
            {
            return ERROR;
            }
        }
    else
        {
        leftVol     = (UINT8)((AIC3106_ADC_MAX_VOL * left) / 100);
        rightVol    = (UINT8)((AIC3106_ADC_MAX_VOL * right) / 100);

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_LADC_PGA_CTRL, leftVol))
            {
            return ERROR;
            }

        if (ERROR == tiAic3106I2CWrite (pDev, AIC3106_RADC_PGA_CTRL, rightVol))
            {
            return ERROR;
            }
        }

    return OK;
    }
