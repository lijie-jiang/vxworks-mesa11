/* audioDrvFslSsi.h - Freescale i.MX series SSI functions header file */

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
08feb14,y_f  written
*/

#ifndef __INCaudioDrvFslSsih
#define __INCaudioDrvFslSsih

#if __cplusplus
extern "C" {
#endif

/* defines */

#define FSL_SSI_AUD_DRIVER_NAME             "fsl,imx-ssi"

#define SSI_STX0                            (0x00)
#define SSI_SRX0                            (0x08)
#define SSI_SCR                             (0x10)
#define SSI_SISR                            (0x14)
#define SSI_SIER                            (0x18)
#define SSI_STCR                            (0x1C)
#define SSI_SRCR                            (0x20)
#define SSI_STCCR                           (0x24)
#define SSI_SRCCR                           (0x28)
#define SSI_SFCSR                           (0x2C)
#define SSI_SACNT                           (0x38)
#define SSI_SACADD                          (0x3C)
#define SSI_SACDAT                          (0x40)
#define SSI_SATAG                           (0x44)
#define SSI_STMSK                           (0x48)
#define SSI_SRMSK                           (0x4C)
#define SSI_SACCST                          (0x50)
#define SSI_SACCEN                          (0x54)
#define SSI_SACCDIS                         (0x58)

/* SSI control register */

#define SSI_SCR_CLK_IST                     9
#define SSI_SCR_I2S_MODE                    5
#define SSI_SCR_SYN                         4
#define SSI_SCR_RE                          2
#define SSI_SCR_TE                          1
#define SSI_SCR_SSIEN                       0

/* SSI interrupt enable register */

#define SSI_SIER_RDMAE                      22
#define SSI_SIER_TDMAE                      20

/* SSI transmit configuration register */

#define SSI_STCR_TXBIT0                     9
#define SSI_STCR_TFEN1                      8
#define SSI_STCR_TFEN0                      7
#define SSI_STCR_TFDIR                      6
#define SSI_STCR_TXDIR                      5
#define SSI_STCR_TSCKP                      3
#define SSI_STCR_TFSI                       2
#define SSI_STCR_TEFS                       0

/* SSI receive configuration register */

#define SSI_SRCR_RXEXT                      10
#define SSI_SRCR_RXBIT0                     9
#define SSI_SRCR_RFEN1                      8
#define SSI_SRCR_RFEN0                      7
#define SSI_SRCR_RSCKP                      3
#define SSI_SRCR_RFSI                       2
#define SSI_SRCR_REFS                       0

/* SSI transmit clock control register */

#define SSI_STCCR_DIV2                      18
#define SSI_STCCR_PSR                       17
#define SSI_STCCR_WL                        13
#define SSI_STCCR_DC                        8

/* SSI receive clock control register */

#define SSI_SRCCR_DIV2                      18
#define SSI_SRCCR_PSR                       17
#define SSI_SRCCR_WL                        13
#define SSI_SRCCR_DC                        8

/* SSI FIFO control/status register */

#define SSI_SFCSR_RFWM1                     20
#define SSI_SFCSR_TFWM1                     16
#define SSI_SFCSR_RFWM0                     4
#define SSI_SFCSR_TFWM0                     0

/* AUDMUX port timing control registers */

#define AUDMUX_PTCR_TFS_DIR_OFFSET          31
#define AUDMUX_PTCR_TFSEL_OFFSET            27
#define AUDMUX_PTCR_TCLKDIR_OFFSET          26
#define AUDMUX_PTCR_TCSEL_OFFSET            22
#define AUDMUX_PTCR_SYN_OFFSET              11

/* AUDMUX port data control registers */

#define AUDMUX_PDCR_RXDSEL_OFFSET           13

#define SSI_SCR_RESET_VALUE                 (0x00000000)
#define SSI_SISR_RESET_VALUE                (0x00003003)
#define SSI_SIER_RESET_VALUE                (0x00003003)
#define SSI_STCR_RESET_VALUE                (0x00000200)
#define SSI_SRCR_RESET_VALUE                (0x00000200)
#define SSI_STCCR_RESET_VALUE               (0x00040000)
#define SSI_SRCCR_RESET_VALUE               (0x00040000)
#define SSI_SFCSR_RESET_VALUE               (0x00810081)
#define SSI_SACNT_RESET_VALUE               (0x00000000)
#define SSI_SACADD_RESET_VALUE              (0x00000000)
#define SSI_SACDAT_RESET_VALUE              (0x00000000)
#define SSI_SATAG_RESET_VALUE               (0x00000000)
#define SSI_STMSK_RESET_VALUE               (0x00000000)
#define SSI_SRMSK_RESET_VALUE               (0x00000000)
#define SSI_SACCST_RESET_VALUE              (0x00000000)
#define SSI_SACCEN_RESET_VALUE              (0x00000000)
#define SSI_SACCDIS_RESET_VALUE             (0x00000000)

#define SSI_SCR_I2S_MODE_MASTER             (0x1)
#define SSI_SCR_I2S_MODE_SLAVE              (0x2)

#define SSI_WORD_LENGTH(bits)               ((bits) / 2 - 1)

#define SSI_TX_WATERMARK                    (0x2)
#define SSI_RX_WATERMARK                    (0x2)

#define SSI_I2S_FRAME_LEN                   (32 * 2)             /* WL * 2 channels*/
#define SSI_DMA_MAX_LEN                     (0xFFFF & (~(0x3))) /* 32bit align */

#define SSI_TX_FIFO_ID                      0
#define SSI_RX_FIFO_ID                      0

#define SSI_READ32(pCtrl,offset)                    \
    vxbRead32 (pCtrl->ssiHandle, (UINT32 *)(pCtrl->ssiAddr + offset))

#define SSI_WRITE32(pCtrl,offset,data)              \
    vxbWrite32 (pCtrl->ssiHandle, (UINT32 *)(pCtrl->ssiAddr + offset), data)

#define AUDMUX_WRITE32(pCtrl,offset,data)           \
    vxbWrite32 (pCtrl->audMuxHandle,                \
                (UINT32 *)(pCtrl->audMuxAddr + offset), data)

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCaudioDrvFslSsih */
