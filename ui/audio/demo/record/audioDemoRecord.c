/* audioDemoRecord.c - Audio Driver Framework Record Demo */

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
This file provides a sample of the use of the audio driver framework. It
demonstrates the method to open the audio device, record PCM data from the
device and play the PCM data on the device.
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
    int             fd;
    int             ad;
    } AUD_INFO;

/* function declarations */

LOCAL STATUS    audioRec (char * fileName, UINT32 sampleRate, UINT32 sampleBits,
                          UINT8 volume, UINT32 recPeriod, BOOL block);

/* functions */

#ifdef _WRS_KERNEL
/*******************************************************************************
*
* audRec - record and play an audio stream
*
* This routine spawns a task to record and play an audio stream.
*
* RETURNS: OK if successful; otherwise ERROR
*
* ERRNO: N/A
*
* \NOMANUAL
*/

STATUS audRec
    (
    char *  fileName,       /* audio file */
    UINT32  sampleRate,     /* record sample rate */
    UINT32  sampleBits,     /* record sample bits */
    UINT8   volume,         /* volume, from 0 to 100 */
    UINT32  recPeriod,      /* record period in second */
    BOOL    block           /* blocking or non-blocking */
    )
    {
    TASK_ID     tid;
#ifdef _WRS_CONFIG_SMP
    cpuset_t    affinity = 0;
#endif /* _WRS_CONFIG_SMP */

    tid = taskSpawn ("tAudioRecordDemo", 110, 0, 8192, (FUNCPTR)audioRec,
                     (_Vx_usr_arg_t)fileName, (_Vx_usr_arg_t)sampleRate,
                     (_Vx_usr_arg_t)sampleBits, (_Vx_usr_arg_t)volume,
                     (_Vx_usr_arg_t)recPeriod, (_Vx_usr_arg_t)block,
                     0, 0, 0, 0);
    if (TASK_ID_ERROR == tid)
        {
        return ERROR;
        }

#ifdef _WRS_CONFIG_SMP
    CPUSET_ZERO (affinity);
    CPUSET_SET (affinity, (vxCpuConfiguredGet () - 1));
    if (ERROR == taskCpuAffinitySet (tid, affinity))
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
    if (argc <= 5)
        {
        return ERROR;
        }

    return audioRec (argv [1], (UINT32)atoi (argv [2]), (UINT32)atoi (argv [3]),
                     (UINT8)atoi (argv [4]), (UINT32)atoi (argv [5]), 
                     (BOOL)atoi (argv [6]));
    }
#endif /* _WRS_KERNEL */

/*******************************************************************************
*
* audioRec - record and play an audio stream
*
* This routine records an audio stream and plays it on an audio device.
*
* RETURNS: OK when the audio stream was successfully recorded and played;
*          otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS audioRec
    (
    char *  fileName,       /* audio file */
    UINT32  sampleRate,     /* record sample rate */
    UINT32  sampleBits,     /* record sample bits */
    UINT8   volume,         /* volume, from 0 to 100 */
    UINT32  recPeriod,      /* record period in second */
    BOOL    block           /* blocking or non-blocking */
    )
    {
    UINT8 *         buffer = NULL;
    UINT32          bufTime;
    ssize_t         dataSize;
    ssize_t         readSize;
    ssize_t         writeSize;
    ssize_t         retRead;
    ssize_t         retWrite;
    AUD_INFO        audInfo;
    AUDIO_IOCTL_ARG ioctlArg;
    UINT32          defPath;
    UINT32          samples;

    if (0 == recPeriod)
        {
        printf ("Record period is 0\n");
        return ERROR;
        }

    bzero ((char *)&audInfo, sizeof (AUD_INFO));
    audInfo.ad  = ERROR;
    bufTime     = 1000 * 2; /* buffer time of audio driver is 2s */

    audInfo.dataInfo.channels       = 2;
    audInfo.dataInfo.sampleBits     = (UINT8)sampleBits;
    audInfo.dataInfo.sampleRate     = sampleRate;
    audInfo.dataInfo.sampleBytes    = (UINT8)(sampleBits >> 3);

    audInfo.dataSize = audInfo.dataInfo.sampleBytes * sampleRate *
                       audInfo.dataInfo.channels * recPeriod;

    audInfo.fd = ERROR;
    if (NULL != fileName)
        {
        audInfo.fd = open (fileName, O_RDWR | O_CREAT, 0644);
        if (audInfo.fd < 0)
            {
            printf ("Error create file %s\n", fileName);            
            }
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

    defPath = ioctlArg.devInfo.defPaths;

    /* set the output and input path */

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

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.bufTime = bufTime;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_BUFTIME, (char *)&ioctlArg))
        {
        printf ("Set buffer time failed!\n");
        goto error;
        }

    buffer = (UINT8 *)calloc (1, audInfo.dataSize);
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

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.volume.path    = defPath;
    ioctlArg.volume.left    = volume;
    ioctlArg.volume.right   = volume;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_VOLUME, (char *)&ioctlArg))
        {
        printf ("Set volume failed!\n");
        goto error;
        }

    /* start recording and playback */

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.path = defPath;
    if (OK != ioctl (audInfo.ad, AUDIO_START, (char *)&ioctlArg))
        {
        printf ("Start recording failed!\n");
        goto error;
        }

    printf ("Record begin\n");    

    dataSize = audInfo.dataSize;
    readSize = 0;
    writeSize = 0;
    while (dataSize > 0)
        {         
        if (!block)
            {
            bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));            
            if (OK != ioctl (audInfo.ad, FIONREAD, (char *)&ioctlArg))
                {
                printf ("Get num chars available to read failed!\n");
                goto error;
                }
            if (ioctlArg.bufSize <= 0)
                {                             
                continue;
                }
            }      
        
        retRead = read (audInfo.ad,(char *)buffer + readSize, dataSize);
        if (retRead < 0)
            {
            printf ("Read data failed!\n");
            goto error;
            }    
        readSize += retRead;
        dataSize -= readSize;             
        
        while (retRead > 0)
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
            retWrite = write (audInfo.ad, (char *)buffer + writeSize, retRead);
            if (retWrite < 0)
                {
                break;
                }    
            writeSize += retWrite;             
            retRead -= writeSize;            
            }        
        }
        
    printf ("Record stop\n");
    
    if (ERROR != audInfo.fd)
        {
        printf ("Write data to file...\n");
        samples =
            (UINT32)(audInfo.dataSize / (audInfo.dataInfo.sampleBits >> 3));
        if (ERROR == audioWavHeaderWrite (audInfo.fd, audInfo.dataInfo.channels,
                                          audInfo.dataInfo.sampleRate,
                                          audInfo.dataInfo.sampleBits, samples))
            {
            printf ("Write wav file header failed!\n");
            goto error;
            }
            
        dataSize = audInfo.dataSize;         
        while (dataSize > 0)
            {
            writeSize = write (audInfo.fd, (char *)buffer, dataSize);
            if (writeSize < 0)
                {
                printf ("Write wav file data failed!\n");
                goto error;
                }
            dataSize -= writeSize;            
            } 
        }

    /* close the audio file and the audio device */

    ioctlArg.path = defPath;
    (void)ioctl (audInfo.ad, AUDIO_STOP, (char *)&ioctlArg);
    (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL);
    (void)close (audInfo.ad);
    if (ERROR != audInfo.fd)
        {
        (void)close (audInfo.fd);
        }
    free (buffer);

    printf ("Record and Play end\n");
    return OK;

error:
    if (NULL != buffer)
        {
        free (buffer);
        }
    if (ERROR != audInfo.ad)
        {
        (void)close (audInfo.ad);
        }

    if (ERROR != audInfo.fd)
        {
        (void)close (audInfo.fd);
        }

    printf ("Record and Play ERROR!\n");
    return ERROR;
    }   
