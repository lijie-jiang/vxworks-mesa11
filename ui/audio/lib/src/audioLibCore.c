/* audioLibCore.c - Audio Driver Framework Core Library */

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
15sep15,c_l  support non-blocking audio device (V7GFX-252)
02sep14,y_f  allowed register audio codec earlier (V7GFX-208)
22may14,y_f  fixed setting audio path error (V7GFX-163)
20may14,y_f  fixed 32bit read and write error (V7GFX-155)
30oct13,y_f  written (US18166)
*/

/*
DESCRIPTION
This file provides an audio driver framework that manages all audio devices.
*/

/* includes */

#include <audioLibCore.h>
#include <cacheLib.h>
#include <iosLib.h>

/* defines */

#define AUDIO_TIMEOUT                       5 /* 5 sec */
#define AUDIO_TASK_PRIORITY                 (90)
#define AUDIO_TASK_STACK_SIZE               (1024 * 4)
#define AUDIO_DEFAULT_BUFFER_NUM            (2)
#define AUDIO_DEFAULT_BUFFER_TIME           (2000) /* buffer time is 2s */

#define DATA_CONVERT(srcType, destType)                         \
if (srcChan > destChan)                                         \
    {                                                           \
    skipChan = (UINT8)(srcChan - destChan);                     \
    if (srcShiftBits > destShiftBits)                           \
        {                                                       \
        shiftBits = (UINT8)(srcShiftBits - destShiftBits);      \
        while (copyFrames-- > 0)                                \
            {                                                   \
            for (i = 0; i < destChan; i++)                      \
                {                                               \
                *pDest++ = (destType)((*pSrc++) >> shiftBits);  \
                }                                               \
                                                                \
            pSrc += skipChan;                                   \
            }                                                   \
        }                                                       \
    else if (srcShiftBits < destShiftBits)                      \
        {                                                       \
        shiftBits = (UINT8)(destShiftBits - srcShiftBits);      \
        while (copyFrames-- > 0)                                \
            {                                                   \
            for (i = 0; i < destChan; i++)                      \
                {                                               \
                *pDest++ = (destType)((*pSrc++) << shiftBits);  \
                }                                               \
                                                                \
            pSrc += skipChan;                                   \
            }                                                   \
        }                                                       \
    else                                                        \
        {                                                       \
        while (copyFrames-- > 0)                                \
            {                                                   \
            for (i = 0; i < destChan; i++)                      \
                {                                               \
                *pDest++ = (destType)(*pSrc++);                 \
                }                                               \
                                                                \
            pSrc += skipChan;                                   \
            }                                                   \
        }                                                       \
    }                                                           \
else if (srcChan < destChan)                                    \
    {                                                           \
    if (srcShiftBits > destShiftBits)                           \
        {                                                       \
        shiftBits = (UINT8)(srcShiftBits - destShiftBits);      \
        while (copyFrames-- > 0)                                \
            {                                                   \
            for (i = 0; i < srcChan; i++)                       \
                {                                               \
                tempValue = (destType)((*pSrc++) >> shiftBits); \
                *pDest++ = tempValue;                           \
                }                                               \
                                                                \
            for (i = srcChan; i < destChan; i++)                \
                {                                               \
                *pDest++ = tempValue;                           \
                }                                               \
            }                                                   \
        }                                                       \
    else if (srcShiftBits < destShiftBits)                      \
        {                                                       \
        shiftBits = (UINT8)(destShiftBits - srcShiftBits);      \
        while (copyFrames-- > 0)                                \
            {                                                   \
            for (i = 0; i < srcChan; i++)                       \
                {                                               \
                tempValue = (destType)((*pSrc++) << shiftBits); \
                *pDest++ = tempValue;                           \
                }                                               \
                                                                \
            for (i = srcChan; i < destChan; i++)                \
                {                                               \
                *pDest++ = tempValue;                           \
                }                                               \
            }                                                   \
        }                                                       \
    else                                                        \
        {                                                       \
        while (copyFrames-- > 0)                                \
            {                                                   \
            for (i = 0; i < srcChan; i++)                       \
                {                                               \
                tempValue = (destType)(*pSrc++);                \
                *pDest++ = tempValue;                           \
                }                                               \
                                                                \
            for (i = srcChan; i < destChan; i++)                \
                {                                               \
                *pDest++ = tempValue;                           \
                }                                               \
            }                                                   \
        }                                                       \
    }                                                           \
else                                                            \
    {                                                           \
    copyFrames *= srcChan;                                      \
                                                                \
    if (srcShiftBits > destShiftBits)                           \
        {                                                       \
        shiftBits = (UINT8)(srcShiftBits - destShiftBits);      \
        while (copyFrames-- > 0)                                \
            {                                                   \
            *pDest++ = (destType)((*pSrc++) >> shiftBits);      \
            }                                                   \
        }                                                       \
    else if (srcShiftBits < destShiftBits)                      \
        {                                                       \
        shiftBits = (UINT8)(destShiftBits - srcShiftBits);      \
        while (copyFrames-- > 0)                                \
            {                                                   \
            *pDest++ = (destType)((*pSrc++) << shiftBits);      \
            }                                                   \
        }                                                       \
    else                                                        \
        {                                                       \
        if (sizeof (srcType) == sizeof (destType))              \
            {                                                   \
            bcopy ((char *)pSrc, (char *)pDest,                 \
                   copyFrames * sizeof (destType));             \
            }                                                   \
        else                                                    \
            {                                                   \
            while (copyFrames-- > 0)                            \
                {                                               \
                *pDest++ = (destType)(*pSrc++);                 \
                }                                               \
            }                                                   \
        }                                                       \
    }

/* debug */

#undef AUDIO_CORE_DEBUG_ON
#ifdef AUDIO_CORE_DEBUG_ON

LOCAL int audioCoreDebugLevel = 500;

#   ifndef AUDIO_CORE_DBG_MSG
#       define AUDIO_CORE_DBG_MSG(level,fmt,a,b,c,d,e,f)  \
            if (audioCoreDebugLevel >= level) (void)kprintf (fmt,a,b,c,d,e,f)
#   endif

#else

#   define AUDIO_CORE_DBG_MSG(level,fmt,a,b,c,d,e,f)

#endif /* AUDIO_CORE_DEBUG_ON */

/* typedefs */

typedef enum audioFlag
    {
    AUDIO_FLAG_NONBLOCK = 0,
    AUDIO_FLAG_BLOCK
    } AUDIO_FLAG;
    
typedef enum audioState
    {
    AUDIO_STATE_IDLE = 0,
    AUDIO_STATE_ACTIVE
    } AUDIO_STATE;

typedef struct audioBufferInfo
    {
    UINT8 *         pBuf;       /* start address of the buffer */
    ssize_t         size;       /* capacity of this buffer */
    ssize_t         usedSize;   /* valid data size of this buffer*/
    AUDIO_DATA_INFO dataInfo;
    } AUDIO_BUFFER_INFO;

typedef struct audioChanInfo
    {
    AUDIO_BUFFER_INFO * pBufInfo;       /* buffer header array */
    MSG_Q_ID            downStreamMsg;  /* empty buffer from IO to device */
    MSG_Q_ID            upStreamMsg;    /* sampling from device to IO */
    AUDIO_STATE         chanState;
    AUDIO_VOLUME        volume;
    } AUDIO_CHAN_INFO;

typedef struct audioData
    {
    DEV_HDR         devHdr;
    SEM_ID          ioSem;
    BOOL            isOpened;
    TASK_ID         sendTaskId;
    TASK_ID         recvTaskId;
    AUDIO_DATA_INFO dataInfo;
    AUDIO_CHAN_INFO outChanInfo;
    AUDIO_CHAN_INFO inChanInfo;
    AUDIO_DEV *     pCodec;
    AUDIO_DEV *     pTrans;
    UINT32          bufTime;
    UINT32          curPaths;
    AUDIO_STATE     devState;
    AUDIO_FLAG      devFlag;   
    } AUDIO_DATA;

typedef struct audioLibCoreData
    {
    UINT32      bufNum;
    UINT32      bufTime;
    DL_LIST *   pCodecList;
    DL_LIST *   pTransList;
    SEM_ID      ioSem;
    }AUDIO_LIB_CORE_DATA;

/* forward declarations */

LOCAL void *    audioCoreOpen (AUDIO_DATA * pDev, const char * name, int flag, int mode);
LOCAL int       audioCoreClose (AUDIO_DATA * pDev);
LOCAL ssize_t   audioCoreRead (AUDIO_DATA * pDev, UINT8 * pBuffer, size_t
                               maxBytes);
LOCAL ssize_t   audioCoreWrite (AUDIO_DATA * pDev, UINT8 * pBuffer, size_t
                                nbytes);
LOCAL STATUS    audioCoreIoctl (AUDIO_DATA * pDev, int function,
                                AUDIO_IOCTL_ARG * pIoctlArg);
LOCAL STATUS    audioCoreIoctlDev (AUDIO_DATA * pDev, AUDIO_IO_CTRL function,
                                   AUDIO_IOCTL_ARG * pIoctlArg);
LOCAL STATUS    audioCoreIoctlChan (AUDIO_DATA * pDev, AUDIO_IO_CTRL function,
                                    AUDIO_IOCTL_ARG * pIoctlArg);
LOCAL STATUS    audioCoreChanStart (AUDIO_DATA * pDev, AUDIO_CHAN_INFO * pChan);
LOCAL void      audioCoreChanStop (AUDIO_DATA * pDev, AUDIO_CHAN_INFO * pChan);
LOCAL STATUS    audioCoreAllocBuffer (AUDIO_CHAN_INFO * pChanInfo, ssize_t
                                      bufSize);
LOCAL void      audioCoreFreeBuffer (AUDIO_CHAN_INFO * pChanInfo);
LOCAL void      audioCoreConvert (AUDIO_BUFFER_INFO * pSrcBufInfo,
                                  AUDIO_BUFFER_INFO * pDestBufInfo);
LOCAL void      audioCoreConvert24to24 (AUDIO_BUFFER_INFO * pSrcBufInfo,
                                        AUDIO_BUFFER_INFO * pDestBufInfo, UINT32
                                        copyFrames);
LOCAL void      audioCoreConvert24to32 (AUDIO_BUFFER_INFO * pSrcBufInfo,
                                        AUDIO_BUFFER_INFO * pDestBufInfo, UINT8
                                        destShiftBits, UINT32 copyFrames);
LOCAL void      audioCoreConvert32to24 (AUDIO_BUFFER_INFO * pSrcBufInfo,
                                        AUDIO_BUFFER_INFO * pDestBufInfo, UINT8
                                        srcShiftBits, UINT32 copyFrames);
LOCAL STATUS    audioCoreDevIdxAlloc (UINT32 * pIdx);
LOCAL void      audioCoreDevIdxFree (UINT32 idx);
LOCAL void      audioCoreSendTask (AUDIO_DATA * pDev);
LOCAL void      audioCoreRecvTask (AUDIO_DATA * pDev);
LOCAL void      audioCoreInitMsgQ (AUDIO_CHAN_INFO * pChanInfo, MSG_Q_ID msgQ);
LOCAL void      audioCoreClrMsgQ (AUDIO_CHAN_INFO * pChanInfo, MSG_Q_ID msgQ);

/* locals */

LOCAL UINT32                audioDevIdx = 0;
LOCAL AUDIO_LIB_CORE_DATA * pAudioHd    = NULL;
LOCAL DL_LIST               audioCodecList = {NULL, NULL};

/* functions */

/*******************************************************************************
*
* audioCoreInit - install the audio device framework
*
* This routine installs the audio device framework.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void audioCoreInit
    (
    UINT32  bufNum,
    UINT32  bufTime
    )
    {
    AUDIO_LIB_CORE_DATA *   pAudioCore  = NULL;

    if (0 == bufNum)
        {
        bufNum = AUDIO_DEFAULT_BUFFER_NUM;
        }

    if (0 == bufTime)
        {
        bufTime = AUDIO_DEFAULT_BUFFER_TIME;
        }

    pAudioCore = (AUDIO_LIB_CORE_DATA *)calloc (1,
        sizeof (AUDIO_LIB_CORE_DATA));

    if (NULL == pAudioCore)
        {
        return;
        }

    pAudioCore->bufNum      = bufNum;
    pAudioCore->bufTime     = bufTime;
    pAudioCore->pCodecList  = &audioCodecList;
    pAudioCore->pTransList  = dllCreate ();
    if (NULL == pAudioCore->pTransList)
        {
        goto error;
        }

    pAudioCore->ioSem = semBCreate (SEM_Q_FIFO, SEM_FULL);
    if (SEM_ID_NULL == pAudioCore->ioSem)
        {
        goto error;
        }

    pAudioHd = pAudioCore;
    return;

error:
    if (SEM_ID_NULL != pAudioCore->ioSem)
        {
        (void)semDelete (pAudioCore->ioSem);
        }

    if (NULL != pAudioCore->pTransList)
        {
        (void)dllDelete (pAudioCore->pTransList);
        }

    free (pAudioCore);
    }

/*******************************************************************************
*
* audioCoreRegCodec - register a codec device to codecs list
*
* This routine registers a codec device to codecs list.
*
* RETURNS: OK, or ERROR if the device cannot be installed
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS audioCoreRegCodec
    (
    AUDIO_DEV * pCodecsInfo
    )
    {
    AUDIO_LIB_CORE_DATA *   pAudioCore  = pAudioHd;
    DL_NODE *               pNode;
    DL_LIST *               pCodecList  = &audioCodecList;

    if ((NULL == pCodecsInfo) || (0 == strlen (pCodecsInfo->devInfo.name)) ||
        (NULL == pCodecsInfo->ioctl))
        {
        return ERROR;
        }

    if (NULL != pAudioCore)
        {
        if (OK != semTake (pAudioCore->ioSem,
                           (sysClkRateGet () * AUDIO_TIMEOUT)))
            {
            return ERROR;
            }
        }

    /* check whether the codec device has been registered before */

    pNode = DLL_FIRST (pCodecList);
    while (NULL != pNode)
        {
        if (pCodecsInfo == (AUDIO_DEV *)pNode)
            {
            if (NULL != pAudioCore)
                {
                (void)semGive (pAudioCore->ioSem);
                }
            return OK;
            }

        pNode = DLL_NEXT (pNode);
        }

    pCodecsInfo->node.next      = NULL;
    pCodecsInfo->node.previous  = NULL;
    pCodecsInfo->attTransUnit   = AUDIO_DEV_MAX;
    dllAdd (pCodecList, &pCodecsInfo->node);

    if (NULL != pAudioCore)
        {
        (void)semGive (pAudioCore->ioSem);
        }

    return OK;
    }

/*******************************************************************************
*
* audioCoreUnregCodec - unregister a codec device from the codecs list
*
* This routine unregisters a codec device from the codecs list.
*
* RETURNS: OK when the device successfully unregistered; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS audioCoreUnregCodec
    (
    AUDIO_DEV * pCodecsInfo
    )
    {
    AUDIO_LIB_CORE_DATA *   pAudioCore  = pAudioHd;
    AUDIO_DEV *             pTransInfo  = NULL;
    STATUS                  result      = ERROR;
    DL_NODE *               pCodecNode;
    DL_NODE *               pTransNode;
    DL_LIST *               pCodecList  = &audioCodecList;

    if (NULL == pCodecsInfo)
        {
        return result;
        }

    if (NULL != pAudioCore)
        {
        if (OK != semTake (pAudioCore->ioSem,
                           (sysClkRateGet () * AUDIO_TIMEOUT)))
            {
            return ERROR;
            }
        }

    pCodecNode = DLL_FIRST (pCodecList);
    while (NULL != pCodecNode)
        {
        if (pCodecsInfo == (AUDIO_DEV *)pCodecNode)
            {
            dllRemove (pCodecList, pCodecNode);

            if ((pCodecsInfo->attTransUnit < AUDIO_DEV_MAX) &&
                (NULL != pAudioCore))
                {
                /* check whether the transceiver has been registered before */

                pTransNode = DLL_FIRST (pAudioCore->pTransList);
                while (NULL != pTransNode)
                    {
                    pTransInfo  = (AUDIO_DEV *)pTransNode;
                    if (pCodecsInfo->attTransUnit == pTransInfo->unit)
                        {
                        (void)semGive (pAudioCore->ioSem);
                        (void)audioCoreUnregTransceiver (pTransInfo);
                        return OK;
                        }

                    pTransNode = DLL_NEXT (pTransNode);
                    }
                }

            result = OK;
            break;
            }

        pCodecNode = DLL_NEXT (pCodecNode);
        }

    if (NULL != pAudioCore)
        {
        (void)semGive (pAudioCore->ioSem);
        }

    return result;
    }

/*******************************************************************************
*
* audioCoreRegTransceiver - register an audio transceiver to I/O system
*
* This routine registers an audio transceiver to I/O system.
* For registering successfully, please make sure the required codec device has
* been registered to codecs list.
*
* RETURNS: OK when device successfully registered; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS audioCoreRegTransceiver
    (
    AUDIO_DEV * pTransceiverInfo,
    int         codecUnit
    )
    {
    int                     drvNum;
    char                    devName[AUDIO_DEV_NAME_LEN];
    DL_NODE *               pNode;
    AUDIO_DEV *             pCodecInfo  = NULL;
    AUDIO_DATA *            pAudio      = NULL;
    AUDIO_LIB_CORE_DATA *   pAudioCore  = pAudioHd;

    if ((NULL == pAudioCore) || (NULL == pTransceiverInfo) ||
        (0 == strlen (pTransceiverInfo->devInfo.name)) ||
        (NULL == pTransceiverInfo->ioctl))
        {
        return ERROR;
        }

    if (OK != semTake (pAudioCore->ioSem, (sysClkRateGet () * AUDIO_TIMEOUT)))
        {
        return ERROR;
        }

    /* check whether the transceiver has been registered before */

    pNode = DLL_FIRST (pAudioCore->pTransList);
    while (NULL != pNode)
        {
        if (pTransceiverInfo == (AUDIO_DEV *)pNode)
            {
            (void)semGive (pAudioCore->ioSem);
            return OK;
            }

        pNode = DLL_NEXT (pNode);
        }

    pAudio = (AUDIO_DATA *)calloc (1, sizeof (AUDIO_DATA));
    if (NULL == pAudio)
        {
        (void)semGive (pAudioCore->ioSem);
        return ERROR;
        }

    if (ERROR == audioCoreDevIdxAlloc (&pTransceiverInfo->unit))
        {
        free (pAudio);
        (void)semGive (pAudioCore->ioSem);
        return ERROR;
        }

    pAudio->pTrans      = pTransceiverInfo;
    pAudio->sendTaskId  = TASK_ID_ERROR;
    pAudio->recvTaskId  = TASK_ID_ERROR;

    if (codecUnit >= 0)
        {
        /* find the required codec from codecs list */

        pNode = DLL_FIRST (pAudioCore->pCodecList);
        while (NULL != pNode)
            {
            pCodecInfo = (AUDIO_DEV *)pNode;
            if (pCodecInfo->unit == codecUnit)
                {
                pAudio->pCodec                  = pCodecInfo;
                pAudio->pCodec->attTransUnit    = pTransceiverInfo->unit;
                break;
                }
            pNode = DLL_NEXT (pNode);
            }

        if (NULL == pAudio->pCodec)
            {
            goto error;
            }
        }

    pAudio->ioSem = semBCreate (SEM_Q_FIFO, SEM_FULL);
    if (SEM_ID_NULL == pAudio->ioSem)
        {
        goto error;
        }

    bzero ((char *)devName, sizeof (devName));
    (void)snprintf ((char *)devName, AUDIO_DEV_NAME_LEN, "%s%d",
                    AUDIO_DEV_PREFIX, pTransceiverInfo->unit);

    /* install the driver */

    drvNum = iosDrvInstall (NULL,                           /* creat() */
                            NULL,                           /* remove() */
                            (DRV_OPEN_PTR)audioCoreOpen,    /* open() */
                            (DRV_CLOSE_PTR)audioCoreClose,  /* close() */
                            (DRV_READ_PTR)audioCoreRead,    /* read() */
                            (DRV_WRITE_PTR)audioCoreWrite,  /* write() */
                            (DRV_IOCTL_PTR)audioCoreIoctl); /* ioctl() */

    if (ERROR == drvNum)
        {
        goto error;
        }

    if (ERROR == iosDevAdd ((DEV_HDR *)&pAudio->devHdr, devName, drvNum))
        {
        goto error;
        }

    if (NULL != pAudio->pTrans->write)
        {
        pAudio->outChanInfo.downStreamMsg =
            msgQCreate (pAudioCore->bufNum, sizeof (char *),
                        MSG_Q_PRIORITY);
        if (MSG_Q_ID_NULL == pAudio->outChanInfo.downStreamMsg)
            {
            goto error;
            }

        pAudio->outChanInfo.upStreamMsg =
            msgQCreate (pAudioCore->bufNum, sizeof (char *),
                        MSG_Q_PRIORITY);
        if (MSG_Q_ID_NULL == pAudio->outChanInfo.upStreamMsg)
            {
            goto error;
            }

        bzero ((char *)devName, sizeof (devName));
        (void)snprintf (devName, AUDIO_DEV_NAME_LEN, "audio%dSend",
                        pTransceiverInfo->unit);
        pAudio->sendTaskId = taskSpawn (devName, AUDIO_TASK_PRIORITY, 0,
                                        AUDIO_TASK_STACK_SIZE,
                                        (FUNCPTR) audioCoreSendTask,
                                        (_Vx_usr_arg_t)pAudio,
                                        0, 0, 0, 0, 0, 0, 0, 0, 0);
        if (TASK_ID_ERROR == pAudio->sendTaskId)
            {
            goto error;
            }
        }

    if (NULL != pAudio->pTrans->read)
        {
        pAudio->inChanInfo.downStreamMsg =
            msgQCreate (pAudioCore->bufNum, sizeof (char *),
                        MSG_Q_PRIORITY);
        if (MSG_Q_ID_NULL == pAudio->inChanInfo.downStreamMsg)
            {
            goto error;
            }

        pAudio->inChanInfo.upStreamMsg =
            msgQCreate (pAudioCore->bufNum, sizeof (char *),
                        MSG_Q_PRIORITY);
        if (MSG_Q_ID_NULL == pAudio->inChanInfo.upStreamMsg)
            {
            goto error;
            }

        bzero ((char *)devName, sizeof (devName));
        (void)snprintf (devName, AUDIO_DEV_NAME_LEN, "audio%dRecv",
                        pTransceiverInfo->unit);
        pAudio->recvTaskId = taskSpawn (devName, AUDIO_TASK_PRIORITY, 0,
                                        AUDIO_TASK_STACK_SIZE,
                                        (FUNCPTR) audioCoreRecvTask,
                                        (_Vx_usr_arg_t)pAudio,
                                        0, 0, 0, 0, 0, 0, 0, 0, 0);
        if (TASK_ID_ERROR == pAudio->recvTaskId)
            {
            goto error;
            }
        }

    pTransceiverInfo->node.next     = NULL;
    pTransceiverInfo->node.previous = NULL;
    dllAdd (pAudioCore->pTransList, &pTransceiverInfo->node);

    (void)semGive (pAudioCore->ioSem);
    return OK;

error:
    if (NULL != pAudio->devHdr.name)
        {
        (void)iosDevDelete ((DEV_HDR *)pAudio);
        }
    if (MSG_Q_ID_NULL != pAudio->outChanInfo.downStreamMsg)
        {
        (void)msgQDelete (pAudio->outChanInfo.downStreamMsg);
        }
    if (MSG_Q_ID_NULL != pAudio->outChanInfo.upStreamMsg)
        {
        (void)msgQDelete (pAudio->outChanInfo.upStreamMsg);
        }
    if (MSG_Q_ID_NULL != pAudio->inChanInfo.downStreamMsg)
        {
        (void)msgQDelete (pAudio->inChanInfo.downStreamMsg);
        }
    if (MSG_Q_ID_NULL != pAudio->inChanInfo.upStreamMsg)
        {
        (void)msgQDelete (pAudio->inChanInfo.upStreamMsg);
        }
    if (TASK_ID_ERROR != pAudio->sendTaskId)
        {
        (void)taskDelete (pAudio->sendTaskId);
        }
    if (TASK_ID_ERROR != pAudio->recvTaskId)
        {
        (void)taskDelete (pAudio->recvTaskId);
        }
    if (SEM_ID_NULL != pAudio->ioSem)
        {
        (void)semDelete (pAudio->ioSem);
        }
    if (NULL != pAudio->pCodec)
        {
        pAudio->pCodec->attTransUnit = AUDIO_DEV_MAX;
        }

    audioCoreDevIdxFree (pTransceiverInfo->unit);
    free (pAudio);
    (void)semGive (pAudioCore->ioSem);
    return ERROR;
    }

/*******************************************************************************
*
* audioCoreUnregTransceiver - unregister an audio transceiver from list
*
* This routine unregisters  an audio transceiver from list.
*
* RETURNS: OK when the device successfully unregistered; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS audioCoreUnregTransceiver
    (
    AUDIO_DEV * pTransceiverInfo
    )
    {
    char                    devName[AUDIO_DEV_NAME_LEN];
    DL_NODE *               pNode;
    AUDIO_LIB_CORE_DATA *   pAudioCore  = pAudioHd;
    AUDIO_DATA *            pAudio      = NULL;
    AUDIO_DEV *             pTransInfo  = NULL;
    char *                  pTail;

    if ((NULL == pAudioCore) || (NULL == pTransceiverInfo) ||
        (pTransceiverInfo->unit >= AUDIO_DEV_MAX))
        {
        return ERROR;
        }

    if (OK != semTake (pAudioCore->ioSem, (sysClkRateGet () * AUDIO_TIMEOUT)))
        {
        return ERROR;
        }

    /* check whether the transceiver has been registered before */

    pNode = DLL_FIRST (pAudioCore->pTransList);
    while (NULL != pNode)
        {
        if (pTransceiverInfo == (AUDIO_DEV *)pNode)
            {
            pTransInfo  = pTransceiverInfo;
            break;
            }

        pNode = DLL_NEXT (pNode);
        }

    if (NULL == pTransInfo)
        {
        (void)semGive (pAudioCore->ioSem);
        return ERROR;
        }

    bzero ((char *)devName, sizeof (devName));
    (void)snprintf ((char *)devName, AUDIO_DEV_NAME_LEN, "%s%d",
                    AUDIO_DEV_PREFIX, pTransceiverInfo->unit);

    pAudio = (AUDIO_DATA *)iosDevFind (devName, (const char **)&pTail);

    if ((NULL != pAudio) && (0 == strcmp (pTail, "")))
        {
        if (pAudio->isOpened)
            {            
            (void)audioCoreClose (pAudio);
            }

        (void)iosDevDelete ((DEV_HDR *)pAudio);

        if (MSG_Q_ID_NULL != pAudio->outChanInfo.downStreamMsg)
            {
            (void)msgQDelete (pAudio->outChanInfo.downStreamMsg);
            }
        if (MSG_Q_ID_NULL != pAudio->outChanInfo.upStreamMsg)
            {
            (void)msgQDelete (pAudio->outChanInfo.upStreamMsg);
            }
        if (MSG_Q_ID_NULL != pAudio->inChanInfo.downStreamMsg)
            {
            (void)msgQDelete (pAudio->inChanInfo.downStreamMsg);
            }
        if (MSG_Q_ID_NULL != pAudio->inChanInfo.upStreamMsg)
            {
            (void)msgQDelete (pAudio->inChanInfo.upStreamMsg);
            }
        if (TASK_ID_ERROR != pAudio->sendTaskId)
            {
            (void)taskDelete (pAudio->sendTaskId);
            }
        if (TASK_ID_ERROR != pAudio->recvTaskId)
            {
            (void)taskDelete (pAudio->recvTaskId);
            }
        if (SEM_ID_NULL != pAudio->ioSem)
            {
            (void)semDelete (pAudio->ioSem);
            }
        if (NULL != pAudio->pCodec)
            {
            pAudio->pCodec->attTransUnit = AUDIO_DEV_MAX;
            }

        audioCoreDevIdxFree (pTransceiverInfo->unit);
        free (pAudio);
        dllRemove (pAudioCore->pTransList, (DL_NODE *)pTransInfo);
        (void)semGive (pAudioCore->ioSem);
        return OK;
        }
    else
        {
        (void)semGive (pAudioCore->ioSem);
        return ERROR;
        }
    }

/*******************************************************************************
*
* audioCoreOpen - open the audio device
*
* This routine open the audio device.
*
* RETURNS: audio device structure when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL void * audioCoreOpen
    (
    AUDIO_DATA *    pDev,
    const char *    name,
    int             flag,
    int             mode
    )
    {
    AUDIO_LIB_CORE_DATA *   pAudioCore  = pAudioHd;
    AUDIO_IOCTL_ARG         ioctlArg;

    if ((NULL == pAudioCore) || (0 != strlen (name)))
        {
        return (void *)ERROR;
        }

    /* wait for device to become available */

    if (OK != semTake (pDev->ioSem, (sysClkRateGet () * AUDIO_TIMEOUT)))
        {
        return (void *)ERROR;
        }

    if (pDev->isOpened)
        {        
        (void)semGive (pDev->ioSem);
        return (void *)ERROR;
        }

    if ((NULL != pDev->pCodec) && (NULL != pDev->pCodec->open))
        {
        if (ERROR == pDev->pCodec->open (pDev->pCodec->extension))
            {
            (void)semGive (pDev->ioSem);
            return (void *)ERROR;
            }
        }

    if (NULL != pDev->pTrans->open)
        {
        if (ERROR == pDev->pTrans->open (pDev->pTrans->extension))
            {
            if ((NULL != pDev->pCodec) && (NULL != pDev->pCodec->close))
                {
                (void)pDev->pCodec->close (pDev->pCodec->extension);
                }

            (void)semGive (pDev->ioSem);
            return (void *)ERROR;
            }
        }

    pDev->devState  = AUDIO_STATE_IDLE;
    pDev->bufTime   = pAudioCore->bufTime;
    if (NULL != pDev->pCodec)
        {
        pDev->curPaths = pDev->pCodec->devInfo.defPaths;
        }
    else
        {
        pDev->curPaths = pDev->pTrans->devInfo.defPaths;
        }

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.path = pDev->curPaths;
    if (ERROR == audioCoreIoctlDev (pDev, AUDIO_SET_PATH, &ioctlArg))
        {
        (void)semGive (pDev->ioSem);
        return (void *)ERROR;
        }

    bzero ((char *)&pDev->dataInfo, sizeof (AUDIO_DATA_INFO));
    if (flag & O_NONBLOCK) 
        {
        pDev->devFlag = AUDIO_FLAG_NONBLOCK;
        }     
    else
        {
        pDev->devFlag = AUDIO_FLAG_BLOCK;
        }
        
    pDev->isOpened = TRUE;    
    (void)semGive (pDev->ioSem);
    return (void *)pDev;
    }

/*******************************************************************************
*
* audioCoreClose - close audio device
*
* This routine close audio device.
*
* RETURNS: the OK when successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL int audioCoreClose
    (
    AUDIO_DATA *    pDev
    )
    {
    /* wait for device to become available */

    if (OK != semTake (pDev->ioSem, (sysClkRateGet () * AUDIO_TIMEOUT)))
        {
        return ERROR;
        }
    
    if (pDev->isOpened)
        {
        (void)audioCoreIoctlChan (pDev, AUDIO_STOP, NULL);

        if ((NULL != pDev->pCodec) && (NULL != pDev->pCodec->close))
            {
            (void)pDev->pCodec->close (pDev->pCodec->extension);
            }

        if (NULL != pDev->pTrans->close)
            {
            (void)pDev->pTrans->close (pDev->pTrans->extension);
            }

        pDev->isOpened  = FALSE;
        pDev->devState  = AUDIO_STATE_IDLE;
        pDev->devFlag   = AUDIO_FLAG_BLOCK;
        }

    (void)semGive (pDev->ioSem);
    return OK;
    }

/*******************************************************************************
*
* audioCoreRead - read audio data from the audio device
*
* This routine read audio data from the audio device.
*
* RETURNS: number of bytes read when successful; otherwise -1
*
* ERRNO: N/A
*/

LOCAL ssize_t audioCoreRead
    (
    AUDIO_DATA *    pDev,
    UINT8 *         pBuffer,    /* location to store read data */
    size_t          maxBytes    /* number of bytes to read */
    )
    {
    size_t              readBytes   = 0;
    AUDIO_BUFFER_INFO   dstBufInfo;
    AUDIO_BUFFER_INFO * pSrcBufInfo = NULL;
    UINT8               channels;
    UINT32              dstFrames;

    /* wait for channel to become available */   
        
    if (OK != semTake (pDev->ioSem, ((pDev->devFlag == AUDIO_FLAG_BLOCK) ?
                       (sysClkRateGet () * AUDIO_TIMEOUT) : NO_WAIT)))
        {
        return 0;
        }
        
    /* input parameter sanity check */

    if ((NULL == pBuffer) || (0 == maxBytes) ||
        (AUDIO_STATE_ACTIVE != pDev->inChanInfo.chanState) ||
        (NULL == pDev->pTrans->read))
        {
        (void)semGive (pDev->ioSem);
        return -1;
        }

    dstBufInfo.dataInfo.channels    = pDev->dataInfo.channels;
    dstBufInfo.dataInfo.sampleBits  = pDev->dataInfo.sampleBits;
    dstBufInfo.dataInfo.sampleBytes = pDev->dataInfo.sampleBytes;
    dstBufInfo.dataInfo.sampleRate  = pDev->dataInfo.sampleRate;
    dstBufInfo.dataInfo.useMsb      = pDev->dataInfo.useMsb;
    dstBufInfo.usedSize             = 0;
    dstBufInfo.size                 = maxBytes - readBytes;

    channels = pDev->dataInfo.channels;
    if (1 == channels)
        {
        channels = 2;  /* mono to stereo */
        }

    if (channels > pDev->pTrans->devInfo.maxChannels)
        {
        channels = pDev->pTrans->devInfo.maxChannels;
        }

    while (readBytes < maxBytes)
        {
        dstFrames = (UINT32)(dstBufInfo.size /
                             dstBufInfo.dataInfo.sampleBytes /
                             dstBufInfo.dataInfo.channels);

        if (0 == dstFrames)
            {
            break;
            }
            
        if (ERROR == msgQReceive (pDev->inChanInfo.upStreamMsg, 
                                  (char *)&pSrcBufInfo,
                                  sizeof (char *), 
                                  ((pDev->devFlag == AUDIO_FLAG_BLOCK) ?
                                   WAIT_FOREVER : NO_WAIT)))
            {
            break;
            }   

        if (pSrcBufInfo->usedSize > 0)
            {
            /* convert sampled data into required format */

            dstBufInfo.pBuf                     = pBuffer + readBytes;
            dstBufInfo.size                     = maxBytes - readBytes;
            dstBufInfo.usedSize                 = 0;
            pSrcBufInfo->dataInfo.channels      = channels;
            pSrcBufInfo->dataInfo.sampleBits    = pDev->dataInfo.sampleBits;
            pSrcBufInfo->dataInfo.useMsb        = pDev->pTrans->devInfo.useMsb;
            pSrcBufInfo->dataInfo.sampleBytes
                = pDev->pTrans->devInfo.bytesPerSample;

            audioCoreConvert (pSrcBufInfo, &dstBufInfo);
            }

        pSrcBufInfo->usedSize = 0;
           
        if (ERROR == msgQSend (pDev->inChanInfo.downStreamMsg,
                               (char *)&pSrcBufInfo, 
                               sizeof (char *),
                               ((pDev->devFlag == AUDIO_FLAG_BLOCK) ?
                               WAIT_FOREVER : NO_WAIT), 
                               MSG_PRI_NORMAL))
            {
            break;
            }          
            
        /* add the used bytes into the total read bytes */

        readBytes += dstBufInfo.usedSize;
        }

    (void)semGive (pDev->ioSem);
    return readBytes;
    }

/*******************************************************************************
*
* audioCoreWrite - write audio data to the audio device
*
* This routine write audio data to the audio device.
*
* RETURNS: number of bytes written when successful; otherwise -1
*
* ERRNO: N/A
*/

LOCAL ssize_t audioCoreWrite
    (
    AUDIO_DATA *    pDev,
    UINT8 *         pBuffer,    /* location to store read data */
    size_t          nbytes      /* number of bytes to output */
    )
    {
    size_t              writtenBytes    = 0;
    AUDIO_BUFFER_INFO   srcBufInfo;
    AUDIO_BUFFER_INFO * pDestBufInfo    = NULL;
    UINT8               channels;
    UINT32              srcFrames;

    /* wait for device to become available */

    if (OK != semTake (pDev->ioSem, ((pDev->devFlag == AUDIO_FLAG_BLOCK) ?
                       (sysClkRateGet () * AUDIO_TIMEOUT) : NO_WAIT)))
        {
        return 0;
        }     

    if ((NULL == pBuffer) || (0 == nbytes) ||
        (AUDIO_STATE_ACTIVE != pDev->outChanInfo.chanState) ||
        (NULL == pDev->pTrans->write))
        {
        (void)semGive (pDev->ioSem);
        return -1;
        }

    srcBufInfo.size                 = nbytes;
    srcBufInfo.usedSize             = nbytes;
    srcBufInfo.dataInfo.channels    = pDev->dataInfo.channels;
    srcBufInfo.dataInfo.sampleBits  = pDev->dataInfo.sampleBits;
    srcBufInfo.dataInfo.sampleBytes = pDev->dataInfo.sampleBytes;
    srcBufInfo.dataInfo.sampleRate  = pDev->dataInfo.sampleRate;
    srcBufInfo.dataInfo.useMsb      = pDev->dataInfo.useMsb;

    channels = pDev->dataInfo.channels;
    if (1 == channels)
        {
        channels = 2;  /* mono to stereo */
        }

    if (channels > pDev->pTrans->devInfo.maxChannels)
        {
        channels = pDev->pTrans->devInfo.maxChannels;
        }

    while (writtenBytes < nbytes)
        {
        srcFrames = (UINT32)(srcBufInfo.usedSize /
                             srcBufInfo.dataInfo.sampleBytes /
                             srcBufInfo.dataInfo.channels);

        if (0 == srcFrames)
            {
            break;
            }            

        if (ERROR == msgQReceive (pDev->outChanInfo.upStreamMsg, 
                                  (char *)&pDestBufInfo,
                                  sizeof (char *), 
                                  ((pDev->devFlag == AUDIO_FLAG_BLOCK) ?
                                   WAIT_FOREVER : NO_WAIT)))
            {
            break;
            }                                  
         
        srcBufInfo.pBuf                     = pBuffer + writtenBytes;
        pDestBufInfo->dataInfo.channels     = channels;
        pDestBufInfo->dataInfo.sampleBits   = pDev->dataInfo.sampleBits;
        pDestBufInfo->dataInfo.useMsb       = pDev->pTrans->devInfo.useMsb;
        pDestBufInfo->dataInfo.sampleBytes
            = pDev->pTrans->devInfo.bytesPerSample;

        audioCoreConvert (&srcBufInfo, pDestBufInfo);

        if (ERROR == msgQSend (pDev->outChanInfo.downStreamMsg,
                               (char *)&pDestBufInfo, 
                               sizeof (char *),
                               ((pDev->devFlag == AUDIO_FLAG_BLOCK) ?
                               WAIT_FOREVER : NO_WAIT), 
                               MSG_PRI_NORMAL))
            {
            break;
            }         
            
        /* add the used bytes into the total write bytes */
        
        writtenBytes = (nbytes - srcBufInfo.usedSize);         
        }

    (void)semGive (pDev->ioSem);
    return writtenBytes;
    }

/*******************************************************************************
*
* audioCoreIoctl - handle ioctls for audio module
*
* This routine handles ioctls to the audio module.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS audioCoreIoctl
    (
    AUDIO_DATA *        pDev,
    int                 function,           /* function to perform */
    AUDIO_IOCTL_ARG *   pIoctlArg           /* function argument */
    )
    {
    AUDIO_LIB_CORE_DATA *   pAudioCore  = pAudioHd;
    STATUS                  status      = OK;
    ssize_t                 bufSize     = 0;
    UINT8                   channels    = 0;
    int                     i;

    /* wait for device to become available */

    if (OK != semTake (pDev->ioSem, (sysClkRateGet () * AUDIO_TIMEOUT)))
        {
        return ERROR;
        }

    switch (function)
        {
        case FIONREAD: /* get num chars available to read */
            if (NULL == pIoctlArg)
                {
                status = ERROR;
                break;
                }

            for (i = 0; i < pAudioCore->bufNum; i++)
                {
                bufSize += pDev->inChanInfo.pBufInfo[i].usedSize;
                }
            
            if (pDev->dataInfo.sampleBits > 16)
                {
                pIoctlArg->bufSize = bufSize;
                }
            else
                {
                pIoctlArg->bufSize =
                    (bufSize /
                     (ssize_t)pDev->pTrans->devInfo.bytesPerSample) *
                     (ssize_t)sizeof (UINT16);
                }
            break;
        case FIONFREE: /* get free byte count on device */
            if (NULL == pIoctlArg)
                {
                status = ERROR;
                break;
                }
                
            channels = pDev->dataInfo.channels;
            if (1 == channels)
                {
                channels = 2;  
                }
            else
                {
                channels = 1;
                }
                
            for (i = 0; i < pAudioCore->bufNum; i++)
                {
                if (pDev->outChanInfo.pBufInfo[i].usedSize == 0)
                    bufSize += pDev->outChanInfo.pBufInfo[i].size;
                } 
            if (bufSize > 0)
                {
                bufSize = (UINT32)(bufSize /
                                   pDev->pTrans->devInfo.bytesPerSample /
                                   channels) * pDev->dataInfo.sampleBytes;                             
                }
            pIoctlArg->bufSize = bufSize;
            break;
        case AUDIO_DEV_ENABLE:
            if (AUDIO_STATE_IDLE == pDev->devState)
                {
                if (NULL != pIoctlArg)
                    {
                    if (NULL != pDev->pCodec)
                        {
                        if (0 ==
                            (pIoctlArg->path & pDev->pCodec->devInfo.availPaths))
                            {
                            status = ERROR;
                            break;
                            }
                        }
                    else
                        {
                        if (0 ==
                            (pIoctlArg->path & pDev->pTrans->devInfo.availPaths))
                            {
                            status = ERROR;
                            break;
                            }
                        }
                    }

                status = audioCoreIoctlDev (pDev, function, pIoctlArg);
                if (OK == status)
                    {
                    pDev->devState = AUDIO_STATE_ACTIVE;
                    }
                }
            break;

        case AUDIO_DEV_DISABLE:
            if (AUDIO_STATE_ACTIVE == pDev->devState)
                {
                if (NULL != pIoctlArg)
                    {
                    if (NULL != pDev->pCodec)
                        {
                        if (0 ==
                            (pIoctlArg->path & pDev->pCodec->devInfo.availPaths))
                            {
                            status = ERROR;
                            break;
                            }
                        }
                    else
                        {
                        if (0 ==
                            (pIoctlArg->path & pDev->pTrans->devInfo.availPaths))
                            {
                            status = ERROR;
                            break;
                            }
                        }
                    }

                status = audioCoreIoctlChan (pDev, AUDIO_STOP, pIoctlArg);
                if (OK != status)
                    {
                    break;
                    }

                status = audioCoreIoctlDev (pDev, function, pIoctlArg);
                if (OK == status)
                    {
                    pDev->devState = AUDIO_STATE_IDLE;
                    }
                }
            break;

        case AUDIO_START:
        case AUDIO_STOP:
            if (AUDIO_STATE_ACTIVE != pDev->devState)
                {
                status = ERROR;
                break;
                }

            if (NULL != pIoctlArg)
                {
                if (NULL != pDev->pCodec)
                    {
                    if (0 ==
                        (pIoctlArg->path & pDev->pCodec->devInfo.availPaths))
                        {
                        status = ERROR;
                        break;
                        }
                    }
                else
                    {
                    if (0 ==
                        (pIoctlArg->path & pDev->pTrans->devInfo.availPaths))
                        {
                        status = ERROR;
                        break;
                        }
                    }
                }

            status = audioCoreIoctlChan (pDev, function, pIoctlArg);
            break;

        case AUDIO_GET_DATA_INFO:
            if (NULL == pIoctlArg)
                {
                status = ERROR;
                break;
                }

            bcopy ((char *)&(pDev->dataInfo), (char *)&(pIoctlArg->dataInfo),
                   sizeof (AUDIO_DATA_INFO));
            break;

        case AUDIO_SET_DATA_INFO:
            if ((NULL == pIoctlArg) || (AUDIO_STATE_IDLE != pDev->devState))
                {
                status = ERROR;
                break;
                }

            status = audioCoreIoctlDev (pDev, function, pIoctlArg);
            if (OK == status)
                {
                bcopy ((char *)&(pIoctlArg->dataInfo),
                       (char *)&(pDev->dataInfo), sizeof (AUDIO_DATA_INFO));
                }
            break;

        case AUDIO_GET_PATH:
            if (NULL == pIoctlArg)
                {
                status = ERROR;
                break;
                }

            pIoctlArg->path = pDev->curPaths;
            break;

        case AUDIO_SET_PATH:
            if ((NULL == pIoctlArg) || (AUDIO_STATE_IDLE != pDev->devState) ||
                (0 == pIoctlArg->path))
                {
                status = ERROR;
                break;
                }

            if (NULL != pDev->pCodec)
                {
                if (0 !=
                    (pIoctlArg->path & (~pDev->pCodec->devInfo.availPaths)))
                    {
                    status = ERROR;
                    break;
                    }
                }
            else
                {
                if (0 !=
                    (pIoctlArg->path & (~pDev->pTrans->devInfo.availPaths)))
                    {
                    status = ERROR;
                    break;
                    }
                }

            status = audioCoreIoctlDev (pDev, function, pIoctlArg);
            if (OK == status)
                {
                pDev->curPaths = pIoctlArg->path;
                }
            break;

        case AUDIO_GET_VOLUME:
            if (NULL == pIoctlArg)
                {
                status = ERROR;
                break;
                }

            if (NULL != pDev->pCodec)
                {
                if (0 ==
                    (pIoctlArg->volume.path & pDev->pCodec->devInfo.availPaths))
                    {
                    status = ERROR;
                    break;
                    }
                }
            else
                {
                if (0 ==
                    (pIoctlArg->volume.path & pDev->pTrans->devInfo.availPaths))
                    {
                    status = ERROR;
                    break;
                    }
                }

            status = audioCoreIoctlChan (pDev, function, pIoctlArg);
            break;

        case AUDIO_SET_VOLUME:
            if ((NULL == pIoctlArg) || (0 == pIoctlArg->volume.path))
                {
                status = ERROR;
                break;
                }

            if (NULL != pDev->pCodec)
                {
                if (0 ==
                    (pIoctlArg->volume.path & pDev->pCodec->devInfo.availPaths))
                    {
                    status = ERROR;
                    break;
                    }
                }
            else
                {
                if (0 ==
                    (pIoctlArg->volume.path & pDev->pTrans->devInfo.availPaths))
                    {
                    status = ERROR;
                    break;
                    }
                }

            status = audioCoreIoctlDev (pDev, function, pIoctlArg);
            if (OK == status)
                {
                status = audioCoreIoctlChan (pDev, function, pIoctlArg);
                }
            break;

        case AUDIO_SET_BUFTIME:
            if ((NULL == pIoctlArg) || (0 == pIoctlArg->bufTime) ||
                (AUDIO_STATE_IDLE != pDev->devState))
                {
                status = ERROR;
                break;
                }

            pDev->bufTime = pIoctlArg->bufTime;
            break;

        case AUDIO_GET_BUFTIME:
            if (NULL == pIoctlArg)
                {
                status = ERROR;
                break;
                }

            pIoctlArg->bufTime = pDev->bufTime;
            break;

        case AUDIO_GET_DEV_INFO:
            if (NULL == pIoctlArg)
                {
                status = ERROR;
                break;
                }

            bcopy ((char *)&(pDev->pTrans->devInfo),
                   (char *)&(pIoctlArg->devInfo), sizeof (AUDIO_DEV_INFO));

            if (NULL != pDev->pCodec)
                {
                pIoctlArg->devInfo.availPaths
                    = pDev->pCodec->devInfo.availPaths;
                pIoctlArg->devInfo.defPaths
                    = pDev->pCodec->devInfo.defPaths;
                }
            break;

        default:
            status = ERROR;
        }

    (void)semGive (pDev->ioSem);
    return status;
    }

/*******************************************************************************
*
* audioCoreIoctlDev - handle ioctls for audio devices
*
* This routine handles ioctls for audio devices.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS audioCoreIoctlDev
    (
    AUDIO_DATA *        pDev,
    AUDIO_IO_CTRL       function,
    AUDIO_IOCTL_ARG *   pIoctlArg
    )
    {
    STATUS  status = OK;

    status = pDev->pTrans->ioctl (pDev->pTrans->extension, function, pIoctlArg);
    if (OK != status)
        {
        return status;
        }

    if (NULL != pDev->pCodec)
        {
        status = pDev->pCodec->ioctl (pDev->pCodec->extension, function,
                                      pIoctlArg);
        if (OK != status)
            {
            return status;
            }
        }

    return status;
    }

/*******************************************************************************
*
* audioRecIoctl - handle in/out chan ioctls for audio module
*
* This routine handles in/out chan ioctls to the audio module.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS audioCoreIoctlChan
    (
    AUDIO_DATA *        pDev,
    AUDIO_IO_CTRL       function,
    AUDIO_IOCTL_ARG *   pIoctlArg
    )
    {
    STATUS  status = OK;
    UINT32  path;

    switch (function)
        {
        case AUDIO_SET_VOLUME:
            if (0 != (pIoctlArg->volume.path & (UINT32)AUDIO_OUT_MASK))
                {
                bcopy ((char *)&(pIoctlArg->volume),
                       (char *)&(pDev->outChanInfo.volume),
                       sizeof (AUDIO_VOLUME));
                }

            if (0 != (pIoctlArg->volume.path & (UINT32)AUDIO_IN_MASK))
                {
                bcopy ((char *)&(pIoctlArg->volume),
                       (char *)&(pDev->inChanInfo.volume),
                       sizeof (AUDIO_VOLUME));
                }
            break;

        case AUDIO_GET_VOLUME:
            if (0 != (pIoctlArg->volume.path & (UINT32)AUDIO_OUT_MASK))
                {
                bcopy ((char *)&(pDev->outChanInfo.volume),
                       (char *)&(pIoctlArg->volume),
                       sizeof (AUDIO_VOLUME));
                }

            if (0 != (pIoctlArg->volume.path & (UINT32)AUDIO_IN_MASK))
                {
                bcopy ((char *)&(pDev->inChanInfo.volume),
                       (char *)&(pIoctlArg->volume),
                       sizeof (AUDIO_VOLUME));
                }
            break;

        case AUDIO_START:
            if (NULL != pIoctlArg)
                {
                path = pIoctlArg->path;
                }
            else
                {
                path = pDev->curPaths;
                }

            if (0 != (path & (UINT32)AUDIO_OUT_MASK))
                {
                status = audioCoreChanStart (pDev, &(pDev->outChanInfo));
                }

            if (0 != (path & (UINT32)AUDIO_IN_MASK))
                {
                status |= audioCoreChanStart (pDev, &(pDev->inChanInfo));
                }
            break;

        case AUDIO_STOP:
            if (NULL != pIoctlArg)
                {
                path = pIoctlArg->path;
                }
            else
                {
                path = pDev->curPaths;
                }

            if (0 != (path & (UINT32)AUDIO_OUT_MASK))
                {
                audioCoreChanStop (pDev, &(pDev->outChanInfo));
                }

            if (0 != (path & (UINT32)AUDIO_IN_MASK))
                {
                audioCoreChanStop (pDev, &(pDev->inChanInfo));
                }
            break;

        default:
            break;
        }

    return status;
    }

/*******************************************************************************
*
* audioCoreChanStart - start transferring data on the channel
*
* This routine starts transferring data on the channel.
*
* RETURNS: OK when operation was successful; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS audioCoreChanStart
    (
    AUDIO_DATA *        pDev,
    AUDIO_CHAN_INFO *   pChan
    )
    {
    AUDIO_LIB_CORE_DATA *   pAudioCore  = pAudioHd;
    ssize_t                 size;
    UINT8                   channels;

    if (AUDIO_STATE_IDLE == pChan->chanState)
        {
        /* allocate input buffers */

        channels = pDev->dataInfo.channels;
        if (1 == channels)
            {
            channels = 2;  /* mono to stereo */
            }

        if (channels > pDev->pTrans->devInfo.maxChannels)
            {
            channels = pDev->pTrans->devInfo.maxChannels;
            }

        size = ((pDev->bufTime / pAudioCore->bufNum) *
                pDev->dataInfo.sampleRate) / 1000 *
               pDev->pTrans->devInfo.bytesPerSample * channels;

        if (ERROR == audioCoreAllocBuffer (pChan, size))
            {
            return ERROR;
            }

        pChan->chanState = AUDIO_STATE_ACTIVE;

        audioCoreInitMsgQ (pChan, pChan->downStreamMsg);

        }

    return OK;
    }

/*******************************************************************************
*
* audioCoreChanStop - stop transferring data on the channel
*
* This routine stops transferring data on the channel.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreChanStop
    (
    AUDIO_DATA *        pDev,
    AUDIO_CHAN_INFO *   pChan
    )
    {
    if (AUDIO_STATE_IDLE != pChan->chanState)
        {
        audioCoreClrMsgQ (pChan, pChan->upStreamMsg);
        audioCoreFreeBuffer (pChan);
        pChan->chanState = AUDIO_STATE_IDLE;
        }
    }

/*******************************************************************************
*
* audioCoreAllocBuffer - allocate audio tx/rx buffers
*
* This routine allocate audio tx/rx buffers.
*
* RETURNS: OK when operation was successful; otherwise ERROR.
*
* ERRNO: N/A
*/

LOCAL STATUS audioCoreAllocBuffer
    (
    AUDIO_CHAN_INFO *   pChanInfo,
    ssize_t             bufSize
    )
    {
    UINT32                  i;
    AUDIO_BUFFER_INFO *     pBufInfo        = NULL;
    STATUS                  status          = OK;
    AUDIO_LIB_CORE_DATA *   pAudioCore      = pAudioHd;

    pBufInfo = (AUDIO_BUFFER_INFO *)calloc (pAudioCore->bufNum,
        (sizeof (AUDIO_BUFFER_INFO)));
    if (NULL == pBufInfo)
        {
        return ERROR;
        }

    pChanInfo->pBufInfo = pBufInfo;

    for (i = 0; i < pAudioCore->bufNum; i++)
        {
        pBufInfo[i].size    = bufSize;
        pBufInfo[i].pBuf    = (UINT8 *)cacheDmaMalloc (pBufInfo[i].size);

        if (NULL == pBufInfo[i].pBuf)
            {
            audioCoreFreeBuffer (pChanInfo);
            status = ERROR;
            break;
            }

        bzero ((char *)pBufInfo[i].pBuf, pBufInfo[i].size);
        }

    return status;
    }

/*******************************************************************************
*
* audioCoreFreeBuffer - free audio tx/rx buffers
*
* This routine free audio tx/rx buffers.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreFreeBuffer
    (
    AUDIO_CHAN_INFO *   pChanInfo
    )
    {
    AUDIO_BUFFER_INFO *     pBufInfoUnit    = NULL;
    AUDIO_LIB_CORE_DATA *   pAudioCore      = pAudioHd;
    UINT32                  i;

    if ((NULL == pChanInfo) || (NULL == pChanInfo->pBufInfo))
        {
        return;
        }

    for (i = 0; i < pAudioCore->bufNum; i++)
        {
        pBufInfoUnit = pChanInfo->pBufInfo + i;
        if (NULL != pBufInfoUnit->pBuf)
            {
            (void)cacheDmaFree (pBufInfoUnit->pBuf);
            }
        }

    free (pChanInfo->pBufInfo);
    pChanInfo->pBufInfo = NULL;
    }

/*******************************************************************************
*
* audioCoreConvert - convert and copy audio data
*
* This routine converts and copies audio data.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreConvert
    (
    AUDIO_BUFFER_INFO * pSrcBufInfo,
    AUDIO_BUFFER_INFO * pDestBufInfo
    )
    {
    UINT32  i;
    UINT8   srcShiftBits    = 0;
    UINT8   destShiftBits   = 0;
    UINT32  srcFrames;
    UINT32  destFrames;
    UINT32  copyFrames;
    UINT8   srcChan     = pSrcBufInfo->dataInfo.channels;
    UINT8   destChan    = pDestBufInfo->dataInfo.channels;
    UINT8   shiftBits;
    UINT8   skipChan;

    if (pSrcBufInfo->dataInfo.useMsb)
        {
        srcShiftBits = (UINT8)((pSrcBufInfo->dataInfo.sampleBytes << 3)
                       - pSrcBufInfo->dataInfo.sampleBits);
        }

    if (pDestBufInfo->dataInfo.useMsb)
        {
        destShiftBits = (UINT8)((pDestBufInfo->dataInfo.sampleBytes << 3)
                        - pDestBufInfo->dataInfo.sampleBits);
        }

    srcFrames = (UINT32)(pSrcBufInfo->usedSize /
                         pSrcBufInfo->dataInfo.sampleBytes / srcChan);
    destFrames = (UINT32)(pDestBufInfo->size /
                          pDestBufInfo->dataInfo.sampleBytes / destChan);
    if (srcFrames > destFrames)
        {
        copyFrames = destFrames;
        }
    else
        {
        copyFrames = srcFrames;
        }

    pSrcBufInfo->usedSize   -= pSrcBufInfo->dataInfo.sampleBytes *
                               copyFrames * srcChan;
    pDestBufInfo->usedSize  += pDestBufInfo->dataInfo.sampleBytes *
                               copyFrames * destChan;

    if (2 == pSrcBufInfo->dataInfo.sampleBytes)
        {
        UINT16 *    pSrc = (UINT16 *)pSrcBufInfo->pBuf;

        if (2 == pDestBufInfo->dataInfo.sampleBytes)
            {
            UINT16 *    pDest = (UINT16 *)pDestBufInfo->pBuf;
            UINT16      tempValue = 0;

            DATA_CONVERT (UINT16, UINT16);
            }
        else if (4 == pDestBufInfo->dataInfo.sampleBytes)
            {
            UINT32 *    pDest = (UINT32 *)pDestBufInfo->pBuf;
            UINT32      tempValue = 0;

            DATA_CONVERT (UINT16, UINT32);
            }
        else
            {
            AUDIO_CORE_DBG_MSG (100, "Don't support converting from %dbit to "
                                "%dbit\n", pSrcBufInfo->dataInfo.sampleBytes,
                                pDestBufInfo->dataInfo.sampleBytes, 0, 0, 0, 0);
            }
        }
    else if (3 == pSrcBufInfo->dataInfo.sampleBytes)
        {
        if (3 == pDestBufInfo->dataInfo.sampleBytes)
            {
            audioCoreConvert24to24 (pSrcBufInfo, pDestBufInfo, copyFrames);
            }
        else if (4 == pDestBufInfo->dataInfo.sampleBytes)
            {
            audioCoreConvert24to32 (pSrcBufInfo, pDestBufInfo, destShiftBits,
                                    copyFrames);
            }
        else
            {
            AUDIO_CORE_DBG_MSG (100, "Don't support converting from %dbit to "
                                "%dbit\n", pSrcBufInfo->dataInfo.sampleBytes,
                                pDestBufInfo->dataInfo.sampleBytes, 0, 0, 0, 0);
            }
        }
    else if (4 == pSrcBufInfo->dataInfo.sampleBytes)
        {
        UINT32 *    pSrc        = (UINT32 *)pSrcBufInfo->pBuf;

        if (2 == pDestBufInfo->dataInfo.sampleBytes)
            {
            UINT16 *    pDest = (UINT16 *)pDestBufInfo->pBuf;
            UINT16      tempValue = 0;

            DATA_CONVERT (UINT32, UINT16);
            }
        else if (3 == pDestBufInfo->dataInfo.sampleBytes)
            {
            audioCoreConvert32to24 (pSrcBufInfo, pDestBufInfo, srcShiftBits,
                                    copyFrames);
            }
        else if (4 == pDestBufInfo->dataInfo.sampleBytes)
            {
            UINT32 *    pDest   = (UINT32 *)pDestBufInfo->pBuf;
            UINT32      tempValue = 0;

            DATA_CONVERT (UINT32, UINT32);
            }
        else
            {
            AUDIO_CORE_DBG_MSG (100, "Don't support converting from %dbit to "
                                "%dbit\n", pSrcBufInfo->dataInfo.sampleBytes,
                                pDestBufInfo->dataInfo.sampleBytes, 0, 0, 0, 0);
            }
        }
    else
        {
        AUDIO_CORE_DBG_MSG (100, "Don't support converting from %dbit to "
                            "%dbit\n", pSrcBufInfo->dataInfo.sampleBytes,
                            pDestBufInfo->dataInfo.sampleBytes, 0, 0, 0, 0);
        }
    }

/*******************************************************************************
*
* audioCoreConvert24to24 - convert 24bit data to 24bit data
*
* This routine converts and copies audio data.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreConvert24to24
    (
    AUDIO_BUFFER_INFO * pSrcBufInfo,
    AUDIO_BUFFER_INFO * pDestBufInfo,
    UINT32              copyFrames
    )
    {
    UINT32  i;
    UINT8   srcChan     = pSrcBufInfo->dataInfo.channels;
    UINT8   destChan    = pDestBufInfo->dataInfo.channels;
    UINT8 * pSrc        = (UINT8 *)pSrcBufInfo->pBuf;
    UINT8 * pDest       = (UINT8 *)pDestBufInfo->pBuf;
    UINT8   tempValue[3]= {0, 0 ,0};
    UINT8   skipChan    = 0;

    if (srcChan > destChan)
        {
        skipChan = (UINT8)((srcChan - destChan) * 3);
        while (copyFrames-- > 0)
            {
            for (i = 0; i < destChan * 3; i++)
                {
                *pDest++ = *pSrc++;
                }

            pSrc += skipChan;
            }
        }
    else if (srcChan < destChan)
        {
        while (copyFrames-- > 0)
            {
            for (i = 0; i < srcChan; i++)
                {
                tempValue[0]    = *pSrc++;
                tempValue[1]    = *pSrc++;
                tempValue[2]    = *pSrc++;
                *pDest++        = tempValue[0];
                *pDest++        = tempValue[1];
                *pDest++        = tempValue[2];
                }

            for (i = srcChan; i < destChan; i++)
                {
                *pDest++    = tempValue[0];
                *pDest++    = tempValue[1];
                *pDest++    = tempValue[2];
                }
            }
        }
    else
        {
        copyFrames *= srcChan;
        bcopy ((char *)(pSrcBufInfo->pBuf), (char *)(pDestBufInfo->pBuf),
               copyFrames * sizeof (UINT8) * 3);
        }
    }

/*******************************************************************************
*
* audioCoreConvert24to32 - convert 24bit data to 32bit data
*
* This routine converts and copies audio data.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreConvert24to32
    (
    AUDIO_BUFFER_INFO * pSrcBufInfo,
    AUDIO_BUFFER_INFO * pDestBufInfo,
    UINT8               destShiftBits,
    UINT32              copyFrames
    )
    {
    UINT32      i;
    UINT8       srcChan     = pSrcBufInfo->dataInfo.channels;
    UINT8       destChan    = pDestBufInfo->dataInfo.channels;
    UINT8 *     pSrc        = (UINT8 *)pSrcBufInfo->pBuf;
    UINT32 *    pDest       = (UINT32 *)pDestBufInfo->pBuf;
    UINT32      tempValue   = 0;
    UINT8       skipChan    = 0;

    if (srcChan > destChan)
        {
        skipChan = (UINT8)((srcChan - destChan) * 3);
        if (0 != destShiftBits)
            {
            while (copyFrames-- > 0)
                {
                for (i = 0; i < destChan; i++)
                    {
                    tempValue   = (UINT32)(*pSrc++);
                    tempValue  |= (UINT32)((*pSrc++) << 8);
                    tempValue  |= (UINT32)((*pSrc++) << 16);
                    *pDest++    = (UINT32)(tempValue << destShiftBits);
                    }

                pSrc += skipChan;
                }
            }
        else
            {
            while (copyFrames-- > 0)
                {
                for (i = 0; i < destChan; i++)
                    {
                    tempValue   = (UINT32)(*pSrc++);
                    tempValue  |= (UINT32)((*pSrc++) << 8);
                    tempValue  |= (UINT32)((*pSrc++) << 16);
                    *pDest++    = tempValue;
                    }

                pSrc += skipChan;
                }
            }
        }
    else if (srcChan < destChan)
        {
        if (0 != destShiftBits)
            {
            while (copyFrames-- > 0)
                {
                for (i = 0; i < srcChan; i++)
                    {
                    tempValue   = (UINT32)(*pSrc++);
                    tempValue  |= (UINT32)((*pSrc++) << 8);
                    tempValue  |= (UINT32)((*pSrc++) << 16);
                    *pDest++    = (UINT32)(tempValue << destShiftBits);
                    }

                for (i = srcChan; i < destChan; i++)
                    {
                    *pDest++ = tempValue;
                    }
                }
            }
        else
            {
            while (copyFrames-- > 0)
                {
                for (i = 0; i < srcChan; i++)
                    {
                    tempValue   = (UINT32)(*pSrc++);
                    tempValue  |= (UINT32)((*pSrc++) << 8);
                    tempValue  |= (UINT32)((*pSrc++) << 16);
                    *pDest++    = tempValue;
                    }

                for (i = srcChan; i < destChan; i++)
                    {
                    *pDest++ = tempValue;
                    }
                }
            }
        }
    else
        {
        copyFrames *= srcChan;

        if (0 != destShiftBits)
            {
            while (copyFrames-- > 0)
                {
                tempValue   = (UINT32)(*pSrc++);
                tempValue  |= (UINT32)((*pSrc++) << 8);
                tempValue  |= (UINT32)((*pSrc++) << 16);
                *pDest++    = (UINT32)(tempValue << destShiftBits);
                }
            }
        else
            {
            while (copyFrames-- > 0)
                {
                tempValue   = (UINT32)(*pSrc++);
                tempValue  |= (UINT32)((*pSrc++) << 8);
                tempValue  |= (UINT32)((*pSrc++) << 16);
                *pDest++    = tempValue;
                }
            }
        }
    }

/*******************************************************************************
*
* audioCoreConvert32to24 - convert 32bit data to 24bit data
*
* This routine converts and copies audio data.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreConvert32to24
    (
    AUDIO_BUFFER_INFO * pSrcBufInfo,
    AUDIO_BUFFER_INFO * pDestBufInfo,
    UINT8               srcShiftBits,
    UINT32              copyFrames
    )
    {
    UINT32      i;
    UINT8       srcChan     = pSrcBufInfo->dataInfo.channels;
    UINT8       destChan    = pDestBufInfo->dataInfo.channels;
    UINT32 *    pSrc        = (UINT32 *)pSrcBufInfo->pBuf;
    UINT8 *     pDest       = (UINT8 *)pDestBufInfo->pBuf;
    UINT32      tempValue   = 0;
    UINT8       skipChan    = 0;

    if (srcChan > destChan)
        {
        skipChan = (UINT8)(srcChan - destChan);
        if (0 != srcShiftBits)
            {
            while (copyFrames-- > 0)
                {
                for (i = 0; i < destChan; i++)
                    {
                    tempValue = (UINT32)(*pSrc++ >> srcShiftBits);
                    *pDest++ = (UINT8)(tempValue & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 8) & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 16) & 0xFF);
                    }

                pSrc += skipChan;
                }
            }
        else
            {
            while (copyFrames-- > 0)
                {
                for (i = 0; i < destChan; i++)
                    {
                    tempValue = *pSrc++;
                    *pDest++ = (UINT8)(tempValue & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 8) & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 16) & 0xFF);
                    }

                pSrc += skipChan;
                }
            }
        }
    else if (srcChan < destChan)
        {
        if (0 != srcShiftBits)
            {
            while (copyFrames-- > 0)
                {
                for (i = 0; i < srcChan; i++)
                    {
                    tempValue = (UINT32)(*pSrc++ >> srcShiftBits);
                    *pDest++ = (UINT8)(tempValue & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 8) & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 16) & 0xFF);
                    }

                for (i = srcChan; i < destChan; i++)
                    {
                    *pDest++ = (UINT8)(tempValue & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 8) & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 16) & 0xFF);
                    }
                }
            }
        else
            {
            while (copyFrames-- > 0)
                {
                for (i = 0; i < srcChan; i++)
                    {
                    tempValue = *pSrc++;
                    *pDest++ = (UINT8)(tempValue & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 8) & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 16) & 0xFF);
                    }

                for (i = srcChan; i < destChan; i++)
                    {
                    *pDest++ = (UINT8)(tempValue & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 8) & 0xFF);
                    *pDest++ = (UINT8)((tempValue >> 16) & 0xFF);
                    }
                }
            }
        }
    else
        {
        copyFrames *= srcChan;

        if (0 != srcShiftBits)
            {
            while (copyFrames-- > 0)
                {
                tempValue = (UINT32)(*pSrc++ >> srcShiftBits);
                *pDest++ = (UINT8)(tempValue & 0xFF);
                *pDest++ = (UINT8)((tempValue >> 8) & 0xFF);
                *pDest++ = (UINT8)((tempValue >> 16) & 0xFF);
                }
            }
        else
            {
            while (copyFrames-- > 0)
                {
                tempValue = *pSrc++;
                *pDest++ = (UINT8)(tempValue & 0xFF);
                *pDest++ = (UINT8)((tempValue >> 8) & 0xFF);
                *pDest++ = (UINT8)((tempValue >> 16) & 0xFF);
                }
            }
        }
    }

/*******************************************************************************
*
* audioCoreDevIdxAlloc - allocate a global audio device index
*
* This routine allocates a global audio device index.
*
* RETURNS: OK or ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS audioCoreDevIdxAlloc
    (
    UINT32 *    pIdx
    )
    {
    UINT32  idx;

    for (idx = 0; idx < AUDIO_DEV_MAX; idx++)
        {
        if (!(audioDevIdx & (1 << idx)))
            {
            audioDevIdx |= (1 << idx);
            break;
            }
        }

    if (AUDIO_DEV_MAX == idx)
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
* audioCoreDevIdxFree - free a global audio device index
*
* This routine frees a global audio device index.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreDevIdxFree
    (
    UINT32  idx
    )
    {
    audioDevIdx &= ~(1 << idx);
    }

/*******************************************************************************
*
* audioCoreSendTask - transmit audio data to the audio device
*
* This routine transmits audio data to the audio device.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreSendTask
    (
    AUDIO_DATA *    pDev
    )
    {
    AUDIO_BUFFER_INFO * pBufferInfo = NULL;

    FOREVER
        {
        (void)msgQReceive (pDev->outChanInfo.downStreamMsg,
                           (char *)&pBufferInfo, sizeof (char *), WAIT_FOREVER);

        /* in idle state, do not send buffer */

        if ((AUDIO_STATE_ACTIVE == pDev->outChanInfo.chanState)
            && (pBufferInfo->usedSize > 0))
            {
            CACHE_DMA_FLUSH (pBufferInfo->pBuf, pBufferInfo->usedSize);
            CACHE_PIPE_FLUSH ();
            (void)pDev->pTrans->write (pDev->pTrans->extension,
                                       (char *)pBufferInfo->pBuf,
                                       pBufferInfo->usedSize);
            pBufferInfo->usedSize = 0;
            }

        (void)msgQSend (pDev->outChanInfo.upStreamMsg, (char *)&pBufferInfo,
                        sizeof (char *), WAIT_FOREVER, MSG_PRI_NORMAL);
        }
           
    }

/*******************************************************************************
*
* audioCoreRecvTask - receive audio data from the audio device
*
* This routine receives data from audio device and fills the receive buffer.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreRecvTask
    (
    AUDIO_DATA *    pDev
    )
    {
    AUDIO_BUFFER_INFO * pBufferInfo = NULL;

    FOREVER
        {
        (void)msgQReceive (pDev->inChanInfo.downStreamMsg, (char *)&pBufferInfo,
                           sizeof (char *), WAIT_FOREVER);

        if (AUDIO_STATE_ACTIVE == pDev->inChanInfo.chanState)
            {
            pBufferInfo->usedSize = pDev->pTrans->read (pDev->pTrans->extension,
                (char *)pBufferInfo->pBuf, pBufferInfo->size);

            CACHE_DMA_INVALIDATE (pBufferInfo->pBuf, pBufferInfo->usedSize);
            }
        else
            {
            pBufferInfo->usedSize = 0;
            }

        /* send the filled buffer into up stream queue */

        (void)msgQSend (pDev->inChanInfo.upStreamMsg, (char *)&pBufferInfo,
                        sizeof (char *), WAIT_FOREVER, MSG_PRI_NORMAL);
        }
           
    }

/*******************************************************************************
*
* audioCoreInitMsgQ - initialize message queue
*
* This routine initializes message queue.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreInitMsgQ
    (
    AUDIO_CHAN_INFO *   pChanInfo,
    MSG_Q_ID            msgQ
    )
    {
    AUDIO_LIB_CORE_DATA *   pAudioCore  = pAudioHd;
    AUDIO_BUFFER_INFO *     pBufferInfo = NULL;
    UINT32                  i;

    for (i = 0; i < pAudioCore->bufNum; i++)
        {
        pBufferInfo             = &(pChanInfo->pBufInfo[i]);
        pBufferInfo->usedSize   = 0;

        (void)msgQSend (msgQ, (char *)&(pBufferInfo), sizeof (char *), NO_WAIT,
                        MSG_PRI_NORMAL);
        }
    }

/*******************************************************************************
*
* audioCoreClrMsgQ - clear messages in message queue
*
* This routine clears messages in message queue.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/

LOCAL void audioCoreClrMsgQ
    (
    AUDIO_CHAN_INFO *   pChanInfo,
    MSG_Q_ID            msgQ
    )
    {
    AUDIO_LIB_CORE_DATA *   pAudioCore  = pAudioHd;
    AUDIO_BUFFER_INFO *     pBufferInfo = NULL;
    UINT32                  i;

    for (i = 0; i < pAudioCore->bufNum; i++)
        {
        (void)msgQReceive (msgQ, (char *)&(pBufferInfo), sizeof (char *),
                           WAIT_FOREVER);
        }
    }

#ifdef AUDIO_CORE_DEBUG_ON
/*******************************************************************************
*
* audioCoreShow - show audio subsystem
*
* This routine shows the audio configuration.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

void audioCoreShow (void)
    {
    AUDIO_LIB_CORE_DATA *   pAudioCore  = pAudioHd;
    AUDIO_DEV *             pCodecsInfo = NULL;
    AUDIO_DEV *             pTransInfo  = NULL;
    DL_NODE *               pNode;

    if (NULL == pAudioCore)
        {
        AUDIO_CORE_DBG_MSG (100, "Audio core library isn't initialized\n",
                            0, 0, 0, 0, 0, 0);
        return;
        }

    AUDIO_CORE_DBG_MSG (100, "Audio core library configuration : bufNum = %d, "
                        "bufTime = %d ms\n",
                        pAudioCore->bufNum, pAudioCore->bufTime, 0, 0, 0, 0);

    pNode = DLL_FIRST (pAudioCore->pCodecList);
    while (NULL != pNode)
        {
        pCodecsInfo = (AUDIO_DEV *)pNode;

        AUDIO_CORE_DBG_MSG (100, "Audio Codec [%s - %d] : attTransUnit = %d\n",
                            (_Vx_usr_arg_t)pCodecsInfo->devInfo.name,
                            pCodecsInfo->unit,
                            pCodecsInfo->attTransUnit, 0, 0, 0);
        pNode = DLL_NEXT (pNode);
        }

    pNode = DLL_FIRST (pAudioCore->pTransList);
    while (NULL != pNode)
        {
        pTransInfo = (AUDIO_DEV *)pNode;

        AUDIO_CORE_DBG_MSG (100, "Audio Transceiver [%s - %d] : bytesPerSample "
                            "= %d, maxChannels = %d, MSB = %d\n",
                            (_Vx_usr_arg_t)pTransInfo->devInfo.name,
                            pTransInfo->unit,
                            pTransInfo->devInfo.bytesPerSample,
                            pTransInfo->devInfo.maxChannels,
                            pTransInfo->devInfo.useMsb);
        pNode = DLL_NEXT (pNode);
        }
    }
#endif /* AUDIO_CORE_DEBUG_ON */
