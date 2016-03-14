/* audioDrvFslSsi.c - Freescale i.MX series SSI driver */

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
12nov15,c_l  removed incorrect register access (V7GFX-290)
01sep14,y_f  removed pSsiAudData (V7GFX-208)
08feb14,y_f  written (US29469)
*/

/*
DESCRIPTION

This is the VxBus driver for Freescale Synchronous Serial Interface (SSI).
This driver provides callback methods to support the audio library, and the
client users should use the API provided by the audio library to access the low
level hardware function.

To add the driver to the vxWorks image, add the following component to the
kernel configuration.

\cs
vxprj component add DRV_AUDIO_FSL_SSI
\ce

This driver is bound to device tree, and the device tree node must specify
below parameters:

\is

\i <compatible>
This required parameter specifies the name of the driver. It must be
"fsl,imx-ssi".

\i <reg>
This required parameter specifies the device address of this module. The first
is the SSI address. The second is the Digital Audio Multiplexer (AUDMUX)
address.

\i <clocks>
This required parameter specifies the list of clock.

\i <clock-names>
This required parameter specifies the clock names of this module.

\i <clk-rate>
This required parameter specifies the clock clock rate of this module. The first
is used for 8KHz, 16KHz, 24KHz, 32KHz and 48KHz. The second is used for
11.025KHz, 22.05KHz and 44.1KHz.

\i <pinmux-0>
This required parameter specifies pin mux configuration for this module.

\i <audmux-codec-port>
This optional parameter specifies the Digital Audio Multiplexer (AUDMUX) port
which the codec device connected. If the AUDMUX address is defined in <reg>
field, the parameter must be defined also.

\i <audmux-ssi-port>
This optional parameter specifies the Digital Audio Multiplexer (AUDMUX) port
which the SSI connected. If the AUDMUX address is defined in <reg> field, the
parameter must be defined also.

\i <tx-dma-num>
This required parameter specifies DMA number of SSI TX.

\i <rx-dma-num>
This required parameter specifies DMA number of SSI RX.

\i <master-enable>
This required parameter specifies whether enable master mode. 1 indicates
enable, 0 indicates disable. If the connected codec device works at master mode,
SSI must work at slave mode and vice versa.

\i <codec-unit>
This required parameter specifies codec device number. The value is defined in
audio codec device tree node.

\ie

An example of device node is shown below:

\cs
    ssi1: ssi@02028000
        {
        compatible = "fsl,imx-ssi";
        reg = <0x02028000 0x4000>,<0x021D8000 0x4000>;
        clocks = <&clk 66>,<&clk 69>;
        clock-names = "ssi1","clko2";
        clk-rate = <98304000>,<90316800>;
        pinmux-0 = <&ssi1_2>;
        audmux-codec-port = <4>;
        audmux-ssi-port = <1>;
        tx-dma-num = <38>;
        rx-dma-num = <37>;
        master-enable = <0>;
        codec-unit = <0>;
        };
\ce

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
#include "audioDrvFslSsi.h"

/* defines */

#undef  FSL_IMX_SSI_DEBUG
#ifdef  FSL_IMX_SSI_DEBUG
#   define FSL_IMX_SSI_LOG(fmt, a, b, c, d, e, f) \
            (void)kprintf (fmt, a, b, c, d, e, f);

#else
#   define FSL_IMX_SSI_LOG(fmt, a, b, c, d, e, f) 
#endif

/* typedefs */

/* structure to store the audio module information */

typedef struct fslSsiAudData
    {
    UINT32                  codecUnit;
    UINT32                  ssiAddr;        /* SSI register base address */
    void *                  ssiHandle;      /* vxBus handle for SSI */
    UINT32                  audMuxAddr;     /* AUDMUX register base address */
    void *                  audMuxHandle;   /* vxBus handle for AUDMUX */
    UINT32                  ssiPort;
    UINT32                  codecPort;
    UINT32                  isMaster;
    UINT32                  ssiClk0;        /* clock for 8k, 16k, 24k, 32k, 48k */
    UINT32                  ssiClk1;        /* clock for 11.025k, 22.05k, 44.1k */
    SEM_ID                  dmaTxSem;
    SEM_ID                  dmaRxSem;
    VXB_DMA_CHAN *          pDmaChanTx;
    VXB_DMA_CHAN *          pDmaChanRx;
    VXB_DMA_SLAVE_CONFIG    dmaTxConfig;
    VXB_DMA_SLAVE_CONFIG    dmaRxConfig;
    AUDIO_DEV               audioDev;
    UINT32                  curPaths;
    VXB_CLK *               pSsiClk;
    VXB_CLK *               pChildClk;
    }FSL_SSI_AUD_DATA;

/* forward declarations */

LOCAL STATUS    fdtFslSsiAudProbe (VXB_DEV_ID pDev);
LOCAL STATUS    fdtFslSsiAudAttach (VXB_DEV_ID pInst);
LOCAL STATUS    ssiInit (FSL_SSI_AUD_DATA * pDev);
LOCAL int       ssiOpen (FSL_SSI_AUD_DATA * pDev);
LOCAL int       ssiClose (FSL_SSI_AUD_DATA * pDev);
LOCAL int       ssiRead (FSL_SSI_AUD_DATA * pDev, char * pBuffer, int maxbytes);
LOCAL int       ssiWrite (FSL_SSI_AUD_DATA * pDev, char * pBuffer, int nbytes);
LOCAL STATUS    ssiIoctl (FSL_SSI_AUD_DATA * pDev, AUDIO_IO_CTRL function,
                          AUDIO_IOCTL_ARG * pIoctlArg);
LOCAL void      ssiReset (FSL_SSI_AUD_DATA * pDev);
LOCAL STATUS    ssiConfig (FSL_SSI_AUD_DATA * pDev, UINT32 sampleBits, UINT32
                           sampleRate);
LOCAL void      ssiTxEn (FSL_SSI_AUD_DATA * pDev, UINT8 txId, BOOL isEn);
LOCAL void      ssiRxEn (FSL_SSI_AUD_DATA * pDev, UINT8 rxId, BOOL isEn);
LOCAL void      ssiDmaCallback (SEM_ID dmaSem);

/* locals */

LOCAL VXB_DRV_METHOD    fdtFslSsiAudMethodList[] =
    {
    { VXB_DEVMETHOD_CALL(vxbDevProbe), fdtFslSsiAudProbe},
    { VXB_DEVMETHOD_CALL(vxbDevAttach), fdtFslSsiAudAttach},
    { 0, NULL }
    };

/* Freescale i.MX series SSI openfirmware driver */

VXB_DRV vxbFdtFslSsiAudDrv =
    {
    {NULL},
    "Freescale i.MX series SSI",            /* Name */
    "Freescale i.MX series SSI Fdt driver", /* Description */
    VXB_BUSID_FDT,                          /* Class */
    0,                                      /* Flags */
    0,                                      /* Reference count */
    fdtFslSsiAudMethodList,                 /* Method table */
    };

LOCAL const VXB_FDT_DEV_MATCH_ENTRY fslSsiAudMatch[] =
    {
        {
        FSL_SSI_AUD_DRIVER_NAME,            /* compatible */
        (void *)NULL,
        },
        {}                                  /* Empty terminated list */
    };

VXB_DRV_DEF(vxbFdtFslSsiAudDrv)

/* functions */

/*******************************************************************************
*
* fdtFslSsiAudProbe - probe for device presence at specific address
*
* Check for Freescale i.MX series SSI (or compatible) device at the specified
* base address. We assume one is present at that address, but we need to verify.
*
* RETURNS: OK if probe passes and assumed a valid Freescale i.MX series SSI
* (or compatible) device. ERROR otherwise.
*
*/

LOCAL STATUS fdtFslSsiAudProbe
    (
    VXB_DEV_ID  pDev
    )
    {
    return vxbFdtDevMatch (pDev, fslSsiAudMatch, NULL);
    }

/*******************************************************************************
*
* fdtFslSsiAudAttach - attach Freescale i.MX series SSI
*
* This is the Freescale i.MX series SSI initialization routine.
*
* RETURNS: OK or ERROR when failed
*
* ERRNO
*/

LOCAL STATUS fdtFslSsiAudAttach
    (
    VXB_DEV_ID  pInst
    )
    {
    VXB_FDT_DEV *           pFdtDev = (VXB_FDT_DEV *)vxbFdtDevGet (pInst);
    FSL_SSI_AUD_DATA *      pDev;
    VXB_RESOURCE *          pSsiRegRes      = NULL;
    VXB_RESOURCE *          pAudMuxRegRes   = NULL;
    VXB_RESOURCE_ADR *      pResAdr;
    int                     len;
    UINT32 *                prop;
    void *                  txDmaProp;
    void *                  rxDmaProp;
    int                     txDmaPropLen;
    int                     rxDmaPropLen;
    char*                   pStr = NULL;
    int                     slen;

    if (NULL == pFdtDev)
        {
        return ERROR;
        }

    pDev = (FSL_SSI_AUD_DATA *)vxbMemAlloc (sizeof (FSL_SSI_AUD_DATA));
    if (NULL == pDev)
        {
        return ERROR;
        }

    /* get resources */

    pSsiRegRes = vxbResourceAlloc (pInst, VXB_RES_MEMORY, 0);
    if (NULL == pSsiRegRes)
        {
        FSL_IMX_SSI_LOG ("get register 0 resource failed\n",
                         1, 2, 3, 4, 5, 6);
        goto error;
        }

    pResAdr = (VXB_RESOURCE_ADR *)pSsiRegRes->pRes;
    if (NULL == pResAdr)
        {
        FSL_IMX_SSI_LOG ("register 0 address is NULL\n", 1, 2, 3, 4, 5, 6);
        goto error;
        }

    pDev->ssiHandle = pResAdr->pHandle;
    pDev->ssiAddr   = (UINT32)pResAdr->virtual;

    pAudMuxRegRes = vxbResourceAlloc (pInst, VXB_RES_MEMORY, 1);
    if (NULL != pAudMuxRegRes)
        {
        pResAdr = (VXB_RESOURCE_ADR *)pAudMuxRegRes->pRes;
        if (NULL == pResAdr)
            {
            FSL_IMX_SSI_LOG ("register 1 address is NULL\n", 1, 2, 3, 4, 5, 6);
            goto error;
            }

        pDev->audMuxHandle  = pResAdr->pHandle;
        pDev->audMuxAddr    = (UINT32)pResAdr->virtual;
        }

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
            FSL_IMX_SSI_LOG ("DMA channel for tx allocation failed\n",
                             1, 2, 3, 4, 5, 6);
            goto error;
            }

        pDev->dmaTxConfig.direction = VXB_DMA_MEM_TO_DEV;
        pDev->dmaTxConfig.dmaEvent  = (int)vxFdt32ToCpu (*(UINT32 *)txDmaProp);

        /* allocate and configure rx DMA channel */

        pDev->pDmaChanRx = vxbDmaChanAlloc (0);
        if (NULL == pDev->pDmaChanRx)
            {
            FSL_IMX_SSI_LOG ("DMA channel for rx allocation failed\n",
                             1, 2, 3, 4, 5, 6);
            goto error;
            }

        pDev->dmaRxConfig.direction = VXB_DMA_DEV_TO_MEM;
        pDev->dmaRxConfig.dmaEvent  = (int)vxFdt32ToCpu (*(UINT32 *)rxDmaProp);
        }
    else
        {
        FSL_IMX_SSI_LOG ("fdtFslSsiAudAttach: lack of property - tx-dma-num "
                         "or rx-dma-num.\n",
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

    if (0 != pDev->audMuxAddr)
        {
        prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "audmux-codec-port",
                                       &len);
        if ((NULL != prop) && (sizeof (UINT32) == len))
            {
            pDev->codecPort = vxFdt32ToCpu (*prop);
            }
        else
            {
            goto error;
            }

        prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "audmux-ssi-port",
                                       &len);
        if ((NULL != prop) && (sizeof (UINT32) == len))
            {
            pDev->ssiPort = vxFdt32ToCpu (*prop);
            }
        else
            {
            goto error;
            }
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

    pStr = (char *)vxFdtPropGet (pFdtDev->offset, "clock-names", &len);
    if ((NULL != pStr) && (len > 0))
        {
        pDev->pSsiClk = vxbClkGet (pInst, pStr);
        if (NULL == pDev->pSsiClk)
            {
            goto error;
            }

        slen = strlen (pStr);
        len = len - slen - 1;
        if (len > 0)
            {
            pStr = pStr + slen + 1;
            pDev->pChildClk = vxbClkGet (pInst, pStr);
            if (NULL == pDev->pChildClk)
                {
                goto error;
                }
            if (ERROR == vxbClkParentSet (pDev->pChildClk, pDev->pSsiClk))
                {
                goto error;
                }
            }
        }
    else
        {
        goto error;
        }

    prop = (UINT32 *)vxFdtPropGet (pFdtDev->offset, "clk-rate", &len);
    if ((NULL != prop) && (sizeof (UINT32) * 2 == len))
        {
        pDev->ssiClk0 = vxFdt32ToCpu (*prop++);
        pDev->ssiClk1 = vxFdt32ToCpu (*prop);
        }
    else
        {
        goto error;
        }

    if (ERROR == vxbPinMuxEnable (pInst))
        {
        goto error;
        }

    (void)snprintf ((char *)pDev->audioDev.devInfo.name, AUDIO_DEV_NAME_LEN,
                    FSL_SSI_AUD_DRIVER_NAME);

    pDev->audioDev.devInfo.bytesPerSample   = sizeof (UINT32);
    pDev->audioDev.devInfo.maxChannels      = 2;
    pDev->audioDev.devInfo.useMsb           = FALSE;
    pDev->audioDev.open                     = ssiOpen;
    pDev->audioDev.close                    = ssiClose;
    pDev->audioDev.read                     = ssiRead;
    pDev->audioDev.write                    = ssiWrite;
    pDev->audioDev.ioctl                    = ssiIoctl;
    pDev->audioDev.extension                = (void *)pDev;

    if (TASK_ID_ERROR == taskSpawn ("audDevInit", AUDIO_DEV_INIT_TASK_PRIO,
                                    0, AUDIO_DEV_INIT_TASK_STACK_SIZE,
                                    (FUNCPTR)ssiInit, (_Vx_usr_arg_t)pDev,
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

    if (NULL != pSsiRegRes)
        {
        (void)vxbResourceFree (pInst, pSsiRegRes);
        }

    if (NULL != pAudMuxRegRes)
        {
        (void)vxbResourceFree (pInst, pAudMuxRegRes);
        }

    vxbMemFree (pDev);
    return ERROR;
    }

/*******************************************************************************
*
* ssiInit - register SSI module to audio framework and initialize AUDMUX
*
* This routine registers SSI module to audio framework and initializes AUDMUX.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*
*/

LOCAL STATUS ssiInit
    (
    FSL_SSI_AUD_DATA *  pDev
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

    if (0 != pDev->audMuxAddr)
        {
        if (pDev->isMaster)
            {
            /* configure SSI port */

            value = (1 << AUDMUX_PTCR_SYN_OFFSET);
            AUDMUX_WRITE32 (pDev, ((pDev->ssiPort - 1) * 8), value);

            /* configure codec port */

            value = (UINT32)(1 << AUDMUX_PTCR_TFS_DIR_OFFSET) |
                    ((pDev->ssiPort - 1) << AUDMUX_PTCR_TFSEL_OFFSET) |
                    (1 << AUDMUX_PTCR_TCLKDIR_OFFSET) |
                    ((pDev->ssiPort - 1) << AUDMUX_PTCR_TCSEL_OFFSET) |
                    (1 << AUDMUX_PTCR_SYN_OFFSET);
            AUDMUX_WRITE32 (pDev, ((pDev->codecPort - 1) * 8), value);
            }
        else
            {
            /* configure SSI port */

            value = (UINT32)(1 << AUDMUX_PTCR_TFS_DIR_OFFSET) |
                    ((pDev->codecPort - 1) << AUDMUX_PTCR_TFSEL_OFFSET) |
                    (1 << AUDMUX_PTCR_TCLKDIR_OFFSET) |
                    ((pDev->codecPort - 1) << AUDMUX_PTCR_TCSEL_OFFSET) |
                    (1 << AUDMUX_PTCR_SYN_OFFSET);
            AUDMUX_WRITE32 (pDev, ((pDev->ssiPort - 1) * 8), value);

            /* configure codec port */

            value = (1 << AUDMUX_PTCR_SYN_OFFSET);
            AUDMUX_WRITE32 (pDev, ((pDev->codecPort - 1) * 8), value);
            }

        value = ((pDev->codecPort - 1) << AUDMUX_PDCR_RXDSEL_OFFSET);
        AUDMUX_WRITE32 (pDev, ((pDev->ssiPort - 1) * 8 + 4), value);

        value = ((pDev->ssiPort - 1) << AUDMUX_PDCR_RXDSEL_OFFSET);
        AUDMUX_WRITE32 (pDev, ((pDev->codecPort - 1) * 8 + 4), value);
        }

    return OK;
    }

/*******************************************************************************
*
* ssiOpen - initialize the SSI module and set the SSI to default status
*
* This routine initializes the SSI module and set the SSI to default status.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL int ssiOpen
    (
    FSL_SSI_AUD_DATA *  pDev
    )
    {
    UINT32  value;

    while (OK == vxbClkDisable (pDev->pSsiClk))
        {
        continue;
        }

    if (ERROR == vxbClkRateSet (pDev->pSsiClk, pDev->ssiClk0))
        {
        return ERROR;
        }

    if (ERROR == vxbClkEnable (pDev->pSsiClk))
        {
        return ERROR;
        }

    ssiReset (pDev);

    /* configure TX channel */

    if (pDev->isMaster)
        {
        value = (1 << SSI_STCR_TSCKP) | (1 << SSI_STCR_TEFS) |
                (1 << SSI_STCR_TFSI) | (1 << SSI_STCR_TXBIT0) |
                (1 << SSI_STCR_TXDIR) | (1 << SSI_STCR_TFDIR);
        }
    else
        {
        value = (1 << SSI_STCR_TSCKP) | (1 << SSI_STCR_TEFS) |
                (1 << SSI_STCR_TFSI) | (1 << SSI_STCR_TXBIT0);
        }
    SSI_WRITE32 (pDev, SSI_STCR, value);

    /* configure RX channel */

    value = (1 << SSI_SRCR_RSCKP) | (1 << SSI_SRCR_REFS) |
            (1 << SSI_SRCR_RFSI) | (1 << SSI_SRCR_RXBIT0);
    SSI_WRITE32 (pDev, SSI_SRCR, value);

    value = SSI_READ32 (pDev, SSI_SFCSR);
    value &= (~(0xF << SSI_SFCSR_RFWM1)) & (~(0xF << SSI_SFCSR_TFWM1)) &
             (~(0xF << SSI_SFCSR_RFWM0)) & (~(0xF << SSI_SFCSR_TFWM0));
    value |= (SSI_RX_WATERMARK << SSI_SFCSR_RFWM1) |
             (SSI_TX_WATERMARK << SSI_SFCSR_TFWM1) |
             (SSI_RX_WATERMARK << SSI_SFCSR_RFWM0) |
             (SSI_TX_WATERMARK << SSI_SFCSR_TFWM0);
    SSI_WRITE32 (pDev, SSI_SFCSR, value);

    value = (1 << SSI_SIER_RDMAE) | (1 << SSI_SIER_TDMAE);
    SSI_WRITE32 (pDev, SSI_SIER, value);

    if (pDev->isMaster)
        {
        value = (1 << SSI_SCR_CLK_IST) |
                (SSI_SCR_I2S_MODE_MASTER << SSI_SCR_I2S_MODE) |
                (1 << SSI_SCR_SYN);
        }
    else
        {
        value = (1 << SSI_SCR_CLK_IST) |
                (SSI_SCR_I2S_MODE_SLAVE << SSI_SCR_I2S_MODE) |
                (1 << SSI_SCR_SYN);
        }
    SSI_WRITE32 (pDev, SSI_SCR, value);

    return OK;
    }

/*******************************************************************************
*
* ssiClose - set the SSI to default status
*
* This routine sets the SSI to default status.
*
* RETURNS: always OK
*
* ERRNO: N/A
*/

LOCAL int ssiClose
    (
    FSL_SSI_AUD_DATA *  pDev
    )
    {
    ssiReset (pDev);
    return OK;
    }

/*******************************************************************************
*
* ssiRead - read audio data from SSI
*
* This routine reads audio data from SSI.
*
* RETURNS: number of bytes read
*
* ERRNO: N/A
*/

LOCAL int ssiRead
    (
    FSL_SSI_AUD_DATA *  pDev,
    char *              pBuffer,    /* location to store read data */
    int                 maxbytes    /* number of bytes to read */
    )
    {
    int             bytesToRd;
    int             bytesRest = maxbytes;
    VXB_DMA_TX *    pDmaTx;

    while (bytesRest > 0)
        {
        if (bytesRest > SSI_DMA_MAX_LEN)
            {
            bytesToRd = SSI_DMA_MAX_LEN;
            }
        else
            {
            bytesToRd = bytesRest;
            }

        pDmaTx = vxbDmaChanPreMemCpy (pDev->pDmaChanRx,
                                      (VIRT_ADDR)(pDev->ssiAddr + SSI_SRX0),
                                      (VIRT_ADDR)pBuffer, bytesToRd, 0);
        if (NULL == pDmaTx)
            {
            return (maxbytes - bytesRest);
            }

        pDmaTx->fn      = ssiDmaCallback;
        pDmaTx->pArg    = (void *)pDev->dmaRxSem;

        if (ERROR == vxbDmaChanStart (pDmaTx))
            {
            return (maxbytes - bytesRest);
            }

        (void)semTake (pDev->dmaRxSem, WAIT_FOREVER);
        pBuffer    += bytesToRd;
        bytesRest  -= bytesToRd;
        }

    return maxbytes;
    }

/*******************************************************************************
*
* ssiWrite - write audio data to SSI
*
* This routine writes audio data to SSI.
*
* RETURNS: number of bytes written
*
* ERRNO: N/A
*/

LOCAL int ssiWrite
    (
    FSL_SSI_AUD_DATA *  pDev,
    char *              pBuffer,    /* location of data buffer */
    int                 nbytes      /* number of bytes to output */
    )
    {
    int             bytesToWt;
    int             bytesRest = nbytes;
    VXB_DMA_TX *    pDmaTx;

    while (bytesRest > 0)
        {
        if (bytesRest > SSI_DMA_MAX_LEN)
            {
            bytesToWt = SSI_DMA_MAX_LEN;
            }
        else
            {
            bytesToWt = bytesRest;
            }

        pDmaTx = vxbDmaChanPreMemCpy (pDev->pDmaChanTx, (VIRT_ADDR)pBuffer,
                                      (VIRT_ADDR)(pDev->ssiAddr + SSI_STX0),
                                      bytesToWt, 0);
        if (NULL == pDmaTx)
            {
            return (nbytes - bytesRest);
            }

        pDmaTx->fn      = ssiDmaCallback;
        pDmaTx->pArg    = (void *)pDev->dmaTxSem;

        if (ERROR == vxbDmaChanStart (pDmaTx))
            {
            return (nbytes - bytesRest);
            }

        (void)semTake (pDev->dmaTxSem, WAIT_FOREVER);

        pBuffer    += bytesToWt;
        bytesRest  -= bytesToWt;
        }

    return nbytes;
    }

/*******************************************************************************
*
* ssiIoctl - handle ioctls for SSI
*
* This routine handles ioctls for SSI.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS ssiIoctl
    (
    FSL_SSI_AUD_DATA *  pDev,
    AUDIO_IO_CTRL       function,   /* function to perform */
    AUDIO_IOCTL_ARG *   pIoctlArg   /* function argument */
    )
    {
    STATUS  status  = OK;

    switch (function)
        {
        case AUDIO_SET_DATA_INFO:
            status = ssiConfig (pDev, pIoctlArg->dataInfo.sampleBits,
                                pIoctlArg->dataInfo.sampleRate);
            break;

        case AUDIO_SET_PATH:
            pDev->curPaths = pIoctlArg->path;
            break;

        case AUDIO_DEV_ENABLE:
            if (0 != (pDev->curPaths & (UINT32)AUDIO_OUT_MASK))
                {
                ssiTxEn (pDev, SSI_TX_FIFO_ID, TRUE);
                }

            if (0 != (pDev->curPaths & (UINT32)AUDIO_IN_MASK))
                {
                ssiRxEn (pDev, SSI_RX_FIFO_ID, TRUE);
                }
            break;

        case AUDIO_DEV_DISABLE:
            if (0 != (pDev->curPaths & (UINT32)AUDIO_OUT_MASK))
                {
                ssiTxEn (pDev, SSI_TX_FIFO_ID, FALSE);
                }

            if (0 != (pDev->curPaths & (UINT32)AUDIO_IN_MASK))
                {
                ssiRxEn (pDev, SSI_RX_FIFO_ID, FALSE);
                }
            break;

        default:
            break;
        }

    return status;
    }

/*******************************************************************************
*
* ssiReset - reset SSI
*
* This routine resets SSI.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void ssiReset
    (
    FSL_SSI_AUD_DATA *  pDev
    )
    {
    UINT32  value;

    value = SSI_READ32 (pDev, SSI_SCR);
    value &= ~(1 << SSI_SCR_SSIEN);
    SSI_WRITE32 (pDev, SSI_SCR, value);

    SSI_WRITE32 (pDev, SSI_SCR, SSI_SCR_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SISR, SSI_SISR_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SIER, SSI_SIER_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_STCR, SSI_STCR_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SRCR, SSI_SRCR_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_STCCR, SSI_STCCR_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SRCCR, SSI_SRCCR_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SFCSR, SSI_SFCSR_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SACNT, SSI_SACNT_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SACADD, SSI_SACADD_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SACDAT, SSI_SACDAT_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SATAG, SSI_SATAG_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_STMSK, SSI_STMSK_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SRMSK, SSI_SRMSK_RESET_VALUE);    
    SSI_WRITE32 (pDev, SSI_SACCEN, SSI_SACCEN_RESET_VALUE);
    SSI_WRITE32 (pDev, SSI_SACCDIS, SSI_SACCDIS_RESET_VALUE);
    }

/*******************************************************************************
*
* ssiConfig - configure the SSI module
*
* This routine configures the SSI module.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS ssiConfig
    (
    FSL_SSI_AUD_DATA *  pDev,
    UINT32              sampleBits,
    UINT32              sampleRate
    )
    {
    UINT32  value;
    UINT32  pmValue = 0x100;
    UINT8   div2    = 0;
    UINT8   psr     = 0;
    UINT32  divider;
    UINT32  clkRate;

    switch (sampleBits)
        {
        case 16:
            pDev->audioDev.devInfo.bytesPerSample   = sizeof (UINT16);
            break;

        case 24:
            pDev->audioDev.devInfo.bytesPerSample   = sizeof (UINT8) * 3;
            break;

        case 32:
            pDev->audioDev.devInfo.bytesPerSample   = sizeof (UINT32);
            break;

        default:
            return ERROR;
        }

    pDev->dmaTxConfig.destAddrWidth = pDev->audioDev.devInfo.bytesPerSample;
    pDev->dmaTxConfig.destMaxBurst  = SSI_TX_WATERMARK *
                                      pDev->audioDev.devInfo.bytesPerSample;
    if (ERROR == vxbDmaChanConfig (pDev->pDmaChanTx, &pDev->dmaTxConfig))
        {
        FSL_IMX_SSI_LOG ("DMA channel for tx configuration failed\n",
                         1, 2, 3, 4, 5, 6);
        return ERROR;
        }

    pDev->dmaRxConfig.srcAddrWidth  = pDev->audioDev.devInfo.bytesPerSample;
    pDev->dmaRxConfig.srcMaxBurst   = SSI_RX_WATERMARK *
                                      pDev->audioDev.devInfo.bytesPerSample;
    if (ERROR == vxbDmaChanConfig (pDev->pDmaChanRx, &pDev->dmaRxConfig))
        {
        FSL_IMX_SSI_LOG ("DMA channel for rx configuration failed\n",
                         1, 2, 3, 4, 5, 6);
        return ERROR;
        }

    if (0 == (sampleRate % 1000))
        {
        clkRate = pDev->ssiClk0;
        }
    else
        {
        clkRate = pDev->ssiClk1;
        }

    if (NULL != pDev->pChildClk)
        {
        while (OK == vxbClkDisable (pDev->pChildClk))
            {
            continue;
            }
        }

    while (OK == vxbClkDisable (pDev->pSsiClk))
        {
        continue;
        }

    if (ERROR == vxbClkRateSet (pDev->pSsiClk, clkRate))
        {
        return ERROR;
        }

    if (ERROR == vxbClkEnable (pDev->pSsiClk))
        {
        return ERROR;
        }

    if (NULL != pDev->pChildClk)
        {
        if (ERROR == vxbClkEnable (pDev->pChildClk))
            {
            return ERROR;
            }
        }

    while (pmValue > 0xFF)
        {
        divider  = sampleRate * (div2 + 1) * (psr * 7 + 1);
        pmValue  = (clkRate * 10 / divider + 5) / 10;
        pmValue  = (pmValue * 10 / SSI_I2S_FRAME_LEN + 5) / 10;
        pmValue  = (pmValue * 10 / 2 + 5 ) / 10;
        pmValue -= 1;
        if (pmValue > 0xFF)
            {
            if (0 == div2)
                {
                div2 = 1;
                }
            else if (0 == psr)
                {
                psr = 1;
                }
            else
                {
                return ERROR;
                }
            }
        else
            {
            break;
            }
        }

    value = (SSI_WORD_LENGTH (sampleBits) << SSI_STCCR_WL) |
            (div2 << SSI_STCCR_DIV2) | (psr << SSI_STCCR_PSR) |
            (1 << SSI_STCCR_DC) | pmValue;
    SSI_WRITE32 (pDev, SSI_STCCR, value);

    value = (SSI_WORD_LENGTH (sampleBits) << SSI_SRCCR_WL) |
            (div2 << SSI_SRCCR_DIV2) | (psr << SSI_SRCCR_PSR) |
            (1 << SSI_SRCCR_DC) | pmValue;
    SSI_WRITE32 (pDev, SSI_SRCCR, value);

    value = SSI_READ32 (pDev, SSI_SCR);
    value |= (1 << SSI_SCR_SSIEN);
    SSI_WRITE32 (pDev, SSI_SCR, value);
    return OK;
    }

/*******************************************************************************
*
* ssiTxEn - enable or disable SSI TX
*
* This routine enables or disables SSI TX.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void ssiTxEn
    (
    FSL_SSI_AUD_DATA *  pDev,
    UINT8               txId,
    BOOL                isEn
    )
    {
    UINT32  value;

    if (isEn)
        {
        value = SSI_READ32 (pDev, SSI_SCR);
        value |= (1 << SSI_SCR_TE);
        SSI_WRITE32 (pDev, SSI_SCR, value);

        value = SSI_READ32 (pDev, SSI_STCR);
        if (0 == txId)
            {
            value |= (1 << SSI_STCR_TFEN0);
            }
        else
            {
            value |= (1 << SSI_STCR_TFEN1);
            }

        SSI_WRITE32 (pDev, SSI_STCR, value);
        }
    else
        {
        value = SSI_READ32 (pDev, SSI_SCR);
        value &= (~(1 << SSI_SCR_TE));
        SSI_WRITE32 (pDev, SSI_SCR, value);

        value = SSI_READ32 (pDev, SSI_STCR);
        if (0 == txId)
            {
            value &= (~(1 << SSI_STCR_TFEN0));
            }
        else
            {
            value &= (~(1 << SSI_STCR_TFEN1));
            }

        SSI_WRITE32 (pDev, SSI_STCR, value);
        }
    }

/*******************************************************************************
*
* ssiRxEn - enable or disable SSI RX
*
* This routine enables or disables SSI RX.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void ssiRxEn
    (
    FSL_SSI_AUD_DATA *  pDev,
    UINT8               rxId,
    BOOL                isEn
    )
    {
    UINT32  value;

    if (isEn)
        {
        value = SSI_READ32 (pDev, SSI_SCR);
        value |= (1 << SSI_SCR_RE);
        SSI_WRITE32 (pDev, SSI_SCR, value);

        value = SSI_READ32 (pDev, SSI_SRCR);
        if (0 == rxId)
            {
            value |= (1 << SSI_SRCR_RFEN0);
            }
        else
            {
            value |= (1 << SSI_SRCR_RFEN1);
            }

        SSI_WRITE32 (pDev, SSI_SRCR, value);
        }
    else
        {
        value = SSI_READ32 (pDev, SSI_SCR);
        value &= (~(1 << SSI_SCR_RE));
        SSI_WRITE32 (pDev, SSI_SCR, value);

        value = SSI_READ32 (pDev, SSI_SRCR);
        if (0 == rxId)
            {
            value &= (~(1 << SSI_SRCR_RFEN0));
            }
        else
            {
            value &= (~(1 << SSI_SRCR_RFEN1));
            }

        SSI_WRITE32 (pDev, SSI_SRCR, value);
        }
    }

/*******************************************************************************
*
* ssiDmaCallback - callback function for SSI DMA interrupt
*
* This routine is the ccallback function for SSI DMA interrupt.
*
* RETURNS: N/A
*
* ERRNO
*/

LOCAL void ssiDmaCallback
    (
    SEM_ID  dmaSem
    )
    {
    (void)semGive (dmaSem);
    }
