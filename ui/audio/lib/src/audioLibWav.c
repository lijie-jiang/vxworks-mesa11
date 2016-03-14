/* audioLibWave.c - utility functions to handle a Wav file */

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
15sep15,c_l  support handling Wav file from a memory address.(V7GFX-251)
14feb14,y_f  written
*/

/*
DESCRIPTION
This file provides utility functions for handling Wav files.
*/

/* includes */

#include <audioLibCore.h>
#include <audioLibWav.h>

/* defines */

#if (_BYTE_ORDER == _BIG_ENDIAN)
#define MEM_SWAP_LONG(x)    LONGSWAP (x)
#define MEM_SWAP_WORD(x)    (MSB (x) | LSB (x) << 8)
#define MEMLNBYTE(x)        LMSB (x)
#define MEMLSBYTE(x)        LNMSB (x)
#define MEMLTBYTE(x)        LNLSB (x)
#define MEMLFBYTE(x)        LLSB (x)
#define MEMLWORD(x)         MSW (x)
#define MEMMWORD(x)         LSW (x)
#else
#define MEM_SWAP_LONG(x)    (x)
#define MEM_SWAP_WORD(x)    (x)
#define MEMLNBYTE(x)        LLSB (x)
#define MEMLSBYTE(x)        LNLSB (x)
#define MEMLTBYTE(x)        LNMSB (x)
#define MEMLFBYTE(x)        LMSB (x)
#define MEMLWORD(x)         LSW (x)
#define MEMMWORD(x)         MSW (x)
#endif /* _BYTE_ORDER == _BIG_ENDIAN */

#define BUFFERSIZE          1024

/* Type of wave data format */

#define WAVE_FORMAT_PCM             1
#define WAVE_FORMAT_EXTENSIBLE      0xFFFE

/* typedefs */

/* PCM waveform format */

typedef struct pcmWaveFormat
    {
    UINT16  formatTag;          /* type of wave form data */
    UINT16  channels;           /* number of audio channels */
    UINT32  samplesPerSec;      /* audio samples per second */
    UINT32  avgBytesPerSec;     /* average transfer rate */
    UINT16  blockAlign;         /* bytes required for a single sample */
    UINT16  bitsPerSample;      /* bits per sample */
    } PCM_WAVE_FORMAT;

/* extensive waveform format */

typedef struct extWaveFormat
    {
    UINT16  formatTag;          /* type of wave form data */
    UINT16  channels;           /* number of audio channels */
    UINT32  samplesPerSec;      /* audio samples per second */
    UINT32  avgBytesPerSec;     /* average transfer rate */
    UINT16  blockAlign;         /* bytes required for a single sample */
    UINT16  bitsPerSample;      /* bits per sample */
    UINT16  extSize;
    UINT16  validBitsPerSample;
    UINT32  channelMask;
    UINT8   subFormat [16];
    } EXT_WAVE_FORMAT;

/* PCM wave file header */

typedef struct pcmFileHeader
    {
    UINT8           tagRIFF [4];
    UINT32          lenRIFF;
    UINT8           tagWAVE [4];
    UINT8           tagFMT [4];
    UINT32          lenFMT;
    PCM_WAVE_FORMAT pcmFormat;
    UINT8           tagDATA [4];
    UINT32          lenDATA;
    } PCM_FILE_HEADER;

/* extensive wave file header */

typedef struct extFileHeader
    {
    UINT8           tagRIFF [4];
    UINT32          lenRIFF;
    UINT8           tagWAVE [4];
    UINT8           tagFMT [4];
    UINT32          lenFMT;
    EXT_WAVE_FORMAT extFormat;
    UINT8           tagFACT [4];
    UINT32          lenFACT;
    UINT32          sampleLen;
    UINT8           tagDATA [4];
    UINT32          lenDATA;
    } EXT_FILE_HEADER;

/* functions */

/*******************************************************************************
*
* audioWavHeaderRead - read a WAVE form header from file
*
* This routine reads the WAVE form file <fd> to retrieve its header information.
* The number of channels, sample rate, number of bits per sample, number of
* samples, and the starting location of the data are returned as parameters.
*
* RETURNS: OK when header successfully read; otherwise ERROR
*
* ERRNO: N/A
*
* NOTES:
*   This function is not reentrant
*
* SEE ALSO:
*
*/

STATUS audioWavHeaderRead
    (
    int         fd,             /* file descriptor of wave form file */
    int *       pChannels,      /* number of channels */
    UINT32 *    pSampleRate,    /* sample rate */
    int *       pSampleBits,    /* size of sample */
    UINT32 *    pSamples,       /* number of samples */
    UINT32 *    pDataStart      /* start of data section */
    )
    {
    char   buffer [BUFFERSIZE];
    UINT32 readBytes = 0;
    
    if (fd < 0)
        {
        return ERROR;
        }

    /* Position to start of file */

    if (ERROR == lseek (fd, 0L, SEEK_SET))
        {
        return ERROR;
        }

    /* Read enough data from the file to get the header */

    readBytes = (UINT32)read (fd, buffer, BUFFERSIZE);

    if (readBytes < sizeof (PCM_FILE_HEADER))
        {
        return ERROR;
        }
        
    if (ERROR == audioWavHeaderReadBuf (buffer, 
                                        readBytes, 
                                        pChannels, 
                                        pSampleRate, 
                                        pSampleBits, 
                                        pSamples, 
                                        pDataStart))
        {
        return ERROR;
        }   
        
    return OK;
    }

/*******************************************************************************
*
* audioWavHeaderWrite - format a file header for a WAVE file.
*
* This routine write header information to the WAVE form file <fd>.
* The number of channels, sample rate, number of bits per sample, number of
* samples shall be provided. After writting the header, the file write pointer
* is pointed at the end of the header, that is just at the beginning of the
* data chunk.
*
* RETURNS: OK when header successfully written; otherwise ERROR
*
* ERRNO: N/A
*
* NOTES:
*   This function is not reentrant
*
* SEE ALSO:
*
*/

STATUS audioWavHeaderWrite
    (
    int     fd,                 /* file descriptor of wave form file */
    UINT8   nChannels,          /* number of channels */
    UINT32  nSampleRate,        /* sample rate */
    UINT8   nSampleBits,        /* size of sample */
    UINT32  nSamples            /* number of samples */
    )
    {
    char  buffer[BUFFERSIZE];

    if (fd < 0)
        {
        return ERROR;
        }

    if (ERROR == audioWavHeaderWriteBuf (buffer, 
                                         BUFFERSIZE,
                                         nChannels,
                                         nSampleRate,
                                         nSampleBits,
                                         nSamples))
        {
        return ERROR;
        }   
        
    /* Position to start of file */

    if (ERROR == lseek (fd, 0L, SEEK_SET))
        {
        return ERROR;
        }
        
    if (24 == nSampleBits)
        {

        /* write header */

        if (ERROR == write (fd, (char *)buffer,
                            sizeof (EXT_FILE_HEADER)))
            {
            return ERROR;
            }
        }
    else
        {

        /* write header */

        if (ERROR == write (fd, (char *)buffer,
                            sizeof (PCM_FILE_HEADER)))
            {
            return ERROR;
            }
        }

    /* after returned, the file write pointer is just at start of data chunk */

    return OK;
    }

/*******************************************************************************
*
* audioWavHeaderReadBuf - read a WAVE form header from buffer
*
* This routine reads the WAVE form buffer <pBuf> to retrieve its header information.
* The number of channels, sample rate, number of bits per sample, number of
* samples, and the starting location of the data are returned as parameters.
*
* RETURNS: OK when header successfully read; otherwise ERROR
*
* ERRNO: N/A
*
* NOTES:
*   This function is reentrant
*
* SEE ALSO:
*
*/

STATUS audioWavHeaderReadBuf
    (
    char *      pBuf,           /* memory address of wave form buffer */
    size_t      bufSize,        /* size of buffer */
    int *       pChannels,      /* number of channels */
    UINT32 *    pSampleRate,    /* sample rate */
    int *       pSampleBits,    /* size of sample */
    UINT32 *    pSamples,       /* number of samples */
    UINT32 *    pDataStart      /* start of data section */
    )
    {    
    char *              ptr;
    PCM_FILE_HEADER *   pPcmFileHeader = (PCM_FILE_HEADER *)pBuf;
    UINT32              dataBytes;

    if (pBuf == NULL)
        {
        return ERROR;
        }    

    if (bufSize < sizeof (PCM_FILE_HEADER))
        {
        return ERROR;
        }

    if (memcmp (pPcmFileHeader->tagRIFF, "RIFF", 4))
        {
        return ERROR;
        }

    if (memcmp (pPcmFileHeader->tagWAVE, "WAVE", 4))
        {
        return ERROR;
        }

    if (memcmp (pPcmFileHeader->tagFMT, "fmt ", 4))
        {
        return ERROR;
        }

    pPcmFileHeader->pcmFormat.avgBytesPerSec   =
        MEM_SWAP_LONG (pPcmFileHeader->pcmFormat.avgBytesPerSec);
    pPcmFileHeader->pcmFormat.bitsPerSample    =
        MEM_SWAP_WORD (pPcmFileHeader->pcmFormat.bitsPerSample);
    pPcmFileHeader->pcmFormat.blockAlign       =
        MEM_SWAP_WORD (pPcmFileHeader->pcmFormat.blockAlign);
    pPcmFileHeader->pcmFormat.channels         =
        MEM_SWAP_WORD (pPcmFileHeader->pcmFormat.channels);
    pPcmFileHeader->pcmFormat.formatTag        =
        MEM_SWAP_WORD (pPcmFileHeader->pcmFormat.formatTag);
    pPcmFileHeader->pcmFormat.samplesPerSec    =
        MEM_SWAP_LONG (pPcmFileHeader->pcmFormat.samplesPerSec);

    /* Validate that the collected data is reasonable */

    if (pPcmFileHeader->pcmFormat.samplesPerSec !=
        (pPcmFileHeader->pcmFormat.avgBytesPerSec /
         pPcmFileHeader->pcmFormat.blockAlign))
        {
        return ERROR;
        }

    if (pPcmFileHeader->pcmFormat.samplesPerSec !=
        (pPcmFileHeader->pcmFormat.avgBytesPerSec /
         pPcmFileHeader->pcmFormat.channels /
         (pPcmFileHeader->pcmFormat.bitsPerSample >> 3)))
        {
        return ERROR;
        }

    /* Look for the data chunk */

    ptr = (char *)&(pPcmFileHeader->pcmFormat) +
          MEM_SWAP_LONG (pPcmFileHeader->lenFMT);

    while (memcmp (ptr, "data", 4))
        {
        /* Move to next tag, add the pointer by two int32 and the chunk size */

        ptr = ptr + 8 + MEM_SWAP_LONG (*((UINT32*)(ptr + 4)));

        /* If data chunk is not located within the buffer, return error */

        if ((ptr < pBuf) || (ptr > pBuf + bufSize))
            {
            return ERROR;
            }
        }

    dataBytes = MEM_SWAP_LONG (*((UINT32*)(ptr + 4)));
    if (NULL != pDataStart)
        {
        *pDataStart = (UINT32)(ptr + 8 - pBuf);
        }

    /* Header information available, pass to caller */

    if (NULL != pChannels)
        {
        *pChannels = pPcmFileHeader->pcmFormat.channels;
        }

    if (NULL != pSampleRate)
        {
        *pSampleRate = pPcmFileHeader->pcmFormat.samplesPerSec;
        }

    if (NULL != pSampleBits)
        {
        *pSampleBits = pPcmFileHeader->pcmFormat.bitsPerSample;
        }

    if (NULL != pSamples)
        {
        *pSamples = dataBytes / pPcmFileHeader->pcmFormat.blockAlign;
        }

    return OK;
    }

/*******************************************************************************
*
* audioWavHeaderWriteBuf - format a file header for a WAVE buffer.
*
* This routine write header information to the WAVE form buffer <pBuf>.
* The number of channels, sample rate, number of bits per sample, number of
* samples shall be provided. After writting the header, the file write pointer
* is pointed at the end of the header, that is just at the beginning of the
* data chunk.
*
* RETURNS: OK when header successfully written; otherwise ERROR
*
* ERRNO: N/A
*
* NOTES:
*   This function is reentrant
*
* SEE ALSO:
*
*/

STATUS audioWavHeaderWriteBuf
    (
    char *  pBuf,               /* memory address of wave form buffer */
    size_t  bufSize,            /* size of buffer */
    UINT8   nChannels,          /* number of channels */
    UINT32  nSampleRate,        /* sample rate */
    UINT8   nSampleBits,        /* size of sample */
    UINT32  nSamples            /* number of samples */
    )
    {
    PCM_FILE_HEADER *   pcmFileHeader = (PCM_FILE_HEADER *)pBuf;
    EXT_FILE_HEADER *   extFileHeader = (EXT_FILE_HEADER *)pBuf;
    char                subFormatGUID [16] =
        {
        (char)0x01, (char)0x00, (char)0x00, (char)0x00, (char)0x00, (char)0x00,
        (char)0x10, (char)0x00, (char)0x80, (char)0x00, (char)0x00, (char)0xAA,
        (char)0x00, (char)0x38, (char)0x9B, (char)0x71
        };

    if (pBuf == NULL)
        {
        return ERROR;
        }    

    if (24 == nSampleBits)
        {
        if (bufSize < sizeof(EXT_FILE_HEADER))  
            {
            return ERROR;
            }             
        bzero ((char *)extFileHeader, sizeof (EXT_FILE_HEADER));
        memcpy (extFileHeader->tagRIFF, "RIFF", 4);

        /*
         * lenRIFF equals header size not including the begining tagRIFF and
         * lenRIFF fields, and adding the total data size
         */

        extFileHeader->lenRIFF = (UINT32)(sizeof (EXT_FILE_HEADER) -
                                         sizeof (INT32) * 2 + nSamples *
                                         (nSampleBits >> 3));
        memcpy (extFileHeader->tagWAVE, "WAVE", 4);
        memcpy (extFileHeader->tagFMT, "fmt ", 4);

        extFileHeader->lenFMT = (UINT32)sizeof (EXT_WAVE_FORMAT);
        extFileHeader->extFormat.formatTag = (UINT16)WAVE_FORMAT_EXTENSIBLE;
        extFileHeader->extFormat.channels            = (UINT16)nChannels;
        extFileHeader->extFormat.samplesPerSec       = nSampleRate;
        extFileHeader->extFormat.bitsPerSample       = (UINT16)nSampleBits;
        extFileHeader->extFormat.avgBytesPerSec = (UINT32)(nSampleRate *
                                                          nChannels *
                                                          (nSampleBits >> 3));
        extFileHeader->extFormat.blockAlign          = (UINT16)(nChannels *
                                                            (nSampleBits >> 3));
        extFileHeader->extFormat.extSize             = (UINT16)22;
        extFileHeader->extFormat.validBitsPerSample  = (UINT16)nSampleBits;
        extFileHeader->extFormat.channelMask         = (UINT32)0;
        memcpy (extFileHeader->extFormat.subFormat, subFormatGUID, 16);

        memcpy (extFileHeader->tagFACT, "fact", 4);
        extFileHeader->lenFACT = 4;

        memcpy (extFileHeader->tagDATA, "data", 4);
        extFileHeader->lenDATA = nSamples * (nSampleBits >> 3);

        /* exporting big-endian header is not supported*/
        
        }
    else
        {
        if (bufSize < sizeof(PCM_FILE_HEADER))  
            {
            return ERROR;
            }          
        bzero ((char *)pcmFileHeader, sizeof (PCM_FILE_HEADER));
        memcpy (pcmFileHeader->tagRIFF, "RIFF", 4);

        /* lenRIFF equals header size not including the begining tagRIFF and
         * lenRIFF fields, and adding the total data size */

        pcmFileHeader->lenRIFF = (UINT32)(sizeof (PCM_FILE_HEADER) -
                                         sizeof (INT32) * 2 + nSamples *
                                         (nSampleBits >> 3));
        memcpy (pcmFileHeader->tagWAVE, "WAVE", 4);
        memcpy (pcmFileHeader->tagFMT, "fmt ", 4);

        pcmFileHeader->lenFMT = (UINT32)sizeof (PCM_WAVE_FORMAT);
        pcmFileHeader->pcmFormat.formatTag       = (UINT16)WAVE_FORMAT_PCM;
        pcmFileHeader->pcmFormat.channels        = (UINT16)nChannels;
        pcmFileHeader->pcmFormat.samplesPerSec   = nSampleRate;
        pcmFileHeader->pcmFormat.bitsPerSample   = (UINT16)nSampleBits;
        pcmFileHeader->pcmFormat.avgBytesPerSec  = (UINT32)(nSampleRate *
                                                           nChannels *
                                                           (nSampleBits >> 3));
        pcmFileHeader->pcmFormat.blockAlign = (UINT16)(nChannels *
                                                      (nSampleBits >> 3));

        memcpy (pcmFileHeader->tagDATA, "data", 4);
        pcmFileHeader->lenDATA = nSamples * (nSampleBits >> 3);

        /* exporting big-endian header is not supported*/  

        }
    
    return OK;
    }    
