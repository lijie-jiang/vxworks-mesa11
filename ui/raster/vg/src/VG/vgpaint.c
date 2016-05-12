/* vgpaint.c - Wind River VG paint Functionality */

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
13dec09,m_c  Written
*/

/*
DESCRIPTION
These routines provide paint functionality that support the OpenVG
implementation.
*/

/* includes */

#include <stdlib.h>
#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/* forward declarations */

LOCAL void deletePaint(paint_t*);

/*******************************************************************************
 *
 *  deletePaint - unlink and delete the specified paint
 *
 */
LOCAL void deletePaint
    (
    paint_t* pPaint
    )
    {
    image_t*                        pImage;

    /* Unlink */
    LL_REMOVE(pPaint->pGc->vg.pPaints, pPaint);

    /* Unbind pattern */
    if (pPaint->pattern.pImage != NULL)
        {
        pImage = pPaint->pattern.pImage;

        pImage->refCount--;
        if ((pImage->refCount == 0) && (pImage->deletePending))
            {
            pImage->deletePending = FALSE;
            vgDestroyImage((VGImage)pImage);
            }
        }

    /* Discard data */
    #ifdef  PATTERN_DENORM_LUT_IS_DYNAMIC
    free(pPaint->pattern.denormLut);
    #endif  /* PATTERN_DENORM_LUT_IS_DYNAMIC */
    free(pPaint->colorRamp);

    /* Free allocated memory */
    free(pPaint);
    }

/*******************************************************************************
 *
 *  vgCreatePaint
 *
 */
VG_API_CALL VGPaint VG_API_ENTRY vgCreatePaint
    (
    ) VG_API_EXIT
    {
    paint_t*                        pPaint = NULL;

    GET_GC();
    if (pGc != NULL)
        {
        /* Allocate memory for the structure */
        pPaint = malloc(sizeof(paint_t));
        if (pPaint == NULL)
            VG_FAIL(VG_OUT_OF_MEMORY_ERROR);

        /* Initialize to default values (see specifications for details) */
        LL_ADD_HEAD(pGc->vg.pPaints, pPaint);
        pPaint->pGc = pGc;
        pPaint->refCount = 0;
        pPaint->deletePending = FALSE;
        pPaint->dirty = TRUE;
        pPaint->type = VG_PAINT_TYPE_COLOR;
        pPaint->color[VG_R] = 0.0;
        pPaint->color[VG_G] = 0.0;
        pPaint->color[VG_B] = 0.0;
        pPaint->color[VG_A] = 1.0;
        pPaint->spreadMode = VG_COLOR_RAMP_SPREAD_PAD;
        pPaint->numStops = 0;
        pPaint->premultiplyColorRamp= VG_TRUE;
        pPaint->linearGradient_x0 = 0.0;
        pPaint->linearGradient_y0 = 0.0;
        pPaint->linearGradient_x1 = 1.0;
        pPaint->linearGradient_y1 = 0.0;
        pPaint->linearGradient_dx = 1.0;
        pPaint->linearGradient_dy = 0.0;
        pPaint->linearGradient_q = 1.0;
        pPaint->radialGradient_cx = 0.0;
        pPaint->radialGradient_cy = 0.0;
        pPaint->radialGradient_fx = 0.0;
        pPaint->radialGradient_fy = 0.0;
        pPaint->radialGradient_r = 1.0;
        pPaint->radialGradient_a = 1.0;
        pPaint->radialGradient_b = 1.0;
        pPaint->radialGradient_c = 0.0;
        pPaint->radialGradient_d = 0.0;
        pPaint->radialGradient_e = 0.0;
        pPaint->format = -1;
        pPaint->color2 = 0x00000000;
        pPaint->colorRamp = NULL;
        pPaint->pattern.tilingMode = VG_TILE_FILL;
        pPaint->pattern.pImage = NULL;
        #ifdef  PATTERN_DENORM_LUT_IS_DYNAMIC
        pPaint->pattern.denormLut = NULL;
        #endif  /* PATTERN_DENORM_LUT_IS_DYNAMIC */
        }

zz: return ((VGPaint)pPaint);
    }

/*******************************************************************************
 *
 *  vgDestroyPaint
 *
 */
VG_API_CALL void VG_API_ENTRY vgDestroyPaint
    (
    VGPaint paint
    ) VG_API_EXIT
    {
    paint_t*                        pPaint = (paint_t*)paint;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPaint(pPaint, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Delete the paint */
        if (pPaint->refCount > 0)
            pPaint->deletePending = TRUE;
        else
            deletePaint(pPaint);
        }
    }

/*******************************************************************************
 *
 *  vgGetColor
 *
 */
VG_API_CALL VGuint VG_API_ENTRY vgGetColor
    (
    VGPaint paint
    ) VG_API_EXIT
    {
    VGuint                          color = 0;
    paint_t*                        pPaint = (paint_t*)paint;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPaint(pPaint, pGc))
            VG_FAIL(VG_BAD_HANDLE_ERROR);

        /* Convert color into the 32-bit non-premultiplied sRGBA_8888 representation */
        color = vgConvertColorWRS(pPaint->color, VG_sRGBA_8888);
        }

zz: return (color);
    }

/*******************************************************************************
 *
 *  vgGetPaint
 *
 */
VG_API_CALL VGPaint VG_API_ENTRY vgGetPaint
    (
    VGPaintMode paintMode
    ) VG_API_EXIT
    {
    paint_t*                        pPaint = NULL;

    GET_GC();
    if (pGc != NULL)
        {
        /* Return the paint for the specified mode */
        switch (paintMode)
            {
            case VG_FILL_PATH:
                if ((pGc->vg.pFillPaint != NULL) &&
                    (!pGc->vg.pFillPaint->deletePending))
                    pPaint = pGc->vg.pFillPaint;
                break;

            case VG_STROKE_PATH:
                if ((pGc->vg.pStrokePaint != NULL) &&
                    (!pGc->vg.pStrokePaint->deletePending))
                    pPaint = pGc->vg.pStrokePaint;
                break;

            default:
                VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);
                break;
            }
        }

zz: return ((VGPaint)pPaint);
    }

/*******************************************************************************
 *
 *  vgPaintPattern
 *
 */
VG_API_CALL void VG_API_ENTRY vgPaintPattern
    (
    VGPaint paint,
    VGImage image
    ) VG_API_EXIT
    {
    image_t*                        pOldImage;
    paint_t*                        pPaint = (paint_t*)paint;
    image_t*                        pImage = (image_t*)image;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPaint(pPaint, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((pImage != VG_INVALID_HANDLE) && (isInvalidImage(pImage, pGc)))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((pImage != VG_INVALID_HANDLE) && (pImage->inUse))
            VG_FAIL_VOID(VG_IMAGE_IN_USE_ERROR);

#ifdef  PATTERN_DENORM_LUT_IS_DYNAMIC
        /* Allocate the memory for the denormalization look-up-table */
        if (pPaint->pattern.denormLut == NULL)
            {
            pPaint->pattern.denormLut = malloc(PATTERN_DENORM_LUT_SIZE * 2 * sizeof(int));
            if (pPaint->pattern.denormLut == NULL)
                VG_FAIL_VOID(VG_OUT_OF_MEMORY_ERROR);
            }
#endif  /* PATTERN_DENORM_LUT_IS_DYNAMIC */

        /* Avoid unnecessary work */
        if (pPaint->pattern.pImage != pImage)
            {
            pOldImage = pPaint->pattern.pImage;

            /* Delete old pattern if needed */
            if (pOldImage != NULL)
                {
                pOldImage->refCount--;
                if ((pOldImage->refCount == 0) && (pOldImage->deletePending))
                    {
                    pOldImage->deletePending = FALSE;
                    vgDestroyImage((VGImage)pOldImage);
                    }
                }

            /* Set new pattern */
            pPaint->pattern.pImage = pImage;

            /* Bind image to paint */
            if (pImage != VG_INVALID_HANDLE)
                pImage->refCount++;

            pPaint->dirty = TRUE;
            }
        }
    }

/*******************************************************************************
 *
 *  vgSetColor
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetColor
    (
    VGPaint paint,
    VGuint color
    ) VG_API_EXIT
    {
    paint_t*                        pPaint = (paint_t*)paint;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (isInvalidPaint(pPaint, pGc))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        /* Convert the sRGBA_8888 color to the internal format */
        pPaint->color[VG_R] = (float)((color >> 24) & 255) / 255.0f;
        pPaint->color[VG_G] = (float)((color >> 16) & 255) / 255.0f;
        pPaint->color[VG_B] = (float)((color >> 8) & 255) / 255.0f;
        pPaint->color[VG_A] = (float)(color & 255) / 255.0f;

        pPaint->dirty = TRUE;
        }
    }

/*******************************************************************************
 *
 *  vgSetPaint
 *
 */
VG_API_CALL void VG_API_ENTRY vgSetPaint
    (
    VGPaint paint,
    VGbitfield paintModes
    ) VG_API_EXIT
    {
    paint_t*                        pOldPaint;
    paint_t*                        pPaint = (paint_t*)paint;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if ((pPaint != NULL) && (isInvalidPaint(pPaint, pGc)))
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if ((paintModes & ~VG_PAINT_MODE_MASK) != 0)
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Set stroke paint */
        if ((paintModes & VG_STROKE_PATH) && (pGc->vg.pStrokePaint != pPaint))
            {
            pOldPaint = pGc->vg.pStrokePaint;

            if (pOldPaint != NULL)
                {
                pOldPaint->refCount--;
                if ((pOldPaint->refCount == 0) && (pOldPaint->deletePending))
                    deletePaint(pOldPaint);
                }

            if (pPaint != NULL)
                pPaint->refCount++;

            pGc->vg.pStrokePaint = pPaint;
            }

        /* Set fill paint */
        if ((paintModes & VG_FILL_PATH) && (pGc->vg.pFillPaint != pPaint))
            {
            pOldPaint = pGc->vg.pFillPaint;

            if (pOldPaint != NULL)
                {
                pOldPaint->refCount--;
                if ((pOldPaint->refCount == 0) && (pOldPaint->deletePending))
                    deletePaint(pOldPaint);
                }

            if (pPaint != NULL)
                pPaint->refCount++;

            pGc->vg.pFillPaint = pPaint;
            }
        }
    }
