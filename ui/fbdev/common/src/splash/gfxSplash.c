/* gfxSplash.c - Splash screen */

/*
 * Copyright (c) 2014, 2016 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
06jan16,rpc  Replaced gfxFbSplashBlit1 with integer version (US73127)
22dec14,yat  Fix build warnings for LP64 (US50456)
24jan14,mgc  Modified for VxWorks 7 release
*/

/* includes */

#include <vxWorks.h>
#include <stdlib.h>
#include <string.h>
#include "inflate.inl"

#define SCALE_FACTOR_BITS   10
#define SCALE_FACTOR        (1 << SCALE_FACTOR_BITS)

/* forward declarations */

IMPORT unsigned int     wrSplashInflatedSize;
IMPORT unsigned char    wrSplashDeflatedData[];
IMPORT unsigned int     wrSplashDeflatedSize;

/*******************************************************************************
*
* gfxFbSplashBlit - Stretch-blit the splash screen
*
* RETURNS: N/A
*/
STATUS gfxFbSplashBlit1
    (
    void*                   pFb,       /* frame buffer information */
    int                     xres,
    int                     yres,
    int                     bpp,
    int                     stride
    )
    {
    int                     ix, iy, n;
    int                     palette[16];
    unsigned char*          pBmp0;
    unsigned char*          pBmp;
    int                     x, dx;
    int                     y, dy;
    int                     cnt;

    if (pFb == NULL) return ERROR;

    pBmp0 = malloc (wrSplashInflatedSize);
    if (pBmp0 == NULL) return ERROR;
    pBmp = pBmp0;

    (void)splash_inflate(wrSplashDeflatedData, pBmp, wrSplashDeflatedSize);

    dx = 640 * SCALE_FACTOR / xres;
    dy = 480 * SCALE_FACTOR / yres;
    cnt = 0;

    switch (bpp)
        {
        case 16:
            {
            unsigned short* pBuf = (unsigned short*)pFb;
            for (n = 0; n < 64; n += 4)
                {
                palette[n >> 2] = ((pBmp[54 + n + 2] & 0xf8) << 8) +
                                  ((pBmp[54 + n + 1] & 0xfc) << 3) +
                                  ((pBmp[54 + n + 0] >> 3) & 0x1f);
                }
            pBmp += pBmp[10];
            for (y = 0; y < (480 * SCALE_FACTOR);)
                {
                unsigned short* pBuf2 = pBuf;
                for (x = 0; x < (640 * SCALE_FACTOR); x += dx)
                    {
                    ix = x >> SCALE_FACTOR_BITS;
                    n = pBmp[ix >> 1];
                    n = (ix & 1) ? (n) : (n >> 4);
                    n &= 0x0f;
                    if (cnt == (xres * yres))
                        break;
                    *pBuf2++ = (unsigned short)palette[n];
                    cnt++;
                    }
                pBuf += (stride >> 1);
                iy = (y >> SCALE_FACTOR_BITS) << SCALE_FACTOR_BITS;
                y += dy;
                pBmp += ((y - iy) >> SCALE_FACTOR_BITS) * ((640 * 4) / 8);
                }
            break;
            }
        case 24:
            {
            unsigned char* pBuf = (unsigned char*)pFb;
            memcpy(palette, pBmp + 54, 64);
            pBmp += pBmp[10];
            for (y = 0; y < (480 * SCALE_FACTOR);)
                {
                for (x = 0; x < (640 * SCALE_FACTOR); x += dx)
                    {
                    ix = x >> SCALE_FACTOR_BITS;
                    n = pBmp[ix >> 1];
                    n = (ix & 1) ? (n << 2) : (n >> 2);
                    n &= 0x3c;
                    if (cnt == (xres * yres))
                        break;
                    *pBuf++ = (unsigned char)palette[n + 2];
                    *pBuf++ = (unsigned char)palette[n + 1];
                    *pBuf++ = (unsigned char)palette[n + 0];
                    cnt++;
                    }
                iy = (y >> SCALE_FACTOR_BITS) << SCALE_FACTOR_BITS;
                y += dy;
                pBmp += ((y - iy) >> SCALE_FACTOR_BITS) * ((640 * 4) / 8);
                }
            break;
            }
        case 32:
            {
            unsigned int* pBuf = (unsigned int*)pFb;
            for (n = 0; n < 64; n += 4)
                {
                palette[n >> 2] = (pBmp[54 + n + 2] << 16) +
                                  (pBmp[54 + n + 1] << 8) +
                                  (pBmp[54 + n + 0]);
                }
            pBmp += pBmp[10];
            for (y = 0; y < (480 * SCALE_FACTOR);)
                {
                unsigned int* pBuf2 = pBuf;
                for (x = 0; x < (640 * SCALE_FACTOR); x += dx)
                    {
                    ix = x >> SCALE_FACTOR_BITS;
                    n = pBmp[ix >> 1];
                    n = (ix & 1) ? (n) : (n >> 4);
                    n &= 0x0f;
                    if (cnt == (xres * yres ))
                        break;
                    *pBuf2++ = palette[n];
                    cnt++;
                    }
                pBuf += (stride >> 2);
                iy = (y >> SCALE_FACTOR_BITS) << SCALE_FACTOR_BITS;
                y += dy;
                pBmp += ((y - iy) >> SCALE_FACTOR_BITS) * ((640 * 4) / 8);
                }
            break;
            }
        default:
            break;
        }

    free (pBmp0);

    return OK;
    }

