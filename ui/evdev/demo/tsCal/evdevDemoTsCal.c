/* evdevDemoTsCal.c - Touch Screen Calibration Demo */

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
14sep15,jnl  support querying size of the touchscreen area. (V7GFX-238)
24jun14,y_f  add multitouch support (US41403)
08aug13,y_f  create
*/

/*
DESCRIPTION
This file provides a demo for touch screen calibration.
*/

/* includes */

#include <evdevLib.h>
#include <taskLib.h>
#include <fbdev.h>
#include <stdlib.h>

/* defines */

#define FB_DEFAULT_DEVICE               "/dev/fb0"
#define FB_DEV_NAME_LEN                 32
#define EV_DEV_DEMO_TASK_PRIO           200
#define EV_DEV_DEMO_TASK_STACK_SIZE     (1024 * 8)

#define BLACK                           (0)
#define WHITE                           (1)

#define CURSOR_SIZE                     33
#define CURSOR_CENTER                   16

#undef DEMO_LOG
#define DEMO_LOG                        (void)printf

/* locals */

/* cursor image */

LOCAL UINT8 cursorData[] =
    {
#define O BLACK,
#define M WHITE,
    O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O
    O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O
    O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O
    O O O M O O O O O O O O O O O O O O O O O O O O O O O O O M O O O
    O O O O M O O O O O O O O O O O O O O O O O O O O O O O M O O O O
    O O O O O M O O O O O O O O O O O O O O O O O O O O O M O O O O O
    O O O O O O M O O O O O O O O O O O O O O O O O O O M O O O O O O
    O O O O O O O M O O O O O O O O O O O O O O O O O M O O O O O O O
    O O O O O O O O M O O O O O O O O O O O O O O O M O O O O O O O O
    O O O O O O O O O M O O O O O O O O O O O O O M O O O O O O O O O
    O O O O O O O O O O M O O O O O O O O O O O M O O O O O O O O O O
    O O O O O O O O O O O M O O O O O O O O O M O O O O O O O O O O O
    O O O O O O O O O O O O M O O O O O O O M O O O O O O O O O O O O
    O O O O O O O O O O O O O M O O O O O M O O O O O O O O O O O O O
    O O O O O O O O O O O O O O M O O O M O O O O O O O O O O O O O O
    O O O O O O O O O O O O O O O M O M O O O O O O O O O O O O O O O
    O O O O O O O O O O O O O O O O M O O O O O O O O O O O O O O O O
    O O O O O O O O O O O O O O O M O M O O O O O O O O O O O O O O O
    O O O O O O O O O O O O O O M O O O M O O O O O O O O O O O O O O
    O O O O O O O O O O O O O M O O O O O M O O O O O O O O O O O O O
    O O O O O O O O O O O O M O O O O O O O M O O O O O O O O O O O O
    O O O O O O O O O O O M O O O O O O O O O M O O O O O O O O O O O
    O O O O O O O O O O M O O O O O O O O O O O M O O O O O O O O O O
    O O O O O O O O O M O O O O O O O O O O O O O M O O O O O O O O O
    O O O O O O O O M O O O O O O O O O O O O O O O M O O O O O O O O
    O O O O O O O M O O O O O O O O O O O O O O O O O M O O O O O O O
    O O O O O O M O O O O O O O O O O O O O O O O O O O M O O O O O O
    O O O O O M O O O O O O O O O O O O O O O O O O O O O M O O O O O
    O O O O M O O O O O O O O O O O O O O O O O O O O O O O M O O O O
    O O O M O O O O O O O O O O O O O O O O O O O O O O O O O M O O O
    O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O
    O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O
    O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O O
#undef O
#undef M
    };

/* forward declarations */

LOCAL STATUS    evdevDemoTsCalTask (UINT32 devId, UINT8 calPoints, char *
                                    fbName);
LOCAL STATUS    evdevDemoTsCalSetPoint (int evdevFd, FB_INFO * pFbinfo, int x,
                                        int y);

/* functions */

/*******************************************************************************
*
* evdevDemoTsCal - create a task for running touch screen calibration
*
* This routine creates a task for running touch screen calibration.
*
* RETURNS: OK, or ERROR if create task failed
*
* ERRNO: N/A
*/

#ifdef _WRS_KERNEL
STATUS evdevDemoTsCal
    (
    UINT32  devId,
    UINT8   calPoints,
    char *  fbName
    )
    {
    static char fbDevName[FB_DEV_NAME_LEN];

    if ((5 != calPoints) && (9 != calPoints))
        {
        DEMO_LOG ("Only support 5 or 9 points calibration!\n");
        return ERROR;
        }

    if (devId >= EV_DEV_DEVICE_MAX)
        {
        DEMO_LOG ("The devId is invalid!\n");
        return ERROR;
        }

    if (NULL == fbName)
        {
        fbName = FB_DEFAULT_DEVICE;
        }

    bzero ((char *)fbDevName, sizeof (fbDevName));
    (void)snprintf ((char *)fbDevName, FB_DEV_NAME_LEN, "%s", fbName);

    if (TASK_ID_ERROR == taskSpawn ("evdevDemoTsCal", EV_DEV_DEMO_TASK_PRIO, 0,
                                    EV_DEV_DEMO_TASK_STACK_SIZE,
                                    (FUNCPTR)evdevDemoTsCalTask,
                                    (_Vx_usr_arg_t)devId,
                                    (_Vx_usr_arg_t)calPoints,
                                    (_Vx_usr_arg_t)fbDevName,
                                    0, 0, 0, 0, 0, 0, 0))
        {
        DEMO_LOG ("Create task failed!\n");
        return ERROR;
        }
    else
        {
        return OK;
        }
    }

#else /* _WRS_KERNEL */

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
    char    fbDevName[FB_DEV_NAME_LEN];
    UINT32  devId;
    UINT8   calPoints;
    char *  fbName = NULL;

    if (argc <= 2)
        {
        return ERROR;
        }

    devId       = atoi (argv [1]);
    calPoints   = (UINT8)atoi (argv [2]);

    if (argc > 3)
        {
        fbName  = argv [3];
        }

    if ((5 != calPoints) && (9 != calPoints))
        {
        DEMO_LOG ("Only support 5 or 9 points calibration!\n");
        return ERROR;
        }

    if (devId >= EV_DEV_DEVICE_MAX)
        {
        DEMO_LOG ("The devId is invalid!\n");
        return ERROR;
        }

    if (NULL == fbName)
        {
        fbName = FB_DEFAULT_DEVICE;
        }

    bzero ((char *)fbDevName, sizeof (fbDevName));
    (void)snprintf ((char *)fbDevName, FB_DEV_NAME_LEN, "%s", fbName);

    return evdevDemoTsCalTask (devId, calPoints, fbDevName);
    }
#endif /* _WRS_KERNEL */

/*******************************************************************************
*
* evdevDemoTsCalTask - open the event device and calibrate touch screen
*
* This routine opens the event device and calibrates touch screen.
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

LOCAL STATUS evdevDemoTsCalTask
    (
    UINT32  devId,
    UINT8   calPoints,
    char *  fbName
    )
    {
    int             disFd   = ERROR;
    FB_IOCTL_ARG    fbArg;
    FB_INFO         fbInfo;
    int             evdevFd = ERROR;
    EV_DEV_RECT     disRect;
    char            evdevName[EV_DEV_NAME_LEN + 1];
    UINT32          devCap = 0;

    bzero (evdevName, sizeof (evdevName));
#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    (void)snprintf ((char *)evdevName, EV_DEV_NAME_LEN, "%s", EV_DEV_NAME);
#else /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */
    (void)snprintf ((char *)evdevName, EV_DEV_NAME_LEN, "%s%d",
                    EV_DEV_NAME_PREFIX, devId);
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */

    evdevFd = open (evdevName, 0, 0);
    if (ERROR == evdevFd)
        {
        DEMO_LOG ("Open event device failed!\n");
        goto error;
        }

    disFd = open (fbName, 0, 0);
    if (ERROR == disFd)
        {
        DEMO_LOG ("Open fb device failed!\n");
        goto error;
        }

    bzero ((char *)&fbArg, sizeof (FB_IOCTL_ARG));
    fbArg.setVideoMode.videoMode = FB_DEFAULT_VIDEO_MODE;
    if (ERROR == ioctl (disFd, FB_IOCTL_SET_VIDEO_MODE, (char *)&fbArg))
        {
        DEMO_LOG ("Set fb mode failed!\n");
        goto error;
        }

    bzero ((char *)&fbArg, sizeof (FB_IOCTL_ARG));
    if (ERROR == ioctl (disFd, FB_IOCTL_GET_FB_INFO, (char *)&fbArg))
        {
        DEMO_LOG ("Get fb info failed!\n");
        goto error;
        }

    memcpy ((void *)&fbInfo, (const void *)&fbArg, sizeof (FB_INFO));
    DEMO_LOG ("fb info: width = %d, height = %d, bpp = %d\n",
              fbInfo.width, fbInfo.height, fbInfo.bpp);

    if (ERROR == ioctl (evdevFd, EV_DEV_IO_SET_OPERATE_DEV, (char *)&devId))
        {
        DEMO_LOG ("EV_DEV_IO_SET_OPERATE_DEV failed! devId = %d\n", devId);
        goto error;
        }

    if (ERROR == ioctl (evdevFd, EV_DEV_IO_GET_CAP, (char *)&devCap))
        {
        DEMO_LOG ("EV_DEV_IO_GET_CAP failed!\n");
        goto error;
        }

    DEMO_LOG ("Device capabilities = 0x%x\n", devCap);
    if (0 == (EV_DEV_ABS & devCap))
        {
        DEMO_LOG ("The device is not a touch screen!\n");
        goto error;
        }

    disRect.left    = 0;
    disRect.top     = 0;
    disRect.right   = fbInfo.width - 1;
    disRect.bottom  = fbInfo.height - 1;
    if (ERROR == ioctl (evdevFd, EV_DEV_IO_SET_DIS_RECT, (char *)&disRect))
        {
        DEMO_LOG ("Set display rectangle failed!\n");
        goto error;
        }

    DEMO_LOG ("---Begin calibrating touch screen---\n");

    if (ERROR == ioctl (evdevFd, EV_DEV_IO_START_CALIB, NULL))
        {
        DEMO_LOG ("Begin calibrating touch screen failed!\n");
        goto error;
        }

    if (5 == calPoints)
        {
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo, 0, 0))
            {
            goto error;
            }
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo,
                                    (fbInfo.width - 1 - CURSOR_SIZE), 0))
            {
            goto error;
            }
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo,
                                    (fbInfo.width - 1 - CURSOR_SIZE),
                                    (fbInfo.height - 1 - CURSOR_SIZE)))
            {
            goto error;
            }
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo, 0,
                                    (fbInfo.height - 1 - CURSOR_SIZE)))
            {
            goto error;
            }
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo,
                                    (fbInfo.width / 2 - 1 - CURSOR_CENTER),
                                    (fbInfo.height / 2 - 1 - CURSOR_CENTER)))
            {
            goto error;
            }
        }
    else
        {
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo, 0, 0))
            {
            goto error;
            }
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo,
                                    (fbInfo.width / 2 - 1 - CURSOR_CENTER), 0))
            {
            goto error;
            }
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo,
                                    (fbInfo.width - 1 - CURSOR_SIZE), 0))
            {
            goto error;
            }

        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo, 0,
                                    (fbInfo.height / 2 - 1 - CURSOR_CENTER)))
            {
            goto error;
            }
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo,
                                    (fbInfo.width / 2 - 1 - CURSOR_CENTER),
                                    (fbInfo.height / 2 - 1 - CURSOR_CENTER)))
            {
            goto error;
            }
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo,
                                    (fbInfo.width - 1 - CURSOR_SIZE),
                                    (fbInfo.height / 2 - 1 - CURSOR_CENTER)))
            {
            goto error;
            }

        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo, 0,
                                    (fbInfo.height - 1 - CURSOR_SIZE)))
            {
            goto error;
            }
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo,
                                    (fbInfo.width / 2 - 1 - CURSOR_CENTER),
                                    (fbInfo.height - 1 - CURSOR_SIZE)))
            {
            goto error;
            }
        if (ERROR == evdevDemoTsCalSetPoint (evdevFd, &fbInfo,
                                    (fbInfo.width - 1 - CURSOR_SIZE),
                                    (fbInfo.height - 1 - CURSOR_SIZE)))
            {
            goto error;
            }
        }

    if (ERROR == ioctl (evdevFd, EV_DEV_IO_STOP_CALIB, NULL))
        {
        DEMO_LOG ("Stop calibrating touch screen failed!\n");
        goto error;
        }

    bzero ((char *)fbInfo.pFb, (fbInfo.stride * fbInfo.height));
    DEMO_LOG ("\n---Calibrating touch screen successfully!---\n");

    if (ERROR == close (evdevFd))
        {
        evdevFd = ERROR;
        DEMO_LOG ("Close event device failed!\n");
        goto error;
        }

    if (ERROR != disFd)
        {
        (void)close (disFd);
        }

    return OK;

error:
    if (ERROR != evdevFd)
        {
        (void)close (evdevFd);
        }
    if (ERROR != disFd)
        {
        (void)close (disFd);
        }

    return ERROR;
    }

/*******************************************************************************
*
* evdevDemoTsCalSetPoint - conduct calibration at one point
*
* This routine will show an 'X' at the desired calibration target point and wait
* for the user to click it.
*
* RETURNS: OK, or ERROR if test failed
*
* ERRNO: N/A
*/

LOCAL STATUS evdevDemoTsCalSetPoint
    (
    int         evdevFd,
    FB_INFO *   pFbInfo,
    int         x,
    int         y
    )
    {
    UINT32          i;
    UINT32          j;
    UINT8 *         pCursor = cursorData;
    EV_DEV_POINT    position;
    UINT8           calibStatus;

    bzero ((char *)pFbInfo->pFb, (pFbInfo->stride * pFbInfo->height));

    for (i = 0; i < CURSOR_SIZE; i++)
        {
        for (j = 0; j < CURSOR_SIZE; j++)
            {
            if (WHITE == *pCursor)
                {
                if (16 == pFbInfo->bpp)
                    {
                    *((UINT16 *)pFbInfo->pFb + (i + y) * pFbInfo->width + j + x)
                        = 0xFFFF;
                    }
                else
                    {
                    *((UINT32 *)pFbInfo->pFb + (i + y) * pFbInfo->width + j + x)
                        = 0xFFFFFFFF;
                    }
                }
            pCursor++;
            }
        }

    position.x  = x + CURSOR_CENTER;
    position.y  = y + CURSOR_CENTER;
    if (ERROR == ioctl (evdevFd, EV_DEV_IO_SET_CALIB_DIS_POINT,
                        (char *)&position))
        {
        DEMO_LOG ("Set display point failed!\n");
        return ERROR;
        }

    /* wait for this calibration point to complete */

    do
        {
        (void)taskDelay (2);
        if (ERROR == ioctl (evdevFd, EV_DEV_IO_GET_CALIB_STATUS,
                            (char *)&calibStatus))
            {
            DEMO_LOG ("Get calibration status failed!\n");
            return ERROR;
            }
        } while (TS_CALIB_BUSY == calibStatus);

    return OK;
    }
