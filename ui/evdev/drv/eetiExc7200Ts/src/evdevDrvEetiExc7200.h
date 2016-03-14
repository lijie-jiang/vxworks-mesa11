/* evdevDrvEetiExc7200.h - EETI EXC7200 Multi-touch Controller Register Definitions */

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
01dec14,y_f  created (US50218)
*/

/*
DESCRIPTION
This file contains register definitions for EETI EXC7200 multi-touch controller.
*/

#ifndef __INCevdevDrvEetiExc7200h
#define __INCevdevDrvEetiExc7200h

#ifdef __cplusplus
extern "C" {
#endif

/* defines */

#define EXC7200_DRIVER_NAME                     "eeti,exc7200"

#define EXC7200_TOUCH_STATUS                    0x01
#define EXC7200_TOUCH_XL                        0x02
#define EXC7200_TOUCH_XH                        0x03
#define EXC7200_TOUCH_YL                        0x04
#define EXC7200_TOUCH_YH                        0x05

#define EXC7200_TOUCH_ID_OFFSET                 0x2
#define EXC7200_TOUCH_ID_MASK                   0x1F

#define EXC7200_TOUCH_DOWN_OFFSET               0x0
#define EXC7200_TOUCH_DOWN_MASK                 0x1

#define EXC7200_TOUCH_XL_OFFSET                 0x4
#define EXC7200_TOUCH_XL_MASK                   0xF

#define EXC7200_TOUCH_XH_OFFSET                 0x4

#define EXC7200_TOUCH_YL_OFFSET                 0x4
#define EXC7200_TOUCH_YL_MASK                   0xF

#define EXC7200_TOUCH_YH_OFFSET                 0x4

#define EXC7200_TOUCH_REPORT_ID                 0x4
#define EXC7200_TOUCH_REG_LEN                   0xA
#define EXC7200_TOUCH_DEFAULT_FINGERS           0x2
#define EXC7200_SAMPLE_PERIOD                   20      /* ms */
#define EXC7200_TOUCH_X_MAX                     2047
#define EXC7200_TOUCH_Y_MAX                     2047

/* typedefs */

/* structure to store the touch screen module information */

typedef struct exc7200Data
    {
    VXB_DEV_ID          pInst;
    UINT8               devAddr;   /* base address of touch screen module */
    TS_DEVICE_INFO *    pDevInfo;
    TS_CALIB_DATA *     pCalData;
    UINT32              intPin;
    BOOL                intMode;
    EV_DEV_PTR_MT_DATA  lastMtData;
    UINT8               fingers;
    UINT32              disWidth;
    UINT32              disHeight;
    int                 axisXMax;
    int                 axisYMax;
    }EXC7200_DATA;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevDrvEetiExc7200h */
