/* utils.c - Wind River VG Utilities */

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
13feb12,m_c  Released to Wind River engineering
21may09,m_c  Written
*/

/*
DESCRIPTION
These routines provide simple utility functionality that support the OpenVG
implementation.
*/

/* includes */

#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/*******************************************************************************
 *
 * clamp - clamp a value to the specified range
 *
 * RETURNS: A value in the specified range
 *
 */
float clamp
    (
    float x,
    float low,
    float high
    )
    {
    if (x < low)
        return (low);
    else if (x > high)
        return (high);
    else
        return (x);
    }

/*******************************************************************************
 *
 * clamp1 - clamp a value to the range [0, 1]
 *
 * RETURNS: A value in the range [0, 1]
 *
 */
float clamp1
    (
    float x
    )
    {
    if (x < 0)
        return (0);
    else if (x > 1)
        return (1);
    else
        return (x);
    }

/*******************************************************************************
 *
 * gamma - convert the specified color value from linear to standard color
 *         space, i.e. lRGB to sRGB
 *
 * RETURNS: The gamma mapping of the specified color value
 *
 */
float gamma
    (
    float x
    )
    {
    if (x <= 0.00304f)
        return (x * 12.92f);
    else
        return ((1.0556f * (float)pow((double)x, 1.0 / 2.4)) - 0.0556f);
    }

/*******************************************************************************
 *
 * invert3x3 - invert the specified 3 x 3 matrix
 *
 * RETURNS: The inverted matrix, or zero if the specified matrix is nonsingular
 *
 */
int invert3x3
    (
    const float* M,
    float* N
    )
    {
    float   a, b, c;	/* minors */
    float   det;		/* determinant */

    /* Calculate the determinant (expansion by minors) */
    a = (M[4] * M[8]) - (M[7] * M[5]);
    b = (M[6] * M[5]) - (M[3] * M[8]);
    c = (M[3] * M[7]) - (M[6] * M[4]);
    det = (M[0] * a) + (M[1] * b) + (M[2] * c);

    /* If the determinant is zero, then the matrix is singular (i.e. it is not invertible) */
    if (det == 0.0f)
        return (1);
    det = 1.0f / det;

    /* Calculate the inverse transformation */
    N[0] = det * a;
    N[1] = det * ((M[7] * M[2]) - (M[1] * M[8]));
    N[2] = det * ((M[1] * M[5]) - (M[4] * M[2]));
    N[3] = det * b;
    N[4] = det * ((M[0] * M[8]) - (M[6] * M[2]));
    N[5] = det * ((M[3] * M[2]) - (M[0] * M[5]));
    N[6] = det * c;
    N[7] = det * ((M[6] * M[1]) - (M[0] * M[7]));
    N[8] = det * ((M[0] * M[4]) - (M[0] * M[1]));

    return (0);
    }

/*******************************************************************************
 *
 * invgamma - convert the specified color value from standard to linear color
 *            space, i.e. sRGB to lRGB
 *
 * RETURNS: The inverse gamma mapping of the specified color value
 *
 */
float invgamma
    (
    float x
    )
    {
    if (x <= 0.03928f)
        return (x / 12.92f);
    else
        return ((float)pow((double)(x + 0.0556f) / 1.0556, 2.4));
    }

/*******************************************************************************
 *
 * isInvalidDataPtr - check if the specified pointer is null or is not properly
 *                    aligned for a given pixel format
 *
 * RETURNS: TRUE or FALSE
 *
 */
int isInvalidDataPtr
    (
    const void* pData,
    const int format
    )
    {
    if (pData == NULL)
        return (TRUE);
    switch (format)
        {
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
            return (IS_NOT_ALIGNED(pData, 2));

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
            return (IS_NOT_ALIGNED(pData, 4));

        default:
            return (FALSE);
        }
    }

/*******************************************************************************
 *
 * isInvalidFont - check if the specfied font is invalid
 *
 * RETURNS: TRUE or FALSE
 *
 */
int isInvalidFont(const font_t* pFont, gc_t* pGc)
    {
    if ((pFont != NULL) && (!pFont->deletePending))
        {
        LL_FOREACH(pGc->vg.pFonts, if (pObj == pFont)
                                       return (FALSE));
        }
    return (TRUE);
    }

/*******************************************************************************
 *
 * isInvalidImage - check if the specfied image is invalid
 *
 * RETURNS: TRUE or FALSE
 *
 */
int isInvalidImage
    (
    const image_t* pImage,
    gc_t* pGc
    )
    {
    if ((pImage != NULL) && ((!pImage->deletePending) || (pGc->vg.textMode)))
        {
        LL_FOREACH(pGc->vg.pImages, if (pObj == pImage)
                                        return (FALSE));
        }
    return (TRUE);
    }

/*******************************************************************************
 *
 * isInvalidPaint - check if the specfied paint is invalid
 *
 * RETURNS: TRUE or FALSE
 *
 */
int isInvalidPaint
    (
    const paint_t* pPaint,
    gc_t* pGc
    )
    {
    if ((pPaint != NULL) && (!pPaint->deletePending))
        {
        LL_FOREACH(pGc->vg.pPaints, if (pObj == pPaint)
                                        return (FALSE));
        }
    return (TRUE);
    }

/*******************************************************************************
 *
 * isInvalidPath - check if the specfied path is invalid
 *
 * RETURNS: TRUE or FALSE
 *
 */
int isInvalidPath
    (
    const path_t* pPath,
    gc_t* pGc
    )
    {
    if ((pPath != NULL) && ((!pPath->deletePending) || (pGc->vg.textMode)))
        {
        LL_FOREACH(pGc->vg.pPaths, if (pObj == pPath)
                                       return (FALSE));
        }
    return (TRUE);
    }

/*******************************************************************************
 *
 * getObjectType - get the type of the specified object
 *
 * RETURNS: The type of the specified object if successful, otherwise -1
 *
 */
int getObjectType(const void* pObject, gc_t* pGc)
    {
    if (pObject != NULL)
        {
        const path_t* pPath = pObject;
        LL_FOREACH(pGc->vg.pPaths, if (pObj == pPath)
                                       return ((pPath->deletePending) ? (-1) : (OBJECT_TYPE_PATH)));
        const image_t* pImage = pObject;
        LL_FOREACH(pGc->vg.pImages, if (pObj == pImage)
                                        return ((pImage->deletePending) ? (-1) : (OBJECT_TYPE_IMAGE)));
        const font_t* pFont = pObject;
        LL_FOREACH(pGc->vg.pFonts, if (pObj == pFont)
                                       return ((pFont->deletePending) ? (-1) : (OBJECT_TYPE_FONT)));
        const paint_t* pPaint = pObject;
        LL_FOREACH(pGc->vg.pPaints, if (pObj == pPaint)
                                        return ((pPaint->deletePending) ? (-1) : (OBJECT_TYPE_PAINT)));
        }
    return (-1);
    }
