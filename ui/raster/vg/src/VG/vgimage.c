/* vgimage.c - Wind River VG Image Functionality */

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
14aug13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
20jun09,m_c  Written
*/

/*
DESCRIPTION
These routines provide imaging functionality that support the OpenVG
implementation.
*/

/* includes */

#include <stdlib.h>
#include <string.h>
#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/* forward declarations */

LOCAL void blitCopy32(const void*, int, int, surface_t*, int, int, int, int);
LOCAL void blitFill32(uint, surface_t*, int, int, int, int);
LOCAL void deleteImage(image_t*);
LOCAL void dupBorders32(image_t*);

/*******************************************************************************
 *
 *  blitCopy32 - blit-copy the specified data into a 32-bit surface
 *              (VG_sXRGB_8888 or VG_sARGB_8888_PRE)
 *
 */
LOCAL void blitCopy32
    (
    const void* pData,
    int dataStride,
    int dataFormat,
    surface_t* pSurface,
    int dx,
    int dy,
    int width,
    int height
    )
    {
    uint                            a;          /* alpha */
    uint                            color;      /* raw color */
    uchar                           mask;       /* BW pixel mask */
    int                             padding;    /* padding */
    uint*                           pDst;       /* destination */
    uint*                           pEnd;       /* end of line */
    uint                            r, g, b;    /* exploded color components */

    padding = (pSurface->stride / 4) - width;
    pDst = (void *)((long)pSurface->pBackBuf + (dx * 4) + (dy * pSurface->stride));
    pEnd = pDst + width;
    switch (dataFormat)
        {
        case VG_sRGBX_8888:
            while (height-- > 0)
                {
                const uint* pSrc = pData;
                while (pDst < pEnd)
                    {
                    color = *pSrc++;
                    *pDst++ = ((color >> 8) & 0x00ffffff) | 0xff000000;
                    }
                pData = (void *)((long)pData + dataStride);
                pDst += padding;
                pEnd += pSurface->stride / 4;
                }
            break;

        case VG_sBGRA_8888:
            while (height-- > 0)
                {
                const uint* pSrc = pData;
                while (pDst < pEnd)
                    {
                    color = *pSrc++;
                    a = (color & 255) / 255;
                    b = (color >> 24) & 255;
                    g = (color >> 16) & 255;
                    r = (color >> 8) & 255;
                    *pDst++ = (uint)(((color & 255U) << 24) + ((uint)(r * a) << 16) + ((uint)(g * a) << 8) + ((uint)(b * a)));

                    }
                pData = (void *)((long)pData + dataStride);
                pDst += padding;
                pEnd += pSurface->stride / 4;
                }
            break;

        case VG_sBGRA_8888_PRE:
            /* :TODO: Per specifications, clamp channels to [0, alpha]. Not done yet for obvious performance reasons */
            while (height-- > 0)
                {
                const uint* pSrc = pData;
                while (pDst < pEnd)
                    {
                    color = *pSrc++;
                    *pDst++ = ((color & 0x000000ff) << 24) | ((color & 0x0000ff00) << 8) |
                              ((color & 0x00ff0000) >> 8) | ((color & 0xff000000) >> 24);
                    }
                pData = (void *)((long)pData + dataStride);
                pDst += padding;
                pEnd += pSurface->stride / 4;
                }
            break;

        case VG_sRGBA_8888:
            while (height-- > 0)
                {
                const uint* pSrc = pData;
                while (pDst < pEnd)
                    {
                    color = *pSrc++;
                    a = (color & 255) / 255;
                    r = (color >> 24) & 255;
                    g = (color >> 16) & 255;
                    b = (color >> 8) & 255;
                    *pDst++ = (uint)(((color & 255U) << 24) + ((uint)(r * a) << 16) + ((uint)(g * a) << 8) + ((uint)(b * a)));

                    }
                pData = (void *)((long)pData + dataStride);
                pDst += padding;
                pEnd += pSurface->stride / 4;
                }
            break;

        case VG_sRGBA_8888_PRE:
            /* :TODO: Per specifications, clamp channels to [0, alpha]. Not done yet for obvious performance reasons */
            while (height-- > 0)
                {
                const uint* pSrc = pData;
                while (pDst < pEnd)
                    {
                    color = *pSrc++;
                    *pDst++ = ((color >> 8) & 0x00ffffff) | ((color << 24) & 0xff000000);
                    }
                pData = (void *)((long)pData + dataStride);
                pDst += padding;
                pEnd += pSurface->stride / 4;
                }
            break;

        case VG_BW_1:
            while (height-- > 0)
                {
                const uchar* pSrc = pData;
                mask = 0x80;
                while (pDst < pEnd)
                    {
                    color = *pSrc & mask, pSrc += mask & 0x01;
                    *pDst++ = ~(color - 1);
                    mask = (uchar)(((mask >> 1) + (mask << 7)) & 0xff);
                    }
                pData = (void *)((long)pData + dataStride);
                pDst += padding;
                pEnd += pSurface->stride / 4;
                }
            break;

        case VG_sXRGB_8888:
            while (height-- > 0)
                {
                const uint* pSrc = pData;
                while (pDst < pEnd)
                    {
                    color = *pSrc++;
                    *pDst++ = color | 0xff000000;
                    }
                pData = (void *)((long)pData + dataStride);
                pDst += padding;
                pEnd += pSurface->stride / 4;
                }
            break;

        case VG_sARGB_8888:
            while (height-- > 0)
                {
                const uint* pSrc = pData;
                while (pDst < pEnd)
                    {
                    color = *pSrc++;
                    a = ((color >> 24) & 255) / 255;
                    r = (color >> 16) & 255;
                    g = (color >> 8) & 255;
                    b = color & 255;
                    *pDst++ = (uint)((color & 0xff000000) + ((uint)(r * a) << 16) + ((uint)(g * a) << 8) + ((uint)(b * a)));
                    }
                pData = (void *)((long)pData + dataStride);
                pDst += padding;
                pEnd += pSurface->stride / 4;
                }
            break;

        case VG_sARGB_8888_PRE:
            /* :TODO: Per specifications, clamp channels to the range [0, alpha] */
            while (height-- > 0)
                {
                const uint* pSrc = pData;
                while (pDst < pEnd)
                    {
                    color = *pSrc++;
                    *pDst++ = color;
                    }
                pData = (void *)((long)pData + dataStride);
                pDst += padding;
                pEnd += pSurface->stride / 4;
                }
            break;

        case VG_sABGR_8888:
            while (height-- > 0)
                {
                const uint* pSrc = pData;
                while (pDst < pEnd)
                    {
                    color = *pSrc++;
                    a = ((color >> 24) & 255) / 255;
                    r = color & 255;
                    g = (color >> 8) & 255;
                    b = (color >> 16) & 255;
                    *pDst++ = (uint)((color & 0xff000000) + ((uint)(r * a) << 16) + ((uint)(g * a) << 8) + ((uint)(b * a)));

                    }
                pData = (void *)((long)pData + dataStride);
                pDst += padding;
                pEnd += pSurface->stride / 4;
                }
            break;
        }
    }

/*******************************************************************************
 *
 *  blitFill32 - blit-fill a portion of any 32-bit surface
 *
 */
LOCAL void blitFill32
    (
    uint color,
    surface_t* pSurface,
    int dx,
    int dy,
    int width,
    int height
    )
    {
    int                             padding;    /* padding */
    uint*                           pDst;       /* destination address */
    uint*                           pEnd;       /* end of line address */

    padding = (pSurface->stride / 4) - width;
    pDst = (void *)((long)pSurface->pBackBuf + (dx * 4) + (dy * pSurface->stride));
    pEnd = pDst + width;
    while (height-- > 0)
        {
        while (pDst < pEnd)
            *pDst++ = color;
        pDst += padding;
        pEnd += pSurface->stride / 4;
        }
    }



/*******************************************************************************
 *
 *  deleteImage - unlink and delete the specified image
 *
 */
LOCAL void deleteImage
    (
    image_t* pImage
    )
    {
    image_t*                        pParent;

    /* Unlink */
    LL_REMOVE(pImage->pGc->vg.pImages, pImage);

    /* Unbind from parent, if any, otherwise destroy the underlying surface */
    pParent = pImage->pParent;
    if (pParent == NULL)
        assert(eglDestroySurfaceWRS(pImage->pSurface->pDisplay, pImage->pSurface, EGL_TRUE) == EGL_TRUE);
    else
        {
        pParent->refCount--;
        if ((pParent->refCount == 0) && (pParent->deletePending))
            deleteImage(pParent);
        }

    /* Free allocated memory */
    free(pImage);
    }

/*******************************************************************************
 *
 *  dupBorders32 - duplicate the right and bottom borders of the specified image
 *
 */
LOCAL void dupBorders32
    (
    image_t* pImage
    )
    {
    uint*                           pBuf;   /* start address */
    uint*                           pEnd;   /* end address */
    int                             stride; /* underlying surface stride */

    /* Find the "original" parent */
    while (pImage->pParent != NULL)
        pImage = pImage->pParent;

    /* Get stride */
    stride = pImage->pSurface->stride / 4;

    /* Duplicate the right border */
    pBuf = (void *)((long)pImage->pSurface->pBackBuf + (pImage->width * 4));
    pEnd = pBuf + (pImage->height * stride);
    while (pBuf < pEnd)
        pBuf[0] = pBuf[-1], pBuf += stride;

    /* Duplicate the bottom border */
    pBuf -= pImage->width;
    pEnd++;
    while (pBuf < pEnd)
        pBuf[0] = pBuf[-stride], pBuf++;
    }



/*******************************************************************************
 *
 *  vgChildImage
 *
 */
VG_API_CALL VGImage VG_API_ENTRY vgChildImage
    (
    VGImage parent,
    VGint x,
    VGint y,
    VGint width,
    VGint height
    ) VG_API_EXIT
    {
    image_t*                        pImage = NULL;  /* image handle */
    image_t*                        pParent = (image_t*)parent;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pParent, pGc))
            VG_FAIL(VG_BAD_HANDLE_ERROR);

        if ((x < 0) || (x >= pParent->width) || (y < 0) || (y >= pParent->height))
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((width <= 0) || (height <= 0))
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if the image is a rendering target */
        if (pParent->inUse)
            VG_FAIL(VG_IMAGE_IN_USE_ERROR);

        /* Check if child image lies entirely within its parent */
        if (((x + width) > pParent->width) || ((y + height) > pParent->height))
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Flip Y */
        y = pParent->height - y - height;

        /* Allocate memory for the structure */
        pImage = malloc(sizeof(image_t));
        if (pImage == NULL)
            VG_FAIL(VG_OUT_OF_MEMORY_ERROR);

        /* Initialize the image */
        LL_ADD_HEAD(pGc->vg.pImages, pImage);
        pImage->pGc = pGc;
        pImage->refCount = 0;
        pImage->deletePending = FALSE;
        pImage->inUse = FALSE;
        pImage->pParent = pParent;
        pImage->format = pParent->format;
        pImage->bpp = pParent->bpp;
        pImage->x0 = pParent->x0 + x;
        pImage->y0 = pParent->y0 + y;
        pImage->width = width;
        pImage->height = height;
        pImage->pSurface = pParent->pSurface;

        /* Bind to parent */
        pParent->refCount++;
        }

zz: return ((VGImage)pImage);
    }

/*******************************************************************************
 *
 *  vgClearImage
 *
 */
VG_API_CALL void VG_API_ENTRY vgClearImage
    (
    VGImage image,
    VGint x,
    VGint y,
    VGint width,
    VGint height
    ) VG_API_EXIT
    {
    VGuint                          color;  /* clear color */
    image_t*                        pImage = (image_t*)image;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((width <= 0) || (height <= 0))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if the image is a rendering target */
        if (pImage->inUse)
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Flip Y */
        y = pImage->height - y - height;

        /* Clip (source) */
        /* n/a */

        /* Clear pixels */
        if (1)
            {
            do
                {
                /* Clip (destination) */
                if ((x >= pImage->width) || (y >= pImage->height))
                    return;
                if (x < 0)
                    width += x, x = 0;
                if (y < 0)
                    height += y, y = 0;
                if ((width <= 0) || (height <= 0))
                    return;
                if ((x + width) > pImage->width)
                    width = pImage->width - x;
                if ((y + height) > pImage->height)
                    height = pImage->height - y;

                /* Convert to absolute coordinates */
                x += pImage->x0;
                y += pImage->y0;

                /* Fill */
                color = vgConvertColorWRS(pGc->vg.clearColor, pImage->format);
                switch (pImage->bpp)
                    {
                    case 32:
                        blitFill32(color, pImage->pSurface, x, y, width, height);
                        dupBorders32(pImage);
                        break;
                    }
                } while (0);
            }
        }
    }



/*******************************************************************************
 *
 *  vgCopyImage
 *
 */
VG_API_CALL void VG_API_ENTRY vgCopyImage
    (
    VGImage dstImage,
    VGint dx,
    VGint dy,
    VGImage srcImage,
    VGint sx,
    VGint sy,
    VGint width,
    VGint height,
    VGboolean dither
    ) VG_API_EXIT
    {
    VGint                           bpp;        /* bit depth */
    int                             overlap;    /* set to TRUE if source and destination overlap */
    int                             padding;    /* padding */
    const void*                     pBuf;       /* source buffer */
    int                             stride;     /* stride */
    image_t*                        pDstImage = (image_t*)dstImage;
    image_t*                        pSrcImage = (image_t*)srcImage;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pDstImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidImage(pSrcImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((width <= 0) || (height <= 0))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if either image is a rendering target */
        if ((pDstImage->inUse) || (pSrcImage->inUse))
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Flip Y */
        dy = pDstImage->height - dy - height;
        sy = pSrcImage->height - sy - height;

        /* Clip (source) */
        if ((sx >= pSrcImage->width) || (sy >= pSrcImage->height))
            return;
        if (sx < 0)
            width += sx, dx -= sx, sx = 0;
        if (sy < 0)
            height += sy, dy -= sy, sy = 0;
        if ((width <= 0) || (height <= 0))
            return;
        if ((sx + width) > pSrcImage->width)
            width = pSrcImage->width - sx;
        if ((sy + height) > pSrcImage->height)
            height = pSrcImage->height - sy;

        /* Convert to absolute coordinates */
        sx += pSrcImage->x0;
        sy += pSrcImage->y0;

        /* Copy pixels */
        bpp = vgGetBitDepthWRS(pSrcImage->format);
        stride = pSrcImage->pSurface->stride;
        if (1)
            {
            do
                {
                /* Clip (destination) */
                if ((dx >= pDstImage->width) || (dy >= pDstImage->height))
                    break;
                if (dx < 0)
                    width += dx, sx -= dx, dx = 0;
                if (dy < 0)
                    height += dy, sy -= dy, dy = 0;
                if ((width <= 0) || (height <= 0))
                    break;
                if ((dx + width) > pDstImage->width)
                    width = pDstImage->width - dx;
                if ((dy + height) > pDstImage->height)
                    height = pDstImage->height - dy;

                /* Convert to absolute coordinates */
                dx += pDstImage->x0;
                dy += pDstImage->y0;

                /* If the images share the same surface, the copy needs to handle overlapping */
                if (pDstImage->pSurface == pSrcImage->pSurface)
                    {
                    /* Check if the source and destination overlap */
                    overlap = FALSE;
                    if ((sx < (dx + width)) && ((sx + width) > dx) && (sy < (dy + height)) && ((sy + height) > dy))
                        {
                        /* When regions overlap, there's no need to do a backward blit if the overlapping is friendly with
                         * regards to memory organization
                         */
                        if (sy < dy)
                            overlap = TRUE;
                        else if (sy == dy)
                            overlap = (sx < dx);
                        }

                    /* Copy */
                    switch (bpp)
                        {
                        case 32:
                            padding = (stride /= 4) - width;
                            if (overlap)
                                {
                                uint* pDst = (uint *)((long)pDstImage->pSurface->pBackBuf + ((dx + ((dy + height) * stride)) * 4));
                                uint* pEnd = pDst;
                                uint* pSrc = (uint *)((long)pSrcImage->pSurface->pBackBuf + ((sx + ((sy + height) * stride)) * 4));
                                while (height-- > 0)
                                    {
                                    pDst -= padding;
                                    pEnd -= stride;
                                    pSrc -= padding;
                                    while (pDst > pEnd)
                                        *--pDst = *--pSrc;
                                    }
                                }
                            else
                                {
                                uint* pDst = (uint *)((long)pDstImage->pSurface->pBackBuf + ((dx + (dy * stride)) * 4));
                                uint* pEnd = pDst + width;
                                uint* pSrc = (uint *)((long)pSrcImage->pSurface->pBackBuf + ((sx + (sy * stride)) * 4));
                                while (height-- > 0)
                                    {
                                    while (pDst < pEnd)
                                        *pDst++ = *pSrc++;
                                    pDst += padding;
                                    pEnd += stride;
                                    pSrc += padding;
                                    }
                                }
                            break;
                        }
                    }
                else
                    {
                    /* Blit */
                    pBuf = (void *)((long)pSrcImage->pSurface->pBackBuf + (sx * bpp / 8) + (sy * stride));
                    switch (pDstImage->format)
                    switch (pDstImage->format)
                        {
                        case VG_sXRGB_8888:
                        case VG_sARGB_8888_PRE:
                            blitCopy32(pBuf, stride, pSrcImage->format, pDstImage->pSurface, dx, dy, width, height);
                            break;
                        }
                    }
                } while (0);
            }


        }
    }

/*******************************************************************************
 *
 *  vgCopyPixels
 *
 */
VG_API_CALL void VG_API_ENTRY vgCopyPixels
    (
    VGint dx,
    VGint dy,
    VGint sx,
    VGint sy,
    VGint width,
    VGint height
    ) VG_API_EXIT
    {
    VGint                           bpp;            /* surface bit depth */
    VGImageFormat                   format;         /* surface format */
    int                             overlap;        /* set to TRUE if source and destination overlap */
    int                             padding;        /* padding */
    rectangle_t*                    pClipRect;      /* current clipping rectangle */
    surface_t*                      pDrawSurface;   /* EGL draw surface */
    int                             stride;         /* surface stride */
    VGint                           tmp[6];

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if ((width <= 0) || (height <=0))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Get the draw surface, its format, bit depth and stride */
        pDrawSurface = pGc->vg.pDrawSurface;
        if (pDrawSurface == NULL)
            VG_FAIL_VOID(VG_NO_CONTEXT_ERROR);
        format = vgGetSurfaceFormatWRS(pDrawSurface);
        bpp = vgGetBitDepthWRS(format);
        stride = pDrawSurface->stride;

        /* Flip Y */
        dy = pDrawSurface->height - dy - height;
        sy = pDrawSurface->height - sy - height;

        /* Clip (source) */
        if ((sx >= pDrawSurface->width) || (sy >= pDrawSurface->height))
            return;
        if (sx < 0)
            width += sx, dx -= sx, sx = 0;
        if (sy < 0)
            height += sy, dy -= sy, sy = 0;
        if ((width <= 0) || (height <= 0))
            return;
        if ((sx + width) > pDrawSurface->width)
            width = pDrawSurface->width - sx;
        if ((sy + height) > pDrawSurface->height)
            height = pDrawSurface->height - sy;

        /* Copy */
        pClipRect = (pGc->vg.enableScissoring) ? (pGc->vg.pScissorRects) : (&pDrawSurface->rect);
        while (pClipRect != NULL)
            {
            tmp[0] = dx, tmp[1] = dy, tmp[2] = sx, tmp[3] = sy, tmp[4] = width, tmp[5] = height;

            do
                {
                /* Clip (destination) */
                if ((dx >= (VGint)pClipRect->x1) || (dy >= (VGint)pClipRect->y1))
                    break;
                if (dx < (VGint)pClipRect->x0)
                    width -= ((VGint)pClipRect->x0 - dx), sx += ((VGint)pClipRect->x0 - dx), dx = (VGint)pClipRect->x0;
                if (dy < (VGint)pClipRect->y0)
                    height -= ((VGint)pClipRect->y0 - dy), sy += ((VGint)pClipRect->y0 - dy), dy = (VGint)pClipRect->y0;
                if ((width <= 0) || (height <= 0))
                    break;
                if ((dx + width) > (VGint)pClipRect->x1)
                    width = (VGint)pClipRect->x1 - dx;
                if ((dy + height) > (VGint)pClipRect->y1)
                    height = (VGint)pClipRect->y1 - dy;

                /* Check if the source and destination overlap */
                overlap = FALSE;
                if ((sx < (dx + width)) && ((sx + width) > dx) && (sy < (dy + height)) && ((sy + height) > dy))
                    {
                    /* When regions overlap, there's no need to do a backward blit if the overlapping is friendly with
                     * regards to memory organization
                     */
                    if (sy < dy)
                        overlap = TRUE;
                    else if (sy == dy)
                        overlap = (sx < dx);
                    }

                /* Copy */
                switch (bpp)
                    {
                    case 32:
                        padding = (stride /= 4) - width;
                        if (overlap)
                            {
                            uint* pDst = (uint *)((long)pDrawSurface->pBackBuf + ((dx + ((dy + height) * stride)) * 4));
                            uint* pEnd = pDst;
                            uint* pSrc = (uint *)((long)pDrawSurface->pBackBuf + ((sx + ((sy + height) * stride)) * 4));
                            while (height-- > 0)
                                {
                                pDst -= padding;
                                pEnd -= stride;
                                pSrc -= padding;
                                while (pDst > pEnd)
                                    *--pDst = *--pSrc;
                                }
                            }
                        else
                            {
                            uint* pDst = (uint *)((long)pDrawSurface->pBackBuf + ((dx + (dy * stride)) * 4));
                            uint* pEnd = pDst + width;
                            uint* pSrc = (uint *)((long)pDrawSurface->pBackBuf + ((sx + (sy * stride)) * 4));
                            while (height-- > 0)
                                {
                                while (pDst < pEnd)
                                    *pDst++ = *pSrc++;
                                pDst += padding;
                                pEnd += stride;
                                pSrc += padding;
                                }
                            }
                        break;
                    }
                } while (0);

            dx = tmp[0], dy = tmp[1], sx = tmp[2], sy = tmp[3], width = tmp[4], height= tmp[5];

            pClipRect = pClipRect->pNext;
            }
        }
    }



/*******************************************************************************
 *
 *  vgCreateImage
 *
 */
VG_API_CALL VGImage VG_API_ENTRY vgCreateImage
    (
    VGImageFormat format,
    VGint width,
    VGint height,
    VGbitfield allowedQuality
    ) VG_API_EXIT
    {
    EGLint                          attribs[13];    /* EGL attributes */
    VGint                           bpp;            /* bit depth */
    EGLConfig                       config;         /* EGL configuration */
    surface_t*                      pDrawSurface;   /* EGL draw surface */
    image_t*                        pImage = NULL;  /* image handle */
    surface_t*                      pSurface;       /* underlying EGL surface */
    long long                       size;           /* image size */
    EGLint                          n;              /* number of compatible EGL configurations */

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        bpp = vgGetBitDepthWRS(format);
        if (bpp < 0)
            VG_FAIL(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);

        if ((width <= 0) || (width > MAX_IMAGE_WIDTH) || (height <= 0) || (height > MAX_IMAGE_HEIGHT))
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        if ((allowedQuality & ~VG_IMAGE_QUALITY_MASK) != 0)
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Determine the size of the image */
        size = (width + 1) * (height + 1);
        if ((size > MAX_IMAGE_PIXELS) || ((size * bpp / 8) > MAX_IMAGE_BYTES))
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Always allow non-antialiased quality */
        allowedQuality |= VG_IMAGE_QUALITY_NONANTIALIASED;

        /* Find the EGL configuration matching the specified format */
        pDrawSurface = pGc->vg.pDrawSurface;
        if (pDrawSurface == NULL)
            VG_FAIL(VG_NO_CONTEXT_ERROR);
        attribs[0] = EGL_BUFFER_SIZE;
        attribs[2] = EGL_ALPHA_SIZE;
        attribs[4] = EGL_RED_SIZE;
        attribs[6] = EGL_GREEN_SIZE;
        attribs[8] = EGL_BLUE_SIZE;
        switch (format)
            {
            case VG_sXRGB_8888:
                attribs[1] = 32;
                attribs[3] = 0;
                attribs[5] = 8;
                attribs[7] = 8;
                attribs[9] = 8;
                n = 10;
                break;

            case VG_sARGB_8888_PRE:
                attribs[1] = 32;
                attribs[3] = 8;
                attribs[5] = 8;
                attribs[7] = 8;
                attribs[9] = 8;
                n = 10;
                break;

            default:
                /* :TODO: Add support for more formats */
                attribs[1] = MAX_INT;
                n = 2;
                break;
            }
        attribs[n] = EGL_NONE;
        assert(eglChooseConfig(pDrawSurface->pDisplay, attribs, &config, 1, &n) == EGL_TRUE);
        if (n != 1)
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Allocate memory for the image structure */
        pImage = malloc(sizeof(image_t));
        if (pImage == NULL)
            VG_FAIL(VG_OUT_OF_MEMORY_ERROR);

        /* Create the underlying pbuffer */
        attribs[0] = EGL_WIDTH;
        attribs[1] = width + 1;
        attribs[2] = EGL_HEIGHT;
        attribs[3] = height + 1;
        attribs[4] = EGL_VG_ALPHA_FORMAT;
        attribs[5] = EGL_VG_ALPHA_FORMAT_PRE;
        attribs[6] = EGL_NONE;
        pSurface = eglCreatePbufferSurfaceWRS(pDrawSurface->pDisplay, config, attribs, EGL_TRUE);
        if (pSurface == NULL)
            {
            free(pImage);
            pImage = NULL;
            VG_FAIL(VG_OUT_OF_MEMORY_ERROR);
            }
        memset(pSurface->pBackBuf, 0, pSurface->stride * pSurface->height);

        /* Initialize to default values */
        LL_ADD_HEAD(pGc->vg.pImages, pImage);
        pImage->pGc = pGc;
        pImage->pParent = NULL;
        pImage->refCount = 0;
        pImage->deletePending = FALSE;
        pImage->inUse = FALSE;
        pImage->format = format;
        pImage->bpp = bpp;
        pImage->x0 = pImage->y0 = 0;
        pImage->width = width;
        pImage->height = height;
        pImage->pSurface = pSurface;
        }

zz: return ((VGImage)pImage);
    }

/*******************************************************************************
 *
 *  vgDestroyImage
 *
 */
VG_API_CALL void VG_API_ENTRY vgDestroyImage
    (
    VGImage image
    ) VG_API_EXIT
    {
    image_t*                        pImage = (image_t*)image;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Delete the image */
        if (pImage->refCount > 0)
            pImage->deletePending = TRUE;
        else
            deleteImage(pImage);
        }
    }

/*******************************************************************************
 *
 *  vgGetImageSubData
 *
 */
VG_API_CALL void VG_API_ENTRY vgGetImageSubData
    (
    VGImage image,
    void* pData,
    VGint dataStride,
    VGImageFormat dataFormat,
    VGint sx,
    VGint sy,
    VGint width,
    VGint height
    ) VG_API_EXIT
    {
    VGint                           bpp;    /* bit depth */
    VGint                           dx, dy; /* destination coordinates */
    surface_t                       memory; /* surface */
    const void*                     pBuf;   /* source buffer */
    int                             stride; /* stride */
    image_t*                        pImage = (image_t*)image;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidDataPtr(pData, dataFormat))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (vgGetBitDepthWRS(dataFormat) < 0)
            VG_FAIL_VOID(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);

        if ((width <= 0) || (height <= 0))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if the image is a rendering target */
        if (pImage->inUse)
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Set destination coordinates */
        dx = dy = 0;

        /* Create a dummy destination surface */
        memory.pBackBuf = pData;
        memory.width = width, memory.stride = -dataStride;
        memory.height = height;

        /* Flip Y */
        dy -= height - 1;
        sy = pImage->height - sy - height;

        /* Clip (source) */
        if ((sx >= pImage->width) || (sy >= pImage->height))
            return;
        if (sx < 0)
            width += sx, dx -= sx, sx = 0;
        if (sy < 0)
            height += sy, dy -= sy, sy = 0;
        if ((width <= 0) || (height <= 0))
            return;
        if ((sx + width) > pImage->width)
            width = pImage->width - sx;
        if ((sy + height) > pImage->height)
            height = pImage->height - sy;

        /* Convert to absolute coordinates */
        sx += pImage->x0;
        sy += pImage->y0;

        /* Read pixels */
        bpp = vgGetBitDepthWRS(pImage->format);
        stride = pImage->pSurface->stride;
        if (1)
            {
            do
                {
                /* Clip (destination) */
                /* n/a */

                /* Blit */
                pBuf = (void *)((long)pImage->pSurface->pBackBuf + (sx * bpp / 8) + (sy * stride));
                switch (dataFormat)
                    {
                    case VG_sXRGB_8888:
                    case VG_sARGB_8888_PRE:
                        blitCopy32(pBuf, stride, pImage->format, &memory, dx, dy, width, height);
                        break;
                    default:
                        break;
                    }
                } while (0);
            }
        }
    }

/*******************************************************************************
 *
 *  vgGetParent
 *
 */
VG_API_CALL VGImage VG_API_ENTRY vgGetParent
    (
    VGImage image
    ) VG_API_EXIT
    {
    image_t*                        pParent = NULL; /* parent image */
    image_t*                        pImage = (image_t*)image;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pImage, pGc))
            VG_FAIL(VG_BAD_HANDLE_ERROR);

        /* Check if the image is a rendering target */
        if (pImage->inUse)
            VG_FAIL(VG_IMAGE_IN_USE_ERROR);

        /* Find the closest valid ancestor */
        pParent = pImage->pParent;
        while (pParent != NULL)
            {
            if (!pParent->deletePending)
                goto zz;
            pParent = pParent->pParent;
            }
        pParent = pImage;
        }

zz: return ((VGImage)pParent);
    }

/*******************************************************************************
 *
 *  vgGetPixels
 *
 */
VG_API_CALL void VG_API_ENTRY vgGetPixels
    (
    VGImage image,
    VGint dx,
    VGint dy,
    VGint sx,
    VGint sy,
    VGint width,
    VGint height
    ) VG_API_EXIT
    {
    VGint                           bpp;            /* bit depth */
    VGImageFormat                   format;         /* format */
    const void*                     pBuf;           /* source buffer */
    surface_t*                      pDrawSurface;   /* EGL draw surface */
    int                             stride;         /* stride */
    image_t*                        pImage = (image_t*)image;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((width <= 0) || (height <= 0))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if the image is a rendering target */
        if (pImage->inUse)
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Get the source surface and its format */
        pDrawSurface = pGc->vg.pDrawSurface;
        if (pDrawSurface == NULL)
            VG_FAIL_VOID(VG_NO_CONTEXT_ERROR);
        format = vgGetSurfaceFormatWRS(pDrawSurface);

        /* Flip Y */
        dy = pImage->height - dy - height;
        sy = pDrawSurface->height - sy - height;

        /* Clip (source) */
        if ((sx >= pDrawSurface->width) || (sy >= pDrawSurface->height))
            return;
        if (sx < 0)
            width += sx, dx -= sx, sx = 0;
        if (sy < 0)
            height += sy, dy -= sy, sy = 0;
        if ((width <= 0) || (height <= 0))
            return;
        if ((sx + width) > pDrawSurface->width)
            width = pDrawSurface->width - sx;
        if ((sy + height) > pDrawSurface->height)
            height = pDrawSurface->height - sy;

        /* Retrieve pixels */
        bpp = vgGetBitDepthWRS(format);
        stride = pDrawSurface->stride;
        if (1)
            {
            do
                {
                /* Clip (destination) */
                if ((dx >= pImage->width) || (dy >= pImage->height))
                    break;
                if (dx < 0)
                    width += dx, sx -= dx, dx = 0;
                if (dy < 0)
                    height += dy, sy -= dy, dy = 0;
                if ((width <= 0) || (height <= 0))
                    break;
                if ((dx + width) > pImage->width)
                    width = pImage->width - dx;
                if ((dy + height) > pImage->height)
                    height = pImage->height - dy;

                /* Convert to absolute coordinates */
                dx += pImage->x0;
                dy += pImage->y0;

                /* Blit */
                pBuf = (void *)((long)pDrawSurface->pFrontBuf + (sx * bpp / 8) + (sy * stride));
                switch (pImage->format)
                    {
                    case VG_sXRGB_8888:
                    case VG_sARGB_8888_PRE:
                        blitCopy32(pBuf, stride, format, pImage->pSurface, dx, dy, width, height);
                        dupBorders32(pImage);
                        break;
                    }
                } while (0);
            }
        }
    }

/*******************************************************************************
 *
 *  vgImageSubData
 *
 */
VG_API_CALL void VG_API_ENTRY vgImageSubData
    (
    VGImage image,
    const void* pData,
    VGint dataStride,
    VGImageFormat dataFormat,
    VGint dx,
    VGint dy,
    VGint width,
    VGint height
    ) VG_API_EXIT
    {
    VGint                           bpp;    /* bit depth */
    const void*                     pBuf;   /* source buffer */
    VGint                           sx, sy; /* source coordinates */
    image_t*                        pImage = (image_t*)image;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (isInvalidDataPtr(pData, dataFormat))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (vgGetBitDepthWRS(dataFormat) < 0)
            VG_FAIL_VOID(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);

        if ((width <= 0) || (height <= 0))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if the image is a rendering target */
        if (pImage->inUse)
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Set source coordinates */
        sx = sy = 0;

        /* Flip Y */
        dy = pImage->height - dy - height;

        /* Clip (source) */
        /* n/a */

        /* Read pixels */
        bpp = vgGetBitDepthWRS(dataFormat);
        if (1)
            {
            do
                {
                /* Clip (destination) */
                if ((dx > pImage->width) || (dy > pImage->height))
                    return;
                if (dx < 0)
                    width += dx, sx -= dx, dx = 0;
                if (dy < 0)
                    height += dy, sy -= dy, dy = 0;
                if ((width <= 0) || (height <= 0))
                    return;
                if ((dx + width) > pImage->width)
                    width = pImage->width - dx;
                if ((dy + height) > pImage->height)
                    height = pImage->height - dy;

                /* Convert to absolute coordinates */
                dx += pImage->x0;
                dy += pImage->y0;

                /* Blit */
                pBuf = (void *)((long)pData + (sx * bpp / 8) + (sy * dataStride));
                switch (pImage->format)
                    {
                    case VG_sXRGB_8888:
                    case VG_sARGB_8888_PRE:
                        blitCopy32(pBuf, dataStride, dataFormat, pImage->pSurface, dx, dy, width, height);
                        dupBorders32(pImage);
                        break;
                    }
                } while (0);
            }
        }
    }

/*******************************************************************************
 *
 *  vgReadPixels
 *
 */
VG_API_CALL void VG_API_ENTRY vgReadPixels
    (
    void* pData,
    VGint dataStride,
    VGImageFormat dataFormat,
    VGint sx,
    VGint sy,
    VGint width,
    VGint height
    ) VG_API_EXIT
    {
    VGint                           bpp;            /* bit depth */
    VGint                           dx, dy;         /* destination coordinates */
    VGImageFormat                   format;         /* format */
    surface_t                       memory;         /* surface */
    const void*                     pBuf;           /* source buffer */
    surface_t*                      pDrawSurface;   /* EGL draw surface */
    int                             stride;         /* stride */

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidDataPtr(pData, dataFormat))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (vgGetBitDepthWRS(dataFormat) < 0)
            VG_FAIL_VOID(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);

        if ((width <= 0) || (height <= 0))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Set destination coordinates */
        dx = dy = 0;

        /* Get the source surface and its format */
        pDrawSurface = pGc->vg.pDrawSurface;
        if (pDrawSurface == NULL)
            VG_FAIL_VOID(VG_NO_CONTEXT_ERROR);
        format = vgGetSurfaceFormatWRS(pDrawSurface);

        /* Create a dummy destination surface */
        memory.pBackBuf = pData;
        memory.width = width, memory.stride = -dataStride;
        memory.height = height;

        /* Flip Y */
        dy -= height - 1;
        sy = pDrawSurface->height - sy - height;

        /* Clip (source) */
        if ((sx >= pDrawSurface->width) || (sy >= pDrawSurface->height))
            return;
        if (sx < 0)
            width += sx, dx -= sx, sx = 0;
        if (sy < 0)
            height += sy, dy -= sy, sy = 0;
        if ((width <= 0) || (height <= 0))
            return;
        if ((sx + width) > pDrawSurface->width)
            width = pDrawSurface->width - sx;
        if ((sy + height) > pDrawSurface->height)
            height = pDrawSurface->height - sy;

        /* Copy pixels */
        bpp = vgGetBitDepthWRS(format);
        stride = pDrawSurface->stride;
        if (1)
            {
            do
                {
                /* Clip (destination) */
                /* n/a */

                /* Blit */
                pBuf = (void *)((long)pDrawSurface->pFrontBuf + (sx * bpp / 8) + (sy * stride));
                switch (dataFormat)
                    {
                    case VG_sXRGB_8888:
                    case VG_sARGB_8888_PRE:
                        blitCopy32(pBuf, stride, format, &memory, dx, dy, width, height);
                        break;
                    default:
                        break;
                    }
                } while (0);
            }
        }
    }

/*******************************************************************************
 *
 *  vgSetPixels
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetPixels
    (
    VGint dx,
    VGint dy,
    VGImage image,
    VGint sx,
    VGint sy,
    VGint width,
    VGint height
    ) VG_API_EXIT
    {
    VGint                           bpp;            /* bit depth */
    VGImageFormat                   format;         /* format */
    const void*                     pBuf;           /* source buffer */
    rectangle_t*                    pClipRect;      /* clipping rectangle */
    surface_t*                      pDrawSurface;   /* EGL draw surface */
    int                             stride;         /* stride */
    VGint                           tmp[6];
    image_t*                        pImage = (image_t*)image;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidImage(pImage, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((width <= 0) || (height <= 0))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Check if the image is a rendering target */
        if (pImage->inUse)
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

        /* Get the destination surface and its format */
        pDrawSurface = pGc->vg.pDrawSurface;
        if (pDrawSurface == NULL)
            VG_FAIL_VOID(VG_NO_CONTEXT_ERROR);
        format = vgGetSurfaceFormatWRS(pDrawSurface);

        /* Flip Y */
        dy = pDrawSurface->height - dy - height;
        sy = pImage->height - sy - height;

        /* Clip (source) */
        if ((sx >= pImage->width) || (sy >= pImage->height))
            return;
        if (sx < 0)
            width += sx, dx -= sx, sx = 0;
        if (sy < 0)
            height += sy, dy -= sy, sy = 0;
        if ((width <= 0) || (height <= 0))
            return;
        if ((sx + width) > pImage->width)
            width = pImage->width - sx;
        if ((sy + height) > pImage->height)
            height = pImage->height - sy;

        /* Convert to absolute coordinates */
        sx += pImage->x0;
        sy += pImage->y0;

        /* Copy pixels */
        bpp = pImage->bpp;
        pClipRect = (pGc->vg.enableScissoring) ? (pGc->vg.pScissorRects) : (&pDrawSurface->rect);
        stride = pImage->pSurface->stride;
        while (pClipRect != NULL)
            {
            tmp[0] = dx, tmp[1] = dy, tmp[2] = width, tmp[3] = height, tmp[4] = sx, tmp[5] = sy;

            do
                {
                /* Clip (destination) */
                if ((dx >= (VGint)pClipRect->x1) || (dy >= (VGint)pClipRect->y1))
                    break;
                if (dx < (VGint)pClipRect->x0)
                    width -= ((VGint)pClipRect->x0 - dx), sx += ((VGint)pClipRect->x0 - dx), dx = (VGint)pClipRect->x0;
                if (dy < (VGint)pClipRect->y0)
                    height -= ((VGint)pClipRect->y0 - dy), sy += ((VGint)pClipRect->y0 - dy), dy = (VGint)pClipRect->y0;
                if ((width <= 0) || (height <= 0))
                    break;
                if ((dx + width) > (VGint)pClipRect->x1)
                    width = (VGint)pClipRect->x1 - dx;
                if ((dy + height) > (VGint)pClipRect->y1)
                    height = (VGint)pClipRect->y1 - dy;

                /* Blit */
                pBuf = (void *)((long)pImage->pSurface->pBackBuf + (sx * bpp / 8) + (sy * stride));
                switch (format)
                    {
                    case VG_sXRGB_8888:
                    case VG_sARGB_8888_PRE:
                        blitCopy32(pBuf, stride, pImage->format, pDrawSurface, dx, dy, width, height);
                        break;
                    default:
                        break;
                    }
                } while (0);

            dx = tmp[0], dy = tmp[1], width = tmp[2], height = tmp[3], sx = tmp[4], sy = tmp[5];

            pClipRect = pClipRect->pNext;
            }
        }
    }

/*******************************************************************************
 *
 *  vgWritePixels
 *
 */
VG_API_CALL void VG_API_ENTRY vgWritePixels
    (
    const void* pData,
    VGint dataStride,
    VGImageFormat dataFormat,
    VGint dx,
    VGint dy,
    VGint width,
    VGint height
    ) VG_API_EXIT
    {
    VGint                           bpp;            /* bit depth */
    VGImageFormat                   format;         /* format */
    const void*                     pBuf;           /* buffer  */
    rectangle_t*                    pClipRect;      /* clipping rectangle */
    surface_t*                      pDrawSurface;   /* EGL draw surface */
    VGint                           sx, sy;         /* source coordinates */
    VGint                           tmp[6];

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidDataPtr(pData, dataFormat))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (vgGetBitDepthWRS(dataFormat) < 0)
            VG_FAIL_VOID(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);

        if ((width <= 0) || (height <= 0))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Set source coordinates */
        sx = sy = 0;

        /* Get the destination surface and its format */
        pDrawSurface = pGc->vg.pDrawSurface;
        if (pDrawSurface == NULL)
            VG_FAIL_VOID(VG_NO_CONTEXT_ERROR);
        format = vgGetSurfaceFormatWRS(pDrawSurface);

        /* Flip Y */
        dy = pDrawSurface->height - dy - height;

        /* Clip (source) */
        /* n/a */

        /* Write pixels */
        bpp = vgGetBitDepthWRS(dataFormat);
        pClipRect = (pGc->vg.enableScissoring) ? (pGc->vg.pScissorRects) : (&pDrawSurface->rect);
        while (pClipRect != NULL)
            {
            tmp[0] = dx, tmp[1] = dy, tmp[2] = width, tmp[3] = height, tmp[4] = sx, tmp[5] = sy;

            do
                {
                /* Clip (destination) */
                if ((dx >= (VGint)pClipRect->x1) || (dy >= (VGint)pClipRect->y1))
                    break;
                if (dx < (VGint)pClipRect->x0)
                    width -= ((VGint)pClipRect->x0 - dx), sx += ((VGint)pClipRect->x0 - dx), dx = (VGint)pClipRect->x0;
                if (dy < (VGint)pClipRect->y0)
                    height -= ((VGint)pClipRect->y0 - dy), sy += ((VGint)pClipRect->y0 - dy), dy = (VGint)pClipRect->y0;
                if ((width <= 0) || (height <= 0))
                    break;
                if ((dx + width) > (VGint)pClipRect->x1)
                    width = (VGint)pClipRect->x1 - dx;
                if ((dy + height) > (VGint)pClipRect->y1)
                    height = (VGint)pClipRect->y1 - dy;

                /* Blit */
                pBuf = (void *)((long)pData + (sx * bpp / 8) + (sy * dataStride));
                switch (format)
                    {
                    case VG_sXRGB_8888:
                    case VG_sARGB_8888_PRE:
                        blitCopy32(pBuf, dataStride, dataFormat, pDrawSurface, dx, dy, width, height);
                        break;
                    default:
                        break;
                    }
                } while (0);

            dx = tmp[0], dy = tmp[1], width = tmp[2], height = tmp[3], sx = tmp[4], sy = tmp[5];

            pClipRect = pClipRect->pNext;
            }
        }
    }
