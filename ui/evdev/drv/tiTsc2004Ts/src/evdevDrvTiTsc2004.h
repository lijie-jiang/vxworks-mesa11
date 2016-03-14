/* evdevDrvTiTsc2004.h - TI TSC2004 Touch Screen Controller Register Definitions */

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
20jun14,y_f  updated to support VXBUS GEN2 (US42301)
23jul13,y_f  created
*/

/*
DESCRIPTION
This file contains register definitions for TI TSC2004 touch screen controller.
*/

#ifndef __INCevdevDrvTiTsc2004h
#define __INCevdevDrvTiTsc2004h

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define TI_TSC2004_DRIVER_NAME                  "ti,tsc2004"

#define TI_TSC2004_CONTROL1                     (0x80)
#define TI_TSC2004_CONTROL1_RM_VALUE            (0x1 << 2)
#define TI_TSC2004_CONTROL1_XYZ_VALUE           (0x0 << 3)

#define TI_TSC2004_CONTROL0_PNG_VALUE           (0x1 << 1)

#define TI_TSC2004_READ_CFG0                    (0x61)
#define TI_TSC2004_WRITE_CFG0                   (0x60)
#define TI_TSC2004_CF0_PSM_VALUE                (0x1 << 15)
#define TI_TSC2004_CF0_RM_VALUE                 (0x1 << 13)
#define TI_TSC2004_CL01_VALUE                   (0x1 << 11)
#define TI_TSC2004_PV012_VALUE                  (0x4 << 8)
#define TI_TSC2004_PR012_VALUE                  (0x4 << 5)
#define TI_TSC2004_SNS012_VALUE                 (0x2 << 2)
#define TI_TSC2004_DTW_VALUE                    (0x1 << 1)
#define TI_TSC2004_LSM_VALUE                    (0x1 << 0)

#define TI_TSC2004_WRITE_CFG1                   (0x68)
#define TI_TSC2004_TBM0123_VALUE                (0x3 << 8)
#define TI_TSC2004_BTD012_VALUE                 (0x6 << 0)

#define TI_TSC2004_WRITE_CFG2                   (0x70)
#define TI_TSC2004_PINTDAV_VALUE_INT            (0x0 << 14)
#define TI_TSC2004_PINTDAV_VALUE_PIO            (0x3 << 14)
#define TI_TSC2004_M01_VALUE                    (0x1 << 12)
#define TI_TSC2004_W01_VALUE                    (0x1 << 10)
#define TI_TSC2004_XYZ_VALUE                    (0x5 << 2)

#define TI_TSC2004_READ_STATUS                  (0x39)
#define TI_TSC2004_XYZ_STATUS                   (0xF << 4)

#define TI_TSC2004_DATA_X_ADDR                  (0x1)

#define TI_TSC2004_RELEASE_TIMEOUT              (300)   /* ms */
#define TI_TSC2004_SAMPLE_TIMEOUT               (100)   /* ms */
#define TI_TSC2004_DEFAULT_DIS_WIDTH            800
#define TI_TSC2004_DEFAULT_DIS_HEIGHT           480
#define TI_TSC2004_TOUCH_X_MAX                  4095 
#define TI_TSC2004_TOUCH_Y_MAX                  4095

/* typedefs */

/* structure to store the touch screen module information */

typedef struct tiTsc2004Data
    {
    VXB_DEV_ID          pInst;
    UINT8               devAddr;   /* base address of touch screen module */
    TS_DEVICE_INFO *    pDevInfo;
    TS_CALIB_DATA *     pCalData;
    TS_POINT            point;
    UINT32              disWidth;
    UINT32              disHeight;
    int                 axisXMax;
    int                 axisYMax;
    }TI_TSC2004_DATA;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevDrvTiTsc2004h */
