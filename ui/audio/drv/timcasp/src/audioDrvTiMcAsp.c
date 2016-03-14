/* audioDrvTiMcAsp.c - TI McASP audio driver */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
02sep14,y_f  removed pMcAspAudData (V7GFX-208)
21mar14,y_f  written (US34863)
*/

/*
DESCRIPTION

This is the VxBus driver for TI Multichannel Audio Serial Port (McASP). This
driver provides callback methods to support the audio library, and the client
users should use the API provided by the audio library to access the low level
hardware function.

To add the driver to the vxWorks image, add the following component to the
kernel configuration.

\cs
vxprj component add DRV_AUDIO_TI_MCASP
\ce

This driver is bound to device tree, and the device tree node must specify
below parameters:

\is

\i <compatible>
This required parameter specifies the name of the driver. It must be "ti,mcasp".

\i <reg>
This required parameter specifies the device address of this module. The first
is the McASP address. The second is the McASP data address.

\i <clocks>
This required parameter specifies the list of clock.

\i <clock-names>
This required parameter specifies the clock names of this module.

\i <interrupt-parent>
This required parameter specifies the offset of interrupt controller.

\i <interrupts>
This required parameter specifies the interrupt numbers of this module. The
first is tx interrupt. The second is rx interrupt.

\i <pinmux-0>
This required parameter specifies pin mux configuration for this module.

\i <tx-dma-num>
This required parameter specifies DMA number of TX.

\i <rx-dma-num>
This required parameter specifies DMA number of RX.

\i <tdm-slots>
This required parameter specifies the number of slots the device used.

\i <serial-dir>
This required parameter specifies the serializer configuration. '0' indicates
inactive, '1' indicates tx and '2' indicates rx.

\i <master-enable>
This required parameter specifies whether enable master mode. '1' indicates
enable, '0' indicates disable. If the connected codec device works at master
mode, McASP must work at slave mode and vice versa.

\i <codec-unit>
This required parameter specifies codec device number. The value is defined in
audio codec device tree node.

\ie

An example of device node is shown below:

\cs
    mcasp1@4803C000
        {
        compatible = "ti,mcasp";
        reg = <0x4803C000 0x2000>,<0x46400000 0x400000>;
        clocks = <&clk 81>,<&clk 82>;
        clock-names = "mcasp1_ick","mcasp1_fck";
        interrupt-parent = <&intc>;
        interrupts = <82>,<83>;
        pinmux-0 = <&mcasp1_pads>;
        tdm-slots = <2>;
        serial-dir = <
                     /@ 0: INACTIVE, 1: TX, 2: RX @/
                     0 0 1 2
                     >;
        tx-dma-num = <10>;
        rx-dma-num = <11>;
        master-enable = <0>;
        codec-unit = <0>;
        };
\ce

\tb "AM335x ARM Cortex -A8 Microprocessors(MPUs) Technical Reference Manual"
*/

/* includes */

#include <audioLibCore.h>
#include <hwif/vxBus.h>
#include <hwif/vxbus/vxbLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/clk/vxbClkLib.h>
#include <subsys/dma/vxbDmaLib.h>
#include <subsys/pinmux/vxbPinMuxLib.h>
#include <hwif/util/vxbDmaBufLib.h>
#include "audioDrvTiMcAsp.h"

/* defines */

#undef  TI_MCASP_DEBUG
#ifdef  TI_MCASP_DEBUG
#   define TI_MCASP_LOG(fmt, a, b, c, d, e, f) \
            (void)kprintf (fmt, a, b, c, d, e, f)
#else
#   define TI_MCASP_LOG(fmt, a, b, c, d, e, f) 
#endif

/* typedefs */

/* structure to store the audio module information */

typedef struct tiMcAspAudData
    {
    VXB_DEV_ID              pInst;
    UINT32                  codecUnit;
    UINT32                  mcaspAddr;      /* mcasp register address */
    void *                  mcaspHandle;    /* vxBus handle for mcasp */
    UINT32                  dataAddr;       /* data port register address */
    void *                  dataHandle;     /* vxBus handle for data port */
    SEM_ID                  dmaTxSem;
    SEM_ID                  dmaRxSem;
    VXB_DMA_CHAN *          pDmaChanTx;
    VXB_DMA_CHAN *          pDmaChanRx;
    VXB_DMA_SLAVE_CONFIG    dmaConfigTx;
    VXB_DMA_SLAVE_CONFIG    dmaConfigRx;
    AUDIO_DEV               audioDev;
    UINT32                  curPaths;
    UINT32                  isMaster;
    }TI_MCASP_AUD_DATA;

/* forward declarations */

LOCAL STATUS    fdtTiMcAspAudProbe (VXB_DEV_ID pDev);
LOCAL STATUS    fdtTiMcAspAudAttach (VXB_DEV_ID pInst);
LOCAL STATUS    mcAspInit (TI_MCASP_AUD_DATA * pDev);
LOCAL int       mcAspOpen (TI_MCASP_AUD_DATA * pDev);
LOCAL int       mcAspClose (TI_MCASP_AUD_DATA * pDev);
LOCAL int       mcAspRead (TI_MCASP_AUD_DATA * pDev, char * pBuffer, int
                           maxbytes);
LOCAL int       mcAspWrite (TI_MCASP_AUD_DATA * pDev, char * pBuffer, int
                            nbytes);
LOCAL STATUS    mcAspIoctl (TI_MCASP_AUD_DATA * pDev, AUDIO_IO_CTRL function,
                            AUDIO_IOCTL_ARG * pIoctlArg);
LOCAL void      mcAspReset (TI_MCASP_AUD_DATA * pDev);
LOCAL void      mcAspTxEn (TI_MCASP_AUD_DATA * pDev, BOOL isEn);
LOCAL void      mcAspRxEn (TI_MCASP_AUD_DATA * pDev, BOOL isEn);
LOCAL void      mcAspDmaCallback (SEM_ID dmaSem);
LOCAL void      mcAspTxIrqHandle (TI_MCASP_AUD_DATA * pDev);
LOCAL void      mcAspRxIrqHandle (TI_MCASP_AUD_DATA * pDev);

/* locals */

LOCAL VXB_DRV_METHOD    fdtTiMcAspAudMethodList[] =
    {
    { VXB_DEVMETHOD_CALL(vxbDevProbe), fdtTiMcAspAudProbe},
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtTiMcAspAudAttach},
    { 0, NULL }
    };

/* TI McASP openfirmware driver */

VXB_DRV vxbFdtTiMcAspAudDrv =
    {
    {NULL},
    "TI McAsp",                             /* Name */
    "TI McAsp Fdt driver",                  /* Description */
    VXB_BUSID_FDT,                          /* Class */
    0,                                      /* Flags */
    0,                                      /* Reference count */
    fdtTiMcAspAudMethodList,                /* Method table */
    };

LOCAL const VXB_FDT_DEV_MATCH_ENTRY tiMcAspAudMatch[] =
    {
        {
        TI_MCASP_AUD_DRIVER_NAME,           /* compatible */
        (void *)NULL,
        },
        {}                                  /* Empty terminated list */
    };

VXB_DRV_DEF(vxbFdtTiMcAspAudDrv)

/* functions */

/*******************************************************************************
*
* fdtTiMcAspAudProbe - probe for device presence at specific address
*
* Check for TI McASP (or compatible) device at the specified base address. We
* assume one is present at that address, but we need to verify.
*
* RETURNS: OK if probe passes and assumed a valid TI McASP (or compatible)
* device. ERROR otherwise.
*
*/

LOCAL STATUS fdtTiMcAspAudProbe
    (
    VXB_DEV_ID  pDev
    )
    {
    return vxbFdtDevMatch (pDev, tiMcAspAudMatch, NULL);
    }

/*******************************************************************************
*
* fdtTiMcAspAudAttach - attach TI McASP
*
* This is the TI McASP initialization routine.
*
* RETURNS: OK or ERROR when initialize failed
*
* ERRNO
*/

LOCAL STATUS fdtTiMcAspAudAttach
    (
    VXB_DEV_ID  pInst
    )
    {
    VXB_FDT_DEV *       pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet (pInst);
    TI_MCASP_AUD_DATA * pDev;
    VXB_RESOURCE *      pMcAspRegRes    = NULL;
    VXB_RESOURCE *      pDataRegRes     = NULL;
    VXB_RESOURCE *      pTxIntRes       = NULL;
    VXB_RESOURCE *      pRxIntRes       = NULL;
    VXB_RESOURCE_ADR *  pResAdr;
    int                 len;
    UINT32 *            prop;
    void *              txDmaProp;
    void *              rxDmaProp;
    int                 txDmaPropLen;
    int                 rxDmaPropLen;

    if (NULL == pFdtDev)
        {
        return ERROR;
        }

    pDev = (TI_MCASP_AUD_DATA *)vxbMemAlloc (sizeof (TI_MCASP_AUD_DATA));
    if (NULL == pDev)
        {
        return ERROR;
        }

    /* get resources */

    pMcAspRegRes = vxbResourceAlloc (pInst, VXB_RES_MEMORY, 0);
    if (NULL == pMcAspRegRes)
        {
        TI_MCASP_LOG ("get McASP register resource failed\n", 1, 2, 3, 4, 5, 6);
        goto error;
        }

    pResAdr = (VXB_RESOURCE_ADR *)pMcAspRegRes->pRes;
    if (NULL == pResAdr)
        {
        TI_MCASP_LOG ("McASP register address is NULL\n", 1, 2, 3, 4, 5, 6);
        goto error;
        }

    pDev->mcaspHandle   = pResAdr->pHandle;
    pDev->mcaspAddr     = (UINT32)pResAdr->virtual;

    pDataRegRes = vxbResourceAlloc (pInst, VXB_RES_MEMORY, 1);
    if (NULL == pDataRegRes)
        {
        TI_MCASP_LOG ("get data port register resource failed\n", 
                      1, 2, 3, 4, 5, 6);
        goto error;
        }

    pResAdr = (VXB_RESOURCE_ADR *)pDataRegRes->pRes;
    if (NULL == pResAdr)
        {
        TI_MCASP_LOG ("data port register address is NULL\n",
                      1, 2, 3, 4, 5, 6);
        goto error;
        }

    pDev->dataHandle    = pResAdr->pHandle;
    pDev->dataAddr      = (UINT32)pResAdr->virtual;

    txDmaProp = (void *)vxFdtPropGet (pFdtDev->offset, "tx-dma-num",
                                      &txDmaPropLen);
    rxDmaProp = (void *)vxFdtPropGet (pFdtDev->offset, "rx-dma-num",
                                      &rxDmaPropLen);

    if ((NULL != txDmaProp) && (sizeof (UINT32) == txDmaPropLen) &&
        (NULL != rxDmaProp) && (sizeof (UINT32) == rxDmaPropLen))
        {
        /* allocate and configure tx DMA channel */

        pDev->pDmaChanTx = vxbDmaChanAlloc (0);
        if (NULL == pDev->pDmaChanTx)
            {
            TI_MCASP_LOG ("DMA channel for tx allocation failed\n",
                          1, 2, 3, 4, 5, 6);
            goto error;
            }

        pDev->dmaConfigTx.direction = VXB_DMA_MEM_TO_DEV;
        pDev->dmaConfigTx.dmaEvent  = (int)vxFdt32ToCpu (*(UINT32 *)txDmaProp);

        /* allocate and configure rx DMA channel */

        pDev->pDmaChanRx = vxbDmaChanAlloc (0);
        if (NULL == pDev->pDmaChanRx)
            {
            TI_MCASP_LOG ("DMA channel for rx allocation failed\n",
                          1, 2, 3, 4, 5, 6);
            goto error;
            }

        pDev->dmaConfigRx.direction = VXB_DMA_DEV_TO_MEM;
        pDev->dmaConfigRx.dmaEvent  = (int)vxFdt32ToCpu (*(UINT32 *)rxDmaProp);
        }
    else
        {
        TI_MCASP_LOG ("Lack of property - tx-dma-num or rx-dma-num.\n",
                      1, 2, 3, 4, 5, 6);
        goto error;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "codec-unit", &len);
    if ((NULL != prop) && (sizeof (UINT32) == len))
        {
        pDev->codecUnit = vxFdt32ToCpu (*prop);
        }
    else
        {
        goto error;
        }

    if (ERROR == vxbPinMuxEnable (pInst))
        {
        goto error;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "tdm-slots", &len);
    if ((NULL != prop) && (sizeof (UINT32) == len))
        {
        pDev->audioDev.devInfo.maxChannels = (UINT8)vxFdt32ToCpu (*prop);
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

    /* get interrupt resource */

    pTxIntRes = vxbResourceAlloc (pInst, VXB_RES_IRQ, 0);
    if (NULL == pTxIntRes)
       {
       TI_MCASP_LOG ("get tx interrupt resource failed\n", 1, 2, 3, 4, 5, 6);
       goto error;
       }

    /* connect int */

    if (ERROR == vxbIntConnect (pInst, pTxIntRes, (VOIDFUNCPTR)mcAspTxIrqHandle,
                                pDev))
        {
        TI_MCASP_LOG ("connect tx interrupt failed\n", 1, 2, 3, 4, 5, 6);
        goto error;
        }

    if (ERROR == vxbIntEnable (pInst, pTxIntRes))
        {
        TI_MCASP_LOG ("enable tx interrupt failed\n", 1, 2, 3, 4, 5, 6);
        (void)vxbIntDisconnect (pInst, pTxIntRes);
        goto error;
        }

    /* get interrupt resource */

    pRxIntRes = vxbResourceAlloc (pInst, VXB_RES_IRQ, 1);
    if (NULL == pRxIntRes)
       {
       TI_MCASP_LOG ("get rx interrupt resource failed\n", 1, 2, 3, 4, 5, 6);
       goto error;
       }

    /* connect int */

    if (ERROR == vxbIntConnect (pInst, pRxIntRes, (VOIDFUNCPTR)mcAspRxIrqHandle,
                                pDev))
        {
        TI_MCASP_LOG ("connect rx interrupt failed\n", 1, 2, 3, 4, 5, 6);
        (void)vxbIntDisable (pInst, pTxIntRes);
        (void)vxbIntDisconnect (pInst, pTxIntRes);
        goto error;
        }

    if (ERROR == vxbIntEnable (pInst, pRxIntRes))
        {
        TI_MCASP_LOG ("enable rx interrupt failed\n", 1, 2, 3, 4, 5, 6);
        (void)vxbIntDisable (pInst, pTxIntRes);
        (void)vxbIntDisconnect (pInst, pTxIntRes);
        (void)vxbIntDisconnect (pInst, pRxIntRes);
        goto error;
        }

    (void)snprintf ((char *)pDev->audioDev.devInfo.name, AUDIO_DEV_NAME_LEN,
                    TI_MCASP_AUD_DRIVER_NAME);

    pDev->audioDev.devInfo.bytesPerSample   = sizeof (UINT32);
    pDev->audioDev.devInfo.useMsb           = FALSE;
    pDev->audioDev.open                     = mcAspOpen;
    pDev->audioDev.close                    = mcAspClose;
    pDev->audioDev.read                     = mcAspRead;
    pDev->audioDev.write                    = mcAspWrite;
    pDev->audioDev.ioctl                    = mcAspIoctl;
    pDev->audioDev.extension                = (void *)pDev;
    pDev->pInst                             = pInst;

    if (TASK_ID_ERROR == taskSpawn ("audDevInit", AUDIO_DEV_INIT_TASK_PRIO,
                                    0, AUDIO_DEV_INIT_TASK_STACK_SIZE,
                                    (FUNCPTR)mcAspInit, (_Vx_usr_arg_t)pDev,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0))
        {
        goto error;
        }

    /* save Drvctrl */

    vxbDevSoftcSet (pInst, (void *)pDev);

    return OK;

error:
    if (NULL != pDev->pDmaChanTx)
        {
        vxbDmaChanFree (pDev->pDmaChanTx);
        }

    if (NULL != pDev->pDmaChanRx)
        {
        vxbDmaChanFree (pDev->pDmaChanRx);
        }

    if (NULL != pTxIntRes)
        {
        (void)vxbResourceFree (pInst, pTxIntRes);
        }

    if (NULL != pRxIntRes)
        {
        (void)vxbResourceFree (pInst, pRxIntRes);
        }

    if (NULL != pMcAspRegRes)
        {
        (void)vxbResourceFree (pInst, pMcAspRegRes);
        }

    if (NULL != pDataRegRes)
        {
        (void)vxbResourceFree (pInst, pDataRegRes);
        }

    vxbMemFree (pDev);
    return ERROR;
    }

/*******************************************************************************
*
* mcAspInit - register McASP module to audio framework
*
* This routine registers McASP module to audio framework.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*
*/

LOCAL STATUS mcAspInit
    (
    TI_MCASP_AUD_DATA * pDev
    )
    {
    UINT32  value;

    if (NULL == pDev)
        {
        return ERROR;
        }

    pDev->dmaTxSem = semBCreate (SEM_Q_FIFO | SEM_Q_PRIORITY, SEM_EMPTY);
    if (SEM_ID_NULL == pDev->dmaTxSem)
        {
        return ERROR;
        }

    pDev->dmaRxSem = semBCreate (SEM_Q_FIFO | SEM_Q_PRIORITY, SEM_EMPTY);
    if (SEM_ID_NULL == pDev->dmaRxSem)
        {
        (void)semDelete (pDev->dmaTxSem);
        return ERROR;
        }

    /* register driver handle */

    if (ERROR == audioCoreRegTransceiver (&pDev->audioDev,
                                          (int)pDev->codecUnit))
        {
        (void)semDelete (pDev->dmaTxSem);
        (void)semDelete (pDev->dmaRxSem);
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* mcAspOpen - initialize the McASP module and set the McASP to default status
*
* This routine initializes the McASP module and set the McASP to default status.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL int mcAspOpen
    (
    TI_MCASP_AUD_DATA * pDev
    )
    {
    UINT32          i;
    int             len;
    UINT32 *        prop;
    VXB_FDT_DEV *   pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet (pDev->pInst);
    UINT32          value   = 0;
    UINT32          serialCfg;

    if (NULL == pFdtDev)
        {
        return ERROR;
        }

    if (ERROR == vxbClkEnableAll (pDev->pInst))
        {
        return ERROR;
        }

    mcAspReset (pDev);

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "serial-dir", &len);
    if ((NULL != prop) && (len >= sizeof (UINT32)))
        {
        for (i = 0; i < (len / sizeof (UINT32)); i++)
            {
            serialCfg = vxFdt32ToCpu (*prop++);
            TI_MCASP_LOG ("serialCfg[%d] = 0x%x\n", i, serialCfg, 3, 4, 5, 6);

            if (0x1 == serialCfg)
                {
                MCASP_WRITE32 (pDev, (AM335X_MCASP_SRCTL0 + (i << 2)),
                               SRMOD_TRANSMITTER);
                value |= (0x1 << i);
                continue;
                }

            if (0x2 == serialCfg)
                {
                MCASP_WRITE32 (pDev, (AM335X_MCASP_SRCTL0 + (i << 2)),
                               SRMOD_RECEIVER);
                continue;
                }

            MCASP_WRITE32 (pDev, (AM335X_MCASP_SRCTL0 + (i << 2)),
                           SRMOD_INACTIVE);
            }
        }
    else
        {
        (void)vxbClkDisableAll (pDev->pInst);
        return ERROR;
        }

    /* configure McASP pin functions */

    if (1 == pDev->isMaster)
        {
        value |= AFSR_OUT | AHCLKR_OUT | ACLKR_OUT | AFSX_OUT | AHCLKX_OUT |
                 ACLKX_OUT;
        }

    MCASP_WRITE32 (pDev, AM335X_MCASP_PDIR_REG, value);
    MCASP_WRITE32 (pDev, AM335X_MCASP_PFUNC_REG, 0x0);

    /* configure tx channel */

    MCASP_WRITE32 (pDev, AM335X_MCASP_AFSXCTL_REG,
                   (XMOD (pDev->audioDev.devInfo.maxChannels) | FXWID | FSXP));

    value = CLKRP_FALLING_TX;
    if (1 == pDev->isMaster)
        {
        value |= CLKXM_INTERNAL;
        }

    MCASP_WRITE32 (pDev, AM335X_MCASP_ACLKXCTL_REG, value);

    value = 0;
    for (i = 0; i < pDev->audioDev.devInfo.maxChannels; i++)
        {
        value |= (0x1 << i);
        }

    MCASP_WRITE32 (pDev, AM335X_MCASP_XTDM_REG, value);
    MCASP_WRITE32 (pDev, AM335X_MCASP_XSTAT_REG, 0xFFFFFFFF);

    /* configure rx channel */

    MCASP_WRITE32 (pDev, AM335X_MCASP_AFSRCTL_REG,
                   (RMOD (pDev->audioDev.devInfo.maxChannels) | FRWID | FSRP));

    value = CLKRP_RISING_RX;
    if (1 == pDev->isMaster)
        {
        value |= CLKRM_INTERNAL;
        }

    MCASP_WRITE32 (pDev, AM335X_MCASP_ACLKRCTL_REG, value);

    value = 0;
    for (i = 0; i < pDev->audioDev.devInfo.maxChannels; i++)
        {
        value |= (0x1 << i);
        }

    MCASP_WRITE32 (pDev, AM335X_MCASP_RTDM_REG, value);
    MCASP_WRITE32 (pDev, AM335X_MCASP_RSTAT_REG, 0xFFFFFFFF);

    return OK;
    }

/*******************************************************************************
*
* mcAspClose - set the McASP to default status
*
* This routine sets the McASP to default status.
*
* RETURNS: always OK
*
* ERRNO: N/A
*/

LOCAL int mcAspClose
    (
    TI_MCASP_AUD_DATA * pDev
    )
    {
    mcAspReset (pDev);
    (void)vxbClkDisableAll (pDev->pInst);
    return OK;
    }

/*******************************************************************************
*
* mcAspRead - read audio data from McASP
*
* This routine reads audio data from McASP.
*
* RETURNS: number of bytes read
*
* ERRNO: N/A
*/

LOCAL int mcAspRead
    (
    TI_MCASP_AUD_DATA * pDev,
    char *              pBuffer,    /* location to store read data */
    int                 maxbytes    /* number of bytes to read */
    )
    {
    UINT32          elemCount;
    UINT32          txSize;;
    INT32           txBytes     = maxbytes;
    VXB_DMA_TX *    pDmaTx;

    MCASP_WRITE32 (pDev, AM335X_MCASP_RINTCTL_REG, RX_ROVRN_INT);

    while (txBytes > 0)
        {
        if (txBytes > MCASP_DMA_SIZE_MAX)
            {
            txSize = MCASP_DMA_SIZE_MAX;
            }
        else
            {
            txSize = txBytes;
            }

        elemCount = txSize / pDev->audioDev.devInfo.bytesPerSample;
        if (0 == elemCount)
            {
            return 0;
            }

        if (elemCount > MCASP_BLK_SIZE)
            {
            pDev->dmaConfigRx.srcMaxBurst =
                MCASP_BLK_SIZE * pDev->audioDev.devInfo.bytesPerSample;
            }
        else
            {
            pDev->dmaConfigRx.srcMaxBurst =
                elemCount * pDev->audioDev.devInfo.bytesPerSample;
            }

        txSize  = elemCount * pDev->audioDev.devInfo.bytesPerSample;

        if (ERROR == vxbDmaChanConfig (pDev->pDmaChanRx, &pDev->dmaConfigRx))
            {
            TI_MCASP_LOG ("DMA channel for rx configuration failed\n",
                          1, 2, 3, 4, 5, 6);
            return (maxbytes - txBytes);
            }

        pDmaTx = vxbDmaChanPreMemCpy (pDev->pDmaChanRx,
                                      (VIRT_ADDR)pDev->dataAddr,
                                      (VIRT_ADDR)pBuffer, txSize, 0);
        if (NULL == pDmaTx)
            {
            return (maxbytes - txBytes);
            }

        pDmaTx->fn      = mcAspDmaCallback;
        pDmaTx->pArg    = (void *)pDev->dmaRxSem;

        if (ERROR == vxbDmaChanStart (pDmaTx))
            {
            return (maxbytes - txBytes);
            }

        (void)semTake (pDev->dmaRxSem, WAIT_FOREVER);

        pBuffer += txSize;
        txBytes -= txSize;
        }

    MCASP_WRITE32 (pDev, AM335X_MCASP_RINTCTL_REG, 0);

    return maxbytes;
    }

/*******************************************************************************
*
* mcAspWrite - write audio data to McASP
*
* This routine writes audio data to McASP.
*
* RETURNS: number of bytes written
*
* ERRNO: N/A
*/

LOCAL int mcAspWrite
    (
    TI_MCASP_AUD_DATA * pDev,
    char *              pBuffer,    /* location of data buffer */
    int                 nbytes      /* number of bytes to output */
    )
    {
    UINT32          elemCount;
    UINT32          txSize;
    INT32           txBytes = nbytes;
    VXB_DMA_TX *    pDmaTx;

    MCASP_WRITE32 (pDev, AM335X_MCASP_XINTCTL_REG, TX_XUNDRN_INT);

    while (txBytes > 0)
        {
        if (txBytes > MCASP_DMA_SIZE_MAX)
            {
            txSize = MCASP_DMA_SIZE_MAX;
            }
        else
            {
            txSize = txBytes;
            }

        elemCount = txSize / pDev->audioDev.devInfo.bytesPerSample;
        if (0 == elemCount)
            {
            return 0;
            }

        if (elemCount > MCASP_BLK_SIZE)
            {
            pDev->dmaConfigTx.destMaxBurst =
                MCASP_BLK_SIZE * pDev->audioDev.devInfo.bytesPerSample;
            }
        else
            {
            pDev->dmaConfigTx.destMaxBurst =
                elemCount * pDev->audioDev.devInfo.bytesPerSample;
            }

        txSize  = elemCount * pDev->audioDev.devInfo.bytesPerSample;
        if (ERROR == vxbDmaChanConfig (pDev->pDmaChanTx, &pDev->dmaConfigTx))
            {
            TI_MCASP_LOG ("DMA channel for tx configuration failed\n",
                          1, 2, 3, 4, 5, 6);
            return (nbytes - txBytes);
            }

        pDmaTx = vxbDmaChanPreMemCpy (pDev->pDmaChanTx, (VIRT_ADDR)pBuffer,
                                      (VIRT_ADDR)pDev->dataAddr,
                                      txSize, 0);
        if (NULL == pDmaTx)
            {
            TI_MCASP_LOG ("DMA channel PreMemCpy failed\n",
                          1, 2, 3, 4, 5, 6);
            return (nbytes - txBytes);
            }

        pDmaTx->fn      = mcAspDmaCallback;
        pDmaTx->pArg    = (void *)pDev->dmaTxSem;

        if (ERROR == vxbDmaChanStart (pDmaTx))
            {
            TI_MCASP_LOG ("DMA channel start failed\n",
                          1, 2, 3, 4, 5, 6);
            return (nbytes - txBytes);
            }

        (void)semTake (pDev->dmaTxSem, WAIT_FOREVER);

        pBuffer += txSize;
        txBytes -= txSize;
        }

    MCASP_WRITE32 (pDev, AM335X_MCASP_XINTCTL_REG, 0);

    return nbytes;
    }

/*******************************************************************************
*
* mcAspIoctl - handle ioctls for McASP
*
* This routine handles ioctls for McASP.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS mcAspIoctl
    (
    TI_MCASP_AUD_DATA * pDev,
    AUDIO_IO_CTRL       function,   /* function to perform */
    AUDIO_IOCTL_ARG *   pIoctlArg   /* function argument */
    )
    {
    STATUS  status  = OK;

    switch (function)
        {
        case AUDIO_SET_DATA_INFO:
            switch (pIoctlArg->dataInfo.sampleBits)
                {
                case 16:
                    pDev->audioDev.devInfo.bytesPerSample = sizeof (UINT16);
                    MCASP_WRITE32 (pDev, AM335X_MCASP_XMASK_REG, 0x0000FFFF);
                    MCASP_WRITE32 (pDev, AM335X_MCASP_RMASK_REG, 0x0000FFFF);
                    MCASP_WRITE32 (pDev, AM335X_MCASP_XFMT_REG,
                                   (XRVRS | XSSZ_16 | XROT_16 | XDATDLY (1)));
                    MCASP_WRITE32 (pDev, AM335X_MCASP_RFMT_REG,
                                   (RRVRS | RSSZ_16 | RROT_0 | RDATDLY (1)));
                    break;

                case 24:
                    pDev->audioDev.devInfo.bytesPerSample = sizeof (UINT8) * 3;
                    MCASP_WRITE32 (pDev, AM335X_MCASP_XMASK_REG, 0x00FFFFFF);
                    MCASP_WRITE32 (pDev, AM335X_MCASP_RMASK_REG, 0x00FFFFFF);
                    MCASP_WRITE32 (pDev, AM335X_MCASP_XFMT_REG,
                                   (XRVRS | XSSZ_24 | XROT_24 | XDATDLY (1)));
                    MCASP_WRITE32 (pDev, AM335X_MCASP_RFMT_REG,
                                   (RRVRS | RSSZ_24 | RROT_0 | RDATDLY (1)));
                    break;

                case 32:
                    pDev->audioDev.devInfo.bytesPerSample = sizeof (UINT32);
                    MCASP_WRITE32 (pDev, AM335X_MCASP_XMASK_REG, 0xFFFFFFFF);
                    MCASP_WRITE32 (pDev, AM335X_MCASP_RMASK_REG, 0xFFFFFFFF);
                    MCASP_WRITE32 (pDev, AM335X_MCASP_XFMT_REG,
                                   (XRVRS | XSSZ_32 | XROT_0 | XDATDLY (1)));
                    MCASP_WRITE32 (pDev, AM335X_MCASP_RFMT_REG,
                                   (RRVRS | RSSZ_32 | RROT_0 | RDATDLY (1)));
                    break;

                default:
                    return ERROR;
                }

            pDev->dmaConfigTx.destAddrWidth =
                pDev->audioDev.devInfo.bytesPerSample;

            pDev->dmaConfigRx.srcAddrWidth  =
                pDev->audioDev.devInfo.bytesPerSample;

            break;

        case AUDIO_SET_PATH:
            pDev->curPaths = pIoctlArg->path;
            break;

        case AUDIO_DEV_ENABLE:
            if (0 != (pDev->curPaths & (UINT32)AUDIO_OUT_MASK))
                {
                mcAspTxEn (pDev, TRUE);
                }

            if (0 != (pDev->curPaths & (UINT32)AUDIO_IN_MASK))
                {
                mcAspRxEn (pDev, TRUE);
                }
            break;

        case AUDIO_DEV_DISABLE:
            if (0 != (pDev->curPaths & (UINT32)AUDIO_OUT_MASK))
                {
                mcAspTxEn (pDev, FALSE);
                }

            if (0 != (pDev->curPaths & (UINT32)AUDIO_IN_MASK))
                {
                mcAspRxEn (pDev, FALSE);
                }
            break;

        default:
            break;
        }

    return status;
    }

/*******************************************************************************
*
* mcAspReset - reset McASP
*
* This routine resets McASP.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void mcAspReset
    (
    TI_MCASP_AUD_DATA *  pDev
    )
    {
    MCASP_WRITE32 (pDev, AM335X_MCASP_GBLCTL, 0x0);
    MCASP_WRITE32 (pDev, AM335X_MCASP_PWREMUMGT_REG, MCASP_SOFT);
    }

/*******************************************************************************
*
* mcAspTxEn - enable or disable McASP serializer TX
*
* This routine enables or disables McASP serializer TX.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void mcAspTxEn
    (
    TI_MCASP_AUD_DATA * pDev,
    BOOL                isEn
    )
    {
    UINT32  value;

    if (isEn)
        {
        value = 0;
        MCASP_WRITE32 (pDev, AM335X_MCASP_XGBLCTL, value);
        value |= TXSER;
        MCASP_WRITE32 (pDev, AM335X_MCASP_XGBLCTL, value);
        if (1 == pDev->isMaster)
            {
            value |= XFRST;
            MCASP_WRITE32 (pDev, AM335X_MCASP_XGBLCTL, value);
            value |= XCLKRST;
            MCASP_WRITE32 (pDev, AM335X_MCASP_XGBLCTL, value);
            value |= XHCLKRST;
            MCASP_WRITE32 (pDev, AM335X_MCASP_XGBLCTL, value);
            }

        value |= TXSM;
        MCASP_WRITE32 (pDev, AM335X_MCASP_XGBLCTL, value);

        value = 0;
        MCASP_WRITE32 (pDev, AM335X_MCASP_WFIFOCTL, value);
        value |= NUMEVT (MCASP_BLK_SIZE) | NUMDMA (1);
        MCASP_WRITE32 (pDev, AM335X_MCASP_WFIFOCTL, value);
        value |= ENA;
        MCASP_WRITE32 (pDev, AM335X_MCASP_WFIFOCTL, value);
        MCASP_WRITE32 (pDev, AM335X_MCASP_XSTAT_REG, 0xFFFFFFFF);
        }
    else
        {
        MCASP_WRITE32 (pDev, AM335X_MCASP_XINTCTL_REG, 0);
        MCASP_WRITE32 (pDev, AM335X_MCASP_XGBLCTL, 0);
        MCASP_WRITE32 (pDev, AM335X_MCASP_WFIFOCTL, 0);
        }
    }

/*******************************************************************************
*
* mcAspRxEn - enable or disable McASP serializer RX
*
* This routine enables or disables McASP serializer RX.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void mcAspRxEn
    (
    TI_MCASP_AUD_DATA * pDev,
    BOOL                isEn
    )
    {
    UINT32  value;

    if (isEn)
        {
        value = 0;
        MCASP_WRITE32 (pDev, AM335X_MCASP_RGBLCTL, value);
        value |= RXSER;
        MCASP_WRITE32 (pDev, AM335X_MCASP_RGBLCTL, value);
        if (1 == pDev->isMaster)
            {
            value |= RFRST;
            MCASP_WRITE32 (pDev, AM335X_MCASP_RGBLCTL, value);
            value |= RCLKRST;
            MCASP_WRITE32 (pDev, AM335X_MCASP_RGBLCTL, value);
            value |= RHCLKRST;
            MCASP_WRITE32 (pDev, AM335X_MCASP_RGBLCTL, value);
            }

        value |= RXSM;
        MCASP_WRITE32 (pDev, AM335X_MCASP_RGBLCTL, value);

        value = 0;
        MCASP_WRITE32 (pDev, AM335X_MCASP_RFIFOCTL, value);
        value |= NUMEVT (MCASP_BLK_SIZE) | NUMDMA (1);
        MCASP_WRITE32 (pDev, AM335X_MCASP_RFIFOCTL, value);
        value |= ENA;
        MCASP_WRITE32 (pDev, AM335X_MCASP_RFIFOCTL, value);
        MCASP_WRITE32 (pDev, AM335X_MCASP_RSTAT_REG, 0xFFFFFFFF);
        }
    else
        {
        MCASP_WRITE32 (pDev, AM335X_MCASP_RINTCTL_REG, 0);
        MCASP_WRITE32 (pDev, AM335X_MCASP_RGBLCTL, 0);
        MCASP_WRITE32 (pDev, AM335X_MCASP_RFIFOCTL, 0x0);
        }
    }

/*******************************************************************************
*
* mcAspDmaCallback - callback function for McASP DMA interrupt
*
* This routine is the ccallback function for McASP DMA interrupt.
*
* RETURNS: N/A
*
* ERRNO
*/

LOCAL void mcAspDmaCallback
    (
    SEM_ID  dmaSem
    )
    {
    (void)semGive (dmaSem);
    }

/*******************************************************************************
*
* mcAspTxIrqHandle - interrupt level processing
*
* This routine handles tx interrupts.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void mcAspTxIrqHandle
    (
    TI_MCASP_AUD_DATA * pDev
    )
    {
    UINT32  regValue;

    regValue = MCASP_READ32 (pDev, AM335X_MCASP_XSTAT_REG);
    MCASP_WRITE32 (pDev, AM335X_MCASP_XSTAT_REG, regValue);
    if (regValue & TX_XSTAT_UNDERRUN)
        {
        mcAspTxEn (pDev, TRUE);
        }
    }

/*******************************************************************************
*
* mcAspRxIrqHandle - interrupt level processing
*
* This routine handles rx interrupts.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void mcAspRxIrqHandle
    (
    TI_MCASP_AUD_DATA * pDev
    )
    {
    UINT32  regValue;

    regValue = MCASP_READ32 (pDev, AM335X_MCASP_RSTAT_REG);
    MCASP_WRITE32 (pDev, AM335X_MCASP_RSTAT_REG, regValue);
    if (regValue & TX_RSTAT_ROVRN)
        {
        mcAspRxEn (pDev, TRUE);
        }
    }
