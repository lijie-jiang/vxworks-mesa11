/* evdevLibCore.h - Event Devices Framework Core Library Header */

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
16may14,yat  Add flags to support O_NONBLOCK read I/O. (US24741)
26jun13,y_f  created
*/

#ifndef __INCevdevLibCoreh
#define __INCevdevLibCoreh

#if __cplusplus
extern "C" {
#endif

/* typedefs */

typedef struct evdevDeviceInfo
    {
    UINT16  bustype;
    UINT16  vendor;
    UINT16  product;
    UINT16  version;
    }EV_DEV_DEVICE_INFO;

typedef struct evdevDeviceAxisVal
    {
    UINT32               axisIndex;
    int                  minVal;
    int                  maxVal;
    }EV_DEV_DEVICE_AXIS_VAL;

#ifdef _WRS_KERNEL
typedef struct evdevDeviceData
    {
    UINT32              devCap;
    EV_DEV_DEVICE_INFO  devInfo;
    char *              pDevName;
    FUNCPTR             ioctl;
    void *              pArg;
    } EV_DEV_DEVICE_DATA;

typedef struct evdevHandle
    {
    DEV_HDR                 devHdr;
    int                     drvNum;
    MSG_Q_ID                eventQ;
    SEM_ID                  ioSem;
    SEL_WAKEUP_LIST         selWakeupList;
    UINT32                  devIdx;
    BOOL                    isOpened;
    UINT32                  msgSize;
    EV_DEV_DEVICE_DATA *    pDevData;
    int                     flags;
    }EV_DEV_HANDLE;
#endif /* _WRS_KERNEL */

/* prototypes */

#ifdef _WRS_KERNEL
extern EV_DEV_HANDLE *  evdevRegDev (EV_DEV_DEVICE_DATA * pDevData);
extern STATUS           evdevUnregDev (EV_DEV_HANDLE * pEvdev);
extern int              evdevSendMsg (EV_DEV_HANDLE * pEvdev, char * buffer, int
                                      nBytes);
#endif /* _WRS_KERNEL */

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevLibCoreh */
