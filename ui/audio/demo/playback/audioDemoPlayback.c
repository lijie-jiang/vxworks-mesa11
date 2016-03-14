/* audioDemoPlayback.c - Audio Driver Framework Playback Demo */

/*
 * Copyright (c) 2014-2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */ 

/*
modification history
--------------------
28sep15,c_l  support non-blocking audio device (V7GFX-252) 
14feb14,y_f  written (US18177)
*/

/*
DESCRIPTION
This file provides a sample of the use of the audio driver framework. An audio
file formatted as a <wav> file is played on the audio device.
*/

/* includes */

#include <audioLibCore.h>
#include <audioLibWav.h>
#include <stdlib.h>
#ifdef _WRS_KERNEL
#include <unistd.h>
#endif /* _WRS_KERNEL */

/* typedefs */

typedef struct audInfo
    {
    AUDIO_DATA_INFO dataInfo;
    ssize_t         dataSize;
    ssize_t         dataStart;
    int             fd;
    int             ad;
    } AUD_INFO;

/* function declarations */

LOCAL STATUS    audioOpenFile (AUD_INFO * pAudInfo, UINT8 * fileName);
LOCAL STATUS    audioPlay (char * fileName, UINT8 volume, BOOL block);

/* functions */

#ifdef _WRS_KERNEL
/*******************************************************************************
*
* audPlay - play an audio stream
*
* This routine spawns a task to play an audio stream contained within the file
* specified by <filename>. The audio stream is formatted as a <wav> file.
*
* RETURNS: OK if successful; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS audPlay
    (
    char *  filename,     /* audio file */
    UINT8   volume,       /* volume, from 0 to 100 */
    BOOL    block         /* blocking or non-blocking */
    )
    {
    TASK_ID     tid;
#ifdef _WRS_CONFIG_SMP
    cpuset_t    affinity = 0;
#endif /* _WRS_CONFIG_SMP */

    tid = taskSpawn ("tAudioPlayDemo", 110, 0, 8192, (FUNCPTR)audioPlay,
                     (_Vx_usr_arg_t)filename, (_Vx_usr_arg_t)volume,
                     (_Vx_usr_arg_t)block, 0, 0, 0, 0, 0, 0, 0);
    if (TASK_ID_ERROR == tid)
        {
        return ERROR;
        }

#ifdef _WRS_CONFIG_SMP
    CPUSET_ZERO (affinity);
    CPUSET_SET (affinity, (vxCpuConfiguredGet () - 1));
    if (taskCpuAffinitySet (tid, affinity) == ERROR)
       {
       (void)taskDelete (tid);
       return ERROR;
       }

    (void)taskActivate (tid);
#endif /* _WRS_CONFIG_SMP */

    return OK;
    }

#else

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

    return audioPlay (argv [1], (UINT8)atoi (argv [2]), (BOOL)atoi (argv [3]));
    }
#endif /* _WRS_KERNEL */

/*******************************************************************************
*
* audioPlay - play an audio file
*
* This routine plays the audio file <filename> on an audio device. The audio
* file is read to obtain the header information. The audio stream is checked to
* see if the file is formated as a wav file. Using the header information the
* audio device is placed in the proper mode and then the audio stream is sent to
* the audio device.
*
* RETURNS: OK when the audio file was successfully played; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS audioPlay
    (
    char *  filename,     /* audio file */
    UINT8   volume,       /* volume, from 0 to 100 */
    BOOL    block         /* blocking or non-blocking */
    )
    {
    UINT8 *         buffer = NULL;
    UINT32          bufTime;
    UINT32          bufSize;
    ssize_t         dataSize;
    ssize_t         readSize;
    ssize_t         writeSize;
    ssize_t         ret;
    AUD_INFO        audInfo;
    AUDIO_IOCTL_ARG ioctlArg;
    UINT32          defPath;

    bzero ((char *)&audInfo, sizeof (AUD_INFO));
    audInfo.ad  = ERROR;
    audInfo.fd  = -1;
    bufTime     = 2000; /* buffer time of audio driver is 2s */

    if (NULL == filename)
        {
        return ERROR;
        }

    /* open audio file and get the file information */

    if (ERROR == audioOpenFile (&audInfo, (UINT8 *)filename))
        {
        goto error;
        }

    if (audInfo.fd < 0)
        {
        goto error;
        }

    /* open the audio device */

    if (block)
        {
        audInfo.ad = open (AUDIO_DEFAULT_DEV, O_RDWR, 0);
        printf ("Audio device blocking open - %s\n", AUDIO_DEFAULT_DEV);
        }
    else
        {
        audInfo.ad = open (AUDIO_DEFAULT_DEV, O_RDWR | O_NONBLOCK, 0);
        printf ("Audio device non-blocking open - %s\n", AUDIO_DEFAULT_DEV);
        }    
        
    if (ERROR == audInfo.ad)
        {
        printf ("Audio device unavailable - %s\n", AUDIO_DEFAULT_DEV);
        goto error;
        }

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    if (OK != ioctl (audInfo.ad, AUDIO_GET_DEV_INFO, (char *)&ioctlArg))
        {
        printf ("Get device information failed!\n");
        goto error;
        }

    printf ("----------Audio Device Information----------\n");
    printf ("Device Name: %s\n", ioctlArg.devInfo.name);
    printf ("Device Available Paths: 0x%x\n", ioctlArg.devInfo.availPaths);
    printf ("Device Default Paths: 0x%x\n", ioctlArg.devInfo.defPaths);
    printf ("Device Sample Bytes: 0x%x\n", ioctlArg.devInfo.bytesPerSample);
    printf ("Device Maximum Channels: 0x%x\n", ioctlArg.devInfo.maxChannels);
    printf ("Device MSB Enable: 0x%x\n", ioctlArg.devInfo.useMsb);
    printf ("--------------------End---------------------\n");

    defPath = ioctlArg.devInfo.defPaths & (UINT32)AUDIO_OUT_MASK;

    /* set the output path */

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.path = defPath;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_PATH, (char *)&ioctlArg))
        {
        printf ("Set device path failed!\n");
        goto error;
        }

    /* set the device in proper mode for audio form characteristics */

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    bcopy ((char *)&audInfo.dataInfo, (char *)&(ioctlArg.dataInfo),
           sizeof (AUDIO_DATA_INFO));
    if (OK != ioctl (audInfo.ad, AUDIO_SET_DATA_INFO, (char *)&ioctlArg))
        {
        printf ("Set data information failed!\n");
        goto error;
        }

    bufSize = bufTime * audInfo.dataInfo.sampleRate *
              audInfo.dataInfo.channels / 1000 *
              (audInfo.dataInfo.sampleBits >> 3);

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.bufTime = bufTime;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_BUFTIME, (char *)&ioctlArg))
        {
        printf ("Set buffer time failed!\n");
        goto error;
        }

    buffer = (UINT8 *)calloc (1, bufSize);
    if(NULL == buffer)
        {
        printf ("Memory is not enough\n");
        goto error;
        }

    if (OK != ioctl (audInfo.ad, AUDIO_DEV_ENABLE, NULL))
        {
        printf ("Enable device failed!\n");
        goto error;
        }

    if (OK != ioctl (audInfo.ad, AUDIO_START, NULL))
        {
        printf ("Start playback failed!\n");
        goto error;
        }

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.volume.path    = defPath;
    ioctlArg.volume.left    = volume;
    ioctlArg.volume.right   = volume;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_VOLUME, (char *)&ioctlArg))
        {
        printf ("Set volume failed!\n");
        goto error;
        }

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.volume.path    = defPath;
    if (OK != ioctl (audInfo.ad, AUDIO_GET_VOLUME, (char *)&ioctlArg))
        {
        printf ("Get volume failed!\n");
        goto error;
        }

    printf ("Current playback volume is %d\n", ioctlArg.volume.left);

    /* position to the start of the audio data */

    if (ERROR == lseek (audInfo.fd, audInfo.dataStart, SEEK_SET))
        {
        printf ("Seek file failed!\n");
        goto error;
        }

    dataSize = audInfo.dataSize;
    while (dataSize > 0)
        {
        readSize = read (audInfo.fd, (char *)buffer, bufSize);
        if (readSize <= 0)
            {
            break;
            }            
        dataSize -= readSize;            
        
        writeSize = 0;
        while (readSize > 0)
            {
            if (!block)   
                {
                bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));            
                if (OK != ioctl (audInfo.ad, FIONFREE, (char *)&ioctlArg))
                    {
                    printf ("Get free byte count on device failed!\n");
                    goto error;
                    }
                if (ioctlArg.bufSize <= 0)
                    {                                      
                    continue;
                    }
                }
            ret = write (audInfo.ad, (char *)buffer + writeSize, readSize);
            if (ret < 0)
                {
                break;
                }           
            writeSize += ret;       
            readSize -= ret;            
            }
        }

    /* close the audio file and the audio device */

    (void)ioctl (audInfo.ad, AUDIO_STOP, NULL);
    (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL);
    (void)close (audInfo.ad);
    (void)close (audInfo.fd);
    free (buffer);

    printf ("Play end\n");
    return OK;

error:
    if (NULL != buffer)
        {
        free (buffer);
        }
    if (audInfo.fd >= 0)
        {
        (void)close (audInfo.fd);
        }
    if (ERROR != audInfo.ad)
        {
        (void)close (audInfo.ad);
        }

    printf ("Play ERROR!\n");
    return ERROR;
    }

/*******************************************************************************
*
* audioOpenFile - open audio file and get the information
*
* This routine opens audio file and gets the information.
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS audioOpenFile
    (
    AUD_INFO *  pAudInfo,
    UINT8 *     fileName
    )
    {
    int     sampleBits;
    ssize_t size = 0;
    int     channels;
    UINT32  sampleRate;
    UINT32  samples;
    UINT32  dataStart;

    /* open the audio file */

    pAudInfo->fd = open ((char *)fileName, O_RDONLY, 0666);
    if (pAudInfo->fd < 0)
        {
        printf ("Error opening file %s\n", fileName);
        return ERROR;
        }

    /* assume a wav file and read the wav form header file */

    if (audioWavHeaderRead (pAudInfo->fd, &channels, &sampleRate, &sampleBits,
                            &samples, &dataStart) != OK)
        {
        return ERROR;
        }

    if ((16 != sampleBits) && (24 != sampleBits) & (32 != sampleBits))
        {
        sampleBits = 8;
        }

    size = samples * channels * (sampleBits >> 3);

    /* print characteristics of the sound data stream  */

    printf ("File name:   %s\n", fileName);
    printf ("Channels:    %d\n", channels);
    printf ("Sample Rate: %d\n", sampleRate);
    printf ("Sample Bits: %d\n", sampleBits);
    printf ("samples:     %d\n", samples);

    pAudInfo->dataInfo.channels     = (UINT8)channels;
    pAudInfo->dataInfo.sampleBits   = (UINT8)sampleBits;
    pAudInfo->dataInfo.sampleBytes  = (UINT8)(sampleBits >> 3);
    pAudInfo->dataInfo.sampleRate   = sampleRate;
    pAudInfo->dataInfo.useMsb       = 0;
    pAudInfo->dataStart             = dataStart;
    pAudInfo->dataSize              = size;

    return OK;
    }
