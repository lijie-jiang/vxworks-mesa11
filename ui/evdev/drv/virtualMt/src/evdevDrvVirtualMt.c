/* evdevDrvVirtualMt.c - Virtual Multitouch Driver */

/*
 * Copyright (c) 2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
25jun14,y_f  create  (US41403)
*/

/*
DESCRIPTION
This file contains code for virtual multitouch.
*/

/* includes */

#include <evdevLib.h>

/* locals */

LOCAL TS_CALIB_DATA virtualMtCalData [] =
    {
        {
            {"virtualMt"},                  /* touch screen name */
            {0, 0, (800 - 1), (600 - 1)},   /* display rect */
            5,

             /* display point */

            {{0, 0}, {799, 0}, {799, 599}, {0, 599}, {399, 299}},

            /* touch screen point */

            {{0, 0}, {799, 0}, {799, 599}, {0, 599}, {399, 299}}
        }
    };

LOCAL TS_DEVICE_INFO *  pDevInfo = NULL;

/* functions */

/*******************************************************************************
*
* virtualMtInit - touch screen driver initialization routine
*
* This routine initializes the touch screen device driver.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS virtualMtInit
    (
    UINT32  width,
    UINT32  height
    )
    {
    TS_DEVICE   tsDev;

    if ((0 == width) || (0 == height))
        {
        return ERROR;
        }

    bzero ((char *)&tsDev, sizeof (TS_DEVICE));

    tsDev.devAttr                   = TS_DEV_ATTR_SEND_MSG_SELF |
                                      TS_DEV_ATTR_MULTITOUCH;
    tsDev.pInst                     = NULL;
    tsDev.pCalData                  = virtualMtCalData;
    tsDev.calDataCount              = NELEMENTS (virtualMtCalData);
    tsDev.devData.devCap            = EV_DEV_KEY | EV_DEV_ABS;
    tsDev.devData.pDevName          = "virtualMt";
    tsDev.devData.devInfo.bustype   = EV_DEV_BUS_VIRTUAL;
    tsDev.devData.devInfo.vendor    = 0;
    tsDev.devData.devInfo.product   = 0;
    tsDev.devData.devInfo.version   = 0;
    tsDev.defDisRect.right          = width - 1;
    tsDev.defDisRect.bottom         = height - 1;
    tsDev.defDisRect.left           = 0;
    tsDev.defDisRect.top            = 0;

    pDevInfo = evdevTsReg (&tsDev);
    if (NULL == pDevInfo)
        {
        return ERROR;
        }

    return OK;
    }

/*******************************************************************************
*
* virtualMtSendMsg - send multitouch message to evdev
*
* This routine sends multitouch message to evdev.
*
* RETURNS: OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

STATUS virtualMtSendMsg
    (
    UINT32  count,
    BOOL    isPress
    )
    {
    UINT32              i = 0;
    EV_DEV_PTR_MT_DATA  mtPoints;

    if ((NULL == pDevInfo) || (count > TS_DEV_MULTITOUCH_MAX))
        {
        return ERROR;
        }

    bzero ((char *)&mtPoints, sizeof (EV_DEV_PTR_MT_DATA));
    mtPoints.count = count;

    if (isPress)
        {
        while (i < count)
            {
            mtPoints.points[i].id       = i;
            mtPoints.points[i].pressed  = TRUE;
            mtPoints.points[i].x        = i * 2;
            mtPoints.points[i].y        = i * 4;
            i++;
            }
        }

    return evdevTsSendMtMsg (pDevInfo, &mtPoints);
    }
