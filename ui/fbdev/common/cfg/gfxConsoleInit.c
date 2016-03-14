/* gfxConsoleInit.c - Frame buffer driver initialization */

/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
01oct14,yat  Change sprintf to snprintf (V7GFX-220)
24jan14,mgc  Modified for VxWorks 7 release
04apr13,mgc  Merged with console initialization
17may12,rfm  Written
*/

#if defined(INCLUDE_FBDEV_CONSOLE)

/* includes */

/* 
 * Font Definitions
 * The following header files will define a 'gfxFont' array. The first byte in
 * the array indicates the pixel height of the font.  All fonts are assumed to
 *  be 8 pixel wide. 
 */
#include <gfxConsoleFonts.h>

/* defines */

/* Character dimensions in pixels */
#define GFX_CHAR_HEIGHT     gfxFont[0]
#define GFX_CHAR_WIDTH      8

/* forward declarations */

static void gfxFbConsoleBlitChar(unsigned char, FB_INFO *);

/* locals */

/* Cursor position */
static int gfxConsoleX0 = 0;
static int gfxConsoleY0 = 0;

/*******************************************************************************
*
* gfxFbConsoleInit - initialize frame buffer console
* 
* This routine is called during kernel bootup.  It re-initializes the shell in
* order to redirect IO into a frame buffer.
*
* RETURNS: N/A
*/
void gfxFbConsoleInit
    (
    void
    ) 
    {
    int fdIn;
#if  (FB_CONSOLE_REDIRECT == TRUE)
    int i;
    char deviceName[FB_MAX_STR_CHARS];
    int consoleFd = -1;
#endif

#ifdef INCLUDE_USB_GEN2_KEYBOARD
    char kbdName[128];
    (void)snprintf (kbdName, sizeof (kbdName), "%s%d", USB_GEN2_KEYBOARD_NAME, 0);
    fdIn = open (kbdName, O_RDONLY, 0666);
    if (fdIn == ERROR)
        fdIn = ioGlobalStdGet (STD_IN);
#elif defined (INCLUDE_USB_KEYBOARD)
    fdIn = open ("/usbKb/0", O_RDONLY, 0666);
    if (fdIn == ERROR)
        fdIn = ioGlobalStdGet (STD_IN);
#else
    fdIn = ioGlobalStdGet (STD_IN);
#endif

#if  (FB_CONSOLE_REDIRECT == TRUE)
    for (i = 0; i < FB_MAX_DEVICE; i++)
        {
        (void)snprintf (deviceName, FB_MAX_STR_CHARS, "%s%d", FB_DEVICE_PREFIX, i);
        if ((consoleFd = open(deviceName, O_RDWR, 0666)) != -1)
            {
            break;
            }
        }
    if (i < FB_MAX_DEVICE)
        {
        shellTerminate(shellFirst());
        if (OK != shellGenericInit(SHELL_FIRST_CONFIG, SHELL_STACK_SIZE,
                                   NULL, NULL, TRUE, TRUE, fdIn,
                                   consoleFd, consoleFd))
            {
            (void)close (consoleFd);
            }
        else
            {
            ioGlobalStdSet(STD_OUT, consoleFd);
            ioGlobalStdSet(STD_ERR, consoleFd);
            }
        }
#endif
    }

/*******************************************************************************
*
* gfxFbConsoleWrite - write characters to a frame buffer
* 
* When a frame buffer driver is configured with console support, this routine
* parses incoming strings for rendering to a frame buffer.
*
* RETURNS: The number of bytes written if successful, otherwise, -1
*
* ERRNO:
* \is
* \i EINVAL
* Invalid argument
* \ie
*/
int gfxFbConsoleWrite
    (
    FB_INFO* pFbInfo,   /* frame buffer information */
    const char* pBuf,   /* string of characters to be rendered */
    size_t n            /* size of string buffer */
    )
    {
    char        c;              /* current character */
    int         i;
    int         numBytes = 0;   /* bytes written */
    int         xMax = 0;       /* console x-axis dimensions */
    int         yMax = 0;       /* console y-axis dimensions */

    /* Check parameters */
    if ((pFbInfo == NULL) || (pBuf == NULL))
        {
        errno = EINVAL;
        return (-1);
        }

    /* Check if there's anything to output */
    if (n == 0)
        return (0);

    /* Determine console dimensions */
    xMax = ((pFbInfo->width / GFX_CHAR_WIDTH) - 1) * GFX_CHAR_WIDTH;
    yMax = ((pFbInfo->height / GFX_CHAR_HEIGHT) - 1) * GFX_CHAR_HEIGHT;

    /* Process string */
    while (n-- > 0)
        {
        c = *pBuf++;
        switch (c)
            {
            /* Backspace */
            case '\b':
                if (gfxConsoleX0 > 0)
                    gfxConsoleX0 -= GFX_CHAR_WIDTH;
                break;

            /* Line feed */
            case '\n':
                gfxConsoleX0 = 0;
                if (gfxConsoleY0 < yMax)
                    gfxConsoleY0 += GFX_CHAR_HEIGHT;
                else
                    {
                    void* pDst = pFbInfo->pFb;
                    void* pSrc = (void*)(((char*)pFbInfo->pFb) + 
                                 (GFX_CHAR_HEIGHT * pFbInfo->stride));
                    for (i = yMax; i > 0; i--)
                        {
                        memcpy(pDst, pSrc, pFbInfo->width * (pFbInfo->bpp >> 3));
                        pSrc = (void*)((char*)pSrc + pFbInfo->stride);
                        pDst = (void*)((char*)pDst + pFbInfo->stride);
                        }
                    for (i = GFX_CHAR_HEIGHT; i > 0; i--)
                        {
                        memset(pDst, 0, pFbInfo->width * (pFbInfo->bpp >> 3));
                        pDst = (void*)((char*)pDst + pFbInfo->stride);
                        }
                    }
                break;

            /* Carriage return */
            case '\r':
                gfxConsoleX0 = 0;
                break;

            /* Tab space */
            case '\t':
                gfxConsoleX0 += FB_CONSOLE_TAB * GFX_CHAR_WIDTH;
                break;

            default:
                if (gfxConsoleX0 <= xMax)
                    {
                    gfxFbConsoleBlitChar(c, pFbInfo);
                    gfxConsoleX0 += GFX_CHAR_WIDTH;
                    }
            }
        numBytes++;
        }

    return (numBytes);
    }

/*******************************************************************************
*
* gfxFbConsoleBlitChar - blit a character to a frame buffer
* 
* This routine blits a character based into a graphic device's frame buffer.
* Pixels are colored with either a foreground or background color based on the
* font information provided in the local font array.
*
* RETURNS: N/A
*/
static void gfxFbConsoleBlitChar
    (
    unsigned char c,    /* character to be rendered */
    FB_INFO* pFbInfo    /* frame buffer information */
    )
    {
    const unsigned char*    pChar;      /* pointer to character font */
    void*                   pBuf;
    unsigned int            i;
    unsigned int            bits;       /* current font line */
    unsigned int            mask;       /* current font pixel */
    unsigned int            padding;    /* offset to next character line */
    unsigned int            color;
    unsigned short          color16;

    /* Point to the character's position in the font array */
    pChar = &gfxFont[1 + (c * GFX_CHAR_HEIGHT)];

    /* Point to the correct pixel position in the frame buffer */
    pBuf = (void*)(((char*)pFbInfo->pFb) +
    		(gfxConsoleX0 * (pFbInfo->bpp >> 3)) + (gfxConsoleY0 * pFbInfo->stride));

    padding = pFbInfo->stride - (GFX_CHAR_WIDTH * (pFbInfo->bpp >> 3));

    /* Iterate over the height of the character */
    for (i = 0; i < GFX_CHAR_HEIGHT; i++)
        {
        bits = pChar[i];
        /* Iterate over the width of the character*/
        for (mask = 0x80; mask != 0x00; mask >>= 1)
            {
            color = ((bits & mask) ? (FB_CONSOLE_FGCOLOR) : 
                                     (FB_CONSOLE_BGCOLOR));
            if (pFbInfo->bpp == 16)
                {
                color16 = (unsigned short) (((color & 0xf80000) >> 8)
                          | ((color & 0xfc00) >> 5) | (color & 0xf8) >> 3);
                *((unsigned short *)pBuf) = color16;
                pBuf = (void *)((char*)pBuf + 2);
                }
            else if (pFbInfo->bpp == 32)
                {
            	*((unsigned int *)pBuf) = color;
                pBuf = (void *)((char*)pBuf + 4);
                }
            }
        /* Advance to the next line*/
        pBuf = (void *)((char*)pBuf + padding);
        }
    }

#endif /* INCLUDE_FBDEV_CONSOLE */
