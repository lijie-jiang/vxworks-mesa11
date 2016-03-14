/* evdevLibPtr.h - Pointer Library Header */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
24jun14,y_f  add multitouch support (US41403)
25sep13,j_x  add wheel support
04jul13,y_f  create
*/

#ifndef __INCevdevLibPtrh
#define __INCevdevLibPtrh

#if __cplusplus
extern "C" {
#endif

/* defines */

/* pointer library class value */

#define EV_DEV_PTR_REL_X                0x00
#define EV_DEV_PTR_REL_Y                0x01
#define EV_DEV_PTR_REL_WHEEL            0x08

#define EV_DEV_PTR_ABS_X                0x00
#define EV_DEV_PTR_ABS_Y                0x01

#define EV_DEV_PTR_ABS_MT_SLOT          0x2F
#define EV_DEV_PTR_ABS_MT_POSITION_X    0x35
#define EV_DEV_PTR_ABS_MT_POSITION_Y    0x36
#define EV_DEV_PTR_ABS_MT_TRACKING_ID   0x39

#define EV_DEV_PTR_BTN_LEFT             0x110
#define EV_DEV_PTR_BTN_RIGHT            0x111
#define EV_DEV_PTR_BTN_MIDDLE           0x112
#define EV_DEV_PTR_BTN_TOUCH            0x14A

#define EV_DEV_PTR_BTN_LEFT_BIT         (0x1 << 0)
#define EV_DEV_PTR_BTN_RIGHT_BIT        (0x1 << 1)
#define EV_DEV_PTR_BTN_MIDDLE_BIT       (0x1 << 2)

/* typedefs */

typedef struct evdevRect
    {
    int left;
    int top;
    int right;
    int bottom;
    } EV_DEV_RECT;

typedef struct evdevPoint
    {
    int x;
    int y;
    } EV_DEV_POINT;

typedef struct evdevPtrData
    {
    struct timeval  time;
    UINT16          type;           /* pointer type */
    UINT8           id;             /* pointer position id */
    UINT32          buttonState;    /* pointer button states */
    UINT32          buttonChange;   /* change in pointer button states */
    EV_DEV_POINT    position;       /* position of pointer */
    INT8            wheel;          /* wheel value */
    } EV_DEV_PTR_DATA;

/* prototypes */

#ifdef _WRS_KERNEL
extern EV_DEV_HANDLE *  evdevPtrReg (EV_DEV_DEVICE_DATA * pDevData);
extern STATUS           evdevPtrUnreg (EV_DEV_HANDLE * pEvdevHandle);
extern STATUS           evdevPtrSendMsg (EV_DEV_HANDLE * pEvdevHandle,
                                         EV_DEV_PTR_DATA * pPtrData);
#endif /* _WRS_KERNEL */

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCevdevLibPtrh */
