/* evdevLibTs.c - Touch Screen Library */

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
29dec14,y_f  fixed enabling device error in SMP mode. (V7GFX-228)
24jun14,y_f  added multitouch support (US41403)
04jul13,y_f  created from udIoTsPtrDrv.c version 01d
*/

/*
DESCRIPTION
This file provides a touch screen library that handles messages from touch
screen devices and controls the devices.
*/

/* includes */

#include <evdevLib.h>
#include <excLib.h>
#include <wdLib.h>

/* defines */

#define TS_SUBMIT_TASK_PRIO                 10
#define TS_SUBMIT_TASK_NAME_LEN             32
#define TS_SUBMIT_TASK_MSG_Q_MAX            64
#define TS_SUBMIT_TASK_STACK_SIZE           1024 * 2

#define TS_MOVE_MIN                         2
#define TS_MAX_AXIS_NUM                     2

#ifndef MIN
#define MIN(a, b) ((a < b) ? a : b)
#endif

#ifndef MAX
#define MAX(a, b) ((a > b) ? a : b)
#endif

#undef  EVDEV_LIB_TS_DEBUG
#ifdef  EVDEV_LIB_TS_DEBUG
#   define EVDEV_LIB_TS_LOG(fmt, a, b, c, d, e, f) \
        (void)kprintf (fmt, a, b, c, d, e, f);

#else
#   define EVDEV_LIB_TS_LOG(fmt, a, b, c, d, e, f)
#endif

/* macros to make the coding of MMSE algorithm elegant */

#define CALIB_PT_STRIDE    sizeof (EV_DEV_POINT) / sizeof (int)
#define SIGMA2(m1,m2)      tsCalibInnerProduct (&m1, &m2)
#define SIGMA1(m)          tsCalibElemSum (&m)

/* typedefs */

typedef struct tsPosArray
    {
    int *   addr;
    UINT16  stride;
    UINT16  size;
    } TS_POS_ARRAY;

/* forward declarations */

LOCAL STATUS    tsOpen (TS_DEVICE_INFO * pDev);
LOCAL STATUS    tsClose (TS_DEVICE_INFO * pDev);
LOCAL STATUS    tsIoctl (TS_DEVICE_INFO * pDev, int request, _Vx_usr_arg_t arg);
LOCAL STATUS    tsReadCalData (TS_DEVICE_INFO * pDev, TS_CALIB_DATA *
                               pCalRawData);
LOCAL UINT8     tsFindCalData (TS_CALIB_DATA * pSrcData, UINT8 srcCount, char *
                               pDestName);
LOCAL void      tsSampleTimerIsr (TS_DEVICE_INFO * pDev);
LOCAL void      tsReleaseTimerIsr (TS_DEVICE_INFO * pDev);
LOCAL STATUS    tsSubmitPointTask (TS_DEVICE_INFO * pDev);
LOCAL STATUS    tsSubmitMtPointTask (TS_DEVICE_INFO * pDev);
LOCAL void      tsSamplePoint (TS_DEVICE_INFO * pDev);
LOCAL STATUS    tsCalibSetDisPoint (TS_CALIB_CTRL * pCalib, EV_DEV_POINT *
                                    pDisPoint);
LOCAL STATUS    tsCalibSetTsPoint (TS_CALIB_CTRL * pCalib, EV_DEV_POINT *
                                   pTsPoint);
LOCAL double    tsCalibInnerProduct (TS_POS_ARRAY * a1, TS_POS_ARRAY * a2);
LOCAL double    tsCalibElemSum (TS_POS_ARRAY * a);
LOCAL void      tsCalibCalcuCoef (TS_CALIB_CTRL * pCalib, EV_DEV_RECT *
                                  pDisplayRect);
LOCAL void      tsIdxFree (UINT32 idx);
LOCAL STATUS    tsIdxAlloc (UINT32 * pIdx);

#ifdef _WRS_CONFIG_EVDEV_DISPLAY_POINT_TRANSLATE
LOCAL void      tsCalibGetDisPoint (TS_CALIB_CTRL * pCalib, EV_DEV_POINT *
                                    pTsPoint, EV_DEV_POINT * pDisPoint);
#endif

/* locals */

LOCAL atomicVal_t tsDevIdx = 0;

/* functions */

/*******************************************************************************
*
* evdevTsReg - initialize the touch screen library
*
* This routine initializes the touch screen library and registers the touch
* screen device to evdev.
*
* RETURNS:  Pointer to the device infor structure when the driver is registered
*           and initialized successfully, otherwise NULL
*
* ERRNO: N/A
*
* \NOMANUAL
*/

TS_DEVICE_INFO * evdevTsReg
    (
    TS_DEVICE * pTsDev
    )
    {
    TS_DEVICE_INFO *    pDev = NULL;
    char                taskName[TS_SUBMIT_TASK_NAME_LEN + 1];
    int                 taskFlag = 0;
    size_t              msgSize;
    FUNCPTR             taskFun;
    UINT32              devIdx = 0;

    if (NULL == pTsDev)
        {
        return NULL;
        }

    if ((0 == (pTsDev->devAttr & TS_DEV_ATTR_SEND_MSG_SELF)) &&
        (NULL == pTsDev->func.read))
        {
        return NULL;
        }

    pDev = (TS_DEVICE_INFO *)calloc (1, sizeof (TS_DEVICE_INFO));
    if (NULL == pDev)
        {
        return NULL;
        }
    
    if (ERROR == tsIdxAlloc (&devIdx))
        {
        goto error;
        }

    bcopy ((char *)pTsDev, (char *)&pDev->tsDev, sizeof (TS_DEVICE));

    pDev->isEn                  = FALSE;
    pDev->tsDev.devData.ioctl   = (FUNCPTR)tsIoctl;
    pDev->tsDev.devData.pArg    = (void *)pDev;
    pDev->submitTaskId          = (TASK_ID)TASK_ID_ERROR;
    pDev->tsDev.devIdx          = devIdx;

    if (0 == (pTsDev->devAttr & TS_DEV_ATTR_MULTITOUCH))
        {
        msgSize = sizeof (TS_POINT);
        taskFun = tsSubmitPointTask;
        }
    else
        {
        msgSize = sizeof (EV_DEV_PTR_MT_DATA);
        taskFun = tsSubmitMtPointTask;
        }

    pDev->submitQueue = msgQCreate (TS_SUBMIT_TASK_MSG_Q_MAX, msgSize,
                                    MSG_Q_PRIORITY);

    if (NULL == pDev->submitQueue)
        {
        goto error;
        }

    if ((0 != (pTsDev->devAttr & TS_DEV_ATTR_POLLING_PRESS)) ||
        (0 != (pTsDev->devAttr & TS_DEV_ATTR_POLLING_SAMPLE)))
        {
        pDev->sampleTimer = (WDOG_ID)wdCreate ();
        if (NULL == pDev->sampleTimer)
            {
            goto error;
            }
        }

    if (0 != (pTsDev->devAttr & TS_DEV_ATTR_POLLING_RELEASE))
        {
        pDev->releaseTimer = (WDOG_ID)wdCreate ();
        if (NULL == pDev->releaseTimer)
            {
            goto error;
            }
        }

    bzero ((char *)taskName, (TS_SUBMIT_TASK_NAME_LEN + 1));
    (void)snprintf ((char *)taskName, (TS_SUBMIT_TASK_NAME_LEN + 1),
                    "tsSubmit%d", devIdx);

#if (_VX_CPU_FAMILY == _VX_ARM)
    taskFlag |= VX_VFP_TASK;
#endif /* _VX_CPU_FAMILY == _VX_ARM */

    pDev->submitTaskId = taskSpawn (taskName, TS_SUBMIT_TASK_PRIO, taskFlag,
                                    TS_SUBMIT_TASK_STACK_SIZE, taskFun,
                                    (_Vx_usr_arg_t)pDev,
                                    0, 0, 0, 0, 0, 0, 0, 0, 0);

    if ((TASK_ID)TASK_ID_ERROR == pDev->submitTaskId)
        {
        goto error;
        }

    pDev->pEvdev = evdevPtrReg (&pDev->tsDev.devData);
    if (NULL == pDev->pEvdev)
        {
        goto error;
        }

    return pDev;

error:
    if (NULL != pDev->releaseTimer)
        {
        (void)wdDelete (pDev->releaseTimer);
        }
    if (NULL != pDev->sampleTimer)
        {
        (void)wdDelete (pDev->sampleTimer);
        }
    if ((TASK_ID)TASK_ID_ERROR != pDev->submitTaskId)
        {
        (void)taskDelete (pDev->submitTaskId);
        }
    if (NULL != pDev->submitQueue)
        {
        (void)msgQDelete (pDev->submitQueue);
        }
    if (NULL != pDev->pEvdev)
        {
        (void)evdevPtrUnreg (pDev->pEvdev);
        }

    (void)tsIdxFree (devIdx);

    free (pDev);

    return NULL;
    }

/*******************************************************************************
*
* evdevTsUnreg - unregister a touch screen device from evdev
*
* This routine unregisters a touch screen device from evdev.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevTsUnreg
    (
    TS_DEVICE_INFO *    pTsDevInfo
    )
    {
    if (NULL == pTsDevInfo)
        {
        return ERROR;
        }

    if (NULL != pTsDevInfo->releaseTimer)
        {
        (void)wdCancel (pTsDevInfo->releaseTimer);
        (void)wdDelete (pTsDevInfo->releaseTimer);
        }
    if (NULL != pTsDevInfo->sampleTimer)
        {
        (void)wdCancel (pTsDevInfo->sampleTimer);
        (void)wdDelete (pTsDevInfo->sampleTimer);
        }
    if ((TASK_ID)TASK_ID_ERROR != pTsDevInfo->submitTaskId)
        {
        (void)taskDelete (pTsDevInfo->submitTaskId);
        }
    if (NULL != pTsDevInfo->submitQueue)
        {
        (void)msgQDelete (pTsDevInfo->submitQueue);
        }
    (void)tsIdxFree (pTsDevInfo->tsDev.devIdx);

    (void)evdevPtrUnreg (pTsDevInfo->pEvdev);
    free (pTsDevInfo);

    return OK;
    }

/*******************************************************************************
*
* evdevTsSendMsg - send touch data to the submit task
*
* This routine sends one touch screen point to the submit task.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevTsSendMsg
    (
    TS_DEVICE_INFO *    pDev,           /* device descriptor */
    TS_POINT *          pPoint          /* point to submit */
    )
    {
    if ((NULL == pDev) || (NULL == pPoint))
        {
        return ERROR;
        }

    return msgQSend (pDev->submitQueue, (char *)pPoint, sizeof (TS_POINT),
                     NO_WAIT, MSG_PRI_NORMAL);
    }

/*******************************************************************************
*
* evdevTsSendMtMsg - send multitouch data to the submit task
*
* This routine sends multitouch data to the submit task.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevTsSendMtMsg
    (
    TS_DEVICE_INFO *        pDev,           /* device descriptor */
    EV_DEV_PTR_MT_DATA *    pMtData         /* data to submit */
    )
    {
    if ((NULL == pDev) || (NULL == pMtData) || (0 == pMtData->count) ||
        (pMtData->count > TS_DEV_MULTITOUCH_MAX))
        {
        return ERROR;
        }

    return msgQSend (pDev->submitQueue, (char *)pMtData,
                     sizeof (EV_DEV_PTR_MT_DATA), NO_WAIT, MSG_PRI_NORMAL);
    }

/*******************************************************************************
*
* evdevTsEvalPoint - evaluate the sample data
*
* This routine evaluates the sample data.
*
* The <pDataArray> parameter is a data array pointer. It must store 3 data for
* evaluating. The routine will store the result in pDataArray if the 3 data
* are valid.
*
* RETURNS: TRUE if data is valid, otherwise FALSE
*
* ERRNO: N/A
*
* \NOMANUAL
*/

BOOL evdevTsEvalPoint
    (
    INT16 * pDataArray,
    UINT32  maxDiff
    )
    {
    INT16   diffValue[3];
    INT16   tempData;
    INT16   i;
    BOOL    ret = FALSE;

    if (NULL == pDataArray)
        {
        return ret;
        }

    /* calculate the absolute value of the differences of the sample points */

    diffValue[0] = (INT16)(*pDataArray - *(pDataArray + 1));
    diffValue[1] = (INT16)(*(pDataArray + 1) - *(pDataArray + 2));
    diffValue[2] = (INT16)(*(pDataArray + 2) - *pDataArray);
    diffValue[0] = (INT16)((diffValue[0] > 0)
                           ? (diffValue[0]) : (-diffValue[0]));
    diffValue[1] = (INT16)((diffValue[1] > 0)
                           ? (diffValue[1]) : (-diffValue[1]));
    diffValue[2] = (INT16)((diffValue[2] > 0)
                           ? (diffValue[2]) : (-diffValue[2]));

    for (i = 0; i < 3; i++)
        {
        if (diffValue[i] <= maxDiff)
            {
            ret = TRUE;
            break;
            }
        }

    /* eliminate the one away from other two and add the two */

    if (diffValue[0] < diffValue[1])
        {
        tempData = (INT16)(*pDataArray + ((diffValue[2] < diffValue[0])
                            ? *(pDataArray + 2) : *(pDataArray + 1)));
        }
    else
        {
        tempData = (INT16)(*(pDataArray + 2)
                           + ((diffValue[2] < diffValue[1])
                              ? *pDataArray : *(pDataArray + 1)));
        }

    *pDataArray = tempData / 2;

    return ret;
    }

/*******************************************************************************
*
* evdevTsDevIsr - touch screen driver interrupt routine
*
* This routine handles the interrupt request when touched the screen.
*
* RETURNS:  N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void evdevTsDevIsr
    (
    TS_DEVICE_INFO *    pDev
    )
    {
    TS_POINT    tsPoint = {0, FALSE, 0, 0};

    if (NULL == pDev->tsDev.func.ioctl)
        {
        return;
        }

    if (OK == pDev->tsDev.func.ioctl (pDev->tsDev.pInst, TS_CHECK_INT,
                                      &tsPoint))
        {
        if (0 != (pDev->tsDev.devAttr & TS_DEV_ATTR_POLLING_SAMPLE))
            {
            if (tsPoint.pressed)
                {
                (void)wdStart (pDev->sampleTimer, pDev->tsDev.samplePeriod,
                               (FUNCPTR)tsSampleTimerIsr, (_Vx_usr_arg_t)pDev);
                }
            else
                {
                (void)wdCancel (pDev->sampleTimer);
                }
            }

        if (0 != (pDev->tsDev.devAttr & TS_DEV_ATTR_POLLING_RELEASE))
            {
            (void)wdCancel (pDev->releaseTimer);
            (void)wdStart (pDev->releaseTimer, pDev->tsDev.releasePeriod,
                           (FUNCPTR)tsReleaseTimerIsr, (_Vx_usr_arg_t)pDev);
            }

        (void)excJobAdd ((VOIDFUNCPTR)tsSamplePoint, (_Vx_usr_arg_t)pDev,
                         0, 0, 0, 0, 0);
        }
    }

/*******************************************************************************
*
* tsCalibQuick - quickly calculate calibration coefficient
*
* This routine quickly calculates calibration coefficient. The function only
* supports 5 point calibration.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void evdevTsGetCalibQuick
    (
    int             tsLeft,
    int             tsTop,
    int             tsRight,
    int             tsBottom,
    UINT32          disWidth,
    UINT32          disHeight,
    TS_CALIB_DATA * pCalib
    )
    {
#ifdef  EVDEV_LIB_TS_DEBUG
    UINT8           i;
#endif  /* EVDEV_LIB_TS_DEBUG */
    TS_CALIB_DATA   calibData;
    TS_CALIB_DATA * pCalibData;

    if (NULL == pCalib)
        {
        pCalibData = &calibData;
        bzero ((char *)pCalibData, sizeof (TS_CALIB_DATA));
        pCalibData->calibPtCount = 5;
        }
    else
        {
        pCalibData = pCalib;
        }

    pCalibData->disRect.left    = 0;
    pCalibData->disRect.top     = 0;
    pCalibData->disRect.right   = (int)disWidth - 1;
    pCalibData->disRect.bottom  = (int)disHeight - 1;

    if (5 == pCalibData->calibPtCount)
        {
        pCalibData->disPoint[0].x   = 0;
        pCalibData->disPoint[0].y   = 0;
        pCalibData->disPoint[1].x   = pCalibData->disRect.right;
        pCalibData->disPoint[1].y   = 0;
        pCalibData->disPoint[2].x   = pCalibData->disRect.right;
        pCalibData->disPoint[2].y   = pCalibData->disRect.bottom;
        pCalibData->disPoint[3].x   = 0;
        pCalibData->disPoint[3].y   = pCalibData->disRect.bottom;
        pCalibData->disPoint[4].x   = (pCalibData->disRect.right + 1) / 2 - 1;
        pCalibData->disPoint[4].y   = (pCalibData->disRect.bottom + 1) / 2 - 1;
        pCalibData->tsPoint[0].x    = tsLeft;
        pCalibData->tsPoint[0].y    = tsTop;
        pCalibData->tsPoint[1].x    = tsRight;
        pCalibData->tsPoint[1].y    = tsTop;
        pCalibData->tsPoint[2].x    = tsRight;
        pCalibData->tsPoint[2].y    = tsBottom;
        pCalibData->tsPoint[3].x    = tsLeft;
        pCalibData->tsPoint[3].y    = tsBottom;
        if (tsRight > tsLeft)
            {
            pCalibData->tsPoint[4].x  = tsLeft + (tsRight - tsLeft + 1) / 2 - 1;
            }
        else
            {
            pCalibData->tsPoint[4].x  = tsLeft - (tsLeft - tsRight + 1) / 2 + 1;
            }

        if (tsBottom > tsTop)
            {
            pCalibData->tsPoint[4].y  = tsTop + (tsBottom - tsTop + 1) / 2 - 1;
            }
        else
            {
            pCalibData->tsPoint[4].y  = tsTop - (tsTop - tsBottom + 1) / 2 + 1;
            }

#ifdef  EVDEV_LIB_TS_DEBUG
        EVDEV_LIB_TS_LOG ("Display Resolution: %dx%d\n",
                          (pCalibData->disRect.right -
                           pCalibData->disRect.left + 1),
                          (pCalibData->disRect.bottom -
                           pCalibData->disRect.top + 1),
                          3, 4, 5, 6);
        EVDEV_LIB_TS_LOG ("Touch Screen Raw Data:\n", 1, 2, 3, 4, 5, 6);
        for (i = 0; i < pCalibData->calibPtCount; i++)
            {
             EVDEV_LIB_TS_LOG ("{%d, %d}, ", pCalibData->tsPoint[i].x,
                               pCalibData->tsPoint[i].y, 3, 4, 5, 6);
            }
        EVDEV_LIB_TS_LOG ("\nDisplay Data:\n", 1, 2, 3, 4, 5, 6);
        for (i = 0; i < pCalibData->calibPtCount; i++)
            {
             EVDEV_LIB_TS_LOG ("{%d, %d}, ", pCalibData->disPoint[i].x,
                               pCalibData->disPoint[i].y, 3, 4, 5, 6);
            }
#endif  /* EVDEV_LIB_TS_DEBUG */
        }
    }

/*******************************************************************************
*
* tsOpen - open the touch screen device
*
* This routine opens a touch screen device, connect and enable interrupt.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tsOpen
    (
    TS_DEVICE_INFO *    pDev    /* device descriptor */
    )
    {
    if (NULL != pDev->tsDev.func.open)
        {
        if (ERROR == pDev->tsDev.func.open (pDev->tsDev.pInst))
            {
            return ERROR;
            }
        }

    if (OK != tsReadCalData (pDev, &pDev->tsCalibCtrl.nvData))
        {
        pDev->tsCalibCtrl.calibStatus = TS_CALIB_STATUS_REQUIRED;
        }
    else
        {
        pDev->tsCalibCtrl.calibStatus = TS_CALIB_STATUS_COMPLETED;
        }

    memcpy ((void *)&pDev->tsCalibCtrl.currDisplayRect,
            (const void *)&pDev->tsDev.defDisRect, sizeof (EV_DEV_RECT));

    /* derive the calib coefs, based on current resolution */

    tsCalibCalcuCoef (&pDev->tsCalibCtrl,
                      &pDev->tsCalibCtrl.currDisplayRect);

    if (0 != (pDev->tsDev.devAttr & TS_DEV_ATTR_POLLING_PRESS))
        {
        if (ERROR == wdStart (pDev->sampleTimer, pDev->tsDev.samplePeriod,
                              (FUNCPTR)tsSampleTimerIsr, (_Vx_usr_arg_t)pDev))
            {
            if (NULL != pDev->tsDev.func.close)
                {
                (void)pDev->tsDev.func.close (pDev->tsDev.pInst);
                }
            return ERROR;
            }
        }

    return OK;
    }

/*******************************************************************************
*
* tsClose - close the touch screen device
*
* This routine closes the touch screen device, disconnect and disable interrupt.
*
* RETURNS: always OK
*
* ERRNO: N/A
*/

LOCAL STATUS tsClose
    (
    TS_DEVICE_INFO *    pDev    /* device descriptor */
    )
    {
    if ((0 != (pDev->tsDev.devAttr & TS_DEV_ATTR_POLLING_PRESS)) ||
        (0 != (pDev->tsDev.devAttr & TS_DEV_ATTR_POLLING_SAMPLE)))
        {
        (void)wdCancel (pDev->sampleTimer);
        }

    if (0 != (pDev->tsDev.devAttr & TS_DEV_ATTR_POLLING_RELEASE))
        {
        (void)wdCancel (pDev->releaseTimer);
        }

    if (NULL != pDev->tsDev.func.close)
        {
        (void)pDev->tsDev.func.close (pDev->tsDev.pInst);
        }

    return OK;
    }

/*******************************************************************************
*
* tsIoctl - touch screen driver ioctl
*
* This routine handles the ioctl calls made by the upper level driver. It
* provides the processing for a user level touch screen driver that requires the
* services be performed at the kernel level.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tsIoctl
    (
    TS_DEVICE_INFO *    pDev,       /* device descriptor */
    int                 request,    /* ioctl function */
    _Vx_usr_arg_t       arg         /* function arg */
    )
    {
    int     i;
    STATUS  result = OK;
    UINT32  axisIndex;

    switch (request)
        {
        case EV_DEV_IO_DEV_EN:
            if (!pDev->isEn)
                {
                result = tsOpen (pDev);
                if (OK == result)
                    {
                    pDev->isEn = TRUE;
                    }
                }
            break;

        case EV_DEV_IO_DEV_DIS:
            if (pDev->isEn)
                {
                result = tsClose (pDev);
                if (OK == result)
                    {
                    pDev->isEn = FALSE;
                    }
                }
            break;

        case EV_DEV_IO_SET_DIS_RECT:
            if (NULL == (int *)arg)
                {
                result = ERROR;
                break;
                }

            memcpy ((void *)&pDev->tsCalibCtrl.currDisplayRect,
                    (const void *)arg, sizeof (EV_DEV_RECT));

            /* derive the calib coefs, based on current resolution */

            tsCalibCalcuCoef (&pDev->tsCalibCtrl,
                              &pDev->tsCalibCtrl.currDisplayRect);
            break;

        case EV_DEV_IO_START_CALIB:
            pDev->tsCalibCtrl.nvData.calibPtCount   = 0;
            pDev->tsCalibCtrl.calibProcessStatus    = TS_CALIB_READY;
            break;

        case EV_DEV_IO_STOP_CALIB:
            pDev->tsCalibCtrl.calibProcessStatus = TS_CALIB_STOPPED;

            /* use current resolution to update the nvdata resolution */

            memcpy ((void *)&pDev->tsCalibCtrl.nvData.disRect,
                    (const void *)&pDev->tsCalibCtrl.currDisplayRect,
                    sizeof (EV_DEV_RECT));

            /* save last calib data to NVRAM */

            if (NULL != pDev->tsDev.func.ioctl)
                {
                (void)pDev->tsDev.func.ioctl (pDev->tsDev.pInst,
                                              TS_CALIBDATA_SAVE,
                                              (void *)&pDev->tsCalibCtrl.nvData);
                }

            /* flag calibration status as completed */

            pDev->tsCalibCtrl.calibStatus = TS_CALIB_STATUS_COMPLETED;

            /* derive the calib coefs, based on current resolution */

            tsCalibCalcuCoef (&pDev->tsCalibCtrl,
                              &pDev->tsCalibCtrl.nvData.disRect);

            (void)printf ("Display Resolution: %dx%d\n",
                          pDev->tsCalibCtrl.nvData.disRect.right
                          - pDev->tsCalibCtrl.nvData.disRect.left + 1,
                          pDev->tsCalibCtrl.nvData.disRect.bottom
                          - pDev->tsCalibCtrl.nvData.disRect.top + 1);
            (void)printf ("Touch Screen Raw Data:\n");
            for (i = 0; i < pDev->tsCalibCtrl.nvData.calibPtCount; i++)
                {
                (void)printf("{%d, %d}, ",
                             pDev->tsCalibCtrl.nvData.tsPoint[i].x,
                             pDev->tsCalibCtrl.nvData.tsPoint[i].y);
                }
            (void)printf ("\nDisplay Data:\n");
            for (i = 0; i < pDev->tsCalibCtrl.nvData.calibPtCount; i++)
                {
                (void)printf ("{%d, %d}, ",
                              pDev->tsCalibCtrl.nvData.disPoint[i].x,
                              pDev->tsCalibCtrl.nvData.disPoint[i].y);
                }

            break;

        case EV_DEV_IO_SET_CALIB_DIS_POINT:
            if (NULL == (int *)arg)
                {
                result = ERROR;
                break;
                }

            result = tsCalibSetDisPoint (&pDev->tsCalibCtrl,
                                         (EV_DEV_POINT *)arg);
            break;

        case EV_DEV_IO_GET_CALIB_STATUS:
            if (NULL == (int *)arg)
                {
                result = ERROR;
                break;
                }

            *((UINT8 *)arg) = pDev->tsCalibCtrl.calibProcessStatus;
            break;

        case EV_DEV_IO_GET_AXIS_VAL:
            if (NULL == (EV_DEV_DEVICE_AXIS_VAL *)arg)
                {
                result = ERROR;
                break;
                }
            
            axisIndex = ((EV_DEV_DEVICE_AXIS_VAL *)arg)->axisIndex;
            if (axisIndex >= TS_MAX_AXIS_NUM)
                {
                result = ERROR;
                break;
                }
            *(EV_DEV_DEVICE_AXIS_VAL *)arg = pDev->tsDev.devAxisVal[axisIndex];
            break;

        default:
            result = ERROR;
            break;
        }

    return result;
    }

/*******************************************************************************
*
* tsReadCalData - read default calibration data
*
* This routine reads default calibration data.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tsReadCalData
    (
    TS_DEVICE_INFO *    pDev,         /* device descriptor */
    TS_CALIB_DATA *     pCalRawData
    )
    {
    TS_CALIB_DATA   calData;
    TS_CALIB_DATA * pCalData = NULL;
    UINT8           index = 0;

    /* read NV ram first */

    if ((NULL == pDev->tsDev.pCalData) || (0 == pDev->tsDev.calDataCount))
        {
        return ERROR;
        }

    if (NULL != pDev->tsDev.func.ioctl)
        {
        if (OK == pDev->tsDev.func.ioctl (pDev->tsDev.pInst, TS_CALIBDATA_READ,
                                          &calData))
            {
            index = tsFindCalData (pDev->tsDev.pCalData,
                                   pDev->tsDev.calDataCount,
                                   calData.devName);
            if (index < pDev->tsDev.calDataCount)
                {
                pCalData = &calData;
                }
            }
        }

    /* use default calibration data if the data from NV is not matched */

    if (NULL == pCalData)
        {
        pCalData = pDev->tsDev.pCalData;
        }

    bcopy ((char *)pCalData, (char *)pCalRawData, sizeof (TS_CALIB_DATA));

    return OK;
    }

/*******************************************************************************
*
* tsFindCalData - find matched calibration data
*
* This routine find matched calibration data.
*
* RETURNS: index of matched calibration data
*
* ERRNO: N/A
*/

LOCAL UINT8 tsFindCalData
    (
    TS_CALIB_DATA * pSrcData,
    UINT8           srcCount,
    char *          pDestName
    )
    {
    UINT8           i;
    TS_CALIB_DATA * pCalData;

    if (NULL == pDestName)
        {
        return srcCount;
        }

    for (i = 0; i < srcCount; i++)
        {
        pCalData = pSrcData + i;
        if (0 == strcmp (pCalData->devName, pDestName))
            {
            return i;
            }
        }

    return i;
    }

/*******************************************************************************
*
* tsSampleTimerIsr - sample point timer interrupt routine
*
* This routine handles the timer interrupt request.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void tsSampleTimerIsr
    (
    TS_DEVICE_INFO *    pDev
    )
    {
    (void)wdStart (pDev->sampleTimer, pDev->tsDev.samplePeriod,
                   (FUNCPTR)tsSampleTimerIsr, (_Vx_usr_arg_t)pDev);

    (void)excJobAdd ((VOIDFUNCPTR)tsSamplePoint, (_Vx_usr_arg_t)pDev,
                     0, 0, 0, 0, 0);
    }

/*******************************************************************************
*
* tsReleaseTimerIsr - release check timer interrupt routine
*
* This routine handles the timer interrupt request.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void tsReleaseTimerIsr
    (
    TS_DEVICE_INFO *    pDev
    )
    {
    TS_POINT            tsPoint;
    EV_DEV_PTR_MT_DATA  mtPoints;

    if (0 == (pDev->tsDev.devAttr & TS_DEV_ATTR_MULTITOUCH))
        {
        bzero ((char *)&tsPoint, sizeof (TS_POINT));
        (void)evdevTsSendMsg (pDev, &tsPoint);
        }
    else
        {
        bzero ((char *)&mtPoints, sizeof (EV_DEV_PTR_MT_DATA));
        mtPoints.count = pDev->lastMtData.count;
        (void)evdevTsSendMtMsg (pDev, &mtPoints);
        }
    }

/*******************************************************************************
*
* tsSamplePoint - perform sample point value
*
* This routine implements sample point value from touch screen.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void tsSamplePoint
    (
    TS_DEVICE_INFO *    pDev          /* device descriptor */
    )
    {
    TS_POINT            tsPoint;
    EV_DEV_PTR_MT_DATA  mtPoints;
    UINT32              i = 0;

    bzero ((char *)&tsPoint, sizeof (TS_POINT));
    if (0 == (pDev->tsDev.devAttr & TS_DEV_ATTR_MULTITOUCH))
        {
        if (ERROR == pDev->tsDev.func.read (pDev->tsDev.pInst, &tsPoint))
            {
            return;
            }
        }
    else
        {
        bzero ((char *)&mtPoints, sizeof (EV_DEV_PTR_MT_DATA));
        if (ERROR == pDev->tsDev.func.read (pDev->tsDev.pInst, &mtPoints))
            {
            return;
            }

        while (i < mtPoints.count)
            {
            tsPoint.pressed |= mtPoints.points[i].pressed;
            i++;
            }
        }

    if (tsPoint.pressed)
        {
        if (0 != (pDev->tsDev.devAttr & TS_DEV_ATTR_POLLING_RELEASE))
            {
            (void)wdCancel (pDev->releaseTimer);
            (void)wdStart (pDev->releaseTimer, pDev->tsDev.releasePeriod,
                           (FUNCPTR)tsReleaseTimerIsr, (_Vx_usr_arg_t)pDev);
            }
        }
    else
        {
        if ((0 != (pDev->tsDev.devAttr & TS_DEV_ATTR_INTERRUPT)) &&
            (0 != (pDev->tsDev.devAttr & TS_DEV_ATTR_POLLING_SAMPLE)))
            {
            (void)wdCancel (pDev->sampleTimer);
            }

        if (0 != (pDev->tsDev.devAttr & TS_DEV_ATTR_POLLING_RELEASE))
            {
            (void)wdCancel (pDev->releaseTimer);
            }
        }

    if (0 == (pDev->tsDev.devAttr & TS_DEV_ATTR_MULTITOUCH))
        {
        (void)evdevTsSendMsg (pDev, &tsPoint);
        }
    else
        {
        (void)evdevTsSendMtMsg (pDev, &mtPoints);
        }
    }

/*******************************************************************************
*
* tsSubmitPointTask - submit touch data into the evdev framework
*
* This routine submit one touch screen point to the evdev framework.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL STATUS tsSubmitPointTask
    (
    TS_DEVICE_INFO *    pDev            /* device descriptor */
    )
    {
    EV_DEV_PTR_DATA ptrData;
    EV_DEV_POINT    rawPt;
    EV_DEV_POINT    scrnPt;
    TS_POINT        tsPoint;

    FOREVER
        {
        bzero ((char *)&ptrData, sizeof (EV_DEV_PTR_DATA));
        (void)msgQReceive (pDev->submitQueue, (char *)&tsPoint,
                           sizeof (TS_POINT), WAIT_FOREVER);

        if ((0 == pDev->tsCalibCtrl.currDisplayRect.right) ||
            (0 == pDev->tsCalibCtrl.currDisplayRect.bottom))
            {
            continue;
            }

        /* get calibrated screen point */

        rawPt.x = tsPoint.x;
        rawPt.y = tsPoint.y;

#ifdef _WRS_CONFIG_EVDEV_DISPLAY_POINT_TRANSLATE
        tsCalibGetDisPoint (&pDev->tsCalibCtrl, &rawPt, &scrnPt);
#else
        scrnPt.x = rawPt.x;
        scrnPt.y = rawPt.y;
#endif

        /* save touch screen raw data at calibration mode */

        if (TS_CALIB_STOPPED != pDev->tsCalibCtrl.calibProcessStatus)
            {
            if ((TS_CALIB_BUSY == pDev->tsCalibCtrl.calibProcessStatus) &&
                (tsPoint.pressed) && (!pDev->lastPoint.pressed))
                {
                (void)tsCalibSetTsPoint (&pDev->tsCalibCtrl, &rawPt);
                }

            /* update last point for each pointer */

            pDev->lastPoint.pressed = tsPoint.pressed;
            pDev->lastPoint.x       = scrnPt.x;
            pDev->lastPoint.y       = scrnPt.y;

            /* pointer data is eaten by calibration library, do not send it */

            continue;
            }

        /* Do not send consecutive unpressed messages */

        if ((!pDev->lastPoint.pressed) && (!tsPoint.pressed))
            {
            continue;
            }

        /* Do not send consecutive pressed message if the pointer is not moved */

        if (pDev->lastPoint.pressed && tsPoint.pressed &&
            ((MAX(scrnPt.x, pDev->lastPoint.x) -
             MIN(scrnPt.x, pDev->lastPoint.x)) < TS_MOVE_MIN) &&
            ((MAX(scrnPt.y, pDev->lastPoint.y) -
             MIN(scrnPt.y, pDev->lastPoint.y)) < TS_MOVE_MIN))
            {
            continue;
            }

        if (pDev->lastPoint.pressed != tsPoint.pressed)
            {
            ptrData.buttonChange  = EV_DEV_PTR_BTN_LEFT_BIT;
            }
        else
            {
            ptrData.buttonChange  = 0;
            }

        ptrData.id          = (UINT8)tsPoint.id;
        ptrData.type        = EV_DEV_ABS;
        ptrData.buttonState = 0;

        if (tsPoint.pressed)
            {
            ptrData.buttonState = EV_DEV_PTR_BTN_LEFT_BIT;
            ptrData.position.x  = scrnPt.x;
            ptrData.position.y  = scrnPt.y;
            }
        else
            {
            ptrData.position.x  = pDev->lastPoint.x;
            ptrData.position.y  = pDev->lastPoint.y;
            }

        /* update last point for each pointer */

        pDev->lastPoint.pressed = tsPoint.pressed;
        pDev->lastPoint.x       = ptrData.position.x;
        pDev->lastPoint.y       = ptrData.position.y;
        (void)evdevPtrSendMsg (pDev->pEvdev, &ptrData);
        }

    return ERROR;
    }

/*******************************************************************************
*
* tsSubmitMtPointTask - submit multitouch data into the evdev framework
*
* This routine submit multitouch data to the evdev framework.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL STATUS tsSubmitMtPointTask
    (
    TS_DEVICE_INFO *    pDev            /* device descriptor */
    )
    {
#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    EV_DEV_MSG          msg;
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */
#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
    EV_DEV_EVENT        event;
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */
    EV_DEV_PTR_MT_DATA  ptrMtData;
#ifdef _WRS_CONFIG_EVDEV_SINGLE_TOUCH_MODE
    EV_DEV_PTR_DATA     ptrData;
#endif /* _WRS_CONFIG_EVDEV_SINGLE_TOUCH_MODE */
    EV_DEV_POINT        rawPt;
#ifdef _WRS_CONFIG_EVDEV_DISPLAY_POINT_TRANSLATE
    EV_DEV_POINT        scrnPt;
#endif
    UINT32              i;
    UINT32              j;
    UINT32              size;
    struct timeval      time;
    BOOL                needSend[TS_DEV_MULTITOUCH_MAX + 1];

    FOREVER
        {
        (void)msgQReceive (pDev->submitQueue, (char *)&ptrMtData,
                           sizeof (EV_DEV_PTR_MT_DATA), WAIT_FOREVER);

        if ((0 == pDev->tsCalibCtrl.currDisplayRect.right) ||
            (0 == pDev->tsCalibCtrl.currDisplayRect.bottom) ||
            (ptrMtData.count > TS_DEV_MULTITOUCH_MAX))
            {
            continue;
            }

        /* save touch screen raw data at calibration mode */

        if (TS_CALIB_STOPPED != pDev->tsCalibCtrl.calibProcessStatus)
            {
            if ((TS_CALIB_BUSY == pDev->tsCalibCtrl.calibProcessStatus) &&
                (ptrMtData.points[0].pressed) &&
                (!pDev->lastMtData.points[0].pressed))
                {
                rawPt.x = ptrMtData.points[0].x;
                rawPt.y = ptrMtData.points[0].y;
                (void)tsCalibSetTsPoint (&pDev->tsCalibCtrl, &rawPt);
                }
            }

        bzero ((char *)needSend, sizeof (BOOL) * (TS_DEV_MULTITOUCH_MAX + 1));

        for (i = 0; i < ptrMtData.count; i++)
            {
            if ((ptrMtData.points[i].id >= TS_DEV_MULTITOUCH_MAX) ||
                (ptrMtData.points[i].id < 0))
                {
                ptrMtData.points[i].pressed = FALSE;
                }

            ptrMtData.points[i].id = i;

            if ((!ptrMtData.points[i].pressed) &&
                (!pDev->lastMtData.points[i].pressed))
                {
                continue;
                }

            /* get calibrated screen point */

            if (ptrMtData.points[i].pressed)
                {
                rawPt.x = ptrMtData.points[i].x;
                rawPt.y = ptrMtData.points[i].y;
#ifdef _WRS_CONFIG_EVDEV_DISPLAY_POINT_TRANSLATE
                tsCalibGetDisPoint (&pDev->tsCalibCtrl, &rawPt, &scrnPt);
                ptrMtData.points[i].x = scrnPt.x;
                ptrMtData.points[i].y = scrnPt.y;
#else
                ptrMtData.points[i].x = rawPt.x;
                ptrMtData.points[i].y = rawPt.y;
#endif
                }
            else
                {
                ptrMtData.points[i].x = pDev->lastMtData.points[i].x;
                ptrMtData.points[i].y = pDev->lastMtData.points[i].y;
                }

            needSend[i] = TRUE;

            /*
             * Do not send consecutive pressed message if the pointer is not
             * moved
             */

            if (pDev->lastMtData.points[i].pressed &&
                ptrMtData.points[i].pressed &&
                ((MAX(ptrMtData.points[i].x, pDev->lastMtData.points[i].x) -
                 MIN(ptrMtData.points[i].x, pDev->lastMtData.points[i].x)) <
                 TS_MOVE_MIN) &&
                ((MAX(ptrMtData.points[i].y, pDev->lastMtData.points[i].y) -
                 MIN(ptrMtData.points[i].y, pDev->lastMtData.points[i].y)) < 
                 TS_MOVE_MIN))
                {
                continue;
                }

            needSend[TS_DEV_MULTITOUCH_MAX] = TRUE;
            }

        if (!needSend[TS_DEV_MULTITOUCH_MAX])
            {
            continue;
            }

        bcopy ((char *)&ptrMtData, (char *)&pDev->lastMtData,
               sizeof (EV_DEV_PTR_MT_DATA));

        if (TS_CALIB_STOPPED != pDev->tsCalibCtrl.calibProcessStatus)
            {
            /* pointer data is eaten by calibration library, do not send it */

            continue;
            }

        bzero ((char *)&ptrMtData, sizeof (EV_DEV_PTR_MT_DATA));

        /* filter unsend points */

        for (i = 0, j = 0; i < pDev->lastMtData.count; i++)
            {
            if (needSend[i])
                {
                bcopy ((char *)&pDev->lastMtData.points[i],
                       (char *)&ptrMtData.points[j], sizeof (TS_POINT));
                j++;
                }
            }

        ptrMtData.count         = j;
        time.tv_sec             = tickGet () / sysClkRateGet ();
        time.tv_usec            = (tickGet () * 1000000) / sysClkRateGet () -
                                  time.tv_sec * 1000000;

#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
        size                = sizeof (EV_DEV_EVENT);
        event.time.tv_sec   = time.tv_sec;
        event.time.tv_usec  = time.tv_usec;
        event.type          = EV_DEV_ABS;

        for (i = 0; i < ptrMtData.count; i++)
            {
            event.code  = EV_DEV_PTR_ABS_MT_SLOT;
            event.value = ptrMtData.points[i].id;
            (void)evdevSendMsg (pDev->pEvdev, (char *)&event, size);

            if (ptrMtData.points[i].pressed)
                {
                event.code  = EV_DEV_PTR_ABS_MT_TRACKING_ID;
                event.value = ptrMtData.points[i].id;
                (void)evdevSendMsg (pDev->pEvdev, (char *)&event, size);

                event.code  = EV_DEV_PTR_ABS_MT_POSITION_X;
                event.value = ptrMtData.points[i].x;
                (void)evdevSendMsg (pDev->pEvdev, (char *)&event, size);

                event.code  = EV_DEV_PTR_ABS_MT_POSITION_Y;
                event.value = ptrMtData.points[i].y;
                (void)evdevSendMsg (pDev->pEvdev, (char *)&event, size);
                }
            else
                {
                event.code  = EV_DEV_PTR_ABS_MT_TRACKING_ID;
                event.value = -1;
                (void)evdevSendMsg (pDev->pEvdev, (char *)&event, size);
                }
            }

        event.type  = EV_DEV_SYN;
        event.code  = 0;
        event.value = 0;
        (void)evdevSendMsg (pDev->pEvdev, (char *)&event, size);
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */

#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
        memcpy ((void *)&msg.msgData.mtData, (const void *)&ptrMtData,
                sizeof (EV_DEV_PTR_MT_DATA));

        size                            = sizeof (EV_DEV_MSG);
        msg.msgType                     = EV_DEV_MSG_MT;
        msg.msgData.mtData.time.tv_sec  = time.tv_sec;
        msg.msgData.mtData.time.tv_usec = time.tv_usec;
        (void)evdevSendMsg (pDev->pEvdev, (char *)&msg, size);
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */

#ifdef _WRS_CONFIG_EVDEV_SINGLE_TOUCH_MODE

        /* send pointer message for compatible with single touch application */

        bzero ((char *)&ptrData, sizeof (EV_DEV_PTR_DATA));

        ptrData.type        = EV_DEV_ABS;
        ptrData.position.x  = ptrMtData.points[0].x;
        ptrData.position.y  = ptrMtData.points[0].y;

        if (pDev->lastPoint.pressed != ptrMtData.points[0].pressed)
            {
            ptrData.buttonChange  = EV_DEV_PTR_BTN_LEFT_BIT;
            if (ptrMtData.points[0].pressed)
                {
                ptrData.buttonState = EV_DEV_PTR_BTN_LEFT_BIT;
                }
            }

        pDev->lastPoint.pressed = ptrMtData.points[0].pressed;
        (void)evdevPtrSendMsg (pDev->pEvdev, &ptrData);
#endif /* _WRS_CONFIG_EVDEV_SINGLE_TOUCH_MODE */
        }

    return ERROR;
    }

#ifdef _WRS_CONFIG_EVDEV_DISPLAY_POINT_TRANSLATE
/*******************************************************************************
*
* tsCalibGetDisPoint - calculate display point
*
* This routine implements calculate the display point by using calibration
* data.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void tsCalibGetDisPoint
    (
    TS_CALIB_CTRL * pCalib,
    EV_DEV_POINT *  pTsPoint,
    EV_DEV_POINT *  pDisPoint
    )
    {
    pDisPoint->x = (int)(pCalib->det[1] * (double)pTsPoint->x + pCalib->det[2] *
                         (double)pTsPoint->y + pCalib->det[3] + (double)0.5);

    pDisPoint->y = (int)(pCalib->det[4] * (double)pTsPoint->x + pCalib->det[5] *
                         (double)pTsPoint->y + pCalib->det[6] + (double)0.5);
    }
#endif

/*******************************************************************************
*
* tsCalibSetDisPoint - set display point
*
* This routine implements set the display point at calibration mode.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tsCalibSetDisPoint
    (
    TS_CALIB_CTRL * pCalib,
    EV_DEV_POINT *  pDisPoint
    )
    {
    if (TS_CALIB_READY != pCalib->calibProcessStatus)
        {
        return ERROR;
        }

    if (pCalib->nvData.calibPtCount >= TS_CALIBDATA_MAX_SIZE)
        {
        return ERROR;
        }

    pCalib->requestPoint.x = pDisPoint->x;
    pCalib->requestPoint.y = pDisPoint->y;

    /*
     * Set calib process status to busy, the framework will wait for the
     * next screen click to associate this target point to a raw point
     */

    pCalib->calibProcessStatus = TS_CALIB_BUSY;
    return OK;
    }

/*******************************************************************************
*
* tsCalibSetTsPoint - set touch screen point
*
* This routine implements set the touch screen point at calibration mode.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A 
*/

LOCAL STATUS tsCalibSetTsPoint
    (
    TS_CALIB_CTRL * pCalib,
    EV_DEV_POINT *  pTsPoint
    )
    {
    if (TS_CALIB_BUSY != pCalib->calibProcessStatus)
        {
        return ERROR;
        }

    if (pCalib->nvData.calibPtCount >= TS_CALIBDATA_MAX_SIZE)
        {
        return ERROR;
        }

    pCalib->nvData.tsPoint[pCalib->nvData.calibPtCount].x = pTsPoint->x;
    pCalib->nvData.tsPoint[pCalib->nvData.calibPtCount].y = pTsPoint->y;
    pCalib->nvData.disPoint[pCalib->nvData.calibPtCount].x
                                                       = pCalib->requestPoint.x;
    pCalib->nvData.disPoint[pCalib->nvData.calibPtCount].y
                                                       = pCalib->requestPoint.y;

    pCalib->nvData.calibPtCount++;
    if (pCalib->nvData.calibPtCount >= TS_CALIBDATA_MAX_SIZE)
        {
        pCalib->nvData.calibPtCount = TS_CALIBDATA_MAX_SIZE;
        }
    pCalib->calibProcessStatus = TS_CALIB_READY;
    return OK;
    }

/*******************************************************************************
*
* tsCalibInnerProduct - calculate the inner product of two arrays
*
* This routine implements array dot-product calculation:
* A dot B = a1*b1 + a2*b2 + ... + an*bn;
*
* RETURNS: array dot-product sum
*
* ERRNO: N/A
*/

LOCAL double tsCalibInnerProduct
    (
    TS_POS_ARRAY *  a1,
    TS_POS_ARRAY *  a2
    )
    {
    UINT16  i;
    UINT16  size;
    double  sum = 0.0;

    size = (a1->size < a2->size) ? a1->size : a2->size;

    for(i = 0; i < size; i++)
        {
        sum += (*(a1->addr + a1->stride * i)) * (*(a2->addr + a2->stride * i));
        }

    return sum;
    }

/*******************************************************************************
*
* tsCalibElemSum - calculate the sum of array elements
*
* This routine implements array elements sum calculation:
* sum(A) = a1 + a2 + ... + an;
*
* RETURNS: array elements sum
*
* ERRNO: N/A
*/

LOCAL double tsCalibElemSum
    (
    TS_POS_ARRAY *  a
    )
    {
    UINT16  i;
    double  sum = 0.0;

    for (i = 0; i < a->size; i++)
        {
        sum += *(a->addr + a->stride * i);
        }

    return sum;
    }

/*******************************************************************************
*
* tsCalibCalcuCoef - calculate calibration coefficient
*
* This routine implements calculate calibration coefficient.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void tsCalibCalcuCoef
    (
    TS_CALIB_CTRL * pCalib,
    EV_DEV_RECT *   pDisplayRect
    )
    {
    double          a0;
    double          a1;
    double          a2;
    double          b0;
    double          b1;
    double          b2;
    double          c0;
    double          c1;
    double          c2;
    double          d0;
    double          d1;
    double          d2;
    double          xRatio;
    double          yRatio;
    TS_POS_ARRAY    xO;
    TS_POS_ARRAY    yO;
    TS_POS_ARRAY    xI;
    TS_POS_ARRAY    yI;

    /* initialize arrays */

    xO.addr = &(pCalib->nvData.disPoint[0].x);
    xO.stride = CALIB_PT_STRIDE;
    xO.size = pCalib->nvData.calibPtCount;
    yO.addr = &(pCalib->nvData.disPoint[0].y);
    yO.stride = CALIB_PT_STRIDE;
    yO.size = pCalib->nvData.calibPtCount;
    xI.addr = &(pCalib->nvData.tsPoint[0].x);
    xI.stride = CALIB_PT_STRIDE;
    xI.size = pCalib->nvData.calibPtCount;
    yI.addr = &(pCalib->nvData.tsPoint[0].y);
    yI.stride = CALIB_PT_STRIDE;
    yI.size = pCalib->nvData.calibPtCount;

    /* MMSE algorithm implementation */

    a0 = SIGMA2 (xI,xI) / SIGMA1 (xI);
    a1 = SIGMA2 (xI,yI) / SIGMA1 (yI);
    a2 = SIGMA1 (xI) / (double)pCalib->nvData.calibPtCount;

    b0 = SIGMA2 (xI,yI) / SIGMA1 (xI);
    b1 = SIGMA2 (yI,yI) / SIGMA1 (yI);
    b2 = SIGMA1 (yI) / (double)pCalib->nvData.calibPtCount;

    c0 = SIGMA2 (xO,xI) / SIGMA1 (xI);
    c1 = SIGMA2 (xO,yI) / SIGMA1 (yI);
    c2 = SIGMA1 (xO) / (double)pCalib->nvData.calibPtCount;

    d0 = SIGMA2 (yO,xI) / SIGMA1 (xI);
    d1 = SIGMA2 (yO,yI) / SIGMA1 (yI);
    d2 = SIGMA1 (yO) / (double)pCalib->nvData.calibPtCount;

    pCalib->det[0] = (a0 - a2) * (b1 - b2) - (a1 - a2) * (b0 - b2);
    pCalib->det[1] = (c0 - c2) * (b1 - b2) - (c1 - c2) * (b0 - b2);
    pCalib->det[2] = (c1 - c2) * (a0 - a2) - (c0 - c2) * (a1 - a2);
    pCalib->det[3] = b0 * (a2 * c1 - a1 * c2)
                     + b1 * (a0 * c2 - a2 * c0)
                     + b2 * (a1 * c0 - a0 * c1);
    pCalib->det[4] = (d0 - d2) * (b1 - b2) - (d1 - d2) * (b0 - b2);
    pCalib->det[5] = (d1 - d2) * (a0 - a2) - (d0 - d2) * (a1 - a2);
    pCalib->det[6] = b0 * (a2 * d1 - a1 * d2)
                     + b1 * (a0 * d2 - a2 * d0)
                     + b2 * (a1 * d0 - a0 * d1);

    pCalib->det[1] /= pCalib->det[0];
    pCalib->det[2] /= pCalib->det[0];
    pCalib->det[3] /= pCalib->det[0];
    pCalib->det[4] /= pCalib->det[0];
    pCalib->det[5] /= pCalib->det[0];
    pCalib->det[6] /= pCalib->det[0];

    /* change calib coef to fit current display resolution */

    xRatio = (double)(pDisplayRect->right - pDisplayRect->left) /
        (double)(pCalib->nvData.disRect.right - pCalib->nvData.disRect.left);
    yRatio = (double)(pDisplayRect->bottom - pDisplayRect->top) /
        (double)(pCalib->nvData.disRect.bottom - pCalib->nvData.disRect.top);
    pCalib->det[1] *= xRatio;
    pCalib->det[2] *= xRatio;
    pCalib->det[3] *= xRatio;
    pCalib->det[4] *= yRatio;
    pCalib->det[5] *= yRatio;
    pCalib->det[6] *= yRatio;
    }

/*******************************************************************************
*
* tsIdxAlloc - allocate a global device index
*
* This routine allocates a global device index.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS tsIdxAlloc
    (
    UINT32 *    pIdx
    )
    {
    UINT32  idx;

    for (idx = 0; idx < EV_DEV_DEVICE_MAX; idx++)
        {
        if ((vxAtomicGet(&tsDevIdx) & (1 << idx)) == 0)
            {
            vxAtomicOr(&tsDevIdx, (1 << idx));
            break;
            }
        }

    if (EV_DEV_DEVICE_MAX == idx)
        {
        return ERROR;
        }
    else
        {
        *pIdx = idx;
        return OK;
        }
    }

/*******************************************************************************
*
* tsIdxFree - free a global device index
*
* This routine frees a global device index.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void tsIdxFree
    (
    UINT32  idx
    )
    {
    vxAtomicAnd(&tsDevIdx, ~(1 << idx));
    }

