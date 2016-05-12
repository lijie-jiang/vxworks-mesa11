/* vgext.c - Wind River VG Extension Functionality */

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
06jun09,m_c  Written
*/

/*
DESCRIPTION
These routines provide Wind River specific extension functionality that support
the OpenVG implementation.
*/

/* includes */

#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/*******************************************************************************
 *
 * vgConvertColorWRS - convert an sRGBA color into the specified format
 *
 * RETURNS: A native color value
 *
 * ERRORS: VG_ILLEGAL_ARGUMENT_ERROR VG_UNSUPPORTED_IMAGE_FORMAT_ERROR
 *
 */
VG_API_CALL VGuint VG_API_ENTRY vgConvertColorWRS
    (
    const VGfloat* sRGBA,
    VGImageFormat format
    ) VG_API_EXIT
    {
    float   a, l;               /* alpha channel and luminance */
    float   color[4];           /* normalized color */
    VGuint  pixel = 0x00000000; /* pixel value */
    int     r, g, b;            /* color channels */

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPtr(sRGBA, sizeof(VGfloat)))
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Clamp channels */
        a = clamp1(sRGBA[3]);
        color[VG_R] = clamp1(sRGBA[0]);
        color[VG_G] = clamp1(sRGBA[1]);
        color[VG_B] = clamp1(sRGBA[2]);

        /* Calculate luminance (for VG_sL_8, VG_lL_8 and VG_BW_1) */
        l = (0.2126f * invgamma(color[VG_R])) + (0.7152f * invgamma(color[VG_G])) + (0.0722f * invgamma(color[VG_B]));

        /* Convert color */
        switch (format)
            {
            case VG_sRGBX_8888:
                r = iround(color[VG_R] * 255);
                g = iround(color[VG_G] * 255);
                b = iround(color[VG_B] * 255);
                /**/
                pixel = (r << 24) + (g << 16) + (b << 8) + 255;
                break;

            case VG_sRGBA_8888:
                a *= 255;
                r = iround(color[VG_R] * 255);
                g = iround(color[VG_G] * 255);
                b = iround(color[VG_B] * 255);
                /**/
                pixel = (r << 24) + (g << 16) + (b << 8) + iround(a);
                break;

            case VG_sRGBA_8888_PRE:
                a *= 255;
                r = iround(color[VG_R] * a);
                g = iround(color[VG_G] * a);
                b = iround(color[VG_B] * a);
                /**/
                pixel = (r << 24) + (g << 16) + (b << 8) + iround(a);
                break;

            case VG_sRGB_565:
                r = iround(color[VG_R] * 31);
                g = iround(color[VG_G] * 63);
                b = iround(color[VG_B] * 31);
                /**/
                pixel = (r << 11) + (g << 5) + b;
                break;

            case VG_sRGBA_5551:
                r = iround(color[VG_R] * 31);
                g = iround(color[VG_G] * 31);
                b = iround(color[VG_B] * 31);
                /**/
                pixel = (r << 11) + (g << 6) + (b << 1) + iround(a);
                break;

            case VG_sRGBA_4444:
                a *= 15;
                r = iround(color[VG_R] * 15);
                g = iround(color[VG_G] * 15);
                b = iround(color[VG_B] * 15);
                /**/
                pixel = (r << 12) + (g << 8) + (b << 4) + iround(a);
                break;

            case VG_sL_8:
                l = gamma(l) * 255;
                /**/
                pixel = iround(l);
                break;

            case VG_lRGBX_8888:
                r = iround(invgamma(color[VG_R]) * 255);
                g = iround(invgamma(color[VG_G]) * 255);
                b = iround(invgamma(color[VG_B]) * 255);
                /**/
                pixel = (r << 24) + (g << 16) + (b << 8) + 255;
                break;

            case VG_lRGBA_8888:
                a *= 255;
                r = iround(invgamma(color[VG_R]) * 255);
                g = iround(invgamma(color[VG_G]) * 255);
                b = iround(invgamma(color[VG_B]) * 255);
                /**/
                pixel = (r << 24) + (g << 16) + (b << 8) + iround(a);
                break;

            case VG_lRGBA_8888_PRE:
                a *= 255;
                r = iround(invgamma(color[VG_R]) * a);
                g = iround(invgamma(color[VG_G]) * a);
                b = iround(invgamma(color[VG_B]) * a);
                /**/
                pixel = (r << 24) + (g << 16) + (b << 8) + iround(a);
                break;

            case VG_lL_8:
                l *= 255;
                /**/
                pixel = iround(l);
                break;

            case VG_BW_1:
                pixel = iround(l);
                break;

            case VG_A_8:
                a *= 255;
                /**/
                pixel = iround(a);
                break;

            case VG_A_1:
                pixel = iround(a);
                break;

            case VG_A_4:
                a *= 15;
                /**/
                pixel = iround(a);
                break;

            case VG_sXRGB_8888:
                r = iround(color[VG_R] * 255);
                g = iround(color[VG_G] * 255);
                b = iround(color[VG_B] * 255);
                /**/
                pixel = (255 << 24) + (r << 16) + (g << 8) + b;
                break;

            case VG_sARGB_8888:
                a *= 255;
                r = iround(color[VG_R] * 255);
                g = iround(color[VG_G] * 255);
                b = iround(color[VG_B] * 255);
                /**/
                pixel = (iround(a) << 24) + (r << 16) + (g << 8) + b;
                break;

            case VG_sARGB_8888_PRE:
                a *= 255;
                r = iround(color[VG_R] * a);
                g = iround(color[VG_G] * a);
                b = iround(color[VG_B] * a);
                /**/
                pixel = (iround(a) << 24) + (r << 16) + (g << 8) + b;
                break;

            case VG_sARGB_1555:
                r = iround(color[VG_R] * 31);
                g = iround(color[VG_G] * 31);
                b = iround(color[VG_B] * 31);
                /**/
                pixel = (iround(a) << 15) + (r << 10) + (g << 5) + b;
                break;

            case VG_sARGB_4444:
                a *= 15;
                r = iround(color[VG_R] * 15);
                g = iround(color[VG_G] * 15);
                b = iround(color[VG_B] * 15);
                /**/
                pixel = (iround(a) << 12) + (r << 8) + (g << 4) + b;
                break;

            case VG_lXRGB_8888:
                r = iround(invgamma(color[VG_R]) * 255);
                g = iround(invgamma(color[VG_G]) * 255);
                b = iround(invgamma(color[VG_B]) * 255);
                /**/
                pixel = (255 << 24) + (r << 16) + (g << 8) + b;
                break;

            case VG_lARGB_8888:
                a *= 255;
                r = iround(invgamma(color[VG_R]) * 255);
                g = iround(invgamma(color[VG_G]) * 255);
                b = iround(invgamma(color[VG_B]) * 255);
                /**/
                pixel = (iround(a) << 24) + (r << 16) + (g << 8) + b;
                break;

            case VG_lARGB_8888_PRE:
                a *= 255;
                r = iround(invgamma(color[VG_R]) * a);
                g = iround(invgamma(color[VG_G]) * a);
                b = iround(invgamma(color[VG_B]) * a);
                /**/
                pixel = (iround(a) << 24) + (r << 16) + (g << 8) + b;
                break;

            case VG_sBGRX_8888:
                r = iround(color[VG_R] * 255);
                g = iround(color[VG_G] * 255);
                b = iround(color[VG_B] * 255);
                /**/
                pixel = (b << 24) + (g << 16) + (r << 8) + 255;
                break;

            case VG_sBGRA_8888:
                a *= 255;
                r = iround(color[VG_R] * 255);
                g = iround(color[VG_G] * 255);
                b = iround(color[VG_B] * 255);
                /**/
                pixel = (b << 24) + (g << 16) + (r << 8) + iround(a);
                break;

            case VG_sBGRA_8888_PRE:
                a *= 255;
                r = iround(color[VG_R] * a);
                g = iround(color[VG_G] * a);
                b = iround(color[VG_B] * a);
                /**/
                pixel = (b << 24) + (g << 16) + (r << 8) + iround(a);
                break;

            case VG_sBGR_565:
                r = iround(color[VG_R] * 31);
                g = iround(color[VG_G] * 63);
                b = iround(color[VG_B] * 31);
                /**/
                pixel = (b << 11) + (g << 5) + r;
                break;

            case VG_sBGRA_5551:
                r = iround(color[VG_R] * 31);
                g = iround(color[VG_G] * 31);
                b = iround(color[VG_B] * 31);
                /**/
                pixel = (b << 11) + (g << 6) + (r << 1) + iround(a);
                break;

            case VG_sBGRA_4444:
                a *= 15;
                r = iround(color[VG_R] * 15);
                g = iround(color[VG_G] * 15);
                b = iround(color[VG_B] * 15);
                /**/
                pixel = (b << 12) + (g << 8) + (r << 4) + iround(a);
                break;

            case VG_lBGRX_8888:
                r = iround(invgamma(color[VG_R]) * 255);
                g = iround(invgamma(color[VG_G]) * 255);
                b = iround(invgamma(color[VG_B]) * 255);
                /**/
                pixel = (b << 24) + (g << 16) + (r << 8) + 255;
                break;

            case VG_lBGRA_8888:
                a *= 255;
                r = iround(invgamma(color[VG_R]) * 255);
                g = iround(invgamma(color[VG_G]) * 255);
                b = iround(invgamma(color[VG_B]) * 255);
                /**/
                pixel = (b << 24) + (g << 16) + (r << 8) + iround(a);
                break;

            case VG_lBGRA_8888_PRE:
                a *= 255;
                r = iround(invgamma(color[VG_R]) * a);
                g = iround(invgamma(color[VG_G]) * a);
                b = iround(invgamma(color[VG_B]) * a);
                /**/
                pixel = (b << 24) + (g << 16) + (r << 8) + iround(a);
                break;

            case VG_sXBGR_8888:
                r = iround(color[VG_R] * 255);
                g = iround(color[VG_G] * 255);
                b = iround(color[VG_B] * 255);
                /**/
                pixel = (255 << 24) + (b << 16) + (g << 8) + r;
                break;

            case VG_sABGR_8888:
                a *= 255;
                r = iround(color[VG_R] * 255);
                g = iround(color[VG_G] * 255);
                b = iround(color[VG_B] * 255);
                /**/
                pixel = (iround(a) << 24) + (b << 16) + (g << 8) + r;
                break;

            case VG_sABGR_8888_PRE:
                a *= 255;
                r = iround(color[VG_R] * a);
                g = iround(color[VG_G] * a);
                b = iround(color[VG_B] * a);
                /**/
                pixel = (iround(a) << 24) + (b << 16) + (g << 8) + r;
                break;

            case VG_sABGR_1555:
                r = iround(color[VG_R] * 31);
                g = iround(color[VG_G] * 31);
                b = iround(color[VG_B] * 31);
                /**/
                pixel = (iround(a) << 15) + (b << 10) + (g << 5) + r;
                break;

            case VG_sABGR_4444:
                a *= 15;
                r = iround(color[VG_R] * 15);
                g = iround(color[VG_G] * 15);
                b = iround(color[VG_B] * 15);
                /**/
                pixel = (iround(a) << 12) + (b << 8) + (g << 4) + r;
                break;

            case VG_lXBGR_8888:
                r = iround(invgamma(color[VG_R]) * 255);
                g = iround(invgamma(color[VG_G]) * 255);
                b = iround(invgamma(color[VG_B]) * 255);
                /**/
                pixel = (255 << 24) + (b << 16) + (g << 8) + r;
                break;

            case VG_lABGR_8888:
                a *= 255;
                r = iround(invgamma(color[VG_R]) * 255);
                g = iround(invgamma(color[VG_G]) * 255);
                b = iround(invgamma(color[VG_B]) * 255);
                /**/
                pixel = (iround(a) << 24) + (b << 16) + (g << 8) + r;
                break;

            case VG_lABGR_8888_PRE:
                a *= 255;
                r = iround(invgamma(color[VG_R]) * a);
                g = iround(invgamma(color[VG_G]) * a);
                b = iround(invgamma(color[VG_B]) * a);
                /**/
                pixel = (iround(a) << 24) + (b << 16) + (g << 8) + r;
                break;

            default:
                VG_FAIL(VG_UNSUPPORTED_IMAGE_FORMAT_ERROR);
                break;
            }
        }

zz: return (pixel);
    }

/*******************************************************************************
 *
 * vgGetBitDepthWRS - obtain the bit depth for the specified format
 *
 * RETURNS: A strictly positive value if successful, otherwise -1
 *
 */
VG_API_CALL VGint VG_API_ENTRY vgGetBitDepthWRS
    (
    VGImageFormat format
    ) VG_API_EXIT
    {
    VGint   bpp = -1;

    switch (format)
        {
        case VG_BW_1:
        case VG_A_1:
            bpp = 1;
            break;

        case VG_A_4:
            bpp = 4;
            break;

        case VG_sL_8:
        case VG_lL_8:
        case VG_A_8:
            bpp = 8;
            break;

        case VG_sRGB_565:
        case VG_sRGBA_5551:
        case VG_sRGBA_4444:
        case VG_sARGB_1555:
        case VG_sARGB_4444:
        case VG_sBGR_565:
        case VG_sBGRA_5551:
        case VG_sBGRA_4444:
        case VG_sABGR_1555:
        case VG_sABGR_4444:
            bpp = 16;
            break;

        case VG_sRGBX_8888:
        case VG_sRGBA_8888:
        case VG_sRGBA_8888_PRE:
        case VG_lRGBX_8888:
        case VG_lRGBA_8888:
        case VG_lRGBA_8888_PRE:
        case VG_sXRGB_8888:
        case VG_sARGB_8888:
        case VG_sARGB_8888_PRE:
        case VG_lXRGB_8888:
        case VG_lARGB_8888:
        case VG_lARGB_8888_PRE:
        case VG_sBGRX_8888:
        case VG_sBGRA_8888:
        case VG_sBGRA_8888_PRE:
        case VG_lBGRX_8888:
        case VG_lBGRA_8888:
        case VG_lBGRA_8888_PRE:
        case VG_sXBGR_8888:
        case VG_sABGR_8888:
        case VG_sABGR_8888_PRE:
        case VG_lXBGR_8888:
        case VG_lABGR_8888:
        case VG_lABGR_8888_PRE:
            bpp = 32;
            break;
        default:
            /* Do nothing */
            break;
        }

    return (bpp);
    }

/*******************************************************************************
 *
 * vgGetSurfaceFormatWRS - obtain the format of the specified EGL surface
 *
 * RETURNS: A VGImageFormat value if successful, otherwise -1
 *
 * ERRORS: VG_ILLEGAL_ARGUMENT_ERROR
 *
 */
VG_API_CALL VGint VG_API_ENTRY vgGetSurfaceFormatWRS
    (
    EGLSurface surface
    ) VG_API_EXIT
    {
    int         tmp;
    VGint       vgFormat = -1;
    surface_t * pSurface = (surface_t*)surface;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (pSurface == NULL)
            VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Decode the internal pixel format... */
        tmp = (pSurface->pConfig->pixelFormat.alphaBits << 12) +
              (pSurface->pConfig->pixelFormat.redBits << 8) +
              (pSurface->pConfig->pixelFormat.greenBits << 4) +
              (pSurface->pConfig->pixelFormat.blueBits);
        if (pSurface->pConfig->pixelFormat.flags & FB_PIX_ALPHA_IS_PAD)
            tmp |= 0x00010000;
        if (pSurface->pConfig->pixelFormat.flags & FB_PIX_RGBA)
            tmp |= 0x00020000;
        if (pSurface->pConfig->pixelFormat.flags & FB_PIX_SWAP)
            tmp |= 0x00040000;
        /* ...and match it to an OpenVG format */
        switch (tmp)
            {
            case 0x00008888:
                vgFormat = VG_sARGB_8888_PRE;
                break;

            case 0x00018888:
                vgFormat = VG_sXRGB_8888;
                break;
            }
        }

zz: return (vgFormat);
    }

/*******************************************************************************
 *
 * vgTransformWRS - apply a transformation matrix to the specified point
 *
 * ERRORS: VG_ILLEGAL_ARGUMENT_ERROR
 *
 */
VG_API_CALL void VG_API_ENTRY vgTransformWRS
    (
    const VGfloat* src,
    const VGfloat* matrix,
    VGfloat* dst
    ) VG_API_EXIT
    {
    float   x, y, w;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPtr(src, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(matrix, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(dst, sizeof(VGfloat)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Apply transformation */
        x = src[0], y = src[1], w = src[2];
        dst[0] = (x * matrix[0]) + (y * matrix[3]) + (w * matrix[6]);
        dst[1] = (x * matrix[1]) + (y * matrix[4]) + (w * matrix[7]);
        dst[2] = (x * matrix[2]) + (y * matrix[5]) + (w * matrix[8]);
        }
    }
