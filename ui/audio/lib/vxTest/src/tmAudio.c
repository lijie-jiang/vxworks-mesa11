/* tmAudio.c - Audio Test */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */ 

/*
modification history
--------------------
17dec15,zjl  Only run [44100,32] based on the dev team
25nov15,zjl  sampleBits only run 16 and 24,remove 8 and 32
08oct15,c_l  create
*/

/*
DESCRIPTION
This file provides testing case. 

\cs
<module>
    <component>        INCLUDE_TM_AUDIO_LIB  </component>
    <minVxWorksVer>    7.0               </minVxWorksVer>
    <maxVxWorksVer>    .*                </maxVxWorksVer>
    <arch>             .*                         </arch>
    <cpu>              .*                          </cpu>
    <bsp>                                          </bsp>
</module>
\ce

*/


/* includes */

#include <vxTest.h>
#include <audioLibCore.h>
#include <audioLibWav.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#ifdef _WRS_KERNEL
#include <unistd.h>
#endif /* _WRS_KERNEL */

#include <xbdPartition.h>
#include <xbdRamDisk.h>
#include <dosFsLib.h>

/*defined*/

#define RAMDIR        "/ram"
#define SEPARATOR     '/'
#define BLOCKSIZE     512
#define AT_PATHLENGTH 128
#define MAD_AD        1
#define DISKSIZE (BLOCKSIZE * 20000)

#define     AUDIOERROR                  (-1)
#define     AUDIOOK                     (0)
#define     AUDIOERROR_OPEN             (1<<0)
#define     AUDIOERROR_CLOSE            (1<<1)
#define     AUDIOERROR_OPENDEV          (1<<2)
#define     AUDIOERROR_CLOSEDEV         (1<<3)
#define     AUDIOERROR_WAVHEADERREAD    (1<<4)
#define     AUDIOERROR_WAVHEADERWRITE   (1<<5)
#define     AUDIOERROR_DEV_ENABLE       (1<<6)
#define     AUDIOERROR_DEV_DISABLE      (1<<7)
#define     AUDIOERROR_START            (1<<8)
#define     AUDIOERROR_STOP             (1<<9)
#define     AUDIOERROR_SET_DATA_INFO    (1<<10)
#define     AUDIOERROR_SET_VOLUME       (1<<11)
#define     AUDIOERROR_SET_BUFTIME      (1<<12)
#define     AUDIOERROR_SET_PATH         (1<<13)
#define     AUDIOERROR_GET_DEV_INFO     (1<<14)
#define     AUDIOERROR_GET_DATA_INFO    (1<<15)
#define     AUDIOERROR_GET_VOLUME       (1<<16)
#define     AUDIOERROR_GET_BUFTIME      (1<<17)
#define     AUDIOERROR_GET_PATH         (1<<18)
#define     AUDIOERROR_WRITE            (1<<19)
#define     AUDIOERROR_CREATE           (1<<20)
#define     AUDIOERROR_MEMORY           (1<<21)
#define     AUDIOERROR_OPENMANY         (1<<22)

#define     AT_STOP         0
#define     AT_START        1
#define     AT_PLAY         2
#define     AT_PAUSE        3

#define     MIN(a, b)  (((a)<(b))?(a):(b))
#define     ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#define     atprintf printf

#define     AUDIO_IN_MASK_BITS  (16)
#define     AUDIO_IN_TYPES      ((AUDIO_LINE_IN|AUDIO_MIC_IN)>>AUDIO_IN_MASK_BITS)
#define     AUDIO_OUT_TYPES     (AUDIO_LINE_OUT|AUDIO_SPEAKER_OUT|AUDIO_HP_OUT)

#define     AUDIO_BUFTIME  2

enum AUDIO_OPTION
    {
    OPTION_BLOCK = 1,
    OPTION_SAMPLERATE,
    OPTION_SAMPLEBITS
    };

/* typedefs */

typedef struct dirent DIRENT;

typedef struct atInfo
    {
    AUDIO_DATA_INFO dataInfo;
    ssize_t         dataSize;
    ssize_t         dataStart;
    int             fd;
    int             ad;
    DIR *           dd;
    DIRENT *        entry;
    } AT_INFO;

/*global*/
#if 0
LOCAL UINT32  sampleRate[] = {8000,11025,12000,16000,22050,24000,32000,44100,48000,96000};     /* record sample rate */
LOCAL UINT32  sampleBits[] = {16, 24};     /* record sample bits */
#else
LOCAL UINT32  sampleRate[] = {44100};     /* record sample rate */
LOCAL UINT32  sampleBits[] = {32};     /* record sample bits */    
#endif

LOCAL UINT32  playCircle = 0;
LOCAL char    workDir[AT_PATHLENGTH] = {0};
#define atGetDir() workDir

/*declaration*/

LOCAL STATUS   atSetDir(char* dir);
LOCAL void     atlsWd(char *dir);
LOCAL STATUS   atCreateDisk(UINT32 size);
LOCAL INT32    atOpenDir(char *dir, DIR **dirptr);
LOCAL DIRENT * atFindAudioFile(DIR *dirptr);
LOCAL INT32    atCloseDir(DIR *dirptr);
LOCAL INT32    atRewindDir(char *path);
LOCAL void     atDeviceInfoShow(AUDIO_DEV_INFO *pDevInfo);
LOCAL STATUS   atOpenFile(AT_INFO *  pAudInfo, char * fileName);
LOCAL STATUS   atPlayWav(char * filename, UINT8 volume, UINT32 bufTime, BOOL block);
LOCAL STATUS   atWriteFile(UINT32  sampleRate, UINT32  sampleBits, UINT32  recPeriod, UINT32  channels);
LOCAL STATUS   atRecWav(UINT32 sampleRate, UINT8 sampleBits, UINT8 volume, UINT32 recPeriod, UINT32 channels, UINT32 bufTime, BOOL block);
LOCAL STATUS   atFileListRead(char *path);
LOCAL STATUS   atFileListPlay(char *path, UINT32 circle, BOOL block);
LOCAL STATUS   atFileListWrite(void);
LOCAL STATUS   atFileListRec(UINT32 recPeriod, UINT8 volume, BOOL block);
LOCAL STATUS   atDeviceIoctl(void);
LOCAL STATUS   atDeviceOpenClose(UINT32 num);
LOCAL STATUS   atDeviceOpenClose2(void);

VXTEST_STATUS tmStandIO(UINT32 times);
VXTEST_STATUS tmWavFilesWriteRead();
VXTEST_STATUS tmWavFilesRecPlay();

/* functions */
void atInit()
    {
    atCreateDisk(100);
    }
    
/*******************************************************************************
*
* atLswd - list the files in the working directory
*
* This routine list the files in the working directory.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void atLswd(char *dir)
    {
    DIR *dirptr = NULL;
    struct dirent *entry;
    if(!dir)
        {
        dir = atGetDir();
        }

    if((dirptr = opendir(dir)) == NULL)
        {
        atprintf("open dir error !\n");
        return;
        }
    else
        {
        while ((entry = readdir(dirptr))!=NULL)
            {
            atprintf("%s\n", entry->d_name);
            }
        closedir(dirptr);
        }

    return;
    }

/*******************************************************************************
*
* atCreateDisk - create the ramdisk working space in the ram
*
* This routine creates the ramdisk working space in the ram.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL STATUS atCreateDisk(UINT32 size)
    {
    STATUS error;
    device_t xbd;
    /* Create a RAM disk. Don't support partitions */
    if(size)
        {
        size<<=20;
        }
    else
        {
        size = DISKSIZE;
        }

    xbd = xbdRamDiskDevCreate (BLOCKSIZE, size, 0, RAMDIR);
    if (xbd == NULLDEV)
        {
        atprintf("Failed to create RAM disk. errno = 0x%x\n", errno);
        return AUDIOERROR;
        }

    error = dosFsVolFormat (RAMDIR, 0,0);
    if (error != AUDIOOK)
        {
        atprintf("Failed to format RAM disk. errno = 0x%x\n", errno);
        return AUDIOERROR;
        }
    atprintf ("%s now ready for use.\n", RAMDIR);

    atSetDir(RAMDIR);

    return AUDIOOK;
    }

/*******************************************************************************
*
* atFindAudioFile - find next audio file in an opened directory
*
* This routine find next audio file in an opened directory.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL DIRENT* atFindAudioFile(DIR *dirptr)
    {
    DIRENT *entry;
    UINT32 sl;
    const char *suffix = ".wav";

    if(dirptr == NULL)
        {
        return NULL;
        }
    else
        {
        while ((entry = readdir(dirptr))!=NULL)
            {
            sl = strlen(entry->d_name);
            if(sl > 4)
                {
                if(!strncmp(suffix, entry->d_name+ sl - 4, 4))
                    {
                    atprintf("%s\n", entry->d_name);
                    return entry;
                    }
                }
            }
        }

    return NULL;
    }

/*******************************************************************************
*
* atDeviceInfoShow - print the audio device information
*
* This routine print the audio device information.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL void atDeviceInfoShow(AUDIO_DEV_INFO *pDevInfo)
    {
    atprintf ("--Audio Device Information--\n");
    atprintf ("Device Name: %s\n", pDevInfo->name);
    atprintf ("Device Available Paths: 0x%x\n", pDevInfo->availPaths);
    atprintf ("Device Default Paths: 0x%x\n", pDevInfo->defPaths);
    atprintf ("Device Sample Bytes: 0x%x\n", pDevInfo->bytesPerSample);
    atprintf ("Device Maximum Channels: 0x%x\n", pDevInfo->maxChannels);
    atprintf ("Device MSB Enable: 0x%x\n", pDevInfo->useMsb);
    atprintf ("--Information End---------\n");
    return;
    } 
    
/*******************************************************************************
*
* atSetDir - This routine list the files in the working directory.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL STATUS atSetDir(char* dir)
    {
    UINT32 l = 0;

    if(!dir)
        {
        return AUDIOERROR;
        }

    if(strlen(dir) >= sizeof(workDir))
        {
        return AUDIOERROR;
        } 

    snprintf(workDir, sizeof(workDir), "%s", dir);

    l = strlen(workDir);

    if(!l)
        {
        return AUDIOERROR;
        } 

    if(SEPARATOR == workDir[l -1])
        {
        workDir[l -1] = EOS;
        }
    atprintf("Set workdir to %s\n",workDir);
    return AUDIOOK;
    }

/*******************************************************************************
*
* atOpenDir - open a directory
*
* This routine open a directory.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL INT32 atOpenDir(char *dir, DIR **dirptr)
    {
    if((*dirptr = opendir(dir)) == NULL)
        {
        atprintf("open dir error !\n");
        return AUDIOERROR;
        }

    return AUDIOOK;
    }



/*******************************************************************************
*
* atCloseDir - close a directory
*
* This routine close a directory.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL INT32 atCloseDir(DIR *dirptr)
    {
    return closedir(dirptr);
    }

/*******************************************************************************
*
* atRewindDir - reset the directory stream to start
*
* This routine reset the directory stream to start.
*
* RETURNS: N/A
*
* ERRNO: N/A
*
* \NOMANUAL
*/

LOCAL INT32 atRewindDir(char * path)
    {   
    DIR *dirptr;
    if((dirptr = opendir(path)) == NULL)
        {
        atprintf("open dir error !\n");
        return AUDIOERROR;
        }
    
    rewinddir(dirptr);
    closedir(dirptr);
    
    return AUDIOOK; 
    }

/*******************************************************************************
*
* atOpenFile - open audio file and get the information
*
* This routine opens audio file and gets the information.
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atOpenFile
    (
    AT_INFO *  pAudInfo,
    char *     fileName
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
        atprintf ("Error opening file %s\n", fileName);
        return AUDIOERROR_OPEN;
        }

    /* assume a wav file and read the wav form header file */

    if (audioWavHeaderRead (pAudInfo->fd, &channels, &sampleRate, &sampleBits,
                            &samples, &dataStart) != OK)
        {
        return AUDIOERROR_WAVHEADERREAD;
        }

    if ((16 != sampleBits) && (24 != sampleBits) & (32 != sampleBits))
        {
        sampleBits = 8;
        }

    size = samples * channels * (sampleBits >> 3);

    /* print characteristics of the sound data stream  */

    atprintf ("File name:   %s\n", fileName);
    atprintf ("Channels:    %d\n", channels);
    atprintf ("Sample Rate: %d\n", sampleRate);
    atprintf ("Sample Bits: %d\n", sampleBits);
    atprintf ("samples:     %d\n", samples);

    pAudInfo->dataInfo.channels     = (UINT8)channels;
    pAudInfo->dataInfo.sampleBits   = (UINT8)sampleBits;
    pAudInfo->dataInfo.sampleBytes  = (UINT8)(sampleBits >> 3);
    pAudInfo->dataInfo.sampleRate   = sampleRate;
    pAudInfo->dataInfo.useMsb       = 0;
    pAudInfo->dataStart             = dataStart;
    pAudInfo->dataSize              = size;

    return AUDIOOK;
    }
    
/*******************************************************************************
*
* atWriteFile - create an audio file
*
* This routine create an audio file.
*
* RETURNS: OK when the audio stream was successfully recorded and played;
*          otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atWriteFile
    (
    UINT32  sampleRate,     
    UINT32  sampleBits,  
    UINT32  recPeriod, 
    UINT32  channels
    )
    {
    char fileName[AT_PATHLENGTH];       /* audio file */
    UINT8 *       buffer = NULL;
    AT_INFO    audInfo;
    UINT32        samples;
    UINT32        status = AUDIOOK;

    if (0 == recPeriod)
        {
        recPeriod = 10;
        }

    if(strlen(workDir))
        {
        snprintf(fileName, AT_PATHLENGTH, "%s/Rec_%d_%d_%d.wav", workDir, sampleRate, sampleBits, channels);
        }
    else
        {
        atprintf ("Please call atSetDir to set workdir\n");
        return AUDIOERROR;
        }

    bzero ((char *)&audInfo, sizeof (AT_INFO));

    channels = ((channels == 1)?1:2);
    audInfo.dataInfo.channels       = channels;
    audInfo.dataInfo.sampleBits    = (UINT8)sampleBits;
    audInfo.dataInfo.sampleRate   = sampleRate;
    audInfo.dataInfo.sampleBytes  = (UINT8)(sampleBits >> 3);

    audInfo.dataSize = audInfo.dataInfo.sampleBytes * sampleRate *
                       audInfo.dataInfo.channels * recPeriod;

    audInfo.fd = open (fileName, O_RDWR | O_CREAT, 0666);
    if (audInfo.fd < 0)
        {
        atprintf ("Error create file %s\n", fileName);
        return AUDIOERROR_OPEN;
        }

    buffer = (UINT8 *)malloc(audInfo.dataSize);
    if(NULL == buffer)
        {
        (void)close (audInfo.fd);
        (void)remove(fileName);
        atprintf ("Memory is not enough\n");
        return AUDIOERROR_MEMORY;
        }

    if (AUDIOERROR != audInfo.fd)
        {
        samples = (UINT32)(audInfo.dataSize / (audInfo.dataInfo.sampleBits >> 3));
        if (ERROR == audioWavHeaderWrite (audInfo.fd, audInfo.dataInfo.channels,
                                          audInfo.dataInfo.sampleRate,
                                          audInfo.dataInfo.sampleBits, samples))
            {
            status = AUDIOERROR_WAVHEADERWRITE;
            atprintf ("Write wav file header failed!\n");
            }

        if (audInfo.dataSize != write (audInfo.fd, (char *)buffer, audInfo.dataSize))
            {
            status = AUDIOERROR_WRITE;
            atprintf ("Write wav file data failed!\n");
            }
        }
    free(buffer);
    /* close the audio file and the audio device */
    if (ERROR != audInfo.fd)
        {
        (void)close (audInfo.fd);
        }

    if(OK != status)
        {
        (void)remove(fileName);
        }
    return status;
    }

/*******************************************************************************
*
* atPlayWav - play audio file
*
* This routine plays audio file.
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atPlayWav
    (
    char *  filename,     
    UINT8   volume,       
    UINT32  bufTime,
    BOOL    block
    )
    {
    UINT8 *         buffer = NULL;    
    UINT32          bufSize;
    ssize_t         dataSize;
    ssize_t         readSize;
    ssize_t         writeSize;
    ssize_t         ret;    
    AT_INFO         audInfo;
    AUDIO_IOCTL_ARG ioctlArg;
    UINT32          defPath;
    STATUS          status = AUDIOOK;

    bzero ((char *)&audInfo, sizeof (AT_INFO));
    audInfo.ad  = -1;
    audInfo.fd  = -1;    

    if (NULL == filename)
        {
        return AUDIOERROR;
        }

    /* open audio file and get the file information */
    status = atOpenFile (&audInfo, filename);
    if (AUDIOOK != status)
        {
        goto atperror;
        }

    if (audInfo.fd < 0)
        {
        status = AUDIOERROR;
        goto atperror;
        }
        
    /* open the audio device */

    if (block)
        {
        audInfo.ad = open (AUDIO_DEFAULT_DEV, O_RDWR, 0);
        atprintf ("Audio device blocking open - %s\n", AUDIO_DEFAULT_DEV);
        }
    else
        {
        audInfo.ad = open (AUDIO_DEFAULT_DEV, O_RDWR | O_NONBLOCK, 0);
        atprintf ("Audio device non-blocking open - %s\n", AUDIO_DEFAULT_DEV);
        }    
        
    if (ERROR == audInfo.ad)
        {        
        atprintf ("Audio device unavailable - %s\n", AUDIO_DEFAULT_DEV);
        status = AUDIOERROR;
        goto atperror;
        }  

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    if (OK != ioctl (audInfo.ad, AUDIO_GET_DEV_INFO, (char *)&ioctlArg))
        {        
        atprintf ("Get device information failed!\n");
        status = AUDIOERROR;
        goto atperror;
        }

    atDeviceInfoShow(&(ioctlArg.devInfo));

    defPath = ioctlArg.devInfo.defPaths & (UINT32)AUDIO_OUT_MASK;

    /* set the output path */

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.path = defPath;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_PATH, (char *)&ioctlArg))
        {        
        atprintf ("Set device path failed!\n");
        status = AUDIOERROR;
        goto atperror;
        }

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    if (OK != ioctl (audInfo.ad, AUDIO_GET_PATH, (char *)&ioctlArg))
        {        
        atprintf ("Get device path failed!\n");
        status = AUDIOERROR;
        goto atperror;
        }
    if(ioctlArg.path != defPath)
        {
        atprintf ("Get device path failed!\n");
        status = AUDIOERROR;
        goto atperror;
        }

    /* set the device in proper mode for audio form characteristics */

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    bcopy ((char *)&audInfo.dataInfo, (char *)&(ioctlArg.dataInfo),
           sizeof (AUDIO_DATA_INFO));
    if (OK != ioctl (audInfo.ad, AUDIO_SET_DATA_INFO, (char *)&ioctlArg))
        {        
        atprintf ("Unsupported sampleRate:%d sampleBits:%d !\n",
                   ioctlArg.dataInfo.sampleRate, ioctlArg.dataInfo.sampleBits); 
        status = AUDIOOK;
        goto atperror;
        }
    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    if (OK != ioctl (audInfo.ad, AUDIO_GET_DATA_INFO, (char *)&ioctlArg))
        {        
        atprintf ("Get data information failed!\n");
        status = AUDIOERROR;
        goto atperror;
        }
    if(memcmp(&(audInfo.dataInfo), &(ioctlArg.dataInfo), sizeof(AUDIO_DATA_INFO)))
        {
        atprintf ("Unsupported sampleRate:%d sampleBits:%d !\n",
                   ioctlArg.dataInfo.sampleRate, ioctlArg.dataInfo.sampleBits);         
        status = AUDIOOK;
        goto atperror;
        }

    bufSize = bufTime * audInfo.dataInfo.sampleRate *
              audInfo.dataInfo.channels * audInfo.dataInfo.sampleBytes;

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.bufTime = bufTime * 1000;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_BUFTIME, (char *)&ioctlArg))
        {
        atprintf ("Set buffer time failed!\n");
        status = AUDIOERROR;
        goto atperror;
        }
        
    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    if (OK != ioctl (audInfo.ad, AUDIO_GET_BUFTIME, (char *)&ioctlArg))
        {
        atprintf ("Get buffer time failed!\n");
        status = AUDIOERROR;
        goto atperror;
        }
    if(ioctlArg.bufTime != bufTime * 1000)
        {  
        atprintf ("Get buffer time failed!\n");
        status = AUDIOERROR;
        goto atperror;
        }

    buffer = (UINT8 *)calloc (1, bufSize);
    if(NULL == buffer)
        {
        atprintf ("Memory is not enough\n");
        status = AUDIOERROR;
        goto atperror;
        }

    if (OK != ioctl (audInfo.ad, AUDIO_DEV_ENABLE, NULL))
        {        
        atprintf ("Enable device failed!\n");
        status = AUDIOERROR;
        goto atperror;
        }

    if (OK != ioctl (audInfo.ad, AUDIO_START, NULL))
        {        
        atprintf ("Start playback failed!\n");
        status = AUDIOERROR;
        (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL);
        goto atperror;
        }

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.volume.path    = defPath;
    ioctlArg.volume.left    = volume;
    ioctlArg.volume.right   = volume;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_VOLUME, (char *)&ioctlArg))
        {        
        atprintf ("Set volume failed!\n");
        status = AUDIOERROR;
        (void)ioctl (audInfo.ad, AUDIO_STOP, NULL);
        (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL);        
        goto atperror;
        }
    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.volume.path    = defPath;
    if (OK != ioctl (audInfo.ad, AUDIO_GET_VOLUME, (char *)&ioctlArg))
        {
        atprintf ("Get volume failed!\n");
        status = AUDIOERROR;
        (void)ioctl (audInfo.ad, AUDIO_STOP, NULL);
        (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL);        
        goto atperror;
        }
        
    atprintf ("Current playback volume is %d\n", ioctlArg.volume.left);

    /* position to the start of the audio data */

    if (ERROR == lseek (audInfo.fd, audInfo.dataStart, SEEK_SET))
        {
        atprintf ("Seek file failed!\n");
        status = AUDIOERROR;
        (void)ioctl (audInfo.ad, AUDIO_STOP, NULL);
        (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL);        
        goto atperror;
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
                    atprintf ("Get free byte count on device failed!\n");
                    status = AUDIOERROR;
                    (void)ioctl (audInfo.ad, AUDIO_STOP, NULL);
                    (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL);        
                    goto atperror;
                    }
                if (ioctlArg.bufSize <= 0)
                    { 
                    taskDelay(5);               
                    continue;
                    }
                }
                  
            ret = write (audInfo.ad, (char *)buffer + writeSize, readSize);
            if (ret == 0)
                {
                taskDelay(5);
                continue;
                }            
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

    atprintf ("Play %s end\n",filename);
    return AUDIOOK;

atperror:
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

    atprintf ("Play %s error\n",filename);
    return status;
    }
    
/*******************************************************************************
*
* atRecWav - record and play an audio stream
*
* This routine records an audio stream and plays it on an audio device.
*
* RETURNS: OK when the audio stream was successfully recorded and played;
*          otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atRecWav
    (
    UINT32  sampleRate,     
    UINT8   sampleBits,     
    UINT8   volume,    
    UINT32  recPeriod,  
    UINT32  channels,
    UINT32  bufTime,
    BOOL    block    
    )
    {
    UINT8 *         buffer = NULL;    
    ssize_t         dataSize;
    ssize_t         readSize;
    ssize_t         writeSize;
    ssize_t         retRead;
    ssize_t         retWrite;    
    AT_INFO         audInfo;
    AUDIO_IOCTL_ARG ioctlArg;
    UINT32          defPath;
    UINT32          samples;
    INT32           status = AUDIOOK;
    char            fileName[AT_PATHLENGTH];       /* audio file */

    if (0 == recPeriod)
        {
        atprintf ("Set Record period 15s\n");
        recPeriod = 15;
        }

    if(strlen(workDir))
        {
        snprintf(fileName, AT_PATHLENGTH, "%s/Rec_%d_%d_%d.wav", workDir, sampleRate, sampleBits, channels);
        }
    else
        {
        atprintf ("Please call atSetDir to set workdir\n");
        return AUDIOERROR;
        }
        
    bzero ((char *)&audInfo, sizeof (AT_INFO));
    audInfo.ad  = AUDIOERROR;
    
    audInfo.fd = open (fileName, O_RDWR | O_CREAT, 0666);
    if (audInfo.fd < 0)
        {
        atprintf ("Error create file %s\n", fileName);
        return AUDIOERROR;
        }  

    channels = ((channels == 1)?1:2);
    audInfo.dataInfo.channels       = channels;
    audInfo.dataInfo.sampleBits     = sampleBits;
    audInfo.dataInfo.sampleRate     = sampleRate;
    audInfo.dataInfo.sampleBytes    = sampleBits >> 3;

    audInfo.dataSize = audInfo.dataInfo.sampleBytes * sampleRate *
                       audInfo.dataInfo.channels * recPeriod;
        
    /* open the audio device */
    
    if (block)
        {
        audInfo.ad = open (AUDIO_DEFAULT_DEV, O_RDWR, 0);
        atprintf ("Audio device blocking open - %s\n", AUDIO_DEFAULT_DEV);
        }
    else
        {
        audInfo.ad = open (AUDIO_DEFAULT_DEV, O_RDWR | O_NONBLOCK, 0);
        atprintf ("Audio device non-blocking open - %s\n", AUDIO_DEFAULT_DEV);
        } 
        
    if (ERROR == audInfo.ad)
        {
        atprintf ("Audio device unavailable - %s\n", AUDIO_DEFAULT_DEV);
        status = AUDIOERROR;
        goto arwerror;
        } 

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    if (OK != ioctl (audInfo.ad, AUDIO_GET_DEV_INFO, (char *)&ioctlArg))
        {
        atprintf ("Get device information failed!\n");
        status = AUDIOERROR;
        goto arwerror;
        }

    atDeviceInfoShow(&(ioctlArg.devInfo));

    defPath = ioctlArg.devInfo.defPaths;

    /* set the output and input path */

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.path = defPath;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_PATH, (char *)&ioctlArg))
        {
        atprintf ("Set device path failed!\n");
        status = AUDIOERROR;
        goto arwerror;
        }

    /* set the device in proper mode for audio form characteristics */

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    bcopy ((char *)&audInfo.dataInfo, (char *)&(ioctlArg.dataInfo),
           sizeof (AUDIO_DATA_INFO));
    if (OK != ioctl (audInfo.ad, AUDIO_SET_DATA_INFO, (char *)&ioctlArg))
        {
        atprintf ("Unsupported sampleRate:%d sampleBits:%d !\n",
                   audInfo.dataInfo.sampleRate, audInfo.dataInfo.sampleBits);
        goto arwerror;
        }

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.bufTime = bufTime * 1000;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_BUFTIME, (char *)&ioctlArg))
        {
        atprintf ("Set buffer time failed!\n");
        status = AUDIOERROR;
        goto arwerror;
        }

    buffer = (UINT8 *)calloc (1, audInfo.dataSize);
    if(NULL == buffer)
        {
        atprintf ("Memory is not enough\n");
        status = AUDIOERROR;
        goto arwerror;
        }

    if (OK != ioctl (audInfo.ad, AUDIO_DEV_ENABLE, NULL))
        {
        atprintf ("Enable device failed!\n");
        status = AUDIOERROR;
        goto arwerror;
        }

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.volume.path    = defPath;
    ioctlArg.volume.left    = volume;
    ioctlArg.volume.right   = volume;
    if (OK != ioctl (audInfo.ad, AUDIO_SET_VOLUME, (char *)&ioctlArg))
        {
        atprintf ("Set volume failed!\n");
        status = AUDIOERROR;
        (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL); 
        goto arwerror;
        }

    /* start recording and playback */

    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    ioctlArg.path = defPath;
    if (OK != ioctl (audInfo.ad, AUDIO_START, (char *)&ioctlArg))
        {
        atprintf ("Start recording failed!\n");
        status = AUDIOERROR;
        (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL); 
        goto arwerror;
        }
        
    atprintf ("Record begin sampleRate:%d sampleBits:%d !\n",
               audInfo.dataInfo.sampleRate, audInfo.dataInfo.sampleBits);

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
                atprintf ("Get num chars available to read failed!\n");
                status = AUDIOERROR;
                ioctlArg.path = defPath;
                (void)ioctl (audInfo.ad, AUDIO_STOP, (char *)&ioctlArg);
                (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL); 
                goto arwerror;
                }
            if (ioctlArg.bufSize <= 0)
                {       
                taskDelay (5);
                continue;
                }
            }      
        
        retRead = read (audInfo.ad,(char *)buffer + readSize, dataSize);
        if (retRead == 0)
            {
            taskDelay (5);
            continue;
            }
        if (retRead < 0)
            {
            atprintf ("Read data failed!\n");
            status = AUDIOERROR;
            ioctlArg.path = defPath;
            (void)ioctl (audInfo.ad, AUDIO_STOP, (char *)&ioctlArg);
            (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL); 
            goto arwerror;
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
                    atprintf ("Get free byte count on device failed!\n");
                    status = AUDIOERROR;
                    ioctlArg.path = defPath;
                    (void)ioctl (audInfo.ad, AUDIO_STOP, (char *)&ioctlArg);
                    (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL); 
                    goto arwerror;
                    }
                if (ioctlArg.bufSize <= 0)
                    {         
                    taskDelay (5);
                    continue;
                    }
                }
            retWrite = write (audInfo.ad, (char *)buffer + writeSize, retRead);
            if (retWrite == 0)
                {
                taskDelay(5);
                continue;
                }
            if (retWrite < 0)
                {
                break;
                }    
                
            writeSize += retWrite;             
            retRead -= writeSize;            
            }        
        }
        
    atprintf ("Record stop\n");
    
    if (ERROR != audInfo.fd)
        {
        atprintf ("Write data to file...[%s]\n", fileName);
        samples =
            (UINT32)(audInfo.dataSize / (audInfo.dataInfo.sampleBits >> 3));

        if (ERROR == audioWavHeaderWrite (audInfo.fd, audInfo.dataInfo.channels,
                                          audInfo.dataInfo.sampleRate,
                                          audInfo.dataInfo.sampleBits, samples))
            {
            atprintf ("Write wav file header failed!\n");
            status = AUDIOERROR;
            ioctlArg.path = defPath;
            (void)ioctl (audInfo.ad, AUDIO_STOP, (char *)&ioctlArg);
            (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL); 
            goto arwerror;
            }
            
        dataSize = audInfo.dataSize;         
        writeSize = 0;
        while (dataSize > 0)
            {
            retWrite = write (audInfo.fd, (char *)buffer + writeSize, dataSize);
            if (retWrite <= 0)
                {
                atprintf ("Write wav file data failed!\n");
                status = AUDIOERROR;
                ioctlArg.path = defPath;
                (void)ioctl (audInfo.ad, AUDIO_STOP, (char *)&ioctlArg);
                (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL); 
                goto arwerror;
                }
            writeSize += retWrite;
            dataSize -= retWrite;            
            } 
        }
    
arwerror:
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

    if (status == AUDIOOK)
        atprintf ("Record and Play end\n");     
    else
        {
        (void)remove(fileName);
        atprintf ("Record and Play ERROR!\n");
        }
    
    return status;
    }

/*******************************************************************************
*
* atOptionCheck - check whether support options
*
* This routine check whether support options.
*
* RETURNS: TRUE support; otherwise unsupport
*
* ERRNO: N/A
*/

LOCAL BOOL atOptionCheck
    (
    UINT32    option,
    UINT32    value
    )
    {
    AUDIO_IOCTL_ARG ioctlArg;
    BOOL            status = FALSE;    
    INT32           ad = -1;
     
    /* open the audio device */

    ad = open (AUDIO_DEFAULT_DEV, O_RDWR, 0);        
    if (ERROR == ad)
        {        
        atprintf ("Audio device unavailable - %s\n", AUDIO_DEFAULT_DEV);
        status = FALSE;
        goto atperror;
        }
        
    switch (option)
        {
        case OPTION_BLOCK:
            bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));            
            if (OK != ioctl (ad, FIONFREE, (char *)&ioctlArg))
                {
                status = FALSE;             
                goto atperror;
                } 
            else
                status = TRUE; 
            break;
        case OPTION_SAMPLERATE:
            bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
            if (OK != ioctl (ad, AUDIO_GET_DATA_INFO, (char *)&ioctlArg))
                {        
                atprintf ("Get data information failed!\n");
                status = FALSE;    
                goto atperror;
                }  
            ioctlArg.dataInfo.sampleRate = value;    
            if (OK != ioctl (ad, AUDIO_SET_DATA_INFO, (char *)&ioctlArg))
                {        
                atprintf ("Unsupported sampleRate:%d!\n", ioctlArg.dataInfo.sampleRate); 
                status = FALSE; 
                goto atperror;
                }            
            break;
        case OPTION_SAMPLEBITS:
            bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
            if (OK != ioctl (ad, AUDIO_GET_DATA_INFO, (char *)&ioctlArg))
                {        
                atprintf ("Get data information failed!\n");
                status = FALSE;    
                goto atperror;
                }  
            ioctlArg.dataInfo.sampleBits = value;    
            if (OK != ioctl (ad, AUDIO_SET_DATA_INFO, (char *)&ioctlArg))
                {        
                atprintf ("Unsupported sampleBits:%d !\n", ioctlArg.dataInfo.sampleBits); 
                status = FALSE; 
                goto atperror;
                }            
            break;            
        default:        
            break;
        } 
        
atperror:
    if (ad != -1)
        (void)close (ad);
    return status;
    }
    
/*******************************************************************************
*
* atPlayCircle - play all the audio files in the working directory
*
* This routine plays all the audio files in the working directory.
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL void atSetPlayCircle(UINT32 circle)
    {
    playCircle = circle;
    }

/*******************************************************************************
*
* atPlayCircle - play all the audio files in the working directory
*
* This routine plays all the audio files in the working directory.
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL UINT32  atIsPlayCircle()
    {
    return playCircle;
    }
    
/*******************************************************************************
*
* audioFileListRead - open and close all the audio files in the working directory
*
* This routine opens and close all the audio files in the working directory.
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atFileListRead(char *path)
    {
    AT_INFO      audInfo;
    UINT32 status = 0;
    char fullpath[256];

    if(!path)
        {
        path = atGetDir();
        }

    bzero ((char *)&audInfo, sizeof (AT_INFO));
    audInfo.ad  = AUDIOERROR;
    audInfo.fd  = -1;
    audInfo.dd  = NULL;

    atOpenDir(path, &audInfo.dd);

    /* open audio file and get the file information */
    while((audInfo.entry = atFindAudioFile(audInfo.dd))!=NULL)
        {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, audInfo.entry->d_name);
        status = atOpenFile (&audInfo, fullpath);
        if (AUDIOOK != status)
            {
            atprintf ("Open %s fail, status = %d\n",audInfo.entry->d_name, status);
            }

        if(audInfo.fd >= 0)
            {
            (void)close (audInfo.fd);
            }
        }

    atCloseDir(audInfo.dd);

    return AUDIOOK;
    }

/*******************************************************************************
*
* audioFileListWrite - create all types audio files in the working directory
*
* This routine creates all types audio files in the working directory.
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atFileListWrite(void)
    {
    UINT32 i = 0, j = 0;
    STATUS status = 0;
    STATUS result = AUDIOOK;

    for(i = 0; i < ARRAY_SIZE(sampleRate); i++)
        {
        for(j = 0; j < ARRAY_SIZE(sampleBits); j++)
            {
            status = atWriteFile (sampleRate[i], sampleBits[j], 0, 1);
            if (AUDIOOK != status)
                {
                result = AUDIOERROR;
                atprintf ("Write fail status = %d\n", status);
                }

            status = atWriteFile (sampleRate[i], sampleBits[j], 0, 2);
            if (AUDIOOK != status)
                {
                result = AUDIOERROR;
                atprintf ("Write fail status = %d\n", status);
                }
            }
        }

    atLswd(NULL);

    return result;
    }

/*******************************************************************************
*
* audioFileListRead - play all the audio files in the working directory
*
* This routine plays all the audio files in the working directory.
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atFileListPlay(char *path, UINT32 circle, BOOL block)
    {
    AT_INFO   audInfo;
    UINT32 i = 0;
    UINT32 status = 0;
    UINT8   volume = 100;
    char fullpath[256];

    if(!path)
        {
        path = atGetDir();
        }

    atSetPlayCircle(circle);

    do {
        /* open audio file and get the file information */

         bzero ((char *)&audInfo, sizeof (AT_INFO));
         audInfo.ad  = AUDIOERROR;
         audInfo.fd  = -1;
         audInfo.dd  = NULL;
     
         atOpenDir(path, &audInfo.dd);
     
         while((audInfo.entry = atFindAudioFile(audInfo.dd))!=NULL)
             {
             i++;
             atprintf ("***No.%d***\n", i);
             snprintf(fullpath, sizeof(fullpath), "%s/%s", path, audInfo.entry->d_name);
             status = atPlayWav (fullpath, volume, AUDIO_BUFTIME, block);
             if (AUDIOOK != status)
                 {
                  atprintf ("Play %s fail, status = %d\n", audInfo.entry->d_name, status);
                 }

             atprintf ("\n");
             }

         atCloseDir(audInfo.dd);

        }while(atIsPlayCircle());

    atprintf ("audioFileListPlay End - %s\n", path);

    return AUDIOOK;
    }
    
/*******************************************************************************
*
* audioFileListRec - record and create all types audio files in the working directory
*
* This routine records and create all types audio files in the working directory.
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atFileListRec(UINT32 recPeriod, UINT8 volume, BOOL block)
    {
    UINT32 i = 0, j = 0;
    STATUS status = 0;
    STATUS result = AUDIOOK;

    if(!recPeriod)
        {
        recPeriod = 15;
        }

    for(i = 0; i < ARRAY_SIZE(sampleRate); i++)
        {
        for(j = 0; j < ARRAY_SIZE(sampleBits); j++)
            {
            status = atRecWav(sampleRate[i], sampleBits[j], volume, recPeriod, 1, AUDIO_BUFTIME, block);
            if (AUDIOOK != status)
                {
                result = AUDIOERROR;
                atprintf ("Rec fail status = %d\n", status);                
                }

            status = atRecWav(sampleRate[i], sampleBits[j], volume, recPeriod, 2, AUDIO_BUFTIME, block);
            if (AUDIOOK != status)
                {
                result = AUDIOERROR;
                atprintf ("Rec fail status = %d\n", status);
                }
            }
        }       
        
    atLswd(NULL);
    
    return result;
    }

/*******************************************************************************
*
* atDeviceIoctl - test all type ioctl parameter
*
* This routine test all type ioctl prameter
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atDeviceIoctl(void)
    {
    char *  devName = AUDIO_DEFAULT_DEV;     /* audio device file */
    UINT32          bufTime;
    AT_INFO         audInfo;
    AUDIO_IOCTL_ARG ioctlArg;
    UINT32          defPath;
    UINT8           volume = 100;  
    UINT32          i = 0, j = 0;
    STATUS          status = AUDIOOK;
    STATUS          ioRes = AUDIOOK;
    UINT32          availPaths;

    /* open the audio device */

    audInfo.ad = open (devName, O_RDWR, 0);
    if (ERROR == audInfo.ad)
        {
        atprintf ("ERROR:atDeviceOpenClose %s\n", devName);
        status |= AUDIOERROR_OPEN;
        }

    /* DEV_INFO */
    
    bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
    if (OK != ioctl (audInfo.ad, AUDIO_GET_DEV_INFO, (char *)&ioctlArg))
        {
        atprintf ("Get device information failed!\n");
        status |= AUDIOERROR_GET_DATA_INFO;
        }
    availPaths = ioctlArg.devInfo.availPaths;
    atDeviceInfoShow(&(ioctlArg.devInfo));

    /* DEV_ENABLE */
    
    if (OK != ioctl (audInfo.ad, AUDIO_DEV_ENABLE, NULL))
        {
        atprintf ("Enable device failed!\n");
        status |= AUDIOERROR_DEV_ENABLE;
        }

    /* START */
    
    if (OK != ioctl (audInfo.ad, AUDIO_START, NULL))
        {
        atprintf ("Start playback failed!\n");
        status |= AUDIOERROR_START;
        }

    /* VOLUME */
    
    for(volume = 0; volume < 101; volume++)
        {
        bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
        ioctlArg.volume.path    = availPaths;
        ioctlArg.volume.left    = volume;
        ioctlArg.volume.right   = volume;
        if (OK != ioctl (audInfo.ad, AUDIO_SET_VOLUME, (char *)&ioctlArg))
            {
            atprintf ("AUDIO_SET_VOLUME Failed:%d\n", volume);
            status |= AUDIOERROR_SET_VOLUME;
            }

        bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
        ioctlArg.volume.path    = availPaths;
        if (OK != ioctl (audInfo.ad, AUDIO_GET_VOLUME, (char *)&ioctlArg))
            {
            atprintf ("AUDIO_GET_VOLUME failed!\n");
            status |= AUDIOERROR_GET_VOLUME;
            }

        if((ioctlArg.volume.path != availPaths)||
            (ioctlArg.volume.left != volume)||
            (ioctlArg.volume.right != volume))
            {
            atprintf ("AUDIO_GET_VOLUME value Error :%d\n",volume);
            status |= AUDIOERROR_GET_VOLUME;
            }
        }

    /* STOP */
    
    if (OK != ioctl (audInfo.ad, AUDIO_STOP, NULL))
        {
        atprintf ("AUDIO_GET_VOLUME failed!\n");
        status |= AUDIOERROR_STOP;
        }

    /* DISABLE */

    if (OK != ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL))
        {
        atprintf ("AUDIO_DEV_DISABLE failed!\n");
        status |= AUDIOERROR_DEV_DISABLE;
        }

    /* PATH */
    
    for(i = 0; i <= AUDIO_OUT_TYPES; i++)
        {
        for(j = 0; j <= AUDIO_IN_TYPES; j++)
            {
            defPath = i + (j << AUDIO_IN_MASK_BITS);
            bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
            ioctlArg.path = defPath;

            ioRes = ioctl (audInfo.ad, AUDIO_SET_PATH, (char *)&ioctlArg);
            if(((~availPaths) & defPath)||(!defPath))
                {
                if (OK == ioRes)
                    {
                    atprintf ("Error:Set unsupported path = 0x%x\n", defPath);
                    status |= AUDIOERROR_SET_PATH;
                    }
                else
                    {
                    continue;
                    }
                }
            else
                {
                if (OK != ioRes)
                    {
                    atprintf ("Error:Set device path = 0x%x\n", defPath);
                    status |= AUDIOERROR_SET_PATH;
                    }
                }

            bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
            if (OK != ioctl (audInfo.ad, AUDIO_GET_PATH, (char *)&ioctlArg))
                {
                atprintf ("Error:Get device path failed!\n");
                status |= AUDIOERROR_GET_PATH;
                }

            atprintf ("Get path :0x%x\n",ioctlArg.path);
            if((~availPaths) & defPath)
                {
                if((~availPaths) & ioctlArg.path)
                    {
                    atprintf ("Error:Get unavailable value\n");
                    status |= AUDIOERROR_GET_PATH;
                    }
                }
           else
               {
               if(ioctlArg.path != defPath)
                   {
                   atprintf ("Error:Get device path value!\n");
                   status |= AUDIOERROR_GET_PATH;
                   }
               }
            }
        }

    /*DATA_INFO*/
    
    for(i = 0; i < ARRAY_SIZE(sampleRate); i++)
        {
        for(j = 0; j < ARRAY_SIZE(sampleBits); j++)
            {
            audInfo.dataInfo.sampleRate = sampleRate[i];
            audInfo.dataInfo.sampleBits = sampleBits[j];
            audInfo.dataInfo.sampleBytes = sampleBits[j] >> 3;
            audInfo.dataInfo.channels = 2;
            audInfo.dataInfo.useMsb = 0;

            bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
            bcopy ((char *)&audInfo.dataInfo, (char *)&(ioctlArg.dataInfo), sizeof (AUDIO_DATA_INFO));
            if (OK != ioctl (audInfo.ad, AUDIO_SET_DATA_INFO, (char *)&ioctlArg))
                {
                atprintf ("Unsupported sampleRate:%d sampleBits:%d !\n",
                           audInfo.dataInfo.sampleRate, audInfo.dataInfo.sampleBits); 
                continue;
                }   

            bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
            if (OK != ioctl (audInfo.ad, AUDIO_GET_DATA_INFO, (char *)&ioctlArg))
                {
                atprintf ("Failed AUDIO_GET_DATA_INFO\n");
                status |= AUDIOERROR_GET_DATA_INFO;
                }
            
            if(memcmp(&(audInfo.dataInfo), &(ioctlArg.dataInfo), sizeof(AUDIO_DATA_INFO)))
                {
                atprintf ("Failed AUDIO_SET_DATA_INFO sampleRate:%d sampleBits:%d !\n",
                           audInfo.dataInfo.sampleRate, audInfo.dataInfo.sampleBits); 
                status |= AUDIO_SET_DATA_INFO;
                }
            }
        }

    /* BUFTIME */
    
    for(i = 0; i <= 60; i++)
        {             
        bufTime = i % 1000;
        bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
        ioctlArg.bufTime = bufTime;        
        if (OK != ioctl (audInfo.ad, AUDIO_SET_BUFTIME, (char *)&ioctlArg))
            {
            if(bufTime)
                {
                atprintf ("Set buffer time failed!\n");
                status |= AUDIOERROR_SET_BUFTIME;
                }
            else
                {
                continue;
                }
            }
            
        bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
        if (OK != ioctl (audInfo.ad, AUDIO_GET_BUFTIME, (char *)&ioctlArg))
            {
            atprintf ("Get buffer time failed!\n");
            status |= AUDIOERROR_GET_BUFTIME;
            }
            
        if(ioctlArg.bufTime != bufTime)
            {
            atprintf ("Get buffer time failed!\n");
            status |= AUDIOERROR_GET_BUFTIME;
            }
        }

    (void)close (audInfo.ad);

    if(AUDIOOK == status)
        {
        atprintf ("atDeviceIoctl Pass!\n");
        return AUDIOOK;
        }
    else
        {
        atprintf ("FAIL:atDeviceIoctl status = 0x%x\n",status);
        return AUDIOERROR;
        }
    }

/*******************************************************************************
*
* atDeviceOpenClose - open and close audio device
*
* This routine open and close audio device
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atDeviceOpenClose(UINT32 num)
    {
    char *  devName = AUDIO_DEFAULT_DEV;     /* audio device file */
    int     adArr[MAD_AD];
    UINT32  i;
    STATUS  status = AUDIOOK;

    for(i = 0; i < MAD_AD; i++)
        {
        adArr[i] = ERROR;
        }

    if(num == 0)
        {
        num = 1;
        }
    if(num > MAD_AD)
        {
        num = MAD_AD;
        }

    for(i = 0; i < num; i++)
        {
        adArr[i] = open (devName, O_RDWR, 0);
        if (ERROR == adArr[i])
            {
            status |= AUDIOERROR_OPENDEV;
            atprintf ("Device Open Time = %d\n", i+1);
            break;
            }            
        }

    for(i = 0; i < num; i++)
        {
        if (ERROR != adArr[i])
            {
            (void)close (adArr[i]);
            }
        }
    
    if (status == AUDIOOK)
        {
        atprintf ("atDeviceOpenClose Pass!\n");
        return AUDIOOK;
        }
    else
        {
        atprintf ("atDeviceOpenClose Failed!\n");
        return AUDIOERROR;
        }        
    }

/*******************************************************************************
*
* atDeviceOpenClose2 - open and close audio device
*
* This routine open and close audio device
*
* RETURNS: OK when operation was success; otherwise ERROR
*
* ERRNO: N/A
*/

LOCAL STATUS atDeviceOpenClose2(void)
    {
    char *  devName = AUDIO_DEFAULT_DEV;     /* audio device file */
    int     ad = ERROR;
    int     ad2 = ERROR;
    STATUS  status = AUDIOOK; 
/*1*/
    ad = open (devName, O_RDWR, 0);
    if (ERROR == ad)
        {
        atprintf ("atDeviceOpenClose2  %s ERROR!\n", devName);
        status |= AUDIOERROR_OPEN;
        }
    (void)close (ad);

/*2*/
    ad = open (devName, O_RDWR, 0);
    if (ERROR == ad)
        {
        atprintf ("atDeviceOpenClose2  %s ERROR!\n", devName);
        status |= AUDIOERROR_OPEN;
        }
    ad2 = open (devName, O_RDWR, 0);
    if (ERROR == ad2)
        {
        atprintf ("atDeviceOpenClose2 twice %s ERROR!\n", devName);        
        }

    (void)close (ad);
    if (ad2 >= 0)
        {
        (void)close (ad2);
        }

/*3*/
    ad = open (devName, O_RDWR, 0);
    if (ERROR == ad)
        {
        atprintf ("Audio device unavailable - %s\n", devName);
        status |= AUDIOERROR_OPEN;
        }
    (void)close (ad);
    if (ERROR != close (ad))
        {
        atprintf ("Error:Reclose Success!\n");
        status |= AUDIOERROR_CLOSE;
        }

/*4*/
    ad = open (devName, O_RDWR, 0);
    if (ERROR == ad)
        {
        atprintf ("Audio device unavailable - %s\n", devName);
        status |= AUDIOERROR_OPEN;
        }
    (void)close (ad);

    if (status == AUDIOOK)
        {
        atprintf ("atDeviceOpenClose2 Pass!\n");
        return AUDIOOK;
        }
    else
        {
        atprintf ("atDeviceOpenClose2 Failed!\n");
        return AUDIOERROR;
        } 
    }

/*******************************************************************************
*
* tmWriteReadSpeed - test audio write and read speed
*
* This routine test audio write and read speed
*
* \cs
* <testCase>
*     <timeout>       300000    </timeout>
*     <reentrant>     TRUE    </reentrant>
*     <memCheck>      TRUE     </memCheck>
*     <destructive>   FALSE </destructive>
* </testCase>
* \ce
*
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

VXTEST_STATUS tmWriteReadSpeed(UINT32 recPeriod)
    {
    char *         devName = AUDIO_DEFAULT_DEV;     /* audio device file */
    AT_INFO        audInfo;
    UINT8 *        buffer;
    UINT32         bufSize = 0;
    UINT32         dataSize = 0;
    ssize_t        starttime;
    ssize_t        endtime;
    ssize_t        msecs = 0;
    UINT32         defPath;
    UINT32         i;
    AUDIO_IOCTL_ARG ioctlArg;

    AUDIO_DATA_INFO     testList[] =
        {
            /* rate, bits, bytes, channels, useMsb*/

            { 44100, 8,    1,     2,        FALSE},
            { 44100, 8,    1,     2,        TRUE},
            { 44100, 8,    1,     1,        FALSE},
            { 44100, 8,    1,     1,        TRUE},

            { 44100, 16,   2,     2,        FALSE},
            { 44100, 16,   2,     2,        TRUE},
            { 44100, 16,   2,     1,        FALSE},
            { 44100, 16,   2,     1,        TRUE},

            { 44100, 24,   3,     2,        FALSE},
            { 44100, 24,   3,     2,        TRUE},
            { 44100, 24,   3,     1,        FALSE},
            { 44100, 24,   3,     1,        TRUE},

            { 44100, 32,   4,     2,        FALSE},
            { 44100, 32,   4,     2,        TRUE},
            { 44100, 32,   4,     1,        FALSE},
            { 44100, 32,   4,     1,        TRUE},
        };
        
    if (recPeriod == 0)
        recPeriod = 5;
    
    for (i = 0; i < ARRAY_SIZE(testList); i++)
        {    
        dataSize = testList[i].sampleBytes * testList[i].sampleRate * 
                   testList[i].channels * recPeriod;
        if(bufSize < dataSize)
            {
            bufSize = dataSize;
            }
        }
    buffer = (UINT8 *)malloc(bufSize);
    if(!buffer)
        {
        atprintf ("Malloc buffer failed! - [%02x]\n", bufSize);
        return VXTEST_ABORT;
        }
        
    bzero ((char *)&audInfo, sizeof (AT_INFO));    
    audInfo.ad = open (devName, O_RDWR, 0);
    if (ERROR == audInfo.ad)
        {
        atprintf ("Audio device unavailable - %s\n", devName);
        free(buffer);
        return VXTEST_FAIL;
        }
        
    for (i = 0; i < ARRAY_SIZE(testList); i++)
        {  
        bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
        ioctlArg.dataInfo.channels     = testList[i].channels;
        ioctlArg.dataInfo.sampleBits   = testList[i].sampleBits;
        ioctlArg.dataInfo.sampleRate   = testList[i].sampleRate;
        ioctlArg.dataInfo.sampleBytes  = testList[i].sampleBytes;
        ioctlArg.dataInfo.useMsb       = testList[i].useMsb;
        dataSize = testList[i].sampleBytes * testList[i].sampleRate * 
                   testList[i].channels * recPeriod;
        if (OK != ioctl (audInfo.ad, AUDIO_SET_DATA_INFO, (char *)&ioctlArg))
            {
            atprintf ("Unsupported sampleRate:%d sampleBits:%d !\n",
                       ioctlArg.dataInfo.sampleRate, ioctlArg.dataInfo.sampleBits); 
            continue;
            }
            
        bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
        if (OK != ioctl (audInfo.ad, AUDIO_GET_DEV_INFO, (char *)&ioctlArg))
            {
            atprintf ("Get device information failed!\n");
            free(buffer);
            (void)close (audInfo.ad);
            return VXTEST_FAIL;
            }    
            
        defPath = ioctlArg.devInfo.defPaths;
        bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
        ioctlArg.path = defPath;
        if (OK != ioctl (audInfo.ad, AUDIO_SET_PATH, (char *)&ioctlArg))
            {
            atprintf ("Set device path failed!\n");
            free(buffer);
            (void)close (audInfo.ad);
            return VXTEST_FAIL;
            }  
    
        bzero ((char *)&ioctlArg, sizeof (AUDIO_IOCTL_ARG));
        ioctlArg.bufTime = AUDIO_BUFTIME * 1000;
        if (OK != ioctl (audInfo.ad, AUDIO_SET_BUFTIME, (char *)&ioctlArg))
            {
            atprintf ("Set buffer time failed!\n");
            free(buffer);
            (void)close (audInfo.ad);
            return VXTEST_FAIL;
            }

        if (OK != ioctl (audInfo.ad, AUDIO_DEV_ENABLE, NULL))
            {
            atprintf ("Enable device failed!\n");
            free(buffer);
            (void)close (audInfo.ad);
            return VXTEST_FAIL;
            }
    
        if (OK != ioctl (audInfo.ad, AUDIO_START, NULL))
            {
            atprintf ("Start playback failed!\n");
            free(buffer);
            (void)close (audInfo.ad);
            return VXTEST_FAIL;
            }   
            
        atprintf ("Begin sampleRate:%d sampleBits:%d !\n",
                   testList[i].sampleRate, testList[i].sampleBits);
                   
        /* read */
        starttime = tickGet ();
        (void)read (audInfo.ad, (char *)buffer, dataSize);
        endtime = tickGet ();
        msecs = (endtime - starttime) / sysClkRateGet();

        atprintf("starttime:%d  endtime:%d Time\n", starttime, endtime);
        atprintf("Data Period:%ds  Read Period:%ds\n", recPeriod, msecs);
        
        /* write */
        starttime = tickGet ();
        (void)write (audInfo.ad, (char *)buffer, dataSize);
        endtime = tickGet ();
        msecs = (endtime - starttime) / sysClkRateGet() + AUDIO_BUFTIME;

        atprintf("starttime:%d  endtime:%d Time\n", starttime, endtime);
        atprintf("Data Period:%ds  Write Period:%ds\n", recPeriod, msecs);  
        
        atprintf ("End -------------------------- !\n");
        
        (void)ioctl (audInfo.ad, AUDIO_STOP, (char *)&ioctlArg);
        (void)ioctl (audInfo.ad, AUDIO_DEV_DISABLE, NULL);
        }

    free(buffer);
    (void)close (audInfo.ad);

    return VXTEST_PASS;
    }
    
/******************************************************************************
*
*
* atStandIO - test audio Stand IO
*
* This routine tests audio Stand IO.
*
* \cs
* <testCase>
*     <timeout>       300000    </timeout>
*     <reentrant>     TRUE    </reentrant>
*     <memCheck>      TRUE     </memCheck>
*     <destructive>   FALSE </destructive>
* </testCase>
* \ce
*
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

VXTEST_STATUS tmStandIO(UINT32 times)
    {
    UINT32 i;
    STATUS status = AUDIOOK; 
    VXTEST_STATUS vxTestRes = VXTEST_PASS;

    if (0 == times)
        {
        times = 1;
        }
        
    i = times;    
    while(i--)
        {
        status |= atDeviceOpenClose(MAD_AD);
        }  
        
    i = times;
    while(i--)
        {
        status |= atDeviceIoctl();
        }
        
    i = times;
    while(i--)
        {
        status |= atDeviceOpenClose2();
        }
        
    i = times;
    while(i--)
        {
        status |= atDeviceOpenClose(1);
        status |= atDeviceOpenClose2();
        status |= atDeviceIoctl();
        }

    if(status != AUDIOOK)
        {
        vxTestRes = VXTEST_FAIL;
        atprintf("Error:atStandIO status = %d\n", status);
        }
    return vxTestRes;
}

/******************************************************************************
*
*
* atWavFilesWriteRead - test audio write and read wav files.
*
* This routine test audio write and read wav files.
*
* \cs
* <testCase>
*     <timeout>       300000    </timeout>
*     <reentrant>     TRUE  </reentrant>
*     <memCheck>  TRUE  </memCheck>
*     <destructive>  FALSE </destructive>
* </testCase>
* \ce
*
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

VXTEST_STATUS tmWavFilesWriteRead()
    {
    STATUS        status    = AUDIOOK;    
    char *        path      = atGetDir();

    status = atRewindDir(path);
    if(status != AUDIOOK)
        {
        atprintf("Error:atRewindDir\n");
        return VXTEST_FAIL;
        }
        
    status = atFileListWrite();
    if(status != AUDIOOK)
        {  
        atprintf("Error:atFileListRead\n");
        return VXTEST_FAIL;
        }

    status = atFileListRead(path);
    if(status != AUDIOOK)
        {  
        atprintf("Error:atFileListRead\n");
        return VXTEST_FAIL;
        }

    return VXTEST_PASS;
    }

/******************************************************************************
*
*
* atWavFilesRecPlay - test audio record and play
*
* This routine  tests audio record and play
*
* \cs
* <testCase>
*     <timeout>       1200000    </timeout>
*     <reentrant>     TRUE  </reentrant>
*     <memCheck>  TRUE  </memCheck>
*     <destructive>  FALSE </destructive>
* </testCase>
* \ce
*
*
* RETURNS: VXTEST_ABORT if initialization problem, VXTEST_PASS if testing was
*          successful, else VXTEST_FAIL
*
* ERRNO: N/A
*/

VXTEST_STATUS tmWavFilesRecPlay()
    {
    STATUS        status    = AUDIOOK;     
    char *        path      = atGetDir();

    /* Blocking */
    
    status = atRewindDir(path);
    if(status != AUDIOOK)
        {        
        atprintf("Error:atRewindDir\n");
        return VXTEST_FAIL;
        }
        
    status = atFileListRec(2, 70, TRUE);
    if(status != AUDIOOK)
        {        
        atprintf("Error:atFileListRead\n");
        return VXTEST_FAIL;
        }

    status = atFileListPlay(path, 0, TRUE);
    if(status != AUDIOOK)
        {        
        atprintf("Error:atFileListRead\n");
        return VXTEST_FAIL;
        }
        
    if (atOptionCheck(OPTION_BLOCK, FALSE))
        {
            
        /* Non-blocking */
        
        status = atRewindDir(path);
        if(status != AUDIOOK)
            {        
            atprintf("Error:atRewindDir\n");
            return VXTEST_FAIL;
            }
            
        status = atFileListRec(2, 70, FALSE);
        if(status != AUDIOOK)
            {        
            atprintf("Error:atFileListRead\n");
            return VXTEST_FAIL;
            }

        status = atFileListPlay(path, 0, FALSE);
        if(status != AUDIOOK)
            {        
            atprintf("Error:atFileListRead\n");
            return VXTEST_FAIL;
            }
        }
    else
        atprintf("INFO: skip non-blocking test.\n");
    
    return VXTEST_PASS;
    }
    
LOCAL VXTEST_ENTRY vxTestTbl_tmAudioTest[] = 
{
    /*pTestName,             FUNCPTR,                pArg,   flags,   cpuSet,   timeout,   exeMode,   osMode,   level*/
    {"tmStandIO",           (FUNCPTR)tmStandIO, 0, 0, 0, 5000, VXTEST_EXEMODE_ALL, VXTEST_OSMODE_ALL, 0,"test functionality of audio device driver."},
    {"tmWriteReadSpeed",    (FUNCPTR)tmWriteReadSpeed, 0, 0, 0, 500000, VXTEST_EXEMODE_ALL, VXTEST_OSMODE_ALL, 0,"test functionality of audio device driver."},
    {"tmWavFilesWriteRead", (FUNCPTR)tmWavFilesWriteRead, 0, 0, 0, 5000, VXTEST_EXEMODE_ALL, VXTEST_OSMODE_ALL, 0,"test functionality of audio device driver."},
    {"tmWavFilesRecPlay",   (FUNCPTR)tmWavFilesRecPlay, 0, 0, 0, 500000, VXTEST_EXEMODE_ALL, VXTEST_OSMODE_ALL, 0,"test functionality of audio device driver."},    
    {NULL, (FUNCPTR)"tmAudioTest", 0, 0, 0, 1000000, 0, 0, 0}
};
 
/**************************************************************************
*
* tmAudioTestExec - Exec test module
*
* This routine should be called to initialize the test module.
*
* RETURNS: N/A
*
* NOMANUAL
*/
#ifdef _WRS_KERNEL
 
STATUS tmAudioTestExec
    (
    char * testCaseName,
    VXTEST_RESULT * pTestResult
    )
    {
    return vxTestRun((VXTEST_ENTRY**)&vxTestTbl_tmAudioTest, testCaseName, pTestResult);
    } 

#else 
    
STATUS tmAudioTestExec
    (
    char * testCaseName,
    VXTEST_RESULT * pTestResult,
    int    argc,
    char * argv[]
    )
    {
    return vxTestRun((VXTEST_ENTRY**)&vxTestTbl_tmAudioTest, testCaseName, pTestResult, argc, argv);
    }

/**************************************************************************
* main - User application entry function
*
* This routine is the entry point for user application. A real time process
* is created with first task starting at this entry point. 
*
*/
int main
    (
    int    argc,    /* number of arguments */
    char * argv[]   /* array of arguments */
    )
    {
    return tmAudioTestExec(NULL, NULL, argc, argv);
    }
#endif 
