/* evdevDrvFslCRTouch.h - Freescale CRTouch Screen Controller Register Definitions */

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
18Mar15,c_l create. (US55213)
*/

/*
DESCRIPTION
This file contains register definitions for Freescale CRTouch screen controller.
*/

#ifndef __INCevdevDrvFslCRTouchh
#define __INCevdevDrvFslCRTouchh

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define FSL_CRTOUCH_DRIVER_NAME           "fsl,crtouch"

/* resistive touch registers */

#define FSL_CRTOUCH_RES_STATUS1           (0x01)/* status register1 */
#define FSL_CRTOUCH_RES_STATUS2           (0x02)/* status register2 */
#define FSL_CRTOUCH_RES_X_MSB             (0x03)/* X coordinate MSB */
#define FSL_CRTOUCH_RES_X_LSB             (0x04)/* X coordinate LSB */
#define FSL_CRTOUCH_RES_Y_MSB             (0x05)/* Y coordinate MSB */
#define FSL_CRTOUCH_RES_Y_LSB             (0x06)/* Y coordinate LSB */

/* configuration registers */

#define FSL_CRTOUCH_RES_CFG               (0x40) /* configuration register   */
#define FSL_CRTOUCH_RES_TRIGGER           (0X41) /* Resistive trigger events */
#define FSL_CRTOUCH_RES_FIFO              (0X42) /* Resistive FIFO setup     */
#define FSL_CRTOUCH_RES_SAMPLING          (0X43) /* Sampling rate            */
#define FSL_CRTOUCH_HOR_MSB               (0X4A) /* MSB of display on the x  */
#define FSL_CRTOUCH_HOR_LSB               (0X4B) /* LSB of display on the x  */
#define FSL_CRTOUCH_VER_MSB               (0X4C) /* MSB of display on the y  */
#define FSL_CRTOUCH_VER_LSB               (0X4D) /* LSB of display on the y  */

/* capacitive touch registers */

#define FSL_CRTOUCH_CAP_CFG               (0x60) /* Capactitive configuration*/

#define FSL_CRTOUCH_RTSRDY                (0x1 << 0) /* sample pending    */
#define FSL_CRTOUCH_RTST                  (0X1 << 7) /* currently touched */

#define FSL_CRTOUCH_RTEV                  (0x1 << 0) /* resistive event   */
#define FSL_CRTOUCH_RTREL                 (0X1 << 7) /* touch release     */
#define FSL_CRTOUCH_RES_DEFAULT_SAMPLING  (10)       /* 5 ~ 100           */

#define FSL_CRTOUCH_RELEASE_TIMEOUT       (300)   /* ms */
#define FSL_CRTOUCH_SAMPLE_TIMEOUT        (100)   /* ms */
#define FSL_CRTOUCH_DEFAULT_DIS_WIDTH     (480)
#define FSL_CRTOUCH_DEFAULT_DIS_HEIGHT    (272)
#define FSL_CRTOUCH_X_MAX                 (4095)
#define FSL_CRTOUCH_Y_MAX                 (4095)

/* typedefs */

/* structure to store the touch screen module information */

typedef struct fslCRTouchData
    {
    VXB_DEV_ID          pInst;
    UINT8               devAddr;   /* base address of touch screen module */
    TS_DEVICE_INFO *    pDevInfo;
    TS_CALIB_DATA  *    pCalData;
    UINT32              intPin;
    BOOL                intMode;
    UINT32              disWidth;
    UINT32              disHeight;
    int                 axisXMax;
    int                 axisYMax;
    }FSL_CRTOUCH_DATA;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevDrvFslCRTouchh */
