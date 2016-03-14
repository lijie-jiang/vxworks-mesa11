/* evdevLibPtr.c - Pointer Library */

/*
 * Copyright (c) 2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
04jul13,y_f  create
*/

/*
DESCRIPTION
This file provides a pointer class library that handles messages from pointer
devices and controls the devices.
*/

/* includes */

#include <evdevLib.h>

/* functions */

/*******************************************************************************
*
* evdevPtrReg - register a pointer device to evdev
*
* This routine registers a pointer device to evdev.
*
* RETURNS: pointer of evdev handle structure when the device successfully
* registered; otherwise NULL
*
* ERRNO: N/A
*
* \NOMANUAL
*/

EV_DEV_HANDLE * evdevPtrReg
    (
    EV_DEV_DEVICE_DATA *    pDevData
    )
    {
    return evdevRegDev (pDevData);
    }

/*******************************************************************************
*
* evdevPtrUnreg - unregister a pointer device from evdev
*
* This routine unregisters a pointer device from evdev.
*
* RETURNS: OK when the device successfully unregistered; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevPtrUnreg
    (
    EV_DEV_HANDLE * pEvdevHandle
    )
    {
    return evdevUnregDev (pEvdevHandle);
    }

/*******************************************************************************
*
* evdevPtrSendMsg - send pointer events to evdev event queue
*
* This routine sends pointer events to evdev event queue.
*
* RETURNS: OK when successfully send message; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevPtrSendMsg
    (
    EV_DEV_HANDLE *     pEvdevHandle,
    EV_DEV_PTR_DATA *   pPtrData
    )
    {
#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    EV_DEV_MSG      msg;
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */
#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
    EV_DEV_EVENT    event;
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */
    struct timeval  time;
    UINT32          size;
    STATUS          status  = OK;

    if ((NULL == pEvdevHandle) || (NULL == pPtrData))
        {
        return ERROR;
        }

    time.tv_sec     = tickGet () / sysClkRateGet ();
    time.tv_usec    = (tickGet () * 1000000) / sysClkRateGet () -
                      time.tv_sec * 1000000;

#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
    size                = sizeof (EV_DEV_EVENT);
    event.time.tv_sec   = time.tv_sec;
    event.time.tv_usec  = time.tv_usec;
    event.type          = EV_DEV_KEY;

    if (0 != (pPtrData->buttonChange & EV_DEV_PTR_BTN_LEFT_BIT))
        {
#if ((_VX_CPU_FAMILY != _VX_SIMLINUX) && (_VX_CPU_FAMILY != _VX_SIMNT))
        if (EV_DEV_ABS == pPtrData->type)
            {
            event.code  = EV_DEV_PTR_BTN_TOUCH;
            }
        else
#endif /* (_VX_CPU_FAMILY != _VX_SIMLINUX) && (_VX_CPU_FAMILY != _VX_SIMNT) */
            {
            event.code  = EV_DEV_PTR_BTN_LEFT;
            }
        event.value = pPtrData->buttonState & EV_DEV_PTR_BTN_LEFT_BIT;
        status |= evdevSendMsg (pEvdevHandle, (char *)&event, size);
        }

    if (0 != (pPtrData->buttonChange & EV_DEV_PTR_BTN_RIGHT_BIT))
        {
        event.code  = EV_DEV_PTR_BTN_RIGHT;
        event.value = pPtrData->buttonState & EV_DEV_PTR_BTN_RIGHT_BIT;
        status |= evdevSendMsg (pEvdevHandle, (char *)&event, size);
        }

    if (0 != (pPtrData->buttonChange & EV_DEV_PTR_BTN_MIDDLE_BIT))
        {
        event.code  = EV_DEV_PTR_BTN_MIDDLE;
        event.value = pPtrData->buttonState & EV_DEV_PTR_BTN_MIDDLE_BIT;
        status |= evdevSendMsg (pEvdevHandle, (char *)&event, size);
        }

    event.type = pPtrData->type;

    if (0 != pPtrData->wheel)
        {
        event.code  = EV_DEV_PTR_REL_WHEEL;
        event.value = pPtrData->wheel;
        status |= evdevSendMsg (pEvdevHandle, (char *)&event, size);
        }

    if (EV_DEV_REL == event.type)
        {
        event.code  = EV_DEV_PTR_REL_X;
        }
    else
        {
        event.code  = EV_DEV_PTR_ABS_X;
        }

    event.value = pPtrData->position.x;
    status |= evdevSendMsg (pEvdevHandle, (char *)&event, size);

    if (EV_DEV_REL == event.type)
        {
        event.code  = EV_DEV_PTR_REL_Y;
        }
    else
        {
        event.code  = EV_DEV_PTR_ABS_Y;
        }

    event.value = pPtrData->position.y;
    status |= evdevSendMsg (pEvdevHandle, (char *)&event, size);

    event.type  = EV_DEV_SYN;
    event.code  = 0;
    event.value = 0;
    status |= evdevSendMsg (pEvdevHandle, (char *)&event, size);
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */

#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    memcpy ((void *)&msg.msgData.ptrData, (const void *)pPtrData,
            sizeof (EV_DEV_PTR_DATA));

    size                                = sizeof (EV_DEV_MSG);
    msg.msgType                         = EV_DEV_MSG_PTR;
    msg.msgData.ptrData.time.tv_sec     = time.tv_sec;
    msg.msgData.ptrData.time.tv_usec    = time.tv_usec;
    status |= evdevSendMsg (pEvdevHandle, (char *)&msg, size);
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */

    return status;
    }
