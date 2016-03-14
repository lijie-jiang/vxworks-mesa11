/* evdevDemoBasic.c - Event Devices Framework Basic Demo */

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
24jun14,y_f  add multitouch support (US41403)
25sep13,j_x  add wheel support
03jul13,y_f  created
*/

/*
DESCRIPTION
This file provides a basic demo for event device framework library.
*/

/* includes */

#include <evdevLib.h>
#include <stdlib.h>
#if ((_VX_CPU_FAMILY == _VX_SIMLINUX) || (_VX_CPU_FAMILY == _VX_SIMNT))
#include <fbdev.h>
#endif /* (_VX_CPU_FAMILY == _VX_SIMLINUX) || (_VX_CPU_FAMILY == _VX_SIMNT) */

/* defines */

#define FB_DEFAULT_DEVICE   "/dev/fb0"

#undef DEMO_LOG
#define DEMO_LOG            (void)printf

/* forward declarations */

#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
LOCAL STATUS    evdevDemoBasicOptTask (UINT32 devIdx,UINT32 maxMsg);
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */

#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
LOCAL STATUS    evdevDemoBasicComTask (UINT32 devIdx,UINT32 maxMsg);
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */

/* functions */

/*******************************************************************************
*
* evdevDemoBasic - create a task for running basic demo
*
* This routine creates a task for running basic demo.
*
* RETURNS: OK, or ERROR if create task failed
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevDemoBasic
    (
    UINT32  devIdx,
    UINT32  maxMsg
    )
    {
    STATUS  rc      = OK;

#if ((_VX_CPU_FAMILY == _VX_SIMLINUX) || (_VX_CPU_FAMILY == _VX_SIMNT))
    FB_IOCTL_ARG    fbArg;
    int             disFd   = ERROR;

    disFd = open (FB_DEFAULT_DEVICE, 0, 0);
    if (ERROR == disFd)
        {
        DEMO_LOG ("Open fb device failed!\n");
        return ERROR;
        }

    bzero ((char *)&fbArg, sizeof (FB_IOCTL_ARG));
    fbArg.setVideoMode.videoMode = FB_DEFAULT_VIDEO_MODE;
    if (ERROR == ioctl (disFd, FB_IOCTL_SET_VIDEO_MODE, (char *)&fbArg))
        {
        DEMO_LOG ("Set fb mode failed!\n");
        (void)close (disFd);
        return ERROR;
        }
#endif /* (_VX_CPU_FAMILY == _VX_SIMLINUX) || (_VX_CPU_FAMILY == _VX_SIMNT) */

#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
    rc = evdevDemoBasicComTask (devIdx, maxMsg);
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */

#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    rc = evdevDemoBasicOptTask (devIdx, maxMsg);
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */

#if ((_VX_CPU_FAMILY == _VX_SIMLINUX) || (_VX_CPU_FAMILY == _VX_SIMNT))
    if (ERROR != disFd)
        {
        (void)close (disFd);
        }
#endif /* (_VX_CPU_FAMILY == _VX_SIMLINUX) || (_VX_CPU_FAMILY == _VX_SIMNT) */

    return rc;
    }

#ifndef _WRS_KERNEL
/*******************************************************************************
*
* main - start of the demo program in RTP mode
*
* This is the RTP mode entry point.
*
* RETURNS: EXIT_SUCCESS, or non-zero if the routine had to abort
*
* ERRNO: N/A
*
* \NOMANUAL
*/

int main
    (
    int     argc,
    char *  argv[]
    )
    {
    if (argc <= 2)
        {
        return ERROR;
        }

    return evdevDemoBasic ((UINT32)atoi (argv [1]), (UINT32)atoi (argv [2]));
    }
#endif /* _WRS_KERNEL */

#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
/*******************************************************************************
*
* evdevDemoBasicOptTask - open the event device and read the messages
*
* This routine opens the event device and reads the messages from the device by
* using optimized API.
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

LOCAL STATUS evdevDemoBasicOptTask
    (
    UINT32  devIdx,
    UINT32  maxMsg
    )
    {
    int                    evdevCoreFd = ERROR;
    int                    msgCount;
    EV_DEV_MSG             evdevMsg;
    char                   devName[EV_DEV_NAME_LEN + 1];
    EV_DEV_DEVICE_INFO     devInfo;
    UINT32                 devCap      = 0;
    UINT32                 kbdMode     = 0;
    UINT32                 devCount    = 0;
    UINT32                 evdevVer    = 0;
    UINT32                 i           = 0;
    EV_DEV_DEVICE_AXIS_VAL axisVal[2];

    evdevCoreFd = open (EV_DEV_NAME, 0, 0);
    if (ERROR == evdevCoreFd)
        {
        DEMO_LOG ("Open event device failed!\n");
        goto error;
        }

    if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_GET_VERSION, (char *)&evdevVer))
        {
        DEMO_LOG ("EV_DEV_IO_GET_VERSION failed!\n");
        goto error;
        }

    DEMO_LOG ("Evdev version = 0x%x\n", evdevVer);

    if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_GET_DEV_COUNT,
                        (char *)&devCount))
        {
        DEMO_LOG ("EV_DEV_IO_GET_DEV_COUNT failed!\n");
        goto error;
        }

    DEMO_LOG ("Device count = %d\n", devCount);

    if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_SET_OPERATE_DEV,
                        (char *)&devIdx))
        {
        DEMO_LOG ("EV_DEV_IO_SET_OPERATE_DEV failed! devIdx = %d\n", devIdx);
        goto error;
        }

    bzero ((char *)&devInfo, sizeof (EV_DEV_DEVICE_INFO));
    if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_GET_INFO, (char *)&devInfo))
        {
        DEMO_LOG ("EV_DEV_IO_GET_INFO failed!\n");
        goto error;
        }

    DEMO_LOG ("Device bus type = 0x%x, vendor = 0x%x, product = 0x%x, version ="
              " 0x%x\n", devInfo.bustype, devInfo.vendor, devInfo.product,
              devInfo.version);

    bzero (devName, sizeof (devName));
    if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_GET_NAME, (char *)devName))
        {
        DEMO_LOG ("EV_DEV_IO_GET_NAME failed!\n");
        goto error;
        }

    DEMO_LOG ("Device name = %s\n", devName);

    axisVal[0].axisIndex = 0;
    if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_GET_AXIS_VAL, (char *)&axisVal[0]))
        {
        DEMO_LOG ("Get axis X value failed!\n");
        goto error;
        }

    DEMO_LOG ("Axis X Min = %d\n", axisVal[0].minVal);
    DEMO_LOG ("Axis X Max = %d\n", axisVal[0].maxVal);

    axisVal[1].axisIndex = 1;
    if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_GET_AXIS_VAL, (char *)&axisVal[1]))
        {
        DEMO_LOG ("Get axis Y value failed!\n");
        goto error;
        }

    DEMO_LOG ("Axis Y Min = %d\n", axisVal[1].minVal);
    DEMO_LOG ("Axis Y Max = %d\n", axisVal[1].maxVal);

    if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_GET_CAP, (char *)&devCap))
        {
        DEMO_LOG ("EV_DEV_IO_GET_CAP failed!\n");
        goto error;
        }

    DEMO_LOG ("Device capabilities = 0x%x\n", devCap);

    if ((devCap & (EV_DEV_ABS | EV_DEV_REL)) == 0)
        {
        if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_GET_KBD_MODE, (char *)&kbdMode))
            {
            DEMO_LOG ("EV_DEV_IO_GET_KBD_MODE failed!\n");
            goto error;
            }

        if (EV_DEV_KBD_UNICODE_MODE == kbdMode)
            {
            DEMO_LOG ("Keyboard Mode is Unicode.\n");
            }
        else
            {
            DEMO_LOG ("Keyboard Mode is Keycode.\n");
            }

        kbdMode = EV_DEV_KBD_UNICODE_MODE;
        if (ERROR == ioctl (evdevCoreFd, EV_DEV_IO_SET_KBD_MODE, (char *)&kbdMode))
            {
            DEMO_LOG ("EV_DEV_IO_SET_KBD_MODE failed!\n");
            goto error;
            }

        DEMO_LOG ("Use Unicode Mode.\n");
        }

    /* clear msg */

    if (ERROR == ioctl (evdevCoreFd, FIONREAD, (char *)&msgCount))
        {
        DEMO_LOG ("FIONREAD failed!\n");
        goto error;
        }

    while (msgCount >= sizeof (EV_DEV_MSG))
        {
        if (sizeof (EV_DEV_MSG) !=
            read (evdevCoreFd, (char *)&evdevMsg, sizeof (EV_DEV_MSG)))
            {
            break;
            }

        if (ERROR == ioctl (evdevCoreFd, FIONREAD, (char *)&msgCount))
            {
            break;
            }
        }

    DEMO_LOG ("---Begin receiving %u messages from event device.---\n", maxMsg);

    while (maxMsg > 0)
        {
        msgCount = (int)read (evdevCoreFd, (char *)&evdevMsg,
                              sizeof (EV_DEV_MSG));
        if ((int)sizeof (EV_DEV_MSG) == msgCount)
            {
            DEMO_LOG ("Test Task received a message!\n");

            switch (evdevMsg.msgType)
                {
                case EV_DEV_MSG_KBD:
                    {
                    DEMO_LOG ("Keyboard message: value = 0x%x key = %c state = "
                              "%d\n",
                              evdevMsg.msgData.kbdData.value,
                              evdevMsg.msgData.kbdData.value,
                              evdevMsg.msgData.kbdData.state);
                    }
                    break;

                case EV_DEV_MSG_PTR:
                    {
                    DEMO_LOG ("Pointer message: id = %d, x = %d, y = %d,"
                              " wheel = %d, buttonState = 0x%x, type = %d\n",
                              evdevMsg.msgData.ptrData.id,
                              evdevMsg.msgData.ptrData.position.x,
                              evdevMsg.msgData.ptrData.position.y,
                              evdevMsg.msgData.ptrData.wheel,
                              evdevMsg.msgData.ptrData.buttonState,
                              evdevMsg.msgData.ptrData.type);
                    }
                    break;

                case EV_DEV_MSG_MT:
                    {
                    DEMO_LOG ("Multitouch message: %d points\n",
                              evdevMsg.msgData.mtData.count);
                    for (i = 0; i< evdevMsg.msgData.mtData.count; i++)
                        {
                        DEMO_LOG ("Point[%d]: id = %d, pressed = %d, x = %d,"
                                  " y = %d\n",
                                  i,
                                  evdevMsg.msgData.mtData.points[i].id,
                                  evdevMsg.msgData.mtData.points[i].pressed,
                                  evdevMsg.msgData.mtData.points[i].x,
                                  evdevMsg.msgData.mtData.points[i].y);
                        }
                    }
                    break;

                case EV_DEV_MSG_HOT_PLUG:
                    {
                    if (evdevMsg.msgData.hotPlug.isPlugin)
                        {
                        DEMO_LOG ("Hot plug message: A device is plugged in. "
                                  "devIdx = %d\n",
                                  evdevMsg.msgData.hotPlug.devIdx);
                        }
                    else
                        {
                        DEMO_LOG ("Hot plug message: A device is pulled out. "
                                  "devIdx = %d\n",
                                  evdevMsg.msgData.hotPlug.devIdx);
                        }
                    }
                    break;

                default:
                    DEMO_LOG ("Unknown message = 0x%x\n", evdevMsg.msgType);
                }

            maxMsg--;
            }
        else
            {
            DEMO_LOG ("The size doesn't equal to EV_DEV_MSG!\n");
            goto error;
            }
        }

    DEMO_LOG ("---Stop receiving message from event device.---\n");

    if (ERROR == close (evdevCoreFd))
        {
        evdevCoreFd = ERROR;
        DEMO_LOG ("Close event device failed!\n");
        goto error;
        }

    return OK;

error:
    if (ERROR != evdevCoreFd)
        {
        (void)close (evdevCoreFd);
        }

    return ERROR;
    }
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */

#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
/*******************************************************************************
*
* evdevDemoBasicComTask - open the event device and read the messages
*
* This routine opens the event device and reads the messages from the device by
* using compatible API.
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

LOCAL STATUS evdevDemoBasicComTask
    (
    UINT32  devIdx,
    UINT32  maxMsg
    )
    {
    int                    evdevFd     = ERROR;
    int                    msgCount;
    EV_DEV_EVENT           evdevEvent;
    char                   evdevName[EV_DEV_NAME_LEN + 1];
    char                   devName[EV_DEV_NAME_LEN + 1];
    EV_DEV_DEVICE_INFO     devInfo;
    UINT32                 devCap      = 0;
    UINT32                 kbdMode     = 0;
    UINT32                 devCount    = 0;
    UINT32                 evdevVer    = 0;
    EV_DEV_DEVICE_AXIS_VAL axisVal[2];

    bzero (evdevName, sizeof (evdevName));
    (void)snprintf ((char *)evdevName, EV_DEV_NAME_LEN, "%s%d",
                    EV_DEV_NAME_PREFIX, devIdx);

    evdevFd = open (evdevName, 0, 0);
    if (ERROR == evdevFd)
        {
        DEMO_LOG ("Open event device [%s] failed!\n", evdevName);
        goto error;
        }

    if (ERROR == ioctl (evdevFd, EV_DEV_IO_GET_VERSION, (char *)&evdevVer))
        {
        DEMO_LOG ("EV_DEV_IO_GET_VERSION failed!\n");
        goto error;
        }

    DEMO_LOG ("Evdev version = 0x%x\n", evdevVer);

    if (ERROR == ioctl (evdevFd, EV_DEV_IO_GET_DEV_COUNT, (char *)&devCount))
        {
        DEMO_LOG ("EV_DEV_IO_GET_DEV_COUNT failed!\n");
        goto error;
        }

    DEMO_LOG ("Device count = %d\n", devCount);

    bzero ((char *)&devInfo, sizeof (EV_DEV_DEVICE_INFO));
    if (ERROR == ioctl (evdevFd, EV_DEV_IO_GET_INFO, (char *)&devInfo))
        {
        DEMO_LOG ("EV_DEV_IO_GET_INFO failed!\n");
        goto error;
        }

    DEMO_LOG ("Device bus type = 0x%x, vendor = 0x%x, product = 0x%x, version ="
              " 0x%x\n", devInfo.bustype, devInfo.vendor, devInfo.product,
              devInfo.version);

    bzero (devName, sizeof (devName));
    if (ERROR == ioctl (evdevFd, EV_DEV_IO_GET_NAME, (char *)devName))
        {
        DEMO_LOG ("EV_DEV_IO_GET_NAME failed!\n");
        goto error;
        }

    DEMO_LOG ("Device name = %s\n", devName);

    axisVal[0].axisIndex = 0;
    if (ERROR == ioctl (evdevFd, EV_DEV_IO_GET_AXIS_VAL, (char *)&axisVal[0]))
        {
        DEMO_LOG ("Get axis X value failed!\n");
        goto error;
        }

    DEMO_LOG ("Axis X Min = %d\n", axisVal[0].minVal);
    DEMO_LOG ("Axis X Max = %d\n", axisVal[0].maxVal);

    axisVal[1].axisIndex = 1;
    if (ERROR == ioctl (evdevFd, EV_DEV_IO_GET_AXIS_VAL, (char *)&axisVal[1]))
        {
        DEMO_LOG ("Get axis Y value failed!\n");
        goto error;
        }

    DEMO_LOG ("Axis Y Min = %d\n", axisVal[1].minVal);
    DEMO_LOG ("Axis Y Max = %d\n", axisVal[1].maxVal);

    if (ERROR == ioctl (evdevFd, EV_DEV_IO_GET_CAP, (char *)&devCap))
        {
        DEMO_LOG ("EV_DEV_IO_GET_CAP failed!\n");
        goto error;
        }

    DEMO_LOG ("Device capabilities = 0x%x\n", devCap);

    if ((devCap & (EV_DEV_ABS | EV_DEV_REL)) == 0)
        {
        if (ERROR == ioctl (evdevFd, EV_DEV_IO_GET_KBD_MODE, (char *)&kbdMode))
            {
            DEMO_LOG ("EV_DEV_IO_GET_KBD_MODE failed!\n");
            goto error;
            }

        if (EV_DEV_KBD_UNICODE_MODE == kbdMode)
            {
            DEMO_LOG ("Keyboard Mode is Unicode.\n");
            }
        else
            {
            DEMO_LOG ("Keyboard Mode is Keycode.\n");
            }

        kbdMode = EV_DEV_KBD_KEYCODE_MODE;
        if (ERROR == ioctl (evdevFd, EV_DEV_IO_SET_KBD_MODE, (char *)&kbdMode))
            {
            DEMO_LOG ("EV_DEV_IO_SET_KBD_MODE failed!\n");
            goto error;
            }

        DEMO_LOG ("Use Keycode Mode.\n");
        }

    /* clear msg */

    if (ERROR == ioctl (evdevFd, FIONREAD, (char *)&msgCount))
        {
        DEMO_LOG ("FIONREAD failed!\n");
        goto error;
        }

    while (msgCount >= sizeof (EV_DEV_EVENT))
        {
        if (sizeof (EV_DEV_EVENT) !=
            read (evdevFd, (char *)&evdevEvent, sizeof (EV_DEV_EVENT)))
            {
            break;
            }

        if (ERROR == ioctl (evdevFd, FIONREAD, (char *)&msgCount))
            {
            break;
            }
        }

    DEMO_LOG ("---Begin receiving %u messages from event device.---\n", maxMsg);

    while (maxMsg > 0)
        {
        msgCount = (int)read (evdevFd, (char *)&evdevEvent,
                              sizeof (EV_DEV_EVENT));
        if ((int)sizeof (EV_DEV_EVENT) == msgCount)
            {
            DEMO_LOG ("Test Task received a message!\n");

            switch (evdevEvent.type)
                {
                case EV_DEV_SYN:
                    {
                    DEMO_LOG ("EV_DEV_SYN event\n");
                    maxMsg--;
                    }
                    break;

                case EV_DEV_KEY:
                    {
                    if (0 != (devCap & EV_DEV_REL))
                        {
                        DEMO_LOG ("EV_DEV_KEY event from REL pointer device: "
                                  "code = 0x%x, value = 0x%x\n",
                                  evdevEvent.code, evdevEvent.value);
                        }
                    else if (0 != (devCap & EV_DEV_ABS))
                        {
                        DEMO_LOG ("EV_DEV_KEY event from ABS pointer device: "
                                  "code = 0x%x, value = 0x%x\n",
                                  evdevEvent.code, evdevEvent.value);
                        }
                    else
                        {
                        DEMO_LOG ("EV_DEV_KEY event from keyboard device: "
                                  "value = 0x%x key = %c, value = %d\n",
                                  evdevEvent.code, evdevEvent.code,
                                  evdevEvent.value);
                        }
                    }
                    break;

                case EV_DEV_REL:
                    {
                    DEMO_LOG ("REL pointer device event\n");

                    switch (evdevEvent.code)
                        {
                        case EV_DEV_PTR_REL_X:
                            DEMO_LOG ("EV_DEV_PTR_REL_X event: x = %d\n",
                                      evdevEvent.value);
                            break;

                        case EV_DEV_PTR_REL_Y:
                            DEMO_LOG ("EV_DEV_PTR_REL_X event: y = %d\n",
                                      evdevEvent.value);
                            break;

                        case EV_DEV_PTR_REL_WHEEL:
                            DEMO_LOG ("EV_DEV_PTR_REL_WHEEL event: wheel = "
                                      "%d\n", evdevEvent.value);
                            break;

                        default:
                            DEMO_LOG ("Unknown code = 0x%x\n", evdevEvent.code);
                        }
                    }
                    break;

                case EV_DEV_ABS:
                    {
                    DEMO_LOG ("ABS pointer device event\n");

                    switch (evdevEvent.code)
                        {
                        case EV_DEV_PTR_ABS_X:
                            DEMO_LOG ("EV_DEV_PTR_ABS_X: x = %d\n",
                                      evdevEvent.value);
                            break;

                        case EV_DEV_PTR_ABS_Y:
                            DEMO_LOG ("EV_DEV_PTR_ABS_Y: y = %d\n",
                                      evdevEvent.value);
                            break;

                        case EV_DEV_PTR_ABS_MT_SLOT:
                            DEMO_LOG ("EV_DEV_PTR_ABS_MT_SLOT: slot = %d\n",
                                      evdevEvent.value);
                            break;

                        case EV_DEV_PTR_ABS_MT_POSITION_X:
                            DEMO_LOG ("EV_DEV_PTR_ABS_MT_POSITION_X: x = %d\n",
                                      evdevEvent.value);
                            break;

                        case EV_DEV_PTR_ABS_MT_POSITION_Y:
                            DEMO_LOG ("EV_DEV_PTR_ABS_MT_POSITION_Y: y = %d\n",
                                      evdevEvent.value);
                            break;

                        case EV_DEV_PTR_ABS_MT_TRACKING_ID:
                            DEMO_LOG ("EV_DEV_PTR_ABS_MT_TRACKING_ID: tracking "
                                      "id = %d\n",
                                      evdevEvent.value);
                            break;

                        default:
                            DEMO_LOG ("Unknown code = 0x%x\n", evdevEvent.code);
                        }
                    }
                    break;

                default:
                    DEMO_LOG ("Unknown event = 0x%x\n", evdevEvent.type);
                }

            }
        else
            {
            DEMO_LOG ("The size doesn't equal to EV_DEV_EVENT!\n");
            goto error;
            }
        }

    DEMO_LOG ("---Stop receiving message from event device.---\n");

    if (ERROR == close (evdevFd))
        {
        evdevFd = ERROR;
        DEMO_LOG ("Close event device [%s] failed!\n", evdevName);
        goto error;
        }

    return OK;

error:
    if (ERROR != evdevFd)
        {
        (void)close (evdevFd);
        }

    return ERROR;
    }
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */
