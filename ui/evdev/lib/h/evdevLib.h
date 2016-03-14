/* evdevLib.h - Event Devices Framework Library Header */

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
24jun14,y_f  added multitouch support (US41403)
20dec13,y_f  created
*/

#ifndef __INCevdevLibh
#define __INCevdevLibh

/* includes */

#include <vxWorks.h>
#include <vsbConfig.h>
#include <ioLib.h>
#include <errnoLib.h>
#include <semLib.h>
#include <stdio.h>
#include <sysLib.h>
#include <msgQLib.h>
#include <taskLib.h>
#include <selectLib.h>
#include <tickLib.h>
#include <sys/time.h>
#include <string.h>
#ifndef _WRS_KERNEL
#include <strings.h>
#endif
#include <evdevLibCore.h>
#include <evdevLibKbd.h>
#include <evdevLibKbdMap.h>
#include <evdevLibPtr.h>
#include <evdevLibTs.h>

#if __cplusplus
extern "C" {
#endif

/* defines */

#define EV_DEV_NAME                 "/input/event"
#define EV_DEV_NAME_PREFIX          EV_DEV_NAME
#define EV_DEV_NAME_LEN             32
#define EV_DEV_DEVICE_MAX           32

#define EV_DEV_VERSION              0x1100

#define EV_DEV_TIMEOUT              (10 * sysClkRateGet ())  /* 10 sec */

/* minimum message number */

#define EV_DEV_MIN_MSG_NUM          8

/* typedefs */

typedef enum evdevBusType
    {
    EV_DEV_BUS_VIRTUAL  = 0,
    EV_DEV_BUS_HOST,
    EV_DEV_BUS_USB,
    EV_DEV_BUS_I2C,
    EV_DEV_BUS_SPI
    } EV_DEV_BUS_TYPE;

typedef enum evdevIo
    {
    EV_DEV_IO_DEV_EN = (1 << 16),   /* enable an input device */
    EV_DEV_IO_DEV_DIS,              /* disable an input device */
    EV_DEV_IO_GET_DEV_COUNT,        /* get registered device count */
    EV_DEV_IO_GET_VERSION,          /* get evdev version */
    EV_DEV_IO_GET_INFO,             /* get device information */
    EV_DEV_IO_GET_NAME,             /* get device name */
    EV_DEV_IO_GET_CAP,              /* get device capabilities */
    EV_DEV_IO_GET_KBD_MODE,         /* get keyboard mapping mode */
    EV_DEV_IO_SET_KBD_MODE,         /* set keyboard mapping mode */
    EV_DEV_IO_SET_COUNTRY_CODE,     /* set country code */
    EV_DEV_IO_SET_LED,              /* set LED */
    EV_DEV_IO_SET_DIS_RECT,         /* set display rectangle */
    EV_DEV_IO_SET_OPERATE_DEV,      /* set id of operating device */
    EV_DEV_IO_GET_CALIB_STATUS,     /* read calibration process status */
    EV_DEV_IO_SET_CALIB_DIS_POINT,  /* set pointer calibration position */
    EV_DEV_IO_START_CALIB,          /* start calibration process */
    EV_DEV_IO_STOP_CALIB,           /* stop calibration process */
    EV_DEV_IO_GET_AXIS_VAL          /* get min and max axis val */
    } EV_DEV_IO;

typedef enum evdevEventType
    {
    EV_DEV_SYN    = (1 << 0),  /* sync event, indicates sending report finished */
    EV_DEV_KEY    = (1 << 1),  /* key event */
    EV_DEV_REL    = (1 << 2),  /* relative position event */
    EV_DEV_ABS    = (1 << 3),  /* absolute position event */
    EV_DEV_ABS_MT = (1 << 4)   /* multitouch absolute position event */
    } EV_DEV_EVENT_TYPE;

typedef enum evdevMsgType
    {
    EV_DEV_MSG_PTR,         /* pointer device message */
    EV_DEV_MSG_KBD,         /* keyboard device message */
    EV_DEV_MSG_HOT_PLUG,    /* hot plug message */
    EV_DEV_MSG_MT           /* multitouch message */
    } EV_DEV_MSG_TYPE;

typedef struct evdevEvent
    {
    struct timeval  time;
    UINT16          type;
    UINT16          code;
    INT32           value;
    }EV_DEV_EVENT;

typedef struct evdevHotPlugData
    {
    struct timeval  time;
    BOOL            isPlugin;
    UINT32          devIdx;
    } EV_DEV_HOT_PLUG_DATA;

typedef union evdevMsgData
    {
    EV_DEV_EVENT            event;
    EV_DEV_PTR_DATA         ptrData;
    EV_DEV_PTR_MT_DATA      mtData;
    EV_DEV_KBD_DATA         kbdData;
    EV_DEV_HOT_PLUG_DATA    hotPlug;
    } EV_DEV_MSG_DATA;

typedef struct evdevMsg
    {
    EV_DEV_MSG_TYPE msgType;
    EV_DEV_MSG_DATA msgData;
    } EV_DEV_MSG;

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevLibh */
