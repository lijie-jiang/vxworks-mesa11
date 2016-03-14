/* evdevLibCore.c - Event Devices Framework Core Library */

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
29dec14,y_f  fixed enabling device error in SMP mode. (V7GFX-228)
16may14,yat  Add flags to support O_NONBLOCK read I/O. (US24741)
26jun13,y_f  created
*/

/*
DESCRIPTION
This file provides an event device framework that manages all input devices and
events.
*/

/* includes */

#include <evdevLib.h>
#include <iosLib.h>

/* defines */

#define EV_DEV_IO_TASK_PRIO         200
#define EV_DEV_IO_TASK_STACK_SIZE   (1024 * 8)

/* typedefs */

typedef struct evdevLibCoreDev
    {
    DL_NODE         node;
    EV_DEV_HANDLE * pEvdev;
    } EV_DEV_LIB_CORE_DEV;

/* structure to store the event devices framework core library information */

typedef struct evdevLibCoreData
    {
    UINT32          msgNum;
    UINT32          idx;
    DL_LIST *       pDevList;
    EV_DEV_HANDLE   evdev;
    BOOL            regEn;
    }EV_DEV_LIB_CORE_DATA;

/* forward declarations */

LOCAL void *    evdevOpen (EV_DEV_HANDLE * pEvdev, const char * name, int flags, int mode);
LOCAL STATUS    evdevClose (EV_DEV_HANDLE * pEvdev);
LOCAL int       evdevRead (EV_DEV_HANDLE * pEvdev, char * buffer, int nBytes);
LOCAL int       evdevWrite (EV_DEV_HANDLE * pEvdev, char * buffer, int nBytes);
LOCAL STATUS    evdevIoctl (EV_DEV_HANDLE * pEvdev, int request, _Vx_usr_arg_t
                            arg);
LOCAL STATUS    evdevIdxAlloc (UINT32 * pIdx);
LOCAL void      evdevIdxFree (UINT32 idx);
LOCAL STATUS    evdevAutoEnDevTask (EV_DEV_HANDLE * pEvdev);
#ifndef _WRS_CONFIG_EVDEV_HOT_PLUG
LOCAL STATUS    evdevDisRegTask (void);
#endif /* _WRS_CONFIG_EVDEV_HOT_PLUG */

/* locals */

LOCAL EV_DEV_LIB_CORE_DATA *    pCoreData = NULL;

/* functions */

/*******************************************************************************
*
* evdevInit - install the event device framework
*
* This routine installs the event device framework.
*
* RETURNS: OK, or ERROR if the framework cannot be installed
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevInit
    (
    UINT32  msgNum
    )
    {
    int             drvNum      = ERROR;
    EV_DEV_HANDLE * pCoreInfo   = NULL;

    if (msgNum < EV_DEV_MIN_MSG_NUM)
        {
        msgNum = EV_DEV_MIN_MSG_NUM;
        }

    pCoreData = (EV_DEV_LIB_CORE_DATA *) calloc (1,
        sizeof (EV_DEV_LIB_CORE_DATA));
    if (NULL == pCoreData)
        {
        return ERROR;
        }

    pCoreData->msgNum   = msgNum;
    pCoreData->pDevList = dllCreate ();
    if (NULL == pCoreData->pDevList)
        {
        goto error;
        }

    pCoreInfo = &pCoreData->evdev;
    pCoreInfo->ioSem = semBCreate (SEM_Q_FIFO, SEM_FULL);
    if (SEM_ID_NULL == pCoreInfo->ioSem)
        {
        goto error;
        }

    /* install the driver */

    drvNum = iosDrvInstall (NULL,                       /* creat() */
                            NULL,                       /* remove() */
                            (DRV_OPEN_PTR)evdevOpen,    /* open() */
                            (DRV_CLOSE_PTR)evdevClose,  /* close() */
                            (DRV_READ_PTR)evdevRead,    /* read() */
                            (DRV_WRITE_PTR)evdevWrite,  /* write() */
                            (DRV_IOCTL_PTR)evdevIoctl); /* ioctl() */

    if (ERROR == drvNum)
        {
        goto error;
        }

#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    if (ERROR == iosDevAdd ((DEV_HDR *)&pCoreInfo->devHdr, EV_DEV_NAME,
                            drvNum))
        {
        goto error;
        }
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */

    selWakeupListInit (&pCoreInfo->selWakeupList);

    pCoreInfo->devIdx   = 0;
    pCoreInfo->pDevData = NULL;
    pCoreInfo->isOpened = FALSE;
    pCoreInfo->drvNum   = drvNum;
    pCoreInfo->msgSize  = sizeof (EV_DEV_MSG);
    pCoreInfo->flags    = 0;
    pCoreData->regEn    = TRUE;

#ifndef _WRS_CONFIG_EVDEV_HOT_PLUG
    (void)taskSpawn ("evdevDisReg", EV_DEV_IO_TASK_PRIO, 0,
                     EV_DEV_IO_TASK_STACK_SIZE, (FUNCPTR)evdevDisRegTask,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#endif /* _WRS_CONFIG_EVDEV_HOT_PLUG */

    return OK;

error:
#ifdef _WRS_CONFIG_EVDEV_OPTIMIZED_MODE
    if ((NULL != pCoreInfo) && (NULL != pCoreInfo->devHdr.name))
        {
        (void)iosDevDelete ((DEV_HDR *)pCoreInfo);
        }
#endif /* _WRS_CONFIG_EVDEV_OPTIMIZED_MODE */
    if ((NULL != pCoreInfo) && (SEM_ID_NULL != pCoreInfo->ioSem))
        {
        (void)semDelete (pCoreInfo->ioSem);
        }

    if (ERROR != drvNum)
        {
        (void)iosDrvRemove (drvNum, FALSE);
        }

    if (NULL != pCoreData->pDevList)
        {
        (void)dllDelete (pCoreData->pDevList);
        }

    free (pCoreData);
    return ERROR;
    }

/*******************************************************************************
*
* evdevRegDev - register an event device to the device list
*
* This routine registers an event device to the device list.
*
* RETURNS: pointer of evdev handle structure when the device successfully
* registered; otherwise NULL
*
* ERRNO: N/A
*
* \NOMANUAL
*/

EV_DEV_HANDLE * evdevRegDev
    (
    EV_DEV_DEVICE_DATA *    pDevData
    )
    {
    UINT32                  devIdx;
#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
    char                    devName[EV_DEV_NAME_LEN + 1];
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */
    EV_DEV_HANDLE *         pEvdev          = NULL;
    EV_DEV_HANDLE *         pEvdevCore      = NULL;
    EV_DEV_LIB_CORE_DEV *   pEvdevCoreDev   = NULL;
    EV_DEV_MSG              msg;

    if ((NULL == pCoreData) || (NULL == pDevData) ||
        (NULL == pDevData->ioctl) || (!pCoreData->regEn))
        {
        return NULL;
        }

    pEvdevCore = &pCoreData->evdev;
    if (OK != semTake (pEvdevCore->ioSem, EV_DEV_TIMEOUT))
        {
        return NULL;
        }

    if (ERROR == evdevIdxAlloc (&devIdx))
        {
        (void)semGive (pEvdevCore->ioSem);
        return NULL;
        }

    pEvdev = (EV_DEV_HANDLE *) calloc (1, sizeof (EV_DEV_HANDLE));
    if (NULL == pEvdev)
        {
        goto error;
        }

    pEvdev->devIdx      = devIdx;
    pEvdev->pDevData    = pDevData;
    pEvdev->isOpened    = FALSE;
    pEvdev->flags       = 0;
    pEvdev->ioSem       = SEM_ID_NULL;

    pEvdevCoreDev = (EV_DEV_LIB_CORE_DEV *) calloc (1,
        sizeof (EV_DEV_LIB_CORE_DEV));
    if (NULL == pEvdevCoreDev)
        {
        goto error;
        }

#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
    pEvdev->ioSem = semBCreate (SEM_Q_FIFO, SEM_FULL);
    if (SEM_ID_NULL == pEvdev->ioSem)
        {
        goto error;
        }

    bzero ((char *)devName, sizeof (devName));
    (void)snprintf ((char *)devName, EV_DEV_NAME_LEN, "%s%d",
                    EV_DEV_NAME_PREFIX, devIdx);

    if (ERROR == iosDevAdd ((DEV_HDR *)&pEvdev->devHdr, devName,
                            pEvdevCore->drvNum))
        {
        goto error;
        }

    selWakeupListInit (&pEvdev->selWakeupList);
    pEvdev->msgSize  = sizeof (EV_DEV_EVENT);
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */

    pEvdevCoreDev->node.next        = NULL;
    pEvdevCoreDev->node.previous    = NULL;
    pEvdevCoreDev->pEvdev           = pEvdev;
    dllAdd (pCoreData->pDevList, &pEvdevCoreDev->node);

    /* send hot plug message */

    msg.msgData.hotPlug.time.tv_sec = tickGet () / sysClkRateGet ();
    msg.msgData.hotPlug.time.tv_usec =
        (tickGet () * 1000000) / sysClkRateGet () -
        msg.msgData.hotPlug.time.tv_sec * 1000000;

    msg.msgType                     = EV_DEV_MSG_HOT_PLUG;
    msg.msgData.hotPlug.isPlugin    = TRUE;
    msg.msgData.hotPlug.devIdx      = devIdx;
    (void)evdevWrite (pEvdevCore, (char *)&msg, sizeof (EV_DEV_MSG));

    (void)taskSpawn ("evdevAutoEnDev", EV_DEV_IO_TASK_PRIO, 0,
                     EV_DEV_IO_TASK_STACK_SIZE, (FUNCPTR)evdevAutoEnDevTask,
                     (_Vx_usr_arg_t)pEvdev, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    (void)semGive (pEvdevCore->ioSem);
    return pEvdev;

error:
    (void)evdevIdxFree (devIdx);

    if (NULL != pEvdev)
        {
#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
        if (NULL != pEvdev->devHdr.name)
            {
            (void)iosDevDelete ((DEV_HDR *)pEvdev);
            }
        if (SEM_ID_NULL != pEvdev->ioSem)
            {
            (void)semDelete (pEvdev->ioSem);
            }
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */

        free (pEvdev);
        }

#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
    if (NULL != pEvdevCoreDev)
        {
        free (pEvdevCoreDev);
        }
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */

    (void)semGive (pEvdevCore->ioSem);
    return NULL;
    }

/*******************************************************************************
*
* evdevUnregDev - unregister an event device from the device list
*
* This routine unregisters an event device from the device list.
*
* RETURNS: OK when the device successfully unregistered; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS evdevUnregDev
    (
    EV_DEV_HANDLE * pEvdev
    )
    {
#ifdef _WRS_CONFIG_EVDEV_HOT_PLUG
    EV_DEV_LIB_CORE_DEV *   pDev        = NULL;
    DL_NODE *               pNode       = NULL;
    EV_DEV_HANDLE *         pEvdevCore  = NULL;
    EV_DEV_MSG              msg;
    EV_DEV_EVENT            event;

    if ((NULL == pCoreData) || (NULL == pEvdev))
        {
        return ERROR;
        }

    pEvdevCore = &pCoreData->evdev;
    if (OK != semTake (pEvdevCore->ioSem, EV_DEV_TIMEOUT))
        {
        return ERROR;
        }

    pNode = DLL_FIRST (pCoreData->pDevList);
    while (NULL != pNode)
        {
        pDev = (EV_DEV_LIB_CORE_DEV *)pNode;
        if (pEvdev == pDev->pEvdev)
            {
            if (SEM_ID_NULL != pEvdev->ioSem)
                {
                if (OK != semTake (pEvdev->ioSem, EV_DEV_TIMEOUT))
                    {
                    (void)semGive (pEvdevCore->ioSem);
                    return ERROR;
                    }

                (void)semDelete (pEvdev->ioSem);
                pEvdev->ioSem = SEM_ID_NULL;
                }

            pEvdev->isOpened = FALSE;
            if (NULL != pEvdev->devHdr.name)
                {
                (void)iosDevDelete ((DEV_HDR *)pEvdev);
                }

            if (MSG_Q_ID_NULL != pEvdev->eventQ)
                {
                while (msgQNumMsgs (pEvdev->eventQ) > 0)
                    {
                    (void)msgQReceive (pEvdev->eventQ, (char *)&event,
                                       sizeof (EV_DEV_EVENT), NO_WAIT);
                    }

                (void)msgQDelete (pEvdev->eventQ);
                pEvdev->eventQ = MSG_Q_ID_NULL;
                }

#ifdef _WRS_CONFIG_EVDEV_COMPATIBLE_MODE
            selWakeupListTerm (&pEvdev->selWakeupList);
#endif /* _WRS_CONFIG_EVDEV_COMPATIBLE_MODE */

            if (pEvdev->pDevData == pEvdevCore->pDevData)
                {
                pEvdevCore->pDevData = NULL;
                }

            /* send hot plug message */

            msg.msgData.hotPlug.time.tv_sec = tickGet () / sysClkRateGet ();
            msg.msgData.hotPlug.time.tv_usec =
                (tickGet () * 1000000) / sysClkRateGet () -
                msg.msgData.hotPlug.time.tv_sec * 1000000;

            msg.msgType                     = EV_DEV_MSG_HOT_PLUG;
            msg.msgData.hotPlug.isPlugin    = FALSE;
            msg.msgData.hotPlug.devIdx      = pEvdev->devIdx;

            (void)evdevWrite (pEvdevCore, (char *)&msg, sizeof (EV_DEV_MSG));

            dllRemove (pCoreData->pDevList, pNode);
            (void)evdevIdxFree (pEvdev->devIdx);
            free (pNode);
            free (pEvdev);
            break;
            }

        pNode = DLL_NEXT (pNode);
        }

    (void)semGive (pEvdevCore->ioSem);
    return OK;
#else /* _WRS_CONFIG_EVDEV_HOT_PLUG */
    return ERROR;
#endif /* _WRS_CONFIG_EVDEV_HOT_PLUG */
    }

/*******************************************************************************
*
* evdevSendMsg - send generic events to event queue
*
* This routine sends generic events to event queue.
*
* RETURNS: number of bytes sent
*
* ERRNO: N/A
*
* \NOMANUAL
*/

int evdevSendMsg
    (
    EV_DEV_HANDLE * pEvdev,     /* evdev handle */
    char *          buffer,     /* store data here */
    int             nBytes      /* store this much data */
    )
    {
    EV_DEV_HANDLE * pEvdevHandle;

    if (NULL == pCoreData)
        {
        return 0;
        }

    if (nBytes == sizeof (EV_DEV_EVENT))
        {
        pEvdevHandle = pEvdev;
        }
    else
        {
        pEvdevHandle = &pCoreData->evdev;
        }

    return evdevWrite (pEvdevHandle, buffer, nBytes);
    }

/*******************************************************************************
*
* evdevOpen - open the event device framework
*
* This routine opens the event device framework.
*
* RETURNS: Pointer to event device framework info structure when successfully
*          opened, ERROR otherwise
*
* ERRNO: N/A
*/

LOCAL void * evdevOpen
    (
    EV_DEV_HANDLE * pEvdev,     /* device descriptor */
    const char *    name,
    int             flags,
    int             mode
    )
    {
    /* check arguments */

    if ((NULL == pEvdev) || (0 != strlen (name)))
        {
        return (void *)ERROR;
        }

    if (OK != semTake (pEvdev->ioSem, EV_DEV_TIMEOUT))
        {
        return (void *)ERROR;
        }

    if (pEvdev->isOpened)
        {
        (void)semGive (pEvdev->ioSem);
        return (void *)ERROR;
        }
    else
        {
        pEvdev->eventQ = msgQCreate (pCoreData->msgNum, pEvdev->msgSize,
                                     MSG_Q_PRIORITY);
        if (MSG_Q_ID_NULL == pEvdev->eventQ)
            {
            (void)semGive (pEvdev->ioSem);
            return (void *)ERROR;
            }

        pEvdev->isOpened = TRUE;
        pEvdev->flags = flags;
        (void)semGive (pEvdev->ioSem);
        return (void *)pEvdev;
        }
    }

/*******************************************************************************
*
* evdevClose - close the event device framework
*
* This routine closes the event device framework.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS evdevClose
    (
    EV_DEV_HANDLE * pEvdev      /* device descriptor */
    )
    {
    char *  pMsg;

    /* check arguments */

    if (NULL == pEvdev)
        {
        return ERROR;
        }

    if (OK != semTake (pEvdev->ioSem, EV_DEV_TIMEOUT))
        {
        return ERROR;
        }

    if (!pEvdev->isOpened)
        {
        (void)semGive (pEvdev->ioSem);
        return ERROR;
        }
    else
        {
        pMsg = (char *) calloc (1, pEvdev->msgSize);
        if (NULL != pMsg)
            {
            while (msgQNumMsgs (pEvdev->eventQ) > 0)
                {
                (void)msgQReceive (pEvdev->eventQ, pMsg, pEvdev->msgSize,
                                   NO_WAIT);
                }
            free (pMsg);
            }

        (void)msgQDelete (pEvdev->eventQ);
        pEvdev->eventQ      = NULL;
        pEvdev->isOpened    = FALSE;
        pEvdev->flags       = 0;
        (void)semGive (pEvdev->ioSem);
        return OK;
        }
    }

/*******************************************************************************
*
* evdevRead - read generic event
*
* This routine reads a generic event, if one is available.
*
* RETURNS: number of bytes read
*
* ERRNO: N/A
*/

LOCAL int evdevRead
    (
    EV_DEV_HANDLE * pEvdev,     /* evdev handle */
    char *          buffer,     /* store data here */
    int             nBytes      /* store this much data */
    )
    {
    int count        = 0;
    int numBytesRead = 0;

    if ((NULL == pEvdev) || (NULL == buffer) || (!pEvdev->isOpened) ||
        (nBytes < pEvdev->msgSize))
        {
        return 0;
        }

    while (count < (nBytes / pEvdev->msgSize))
        {
        numBytesRead = (int)msgQReceive (pEvdev->eventQ, buffer,
                                         pEvdev->msgSize,
                                         ((pEvdev->flags & O_NONBLOCK) == O_NONBLOCK)?
                                         NO_WAIT:WAIT_FOREVER);

        if ((ERROR == numBytesRead) && (errno == S_objLib_OBJ_UNAVAILABLE))
            {
            numBytesRead = 0;
            break;
            }
        else if (numBytesRead != pEvdev->msgSize)
            {
            break;
            }

        count++;
        buffer += pEvdev->msgSize;
        }

    return (count * (int)pEvdev->msgSize);
    }

/*******************************************************************************
*
* evdevWrite - write generic events to event queue
*
* This routine writes generic events to event queue.
*
* RETURNS: number of bytes write
*
* ERRNO: N/A
*/

LOCAL int evdevWrite
    (
    EV_DEV_HANDLE * pEvdev,     /* evdev handle */
    char *          buffer,     /* store data here */
    int             nBytes      /* store this much data */
    )
    {
    int count = 0;
    int rc;

    if ((NULL == pEvdev) || (NULL == buffer) || (!pEvdev->isOpened) ||
        (nBytes < pEvdev->msgSize))
        {
        return 0;
        }

    while (count < (nBytes / pEvdev->msgSize))
        {
        rc = msgQSend (pEvdev->eventQ, buffer, pEvdev->msgSize, NO_WAIT,
                       MSG_PRI_NORMAL);

        if (ERROR == rc)
            {
            break;
            }

        count++;
        buffer += pEvdev->msgSize;
        }

    selWakeupAll (&(pEvdev->selWakeupList), SELREAD);
    return (count * (int)pEvdev->msgSize);
    }

/*******************************************************************************
*
* evdevIoctl - event device framework ioctl
*
* This routine handles the ioctl calls made by the upper level application. It
* provides the processing for a user level application that requires the
* services be performed at the kernel level.
*
* RETURNS: OK, if successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS evdevIoctl
    (
    EV_DEV_HANDLE * pEvdev,     /* device descriptor */
    int             request,    /* ioctl function */
    _Vx_usr_arg_t   arg         /* function arg */
    )
    {
    STATUS  result = OK;

    if (NULL == pEvdev)
        {
        return ERROR;
        }

    if (OK != semTake (pEvdev->ioSem, EV_DEV_TIMEOUT))
        {
        return ERROR;
        }

    switch (request)
        {
        case FIONREAD:
            {
            if (NULL == (int *)arg)
                {
                result = ERROR;
                break;
                }

            *((int *)arg) = (int)msgQNumMsgs (pEvdev->eventQ) *
                            (int)pEvdev->msgSize;
            }
            break;

        case FIOSELECT:
            if (NULL == (SEL_WAKEUP_NODE *)arg)
                {
                result = ERROR;
                break;
                }

            /* add node to wakeup list */

            if (ERROR == selNodeAdd (&pEvdev->selWakeupList,
                                     (SEL_WAKEUP_NODE *) arg))
                {
                result = ERROR;
                break;
                }

            if ((selWakeupType ((SEL_WAKEUP_NODE *) arg) == SELREAD)
                && (msgQNumMsgs (pEvdev->eventQ) > 0))
                {
                /* data available, make sure task does not pend */

                selWakeup ((SEL_WAKEUP_NODE *) arg);
                }
            result = OK;
            break;

        case FIOUNSELECT:
            if (NULL == (SEL_WAKEUP_NODE *)arg)
                {
                result = ERROR;
                break;
                }

            /* delete node from wakeup list */

            if (ERROR == selNodeDelete (&pEvdev->selWakeupList,
                                        (SEL_WAKEUP_NODE *) arg))
                {
                result = ERROR;
                }
            break;

        case EV_DEV_IO_GET_DEV_COUNT:
            {
            UINT32  devCount = 0;
            UINT32  idx;

            if (NULL == (UINT32 *)arg)
                {
                result = ERROR;
                break;
                }

            for (idx = 0; idx < EV_DEV_DEVICE_MAX; idx++)
                {
                if (pCoreData->idx & (1 << idx))
                    {
                    devCount++;
                    }
                }

            *((UINT32 *)arg) = devCount;
            }
            break;

        case EV_DEV_IO_SET_OPERATE_DEV:
            {
            EV_DEV_LIB_CORE_DEV *   pDev        = NULL;
            DL_NODE *               pNode       = NULL;
            EV_DEV_HANDLE *         pCoreHandle = &pCoreData->evdev;

            if (NULL == (UINT32 *)arg)
                {
                result = ERROR;
                break;
                }

            if (pCoreHandle == pEvdev)
                {
                pEvdev->pDevData    = NULL;
                result              = ERROR;

                pNode = DLL_FIRST (pCoreData->pDevList);
                while (NULL != pNode)
                    {
                    pDev = (EV_DEV_LIB_CORE_DEV *)pNode;
                    if (*((UINT32 *)arg) == pDev->pEvdev->devIdx)
                        {
                        pEvdev->pDevData = pDev->pEvdev->pDevData;
                        result = OK;
                        break;
                        }

                    pNode = DLL_NEXT (pNode);
                    }
                }
            }
            break;

        case EV_DEV_IO_GET_VERSION:
            if (NULL == (UINT32 *)arg)
                {
                result = ERROR;
                break;
                }

            *((UINT32 *)arg) = EV_DEV_VERSION;
            break;

        case EV_DEV_IO_GET_INFO:
            {
            EV_DEV_DEVICE_INFO *    pInfo = (EV_DEV_DEVICE_INFO *)arg;

            if (NULL == pInfo)
                {
                result = ERROR;
                break;
                }

            if (NULL != pEvdev->pDevData)
                {
                pInfo->bustype  = pEvdev->pDevData->devInfo.bustype;
                pInfo->vendor   = pEvdev->pDevData->devInfo.vendor;
                pInfo->product  = pEvdev->pDevData->devInfo.product;
                pInfo->version  = pEvdev->pDevData->devInfo.version;
                }
            else
                {
                result = ERROR;
                }
            }
            break;

        case EV_DEV_IO_GET_NAME:
            {
            if (NULL == (char *)arg)
                {
                result = ERROR;
                break;
                }

            if (NULL != pEvdev->pDevData)
                {
                memcpy ((void *)arg, (const void *)pEvdev->pDevData->pDevName,
                        (strlen ((const char *)pEvdev->pDevData->pDevName) +
                         1));
                }
            else
                {
                result = ERROR;
                }
            }
            break;

        case EV_DEV_IO_GET_CAP:
            {
            if (NULL == (UINT32 *)arg)
                {
                result = ERROR;
                break;
                }

            if (NULL != pEvdev->pDevData)
                {
                *((UINT32 *)arg) = pEvdev->pDevData->devCap;
                }
            else
                {
                result = ERROR;
                }
            }
            break;

        default:
            {
            if (NULL != pEvdev->pDevData)
                {
                result = pEvdev->pDevData->ioctl (pEvdev->pDevData->pArg,
                                                  request, arg);
                }
            else
                {
                result = ERROR;
                }
            }
        }

    (void)semGive (pEvdev->ioSem);

    return result;
    }

/*******************************************************************************
*
* evdevIdxAlloc - allocate a global device index
*
* This routine allocates a global device index.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS evdevIdxAlloc
    (
    UINT32 *    pIdx
    )
    {
    UINT32  idx;

    for (idx = 0; idx < EV_DEV_DEVICE_MAX; idx++)
        {
        if (!(pCoreData->idx & (1 << idx)))
            {
            pCoreData->idx |= (1 << idx);
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
* evdevIdxFree - free a global device index
*
* This routine frees a global device index.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void evdevIdxFree
    (
    UINT32  idx
    )
    {
    pCoreData->idx &= ~(1 << idx);
    }

/*******************************************************************************
*
* evdevAutoEnDevTask - automatically enable input device
*
* This routine automatically enables input device.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS evdevAutoEnDevTask
    (
    EV_DEV_HANDLE * pEvdev
    )
    {
    EV_DEV_HANDLE *         pEvdevCore  = NULL;
    EV_DEV_LIB_CORE_DEV *   pDev        = NULL;
    DL_NODE *               pNode       = NULL;

    if ((NULL == pCoreData) || (NULL == pEvdev))
        {
        return ERROR;
        }

    pEvdevCore = &pCoreData->evdev;
    if (OK != semTake (pEvdevCore->ioSem, EV_DEV_TIMEOUT))
        {
        return ERROR;
        }

    pNode = DLL_FIRST (pCoreData->pDevList);
    while (NULL != pNode)
        {
        pDev = (EV_DEV_LIB_CORE_DEV *)pNode;
        if (pEvdev == pDev->pEvdev)
            {
            if (SEM_ID_NULL != pEvdev->ioSem)
                {
                if (OK != semTake (pEvdev->ioSem, EV_DEV_TIMEOUT))
                    {
                    (void)semGive (pEvdev->ioSem);
                    return ERROR;
                    }
                }

            if ((NULL != pEvdev->pDevData) && (NULL != pEvdev->pDevData->ioctl))
                {
                 /* wait for device ready */

                (void)taskDelay ((sysClkRateGet () / 10));

                (void)pEvdev->pDevData->ioctl (pEvdev->pDevData->pArg,
                                               EV_DEV_IO_DEV_EN, 0);
                }

            if (SEM_ID_NULL != pEvdev->ioSem)
                {
                (void)semGive (pEvdev->ioSem);
                }

            break;
            }

        pNode = DLL_NEXT (pNode);
        }

    (void)semGive (pEvdevCore->ioSem);
    return OK;
    }

#ifndef _WRS_CONFIG_EVDEV_HOT_PLUG
/*******************************************************************************
*
* evdevDisRegTask - disable register function
*
* This routine disables register function.
*
* RETURNS: always OK
*
* ERRNO: N/A
*/

LOCAL STATUS evdevDisRegTask (void)
    {
    (void)taskDelay (EV_DEV_TIMEOUT);
    pCoreData->regEn = FALSE;
    return OK;
    }
#endif /* _WRS_CONFIG_EVDEV_HOT_PLUG */
