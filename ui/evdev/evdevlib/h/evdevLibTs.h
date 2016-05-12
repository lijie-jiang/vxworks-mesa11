/* evdevLibTs.h - Touch Screen Library Header */

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
21dec15,jnl  fixed null pointer defect. (V7GFX-302)
14sep15,jnl  support querying size of the touchscreen area. (V7GFX-238)
24jun14,y_f  added multitouch support (US41403)
04jul13,y_f  created from udIoTsPtrDrv.h version 01c
*/

#ifndef __INCevdevLibTsh
#define __INCevdevLibTsh

#if __cplusplus
extern "C" {
#endif

/* defines */

#define TS_DEV_NAME_LEN                     32
#define TS_DEV_MULTITOUCH_MAX               20

#define TS_DEV_ATTR_INTERRUPT               (1 << 0)
#define TS_DEV_ATTR_POLLING_PRESS           (1 << 1)
#define TS_DEV_ATTR_POLLING_SAMPLE          (1 << 2)
#define TS_DEV_ATTR_POLLING_RELEASE         (1 << 3)
#define TS_DEV_ATTR_SEND_MSG_SELF           (1 << 4)
#define TS_DEV_ATTR_MULTITOUCH              (1 << 5)

#define TS_CALIBDATA_MAX_SIZE               9

#define TS_CALIB_STOPPED                    (0)
#define TS_CALIB_READY                      (1)
#define TS_CALIB_BUSY                       (2)

#define TS_CALIB_STATUS_NA                  (0)
#define TS_CALIB_STATUS_REQUIRED            (1)
#define TS_CALIB_STATUS_COMPLETED           (2)

#define TS_AXIS_NUM                         (2)

#define TS_DET_NUM                          (7)

#define TS_INIT_TASK_PRIO                   200
#define TS_INIT_TASK_STACK_SIZE             (1024 * 8)

/* typedefs */

typedef struct tsPoint
    {
    int     id;      /* finger id for multi-touch */
    BOOL    pressed; /* TRUE = pressed, FALSE = released */
    int     x;       /* uncalibrated x position */
    int     y;       /* uncalibrated y position */
    } TS_POINT;

typedef struct evdevPtrMtData
    {
    struct timeval  time;
    UINT32          count;
    TS_POINT        points[TS_DEV_MULTITOUCH_MAX];
    } EV_DEV_PTR_MT_DATA;

#ifdef _WRS_KERNEL

typedef enum tsIoCtrl
    {
    TS_CALIBDATA_SAVE = (1 << 8),
    TS_CALIBDATA_READ,
    TS_CHECK_INT
    } TS_IO_CTRL;

typedef struct tsCalibData
    {
    char            devName[TS_DEV_NAME_LEN];
    EV_DEV_RECT     disRect;
    UINT8           calibPtCount;
    EV_DEV_POINT    disPoint[TS_CALIBDATA_MAX_SIZE];
    EV_DEV_POINT    tsPoint[TS_CALIBDATA_MAX_SIZE];
    } TS_CALIB_DATA;

typedef struct tsIoFunc
    {
    FUNCPTR open;
    FUNCPTR close;
    FUNCPTR read;
    FUNCPTR ioctl;
    } TS_IO_FUNC;

typedef struct tsDevice
    {
    void *                 pInst;
    UINT32                 devAttr;
    UINT32                 samplePeriod;
    UINT32                 releasePeriod;
    TS_CALIB_DATA *        pCalData;
    UINT8                  calDataCount;
    EV_DEV_DEVICE_DATA     devData;            /* event device info */
    EV_DEV_RECT            defDisRect;         /* default display area */
    TS_IO_FUNC             func;
    EV_DEV_DEVICE_AXIS_VAL devAxisVal[TS_AXIS_NUM];
    UINT32                 devIdx;
    } TS_DEVICE;

typedef struct tsCalibCtrl
    {
    TS_CALIB_DATA   nvData;
    double          det[TS_DET_NUM];      /* det value of calibration data */
    EV_DEV_POINT    requestPoint;
    UINT8           calibProcessStatus;
    UINT8           calibStatus;
    EV_DEV_RECT     currDisplayRect;
    } TS_CALIB_CTRL;

typedef struct tsDeviceInfo
    {
    TS_DEVICE           tsDev;
    WDOG_ID             sampleTimer;
    WDOG_ID             releaseTimer;
    TASK_ID             submitTaskId;
    MSG_Q_ID            submitQueue;
    TS_CALIB_CTRL       tsCalibCtrl;
    TS_POINT            lastPoint;
    EV_DEV_PTR_MT_DATA  lastMtData;
    EV_DEV_HANDLE *     pEvdev;             /* evdev handle */
    BOOL                isEn;
    } TS_DEVICE_INFO;
#endif /* _WRS_KERNEL */

/* function declarations */

#ifdef _WRS_KERNEL
extern TS_DEVICE_INFO * evdevTsReg (TS_DEVICE * pTsDev);
extern STATUS           evdevTsUnreg (TS_DEVICE_INFO * pTsDevInfo);
extern STATUS           evdevTsSendMsg (TS_DEVICE_INFO * pDev, TS_POINT *
                                        pPoint);
extern STATUS           evdevTsSendMtMsg (TS_DEVICE_INFO * pDev,
                                          EV_DEV_PTR_MT_DATA * pMtData);
extern BOOL             evdevTsEvalPoint (INT16 * data, UINT32 maxDiff);
extern void             evdevTsDevIsr (TS_DEVICE_INFO * pDev);
extern void             evdevTsGetCalibQuick (int tsLeft, int tsTop, int
                                              tsRight, int tsBottom, UINT32
                                              disWidth, UINT32 disHeight,
                                              TS_CALIB_DATA * pCalibData);
#endif /* _WRS_KERNEL */

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevLibTsh */
