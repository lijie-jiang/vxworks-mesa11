/* vgfilters.c - Wind River VG Filter Functionality */

/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
22dec14,yat  Fix build warning for LP64 (US50456)
25jun13,mgc  Modified for VxWorks 7 release
03mar13,m_c  Updated for SMP
10jun09,m_c  Written
*/

/*
DESCRIPTION
These routines provide the filtering capabilities that support the OpenVG
implementation.
*/

/* includes */

#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/* defines */

/* TRUE for RGB pixel formats */
#define	IS_RGB(_format) (((_format) != VG_sL_8) && \
                         ((_format) != VG_lL_8) && \
                         ((_format) != VG_A_8) &&  \
                         ((_format) != VG_BW_1) && \
                         ((_format) != VG_A_1) &&  \
                         ((_format) != VG_A_4))

/* Visual code helper */
#define isStandard  !isLinear

/* Modulus */
#define MOD(_a, _b) (((_a) % (_b)) + (((_a) < 0) * (_b)))

/* Normalization multipliers */
#define OO15    0.066666666666666666666666666666667
#define OO255   0.003921568627450980392156862745098
#define OO31    0.032258064516129032258064516129032
#define OO63    0.015873015873015873015873015873016

/* Unpack a color channel */
#define UNPACK(_pix, _shift, _mask) (((_pix) >> (_shift)) & (_mask))

/* forward declarations */

LOCAL int getFilterChannelMask(const gc_t*, const int);
LOCAL int overlap(const image_t*, const image_t*);
LOCAL void readPixel(const image_t*, int, int, VGTilingMode, float[4], const gc_t*);
LOCAL void writePixel(const image_t*, int, int, float[4], const gc_t*);

/*******************************************************************************
 *
 * getFilterChannelMask - return the filter channel mask adjusted for the
 *                        specified image format
 *
 * RETUSN: A channel mask
 *
 */
LOCAL int getFilterChannelMask
    (
    const gc_t* pGc,
    const int format
    )
    {
    int mask;

    if ((format == VG_sL_8) || (format == VG_lL_8) || (format == VG_BW_1))
        mask = VG_RED | VG_GREEN | VG_BLUE | VG_ALPHA;
    else
        mask = pGc->vg.filterChannelMask & VG_IMAGE_CHANNEL_MASK;

    return (mask);
    }

/*******************************************************************************
 *
 * overlap - test if the specified images overlap
 *
 * RETURNS: TRUE or FALSE
 *
 */
LOCAL int overlap
    (
    const image_t* pImageA,
    const image_t* pImageB
    )
    {
    int ax1, ay1;
    int ax2, ay2;
    int bx1, by1;
    int bx2, by2;

    if (pImageA->pSurface != pImageB->pSurface)
        return (FALSE);

    ax1 = pImageA->x0, ay1 = pImageA->y0;
    ax2 = ax1 + pImageA->width, ay2= ay1 + pImageA->height;
    bx1 = pImageB->x0, by1 = pImageB->y0;
    bx2 = bx1 + pImageB->width, by2= by1 + pImageB->height;

    return ((ax1 < bx2) && (ax2 > bx1) && (ay1 < by2) && (ay2 > by1));
    }

/*******************************************************************************
 *
 * readPixel - read a pixel and convert it to one of sRGBA, sRGBA_PRE, lRGBA or lRGBA_PRE
 *
 */
LOCAL void readPixel
    (
    const image_t* pImage,
    int x,
    int y,
    VGTilingMode tilingMode,
    float color[4],
    const gc_t* pGc
    )
    {
    float       a;          /* alpha channel */
    int         format;     /* image format */
    int         isLinear;   /* TRUE if the source pixel is in linear format */
    uint        pixel;      /* source pixel */
    pixel_ptr_t pBuf;       /* surface buffer */

    /* Get the image format */
    format = pImage->format;

    /* Adjust the coordinates per tiling mode */
    switch (tilingMode)
        {
        case VG_TILE_FILL:
            if ((x < 0) || (x >= pImage->width) || (y < 0) || (y >= pImage->height))
                format = -1;
            break;

        case VG_TILE_PAD:
            x = max(0, min(x, pImage->width - 1));
            y = max(0, min(y, pImage->height - 1));
            break;

        case VG_TILE_REPEAT:
            x = MOD(x, pImage->width);
            y = MOD(y, pImage->height);
            break;

        case VG_TILE_REFLECT:
            x = MOD(x, pImage->width * 2);
            if (x > pImage->width)
                x = pImage->width * 2 - 1 - x;
            y = MOD(y, pImage->height * 2);
            if (y > pImage->height)
                y = (pImage->height * 2) - 1 - y;
            break;
        default:
            /* Do nothing */
            break;
        }

    /* Calculate the address of the pixel to read */
    x += pImage->x0, y += pImage->y0;
    pBuf.v = (void *)((long)pImage->pSurface->pFrontBuf + (x * pImage->bpp / 8) + (y * pImage->pSurface->stride));

    /* Assume that the source pixel is in standard format */
    isLinear = FALSE;

    /* Read and normalize (steps 1, 2 and 3) (see specifications, "12.1 Format Normalization") */
    switch (format)
        {
        case VG_lRGBX_8888:
            isLinear = TRUE;
        case VG_sRGBX_8888:
            pixel = *pBuf.u32;
            color[VG_R] = (float)(UNPACK(pixel, 24, 0xff) * OO255);
            color[VG_G] = (float)(UNPACK(pixel, 16, 0xff) * OO255);
            color[VG_B] = (float)(UNPACK(pixel, 8, 0xff) * OO255);
            color[VG_A] = 1.0f;
            break;

        case VG_lRGBA_8888:
            isLinear = TRUE;
        case VG_sRGBA_8888:
            pixel = *pBuf.u32;
            color[VG_R] = (float)(UNPACK(pixel, 24, 0xff) * OO255);
            color[VG_G] = (float)(UNPACK(pixel, 16, 0xff) * OO255);
            color[VG_B] = (float)(UNPACK(pixel, 8, 0xff) * OO255);
            color[VG_A] = (float)(UNPACK(pixel, 0, 0xff) * OO255);
            break;

        case VG_lRGBA_8888_PRE:
            isLinear = TRUE;
        case VG_sRGBA_8888_PRE:
            pixel = *pBuf.u32, a = (float)UNPACK(pixel, 0, 0xff);
            if (a == 0.0f)
                color[VG_R] = color[VG_G] = color[VG_B] = color[VG_A] = 0.0f;
            else
                {
                color[VG_R] = (float)UNPACK(pixel, 24, 0xff) / a;
                color[VG_G] = (float)UNPACK(pixel, 16, 0xff) / a;
                color[VG_B] = (float)UNPACK(pixel, 8, 0xff) / a;
                color[VG_A] = a * (float)OO255;
                }
            break;

        case VG_sRGB_565:
            pixel = *pBuf.u16;
            color[VG_R] = (float)(UNPACK(pixel, 11, 0x1f) * OO31);
            color[VG_G] = (float)(UNPACK(pixel, 5, 0x3f) * OO63);
            color[VG_B] = (float)(UNPACK(pixel, 0, 0x1f) * OO31);
            color[VG_A] = 1.0f;
            break;

        case VG_sRGBA_5551:
            pixel = *pBuf.u16;
            color[VG_R] = (float)(UNPACK(pixel, 11, 0x1f) * OO31);
            color[VG_G] = (float)(UNPACK(pixel, 6, 0x1f) * OO31);
            color[VG_B] = (float)(UNPACK(pixel, 1, 0x1f) * OO31);
            color[VG_A] = (float)(UNPACK(pixel, 0, 0x01));
            break;

        case VG_sRGBA_4444:
            pixel = *pBuf.u16;
            color[VG_R] = (float)(UNPACK(pixel, 12, 0x0f) * OO15);
            color[VG_G] = (float)(UNPACK(pixel, 8, 0x0f) * OO15);
            color[VG_B] = (float)(UNPACK(pixel, 4, 0x0f) * OO15);
            color[VG_A] = (float)(UNPACK(pixel, 0, 0x0f) * OO15);
            break;

        case VG_lL_8:
            isLinear = TRUE;
        case VG_sL_8:
            pixel = *pBuf.u8;
            color[VG_R] = color[VG_G] = color[VG_B] = (float)(UNPACK(pixel, 0, 0xff) * OO255);
            color[VG_A] = 1.0f;
            break;

        case VG_A_8:
            pixel = *pBuf.u8;
            color[VG_R] = color[VG_G] = color[VG_B] = color[VG_A] = (float)(UNPACK(pixel, 0, 0xff) * OO255);
            break;

        case VG_BW_1:
            isLinear = TRUE;
            pixel = *pBuf.u8;
            color[VG_R] = color[VG_G] = color[VG_B] = (float)UNPACK(pixel, 7 - (x & 7), 0x01);
            color[VG_A] = 1.0f;
            break;

        case VG_A_1:
            pixel = *pBuf.u8;
            color[VG_R] = color[VG_G] = color[VG_B] = color[VG_A] = (float)UNPACK(pixel, 7 - (x & 7), 0x01);
            break;

        case VG_A_4:
            pixel = *pBuf.u8;
            color[VG_R] = color[VG_G] = color[VG_B] = color[VG_A] = (float)UNPACK(pixel, (1 - (x & 1)) * 4, 0x0f);
            break;

        case VG_lXRGB_8888:
            isLinear = FALSE;
        case VG_sXRGB_8888:
            pixel = *pBuf.u32;
            color[VG_R] = (float)(UNPACK(pixel, 16, 0xff) * OO255);
            color[VG_G] = (float)(UNPACK(pixel, 8, 0xff) * OO255);
            color[VG_B] = (float)(UNPACK(pixel, 0, 0xff) * OO255);
            color[VG_A] = 1.0f;
            break;

        case VG_lARGB_8888:
            isLinear = FALSE;
        case VG_sARGB_8888:
            pixel = *pBuf.u32;
            color[VG_R] = (float)(UNPACK(pixel, 16, 0xff) * OO255);
            color[VG_G] = (float)(UNPACK(pixel, 8, 0xff) * OO255);
            color[VG_B] = (float)(UNPACK(pixel, 0, 0xff) * OO255);
            color[VG_A] = (float)(UNPACK(pixel, 24, 0xff) * OO255);
            break;

        case VG_lARGB_8888_PRE:
            isLinear = TRUE;
        case VG_sARGB_8888_PRE:
            pixel = *pBuf.u32, a = (float)UNPACK(pixel, 24, 0xff);
            if (a == 0.0f)
                color[VG_R] = color[VG_G] = color[VG_B] = color[VG_A] = 0.0f;
            else
            {
                color[VG_R] = (float)UNPACK(pixel, 16, 0xff) / a;
                color[VG_G] = (float)UNPACK(pixel, 8, 0xff) / a;
                color[VG_B] = (float)UNPACK(pixel, 0, 0xff) / a;
                color[VG_A] = a * (float)OO255;
            }
            break;

        case VG_sARGB_1555:
            pixel = *pBuf.u16;
            color[VG_R] = (float)(UNPACK(pixel, 10, 0x1f) * OO31);
            color[VG_G] = (float)(UNPACK(pixel, 5, 0x1f) * OO31);
            color[VG_B] = (float)(UNPACK(pixel, 0, 0x1f) * OO31);
            color[VG_A] = (float)(UNPACK(pixel, 15, 0x01));
            break;

        case VG_sARGB_4444:
            pixel = *pBuf.u16;
            color[VG_R] = (float)(UNPACK(pixel, 8, 0x0f) * OO15);
            color[VG_G] = (float)(UNPACK(pixel, 4, 0x0f) * OO15);
            color[VG_B] = (float)(UNPACK(pixel, 0, 0x0f) * OO15);
            color[VG_A] = (float)(UNPACK(pixel, 12, 0x0f) * OO15);
            break;

        case VG_lBGRX_8888:
            isLinear = TRUE;
        case VG_sBGRX_8888:
            pixel = *pBuf.u32;
            color[VG_R] = (float)(UNPACK(pixel, 8, 0xff) * OO255);
            color[VG_G] = (float)(UNPACK(pixel, 16, 0xff) * OO255);
            color[VG_B] = (float)(UNPACK(pixel, 24, 0xff) * OO255);
            color[VG_A] = 1.0f;
            break;

        case VG_lBGRA_8888:
            isLinear = TRUE;
        case VG_sBGRA_8888:
            pixel = *pBuf.u32;
            color[VG_R] = (float)(UNPACK(pixel, 8, 0xff) * OO255);
            color[VG_G] = (float)(UNPACK(pixel, 16, 0xff) * OO255);
            color[VG_B] = (float)(UNPACK(pixel, 24, 0xff) * OO255);
            color[VG_A] = (float)(UNPACK(pixel, 0, 0xff) * OO255);
            break;

        case VG_lBGRA_8888_PRE:
            isLinear = TRUE;
        case VG_sBGRA_8888_PRE:
            pixel = *pBuf.u32, a = (float)UNPACK(pixel, 0, 0xff);
            if (a == 0.0f)
                color[VG_R] = color[VG_G] = color[VG_B] = color[VG_A] = 0.0f;
            else
                {
                color[VG_R] = (float)UNPACK(pixel, 8, 0xff) / a;
                color[VG_G] = (float)UNPACK(pixel, 16, 0xff) / a;
                color[VG_B] = (float)UNPACK(pixel, 24, 0xff) / a;
                color[VG_A] = a * (float)OO255;
                }
            break;

        case VG_sBGR_565:
            pixel = *pBuf.u16;
            color[VG_R] = (float)(UNPACK(pixel, 0, 0x1f) * OO31);
            color[VG_G] = (float)(UNPACK(pixel, 5, 0x3f) * OO63);
            color[VG_B] = (float)(UNPACK(pixel, 11, 0x1f) * OO31);
            color[VG_A] = 1.0f;
            break;

        case VG_sBGRA_5551:
            pixel = *pBuf.u16;
            color[VG_R] = (float)(UNPACK(pixel, 1, 0x1f) * OO31);
            color[VG_G] = (float)(UNPACK(pixel, 6, 0x1f) * OO31);
            color[VG_B] = (float)(UNPACK(pixel, 11, 0x1f) * OO31);
            color[VG_A] = (float)(UNPACK(pixel, 0, 0x01));
            break;

        case VG_sBGRA_4444:
            pixel = *pBuf.u16;
            color[VG_R] = (float)(UNPACK(pixel, 4, 0x0f) * OO15);
            color[VG_G] = (float)(UNPACK(pixel, 8, 0x0f) * OO15);
            color[VG_B] = (float)(UNPACK(pixel, 12, 0x0f) * OO15);
            color[VG_A] = (float)(UNPACK(pixel, 0, 0x0f) * OO15);
            break;

        case VG_lXBGR_8888:
            isLinear = TRUE;
        case VG_sXBGR_8888:
            pixel = *pBuf.u32;
            color[VG_R] = (float)(UNPACK(pixel, 0, 0xff) * OO255);
            color[VG_G] = (float)(UNPACK(pixel, 8, 0xff) * OO255);
            color[VG_B] = (float)(UNPACK(pixel, 16, 0xff) * OO255);
            color[VG_A] = 1.0;
            break;

        case VG_lABGR_8888:
            isLinear = TRUE;
        case VG_sABGR_8888:
            pixel = *pBuf.u32;
            color[VG_R] = (float)(UNPACK(pixel, 0, 0xff) * OO255);
            color[VG_G] = (float)(UNPACK(pixel, 8, 0xff) * OO255);
            color[VG_B] = (float)(UNPACK(pixel, 16, 0xff) * OO255);
            color[VG_A] = (float)(UNPACK(pixel, 24, 0xff) * OO255);
            break;

        case VG_lABGR_8888_PRE:
            isLinear = TRUE;
        case VG_sABGR_8888_PRE:
            pixel = *pBuf.u32, a = (float)UNPACK(pixel, 24, 0xff);
            if (a == 0.0f)
                color[VG_R] = color[VG_G] = color[VG_B] = color[VG_A] = 0.0f;
            else
                {
                color[VG_R] = (float)UNPACK(pixel, 0, 0xff) / a;
                color[VG_G] = (float)UNPACK(pixel, 8, 0xff) / a;
                color[VG_B] = (float)UNPACK(pixel, 16, 0xff) / a;
                color[VG_A] = a * (float)OO255;
                }
            break;

        case VG_sABGR_1555:
            pixel = *pBuf.u16;
            color[VG_R] = (float)(UNPACK(pixel, 0, 0x1f) * OO31);
            color[VG_G] = (float)(UNPACK(pixel, 5, 0x1f) * OO31);
            color[VG_B] = (float)(UNPACK(pixel, 10, 0x1f) * OO31);
            color[VG_A] = (float)(UNPACK(pixel, 15, 0x01));
            break;

        case VG_sABGR_4444:
            pixel = *pBuf.u16;
            color[VG_R] = (float)(UNPACK(pixel, 0, 0x0f) * OO15);
            color[VG_G] = (float)(UNPACK(pixel, 4, 0x0f) * OO15);
            color[VG_B] = (float)(UNPACK(pixel, 8, 0x0f) * OO15);
            color[VG_A] = (float)(UNPACK(pixel, 12, 0x0f) * OO15);
            break;

        default:
            color[VG_R] = pGc->vg.tileFillColor[VG_R];
            color[VG_G] = pGc->vg.tileFillColor[VG_G];
            color[VG_B] = pGc->vg.tileFillColor[VG_B];
            color[VG_A] = pGc->vg.tileFillColor[VG_A];
            break;
        }

    /* Convert, as needed, from linear to standard or vice-versa (step 4) */
    if (pGc->vg.filterLinear)
        {
        if (isStandard)
            {
            color[VG_R] = invgamma(color[VG_R]);
            color[VG_G] = invgamma(color[VG_G]);
            color[VG_B] = invgamma(color[VG_B]);
            }
        }
    else if (isLinear)
        {
        color[VG_R] = gamma(color[VG_R]);
        color[VG_G] = gamma(color[VG_G]);
        color[VG_B] = gamma(color[VG_B]);
        }

    /* Premultiply if needed (step 5) */
    if (pGc->vg.filterPremultiplied)
        {
        a = color[VG_A];
        color[VG_R] *= a;
        color[VG_G] *= a;
        color[VG_B] *= a;
        }
    }

/*******************************************************************************
 *
 * vgColorMatrix
 *
 */
VG_API_CALL void VG_API_ENTRY vgColorMatrix
    (
    VGImage dstImage,
    VGImage srcImage,
    const VGfloat* matrix
    ) VG_API_EXIT
    {
    int         filterChannelMask;          /* filter channel mask */
    int         filterWidth, filterHeight;  /* size of the region to apply the filter to */
    int         i, j, k;
    float       inColor[4], outColor[4];    /* normalized input and output colors */
    float       tmpColor[4];                /* temporary color */
    image_t *   pDstImage = (image_t*)dstImage;
    image_t *   pSrcImage = (image_t*)srcImage;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pDstImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidImage(pSrcImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (overlap(pDstImage, pSrcImage))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(matrix, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if either image is a rendering target */
        if ((pDstImage->inUse) || (pSrcImage->inUse))
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Get the filter channel mask */
        filterChannelMask = getFilterChannelMask(pGc, pSrcImage->format);

        /* Determine the size of the region to apply the filter to */
        filterWidth = min(pDstImage->width, pSrcImage->width);
        filterHeight = min(pDstImage->height, pSrcImage->height);

        /* Apply the filter */
        for (i = 0; i < filterHeight; i++)
            {
            for (j = 0; j < filterWidth; j++)
                {
                /* Read source pixel */
                readPixel(pSrcImage, j, i, VG_TILE_FILL, inColor, pGc);

                /* Apply matrix */
                for (k = 0; k < 4; k++)
                    {
                    tmpColor[k] = ((inColor[VG_R] * matrix[0 + k]) +
                                   (inColor[VG_G] * matrix[4 + k]) +
                                   (inColor[VG_B] * matrix[8 + k]) +
                                   (inColor[VG_A] * matrix[12 + k])) + matrix[16 + k];
                    }

                /* Filter channels */
                outColor[VG_R] = (filterChannelMask & VG_RED) ? (tmpColor[VG_R]) : (inColor[VG_R]);
                outColor[VG_G] = (filterChannelMask & VG_GREEN) ? (tmpColor[VG_G]) : (inColor[VG_G]);
                outColor[VG_B] = (filterChannelMask & VG_BLUE) ? (tmpColor[VG_B]) : (inColor[VG_B]);
                outColor[VG_A] = (filterChannelMask & VG_ALPHA) ? (tmpColor[VG_A]) : (inColor[VG_A]);

                /* Write pixel */
                writePixel(pDstImage, j, i, outColor, pGc);
                }
            }
        }
    }

/*******************************************************************************
 *
 * vgConvolve
 *
 */
VG_API_CALL void VG_API_ENTRY		vgConvolve
    (
    VGImage	dstImage,
    VGImage	srcImage,
    VGint kernelWidth,
    VGint kernelHeight,
    VGint shiftX,
    VGint shiftY,
    const VGshort* kernel,
    VGfloat scale,
    VGfloat bias,
    VGTilingMode tilingMode
    ) VG_API_EXIT
    {
    int filterChannelMask;                  /* filter channel mask */
    int         filterWidth, filterHeight;  /* size of the region to apply the filter to */
    int         i, j, k, l;
    float       inColor[4], outColor[4];    /* normalized colors */
    int         kx, ky;                     /* kernel coordinates */
    float       tmpColor[4];                /* temporary color */
    int         x, y;                       /* pixel coordinates */
    float       weight;                     /* pixel weight */
    image_t *   pDstImage = (image_t*)dstImage;
    image_t *   pSrcImage = (image_t*)srcImage;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pDstImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidImage(pSrcImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (overlap(pDstImage, pSrcImage))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((kernelWidth <= 0) || (kernelWidth > MAX_KERNEL_SIZE) ||
            (kernelHeight <= 0) || (kernelHeight > MAX_KERNEL_SIZE))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(kernel, sizeof(VGshort)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((tilingMode < VG_TILE_FILL) || (tilingMode > VG_TILE_REFLECT))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if either image is a rendering target */
        if ((pDstImage->inUse) || (pSrcImage->inUse))
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Get the filter channel mask */
        filterChannelMask = getFilterChannelMask(pGc, pSrcImage->format);

        /* Determine the size of the region to apply the filter to */
        filterWidth = min(pDstImage->width, pSrcImage->width);
        filterHeight = min(pDstImage->height, pSrcImage->height);

        /* Apply the filter */
        for (i = 0; i < filterHeight; i++)
            {
            for (j = 0; j < filterWidth; j++)
                {
                /* Read source pixel */
                readPixel(pSrcImage, j, i, VG_TILE_FILL, inColor, pGc);

                /* Calculate convolution */
                tmpColor[VG_R] = tmpColor[VG_G] = tmpColor[VG_B] = tmpColor[VG_A] = 0.0;
                for (k = 0; k < kernelHeight; k++)
                    {
                    ky = kernelHeight - k - 1;
                    y = i + k - shiftY;
                    for (l = 0; l < kernelWidth; l++)
                        {
                        kx = kernelWidth - l - 1;
                        x = j + l - shiftX;
                        weight = kernel[(kx * kernelHeight) + ky];
                        readPixel(pSrcImage, x, y, tilingMode, outColor, pGc);
                        tmpColor[VG_R] += outColor[VG_R] * weight;
                        tmpColor[VG_G] += outColor[VG_G] * weight;
                        tmpColor[VG_B] += outColor[VG_B] * weight;
                        tmpColor[VG_A] += outColor[VG_A] * weight;
                        }
                    }

                /* Filter channels */
                outColor[VG_R] = (filterChannelMask & VG_RED) ? ((tmpColor[VG_R] * scale) + bias) : (inColor[VG_R]);
                outColor[VG_G] = (filterChannelMask & VG_GREEN) ? ((tmpColor[VG_G] * scale) + bias) : (inColor[VG_G]);
                outColor[VG_B] = (filterChannelMask & VG_BLUE) ? ((tmpColor[VG_B] * scale) + bias) : (inColor[VG_B]);
                outColor[VG_A] = (filterChannelMask & VG_ALPHA) ? ((tmpColor[VG_A] * scale) + bias) : (inColor[VG_A]);

                /* Write pixel */
                writePixel(pDstImage, j, i, outColor, pGc);
                }
            }
        }
    }

/*******************************************************************************
 *
 * vgLookup
 *
 */
VG_API_CALL void VG_API_ENTRY vgLookup
    (
    VGImage dstImage,
    VGImage srcImage,
    const VGubyte* redLUT,
    const VGubyte* greenLUT,
    const VGubyte* blueLUT,
    const VGubyte* alphaLUT,
    VGboolean outputLinear,
    VGboolean outputPremultiplied
    ) VG_API_EXIT
    {
    float               a;                          /* alpha channel */
    int                 filterChannelMask;          /* filter channel mask */
    int                 filterWidth, filterHeight;  /* size of the region to apply the filter to */
    int                 i, j, k;
    float               inColor[4], outColor[4];    /* normalized colors */
    const VGubyte *     lookupTables[4];            /* lookup tables */
    float               tmpColor[4];                /* temporary color */
    image_t *           pDstImage = (image_t*)dstImage;
    image_t *           pSrcImage = (image_t*)srcImage;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pDstImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidImage(pSrcImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (overlap(pDstImage, pSrcImage))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((redLUT == NULL) || (greenLUT == NULL) ||
            (blueLUT == NULL) || (alphaLUT == NULL))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if either image is a rendering target */
        if ((pDstImage->inUse) || (pSrcImage->inUse))
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Get the filter channel mask */
        filterChannelMask = getFilterChannelMask(pGc, pSrcImage->format);

        /* Determine the size of the region to apply the filter to */
        filterWidth = min(pDstImage->width, pSrcImage->width);
        filterHeight = min(pDstImage->height, pSrcImage->height);

        /* Apply the filter */
        lookupTables[VG_R] = redLUT;
        lookupTables[VG_G] = greenLUT;
        lookupTables[VG_B] = blueLUT;
        lookupTables[VG_A] = alphaLUT;
        for (i = 0; i < filterHeight; i++)
            {
            for (j = 0; j < filterWidth; j++)
                {
                /* Read source pixel */
                readPixel(pSrcImage, j, i, VG_TILE_FILL, inColor, pGc);

                /* Pass all channels through the lookup tables */
                for (k = 0; k < 4; k++)
                    tmpColor[k] = (float)lookupTables[k][iround(inColor[k] * 255)] / 255.0f;

                /* Convert to internal processing format */
                if (pGc->vg.filterLinear)
                    {
                    if (!outputLinear)
                        {
                        tmpColor[VG_R] = invgamma(tmpColor[VG_R]);
                        tmpColor[VG_G] = invgamma(tmpColor[VG_G]);
                        tmpColor[VG_B] = invgamma(tmpColor[VG_B]);
                        }
                    }
                else if (outputLinear)
                    {
                    tmpColor[VG_R] = gamma(tmpColor[VG_R]);
                    tmpColor[VG_G] = gamma(tmpColor[VG_G]);
                    tmpColor[VG_B] = gamma(tmpColor[VG_B]);
                    }
                if (pGc->vg.filterPremultiplied)
                    {
                    if (!outputPremultiplied)
                        {
                        a = tmpColor[VG_A];
                        tmpColor[VG_R] *= a;
                        tmpColor[VG_G] *= a;
                        tmpColor[VG_B] *= a;
                        }
                    }
                else if (outputPremultiplied)
                    {
                    a = tmpColor[VG_A];
                    if (a == 0.0f)
                        tmpColor[VG_R] = tmpColor[VG_G] = tmpColor[VG_B] = 0.0f;
                    else
                        {
                        a = 1.0f / a;
                        tmpColor[VG_R] *= a;
                        tmpColor[VG_G] *= a;
                        tmpColor[VG_B] *= a;
                        }
                    }

                /* Filter channels */
                outColor[VG_R] = (filterChannelMask & VG_RED) ? (tmpColor[VG_R]) : (inColor[VG_R]);
                outColor[VG_G] = (filterChannelMask & VG_GREEN) ? (tmpColor[VG_G]) : (inColor[VG_G]);
                outColor[VG_B] = (filterChannelMask & VG_BLUE) ? (tmpColor[VG_B]) : (inColor[VG_B]);
                outColor[VG_A] = (filterChannelMask & VG_ALPHA) ? (tmpColor[VG_A]) : (inColor[VG_A]);

                /* Write pixel */
                writePixel(pDstImage, j, i, outColor, pGc);
                }
            }
        }
    }

/*******************************************************************************
 *
 * vgLookupSingle
 *
 */
VG_API_CALL void VG_API_ENTRY vgLookupSingle
    (
    VGImage dstImage,
    VGImage srcImage,
    const VGuint* lookupTable,
    VGImageChannel sourceChannel,
    VGboolean outputLinear,
    VGboolean outputPremultiplied
    ) VG_API_EXIT
    {
    float       a;                          /* alpha channel */
    int         filterChannelMask;          /* filter channel mask */
    int         filterWidth, filterHeight;  /* size of the region to apply the filter to */
    int         i, j;
    float       inColor[4], outColor[4];    /* normalized colors */
    VGuint      pixel;                      /* filtered pixel */
    float       tmpColor[4];                /* temporary color */
    image_t *   pDstImage = (image_t*)dstImage;
    image_t *   pSrcImage = (image_t*)srcImage;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pDstImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidImage(pSrcImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (overlap(pDstImage, pSrcImage))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(lookupTable, sizeof(VGuint)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (IS_RGB(pSrcImage->format) && (sourceChannel != VG_RED) &&
            (sourceChannel != VG_GREEN) && (sourceChannel != VG_BLUE) &&
            (sourceChannel != VG_ALPHA))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if either image is a rendering target */
        if ((pDstImage->inUse) || (pSrcImage->inUse))
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Convert the source channel into an channel index */
        switch (pSrcImage->format)
            {
            case VG_sL_8:
            case VG_lL_8:
                sourceChannel = VG_R;
                break;

            case VG_A_8:
            case VG_A_1:
            case VG_A_4:
                sourceChannel = VG_A;
                break;

            default:
                if (sourceChannel == VG_RED)
                    sourceChannel = VG_R;
                else if (sourceChannel == VG_GREEN)
                    sourceChannel = VG_G;
                else if (sourceChannel == VG_BLUE)
                    sourceChannel = VG_B;
                else
                    sourceChannel = VG_A;
                break;
            }

        /* Get the filter channel mask */
        filterChannelMask = getFilterChannelMask(pGc, pSrcImage->format);

        /* Determine the size of the region to apply the filter to */
        filterWidth = min(pDstImage->width, pSrcImage->width);
        filterHeight = min(pDstImage->height, pSrcImage->height);

        /* Apply the filter */
        for (i = 0; i < filterHeight; i++)
            {
            for (j = 0; j < filterWidth; j++)
                {
                /* Read source pixel */
                readPixel(pSrcImage, j, i, VG_TILE_FILL, inColor, pGc);

                /* Pass through lookup table */
                pixel = lookupTable[iround(inColor[sourceChannel] * 255)];
                tmpColor[VG_R] = (float)(((pixel >> 24) & 255) * OO255);
                tmpColor[VG_G] = (float)(((pixel >> 16) & 255) * OO255);
                tmpColor[VG_B] = (float)(((pixel >> 8) & 255) * OO255);
                tmpColor[VG_A] = (float)((pixel & 255) * OO255);

                /* Convert to internal processing format */
                if (pGc->vg.filterLinear)
                    {
                    if (!outputLinear)
                        {
                        tmpColor[VG_R] = invgamma(tmpColor[VG_R]);
                        tmpColor[VG_G] = invgamma(tmpColor[VG_G]);
                        tmpColor[VG_B] = invgamma(tmpColor[VG_B]);
                        }
                    }
                else if (outputLinear)
                    {
                    tmpColor[VG_R] = gamma(tmpColor[VG_R]);
                    tmpColor[VG_G] = gamma(tmpColor[VG_G]);
                    tmpColor[VG_B] = gamma(tmpColor[VG_B]);
                    }
                if (pGc->vg.filterPremultiplied)
                    {
                    if (!outputPremultiplied)
                        {
                        a = tmpColor[VG_A];
                        tmpColor[VG_R] *= a;
                        tmpColor[VG_G] *= a;
                        tmpColor[VG_B] *= a;
                        }
                    }
                else if (outputPremultiplied)
                    {
                    a = tmpColor[VG_A];
                    if (a == 0.0f)
                        tmpColor[VG_R] = tmpColor[VG_G] = tmpColor[VG_B] = 0.0f;
                    else
                        {
                        a = 1.0f / a;
                        tmpColor[VG_R] *= a;
                        tmpColor[VG_G] *= a;
                        tmpColor[VG_B] *= a;
                        }
                    }

                /* Filter channels */
                outColor[VG_R] = (filterChannelMask & VG_RED) ? (tmpColor[VG_R]) : (inColor[VG_R]);
                outColor[VG_G] = (filterChannelMask & VG_GREEN) ? (tmpColor[VG_G]) : (inColor[VG_G]);
                outColor[VG_B] = (filterChannelMask & VG_BLUE) ? (tmpColor[VG_B]) : (inColor[VG_B]);
                outColor[VG_A] = (filterChannelMask & VG_ALPHA) ? (tmpColor[VG_A]) : (inColor[VG_A]);

                /* Write pixel */
                writePixel(pDstImage, j, i, outColor, pGc);
                }
            }
        }
    }

/*******************************************************************************
 *
 * writePixel - write a pixel with the specified color (assumed to be one of sRGBA, sRGBA_PRE, lRGBA or lRGBA_PRE)
 *
 */
LOCAL void writePixel
    (
    const image_t* pImage,
    int x,
    int y,
    float color[4],
    const gc_t* pGc
    )
    {
    float       a;      /* alpha channel */
    pixel_ptr_t pBuf;   /* surface buffer */
    uint    pixel;      /* native pixel */

    /* Clip */
    if ((x < 0) || (x > pImage->width) || (y < 0) || (y > pImage->height))
        return;

    /* Calculate the address of the pixel to write */
    x += pImage->x0, y += pImage->y0;
    pBuf.v = (void *)((long)pImage->pSurface->pBackBuf + (x * pImage->bpp / 8) +
                        (y * pImage->pSurface->stride));

    /* Convert to sRGBA */
    if (pGc->vg.filterPremultiplied)
        {
        a = clamp1(color[VG_A]);
        if (a == 0.0)
            color[VG_R] = color[VG_G] = color[VG_B] = 0.0;
        else
            {
            color[VG_R] = clamp(color[VG_R], 0.0, a) / a;
            color[VG_G] = clamp(color[VG_G], 0.0, a) / a;
            color[VG_B] = clamp(color[VG_B], 0.0, a) / a;
            }
        }
    else
        {
        color[VG_R] = clamp1(color[VG_R]);
        color[VG_G] = clamp1(color[VG_G]);
        color[VG_B] = clamp1(color[VG_B]);
        color[VG_A] = clamp1(color[VG_A]);
        }
    if (pGc->vg.filterLinear)
        {
        color[VG_R] = gamma(color[VG_R]);
        color[VG_G] = gamma(color[VG_G]);
        color[VG_B] = gamma(color[VG_B]);
        }

    /* Write the pixel */
    pixel = vgConvertColorWRS(color, pImage->format);
    switch (pImage->bpp)
        {
        case 32:
            *pBuf.u32 = pixel;
            break;
        }
    }
