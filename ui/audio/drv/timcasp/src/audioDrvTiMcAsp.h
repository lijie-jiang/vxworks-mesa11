/* audioDrvTiMcAsp.h - TI McASP audio driver functions header file */

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
21mar14,y_f  written
*/

#ifndef __INCaudioDrvTiMcAsph
#define __INCaudioDrvTiMcAsph

#if __cplusplus
extern "C" {
#endif

/* defines */

#define TI_MCASP_AUD_DRIVER_NAME        "ti,mcasp"

/* defines */

#define AM335X_MCASP1_DMA_TX_CH         (10)
#define AM335X_MCASP1_DMA_RX_CH         (11)

/* MCASP control registers */

#define AM335X_MCASP_PWREMUMGT_REG      (0x0004)
#define AM335X_MCASP_PFUNC_REG          (0x0010)
#define AM335X_MCASP_PDIR_REG           (0x0014)
#define AM335X_MCASP_GBLCTL             (0x0044)
#define AM335X_MCASP_RGBLCTL            (0x0060)
#define AM335X_MCASP_RMASK_REG          (0x0064)
#define AM335X_MCASP_RFMT_REG           (0x0068)
#define AM335X_MCASP_AFSRCTL_REG        (0x006C)
#define AM335X_MCASP_ACLKRCTL_REG       (0x0070)
#define AM335X_MCASP_RTDM_REG           (0x0078)
#define AM335X_MCASP_RINTCTL_REG        (0x007C)
#define AM335X_MCASP_RSTAT_REG          (0x0080)
#define AM335X_MCASP_XGBLCTL            (0x00A0)
#define AM335X_MCASP_XMASK_REG          (0x00A4)
#define AM335X_MCASP_XFMT_REG           (0x00A8)
#define AM335X_MCASP_AFSXCTL_REG        (0x00AC)
#define AM335X_MCASP_ACLKXCTL_REG       (0x00B0)
#define AM335X_MCASP_XTDM_REG           (0x00B8)
#define AM335X_MCASP_XINTCTL_REG        (0x00BC)
#define AM335X_MCASP_XSTAT_REG          (0x00C0)
#define AM335X_MCASP_SRCTL0             (0x0180)

#define AM335X_MCASP_XBUF               (0x0200)

#define AM335X_MCASP_WFIFOCTL           (0x1000)
#define AM335X_MCASP_WFIFOSTS           (0x1004)
#define AM335X_MCASP_RFIFOCTL           (0x1008)
#define AM335X_MCASP_RFIFOSTS           (0x100C)

/* PWREGMUMGT register macro */

#define MCASP_SOFT                      (0x0)

/* PDIR register macro */

#define AFSR_OUT                        (0x1 << 31)
#define AHCLKR_OUT                      (0x1 << 30)
#define ACLKR_OUT                       (0x1 << 29)
#define AFSX_OUT                        (0x1 << 28)
#define AHCLKX_OUT                      (0x1 << 27)
#define ACLKX_OUT                       (0x1 << 26)

/* RFMT register macro */

#define RROT_0                          (0x0)
#define RROT_8                          (0x2)
#define RROT_16                         (0x4)
#define RROT_24                         (0x6)
#define RSSZ_16                         (0x7 << 4)
#define RSSZ_24                         (0xb << 4)
#define RSSZ_32                         (0xf << 4)
#define RRVRS                           (0x1 << 15)
#define RDATDLY(n)                      (n << 16)

/* AFSRCTL register macro */

#define RMOD(n)                         (n << 7)
#define FRWID                           (0x1 << 4)
#define FSRP                            (0x1)

/* AM335X_MCASP_RINTCTL_REG macro */

#define RX_ROVRN_INT                    (1 << 0)

/* AFSXCTL register macro */

#define XMOD(n)                         (n << 7)
#define FXWID                           (0x1 << 4)
#define FSXP                            (0x1)

/* XFMT register macro */

#define XROT_0                          (0x0)
#define XROT_8                          (0x2)
#define XROT_16                         (0x4)
#define XROT_24                         (0x6)
#define XSSZ_16                         (0x7 << 4)
#define XSSZ_24                         (0xb << 4)
#define XSSZ_32                         (0xf << 4)
#define XRVRS                           (0x1 << 15)
#define XDATDLY(n)                      (n << 16)

/* ACLKRCTL register macro */

#define CLKRP_RISING_RX                 (0x1 << 7)
#define CLKRM_INTERNAL                  (0x1 << 5)

/* ACLKXCTL register macro */

#define CLKRP_FALLING_TX                (0x1 << 7)
#define ACLKXCTL_ASYNC                  (0x1 << 6)
#define CLKXM_INTERNAL                  (0x1 << 5)

/* SRCTL register macro */

#define SRMOD_INACTIVE                  (0x0)
#define SRMOD_TRANSMITTER               (0x1)
#define SRMOD_RECEIVER                  (0x2)

/* FIFO register macro */

#define NUMDMA(n)                       (n)
#define NUMEVT(n)                       (n << 8)
#define ENA                             (0x1 << 16)

/* GBLCTL register macro */

#define RCLKRST                         (0x1 << 0)
#define RHCLKRST                        (0x1 << 1)
#define RXSER                           (0x1 << 2)
#define RXSM                            (0x1 << 3)
#define RFRST                           (0x1 << 4)
#define XCLKRST                         (0x1 << 8)
#define XHCLKRST                        (0x1 << 9)
#define TXSER                           (0x1 << 10)
#define TXSM                            (0x1 << 11)
#define XFRST                           (0x1 << 12)

/* AM335X_MCASP_RSTAT_REG macro */

#define TX_RSTAT_ROVRN                  (1 << 0)

/* AM335X_MCASP_XSTAT_REG macro */

#define TX_XSTAT_XDATA                  (1 << 5)
#define TX_XSTAT_UNDERRUN               (1 << 0)

/* AM335X_MCASP_XINTCTL_REG macro */

#define TX_XUNDRN_INT                   (1 << 0)

#define MCASP_BLK_SIZE                  (0x40)
#define MCASP_DMA_FRAME_MAX             (0xFFFF)
#define MCASP_DMA_SIZE_MAX              (MCASP_BLK_SIZE *       \
                                         MCASP_DMA_FRAME_MAX *  \
                                         pDev->audioDev.devInfo.bytesPerSample)

#define MCASP_READ32(pCtrl,offset)                    \
    vxbRead32 (pCtrl->mcaspHandle, (UINT32 *)(pCtrl->mcaspAddr + offset))

#define MCASP_WRITE32(pCtrl,offset,data)              \
    vxbWrite32 (pCtrl->mcaspHandle, (UINT32 *)(pCtrl->mcaspAddr + offset), data)

#define MCASP_DATA_WRITE32(pCtrl,data)                  \
    vxbWrite32 (pCtrl->dataHandle, (UINT32 *)(pCtrl->dataAddr), data)

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCaudioDrvTiMcAsph */
