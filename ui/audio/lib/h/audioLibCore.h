/* audioLibCore.h - Audio Driver Framework Core Library Header File */

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
02sep14,y_f  allowed register audio codec earlier (V7GFX-208)
30oct13,y_f  written (US18166)
*/

/*
DESCRIPTION
This file defines the audio functions and structures for the audio module
*/

#ifndef __INCaudioLibCoreh
#define __INCaudioLibCoreh

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
#include <tickLib.h>
#include <string.h>
#ifndef _WRS_KERNEL
#include <strings.h>
#endif /* _WRS_KERNEL */

#if __cplusplus
extern "C" {
#endif /* __cplusplus  */

/* defines */

#define AUDIO_DEV_PREFIX                    "/audio/"
#define AUDIO_DEFAULT_DEV                   "/audio/0"
#define AUDIO_DEV_NAME_LEN                  (32)
#define AUDIO_DEV_MAX                       (32)

/* audio out path */

#define AUDIO_OUT_MASK                      (0xFFFF)
#define AUDIO_LINE_OUT                      (1 << 0)
#define AUDIO_SPEAKER_OUT                   (1 << 1)
#define AUDIO_HP_OUT                        (1 << 2)

/* audio in path */

#define AUDIO_IN_MASK                       (0xFFFF << 16)
#define AUDIO_LINE_IN                       (1 << 16)
#define AUDIO_MIC_IN                        (1 << 17)

#define AUDIO_DEV_INIT_TASK_PRIO            200
#define AUDIO_DEV_INIT_TASK_STACK_SIZE      (1024 * 8)

/* typedefs */

typedef enum audioIoCtrl
    {
    AUDIO_DEV_ENABLE = (1 << 8),    /* enable audio devices */
    AUDIO_DEV_DISABLE,              /* disable audio devices */
    AUDIO_START,                    /* start transferring data */
    AUDIO_STOP,                     /* stop transferring data */
    AUDIO_SET_DATA_INFO,            /* configure devices with the information */
    AUDIO_SET_VOLUME,               /* configure the volume */
    AUDIO_SET_BUFTIME,              /* set the buffer time (ms) */
    AUDIO_SET_PATH,                 /* set audio in or out path */
    AUDIO_GET_DEV_INFO,             /* get the device information */
    AUDIO_GET_DATA_INFO,            /* get the information of current data */
    AUDIO_GET_VOLUME,               /* get the volume */
    AUDIO_GET_BUFTIME,              /* get the buffer time (ms) */
    AUDIO_GET_PATH                  /* get audio in or out path */
    } AUDIO_IO_CTRL;

typedef struct audioDataInfo
    {
    UINT32  sampleRate;
    UINT8   sampleBits;         /* valid data bits in data buffer */
    UINT8   sampleBytes;        /* size of sample in data buffer */
    UINT8   channels;
    BOOL    useMsb;             /* valid data stores as MSB or LSB */
    } AUDIO_DATA_INFO;

typedef struct audioVolume
    {
    UINT32  path;
    UINT8   left;               /* left volume */
    UINT8   right;              /* right volume */
    } AUDIO_VOLUME;

typedef struct audioDevInfo
    {
    char    name[AUDIO_DEV_NAME_LEN + 1];
    UINT32  availPaths;     /* available audio paths */
    UINT32  defPaths;       /* default audio in and out paths */
    UINT8   bytesPerSample; /* size of sample in device buffer */
    UINT8   maxChannels;    /* device maximum channels */
    BOOL    useMsb;         /* device data is MSB first */
    } AUDIO_DEV_INFO;

typedef union audioIoctlArg
    {
    AUDIO_VOLUME    volume;
    AUDIO_DEV_INFO  devInfo;
    AUDIO_DATA_INFO dataInfo;
    UINT32          path;
    UINT32          bufTime;            /* buffer time (ms) */
    ssize_t         bufSize;
    } AUDIO_IOCTL_ARG;

#ifdef _WRS_KERNEL
typedef struct audioDev
    {
    DL_NODE         node;
    AUDIO_DEV_INFO  devInfo;
    UINT32          unit;
    UINT32          attTransUnit;   /* the transceiver which codec device attached to */
    void *          extension;      /* optional driver extensions */
    FUNCPTR         open;
    FUNCPTR         close;
    FUNCPTR         read;
    FUNCPTR         write;
    FUNCPTR         ioctl;
    } AUDIO_DEV;
#endif /* _WRS_KERNEL */

/* function declarations */

#ifdef _WRS_KERNEL
STATUS  audioCoreRegCodec (AUDIO_DEV * pCodecsInfo);
STATUS  audioCoreUnregCodec (AUDIO_DEV * pCodecsInfo);
STATUS  audioCoreRegTransceiver (AUDIO_DEV * pTransceiverInfo, int codecUnit);
STATUS  audioCoreUnregTransceiver (AUDIO_DEV * pTransceiverInfo);
#endif /* _WRS_KERNEL */

#if __cplusplus
}
#endif /* __cplusplus  */

#endif /* __INCaudioLibCoreh */
