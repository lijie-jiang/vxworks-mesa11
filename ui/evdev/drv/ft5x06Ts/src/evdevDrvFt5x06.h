/* evdevDrvFt5x06.h - FT5X06 Multi-touch Controller Register Definitions */

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
14sep15,jnl  support querying size of the touchscreen area. (V7GFX-238)
01aug14,y_f  created (US41404)
*/

/*
DESCRIPTION
This file contains register definitions for FT5X06 multi-touch controller.
*/

#ifndef __INCevdevDrvFt5x06h
#define __INCevdevDrvFt5x06h

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define FT5X06_DRIVER_NAME                      "ft,5x06"

#define FT5X06_TOUCH_POINTS                     0x02
#define FT5X06_TOUCH1_XH                        0x03
#define FT5X06_TOUCH1_XL                        0x04
#define FT5X06_TOUCH1_YH                        0x05
#define FT5X06_TOUCH1_YL                        0x06

#define FT5X06_TOUCH_REG_LEN                    0x1F
#define FT5X06_TOUCH_POINTS_MAX                 5
#define FT5X06_SAMPLE_PERIOD                    100     /* ms */
#define FT5X06_INIT_TASK_PRIO                   200
#define FT5X06_INIT_TASK_STACK_SIZE             (1024 * 8)

#define FT5X06_TOUCH_X_MAX                      1023
#define FT5X06_TOUCH_Y_MAX                      767

/* typedefs */

/* structure to store the touch screen module information */

typedef struct ft5x06Data
    {
    VXB_DEV_ID          pInst;
    UINT8               devAddr;   /* base address of touch screen module */
    TS_DEVICE_INFO *    pDevInfo;
    TS_CALIB_DATA *     pCalData;
    UINT32              intPin;
    BOOL                intMode;
    BOOL                xySwap;
    UINT32              disWidth;
    UINT32              disHeight;
    int                 axisXMax;
    int                 axisYMax;
    }FT5X06_DATA;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevDrvFt5x06h */
