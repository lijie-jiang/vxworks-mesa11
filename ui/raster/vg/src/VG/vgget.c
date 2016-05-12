/* vgget.c - Wind River VG Get Functionality */

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
14aug13,mgc  Modified for VxWorks 7 release
13feb12,m_c  Released to Wind River engineering
21may09,m_c  Written
*/

/*
DESCRIPTION
These routines provide get functionality that support the OpenVG
implementation.
*/

/* includes */

#include <float.h>
#include <EGL/eglimpl.h>
#include <VG/vgimpl.h>

/* forward declarations */

LOCAL void get(VGParamType, int, _VGType*, int);
LOCAL void getParameter(void*, VGParamType, int, _VGType*, int);

/* locals */

/* Implementation information */
LOCAL const char vgVendor[]      = VENDOR_STRING;
LOCAL const char vgVersion[]     = "1.1";
LOCAL const char vgRenderer[]    = "software";
LOCAL const char vgExtensions[]  = "OVG_WRS_miscellaneous";

/*******************************************************************************
 *
 * get - return the value of a parameter on the current context
 *
 */
LOCAL void get
    (
    VGParamType param,
    int count,
    _VGType* values,
    int type
    )
    {
    int i;

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        if (count <= 0)
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(values, sizeof(_VGType)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Get parameter */
        switch (param)
            {
            /* Scalars */
            case VG_FILL_RULE:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, pGc->vg.fillRule);
                break;

            case VG_FILTER_CHANNEL_MASK:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, pGc->vg.filterChannelMask);
                break;

            case VG_FILTER_FORMAT_LINEAR:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, pGc->vg.filterLinear);
                break;

            case VG_FILTER_FORMAT_PREMULTIPLIED:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, pGc->vg.filterPremultiplied);
                break;

            case VG_IMAGE_MODE:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, pGc->vg.imageMode);
                break;

            case VG_IMAGE_QUALITY:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, pGc->vg.imageQuality);
                break;

            case VG_MATRIX_MODE:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, pGc->vg.matrixMode);
                break;

            case VG_MAX_COLOR_RAMP_STOPS:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, MAX_STOPS / 5);
                break;

            case VG_MAX_FLOAT:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_FLOAT(values[0], type, FLT_MAX);
                break;

            case VG_MAX_IMAGE_BYTES:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, (MAX_IMAGE_BYTES / 2) - 1);
                break;

            case VG_MAX_IMAGE_HEIGHT:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, MAX_IMAGE_HEIGHT);
                break;

            case VG_MAX_IMAGE_PIXELS:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, (MAX_IMAGE_PIXELS / 2) - 1);
                break;

            case VG_MAX_IMAGE_WIDTH:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, MAX_IMAGE_WIDTH);
                break;

            case VG_MAX_KERNEL_SIZE:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, MAX_KERNEL_SIZE);
                break;

            case VG_MAX_SCISSOR_RECTS:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, MAX_SCISSOR_RECTS / 4);
                break;

            case VG_MAX_SEPARABLE_KERNEL_SIZE:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, MAX_SEPARABLE_KERNEL_SIZE);
                break;

            case VG_PIXEL_LAYOUT:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, pGc->vg.pixelLayout);
                break;

            case VG_SCREEN_LAYOUT:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, VG_PIXEL_LAYOUT_UNKNOWN);
                break;

            case VG_SCISSORING:
                if (count != 1)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_INT(values[0], type, pGc->vg.enableScissoring);
                break;

            /* Vectors */
            case VG_CLEAR_COLOR:
                if (count != 4)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_FLOAT(values[0], type, pGc->vg.clearColor[VG_R]);
                FROM_FLOAT(values[1], type, pGc->vg.clearColor[VG_G]);
                FROM_FLOAT(values[2], type, pGc->vg.clearColor[VG_B]);
                FROM_FLOAT(values[3], type, pGc->vg.clearColor[VG_A]);
                break;

            case VG_GLYPH_ORIGIN:
                if (count != 2)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_FLOAT(values[0], type, pGc->vg.glyphOrigin[0]);
                FROM_FLOAT(values[1], type, pGc->vg.glyphOrigin[1]);
                break;

            case VG_SCISSOR_RECTS:
                if (count > pGc->vg.numScissorRects)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                for (i = 0; i < count; i++)
                    FROM_INT(values[i], type, ((int*)pGc->vg.scissoringData)[i]);
                break;

            case VG_TILE_FILL_COLOR:
                if (count != 4)
                    VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                FROM_FLOAT(values[0], type, pGc->vg.tileFillColor[VG_R]);
                FROM_FLOAT(values[1], type, pGc->vg.tileFillColor[VG_G]);
                FROM_FLOAT(values[2], type, pGc->vg.tileFillColor[VG_B]);
                FROM_FLOAT(values[3], type, pGc->vg.tileFillColor[VG_A]);
                break;

            /* ??? */
            default:
                VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                break;
            }
        }
    }


/*******************************************************************************
 *
 * getParameter - return the value of a parameter on the specified object
 *
 */
LOCAL void getParameter
    (
    void* pObject,
    VGParamType param,
    int count,
    _VGType* values,
    int type
    )
    {
    int         i;
    int         objType;    /* type of object */
    font_t *    pFont;      /* font */
    image_t *   pImage;     /* image */
    paint_t *   pPaint;     /* paint */
    path_t *    pPath;      /* path */

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        objType = getObjectType(pObject, pGc);
        if (objType < 0)
            VG_FAIL_VOID(VG_BAD_HANDLE_ERROR);

        if (count <= 0)
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        if (isInvalidPtr(values, sizeof(_VGType)))
            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

        /* Get parameter */
        switch (objType)
            {
            case OBJECT_TYPE_PATH:
                pPath = pObject;
                switch ((VGPathParamType)param)
                    {
                    case VG_PATH_BIAS:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_FLOAT(values[0], type, pPath->bias);
                        break;

                    case VG_PATH_DATATYPE:
                        if (count != 1)
                        VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pPath->datatype);
                        break;

                    case VG_PATH_FORMAT:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pPath->format);
                        break;

                    case VG_PATH_NUM_COORDS:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pPath->numUserCoords);
                        break;

                    case VG_PATH_NUM_SEGMENTS:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pPath->numSegs);
                        break;

                    case VG_PATH_SCALE:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_FLOAT(values[0], type, pPath->scale);
                        break;

                    default:
                        VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                        break;
                    }
                break;

            case OBJECT_TYPE_IMAGE:
                pImage = pObject;
                switch ((VGImageParamType)param)
                    {
                    case VG_IMAGE_FORMAT:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pImage->format);
                        break;

                    case VG_IMAGE_HEIGHT:
                        if (count > 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pImage->height);
                        break;

                    case VG_IMAGE_WIDTH:
                        if (count > 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pImage->width);
                        break;

                    default:
                        VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                        break;
                    }
                break;

            case OBJECT_TYPE_MASK_LAYER:
                /* :TODO: */
                break;

            case OBJECT_TYPE_FONT:
                pFont = pObject;
                switch ((VGFontParamType)param)
                    {
                    case VG_FONT_NUM_GLYPHS:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pFont->numGlyphs);
                        break;

                    default:
                        VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                        break;
                    }
                break;

            case OBJECT_TYPE_PAINT:
                pPaint = pObject;
                switch ((VGPaintParamType)param)
                    {
                    case VG_PAINT_COLOR:
                        if (count != 4)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_FLOAT(values[0], type, pPaint->color[0]);
                        FROM_FLOAT(values[1], type, pPaint->color[1]);
                        FROM_FLOAT(values[2], type, pPaint->color[2]);
                        FROM_FLOAT(values[3], type, pPaint->color[3]);
                        break;

                    case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pPaint->premultiplyColorRamp);
                        break;

                    case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pPaint->spreadMode);
                        break;

                    case VG_PAINT_COLOR_RAMP_STOPS:
                        if (count > pPaint->numStops)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        for (i = 0; i < count; i++)
                            FROM_FLOAT(values[i], type, pPaint->stops[i]);
                        break;

                    case VG_PAINT_LINEAR_GRADIENT:
                        if (count != 4)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        for (i = 0; i < count; i++)
                            FROM_FLOAT(values[i], type, pPaint->linearGradient[i]);
                        break;

                    case VG_PAINT_PATTERN_TILING_MODE:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pPaint->pattern.tilingMode);
                        break;

                    case VG_PAINT_TYPE:
                        if (count != 1)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        FROM_INT(values[0], type, pPaint->type);
                        break;

                    case VG_PAINT_RADIAL_GRADIENT:
                        if (count != 5)
                            VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);

                        for (i = 0; i < count; i++)
                            FROM_FLOAT(values[i], type, pPaint->radialGradient[i]);
                        break;

                    default:
                        VG_FAIL_VOID(VG_ILLEGAL_ARGUMENT_ERROR);
                        break;
                    }
                break;
            }
        }
    }

/*******************************************************************************
 *
 * vgGetError
 *
 */
VG_API_CALL VGErrorCode VG_API_ENTRY vgGetError
    (
    ) VG_API_EXIT
    {
    VGErrorCode                         lastError = VG_NO_ERROR;

    GET_GC();
    if (pGc != NULL)
        {
        /* Get the last error and reset it */
        lastError = pGc->vg.error;
        pGc->vg.error = VG_NO_ERROR;
        }

    return (lastError);
    }

/*******************************************************************************
 *
 * vgGetf
 *
 */
VG_API_CALL VGfloat VG_API_ENTRY vgGetf
    (
    VGParamType param
    ) VG_API_EXIT
    {
    VGfloat                         value;

    get(param, 1, (_VGType*)&value, VG_FLOAT);
    return (value);
    }

/*******************************************************************************
 *
 * vgGetfv
 *
 */
VG_API_CALL void VG_API_ENTRY vgGetfv
    (
    VGParamType param,
    VGint count,
    VGfloat* values
    ) VG_API_EXIT
    {
    get(param, count, (_VGType*)values, VG_FLOAT);
    }

/*******************************************************************************
 *
 * vgGetiv
 *
 */
VG_API_CALL VGint VG_API_ENTRY vgGeti
    (
    VGParamType param
    ) VG_API_EXIT
    {
    VGint value;

    get(param, 1, (_VGType*)&value, VG_INT);
    return (value);
    }

/*******************************************************************************
 *
 * vgGetiv
 *
 */
VG_API_CALL void VG_API_ENTRY vgGetiv
    (
    VGParamType param,
    VGint count,
    VGint* values
    ) VG_API_EXIT
    {
    get(param, count, (_VGType*)values, VG_INT);
    }

/*******************************************************************************
 *
 * vgGetParameterf
 *
 */
VG_API_CALL VGfloat VG_API_ENTRY vgGetParameterf
    (
    VGHandle object,
    VGint param
    ) VG_API_EXIT
    {
    VGfloat                         value;

    getParameter((void*)object, param, 1, (_VGType*)&value, VG_FLOAT);
    return (value);
    }

/*******************************************************************************
 *
 * vgGetParameterfv
 *
 */
VG_API_CALL void VG_API_ENTRY vgGetParameterfv
    (
    VGHandle object,
    VGint param,
    VGint count,
    VGfloat* values
    ) VG_API_EXIT
    {
    getParameter((void*)object, param, count, (_VGType*)values, VG_FLOAT);
    }

/*******************************************************************************
 *
 * vgGetParameteri
 *
 */
VG_API_CALL VGint VG_API_ENTRY vgGetParameteri
    (
    VGHandle object,
    VGint param
    ) VG_API_EXIT
    {
    VGint                           value;

    getParameter((void *)object, param, 1, (_VGType*)&value, VG_INT);
    return (value);
    }

/*******************************************************************************
 *
 * vgGetParameteriv
 *
 */
VG_API_CALL void VG_API_ENTRY vgGetParameteriv
    (
    VGHandle object,
    VGint param,
    VGint count,
    VGint* values) VG_API_EXIT
    {
    getParameter((void *)object, param, count, (_VGType*)values, VG_INT);
    }

/*******************************************************************************
 *
 * vgGetParameterVectorSize
 *
 */
VG_API_CALL VGint VG_API_ENTRY vgGetParameterVectorSize
    (
    VGHandle object,
    VGint param
    ) VG_API_EXIT
    {
    int                             objType;    /* type of object */
    paint_t*                        pPaint;     /* paint */
    VGint                           size = 0;   /* vector size */

    GET_GC();
    if (pGc != NULL)
        {
        /* Check arguments */
        objType = getObjectType((void *)object, pGc);
        if (objType < 0)
            VG_FAIL(VG_BAD_HANDLE_ERROR);

        /* Get parameter */
        switch (objType)
            {
            case OBJECT_TYPE_PATH:
                switch (param)
                    {
                    case VG_PATH_BIAS:
                    case VG_PATH_DATATYPE:
                    case VG_PATH_FORMAT:
                    case VG_PATH_NUM_COORDS:
                    case VG_PATH_NUM_SEGMENTS:
                    case VG_PATH_SCALE:
                        size = 1;
                        break;

                    default:
                        VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);
                        break;
                    }
                break;

            case OBJECT_TYPE_IMAGE:
                switch (param)
                    {
                    case VG_IMAGE_FORMAT:
                    case VG_IMAGE_HEIGHT:
                    case VG_IMAGE_WIDTH:
                        size = 1;
                        break;

                    default:
                        VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);
                        break;
                    }
                break;

            case OBJECT_TYPE_MASK_LAYER:
                /* :TODO: */
                break;

            case OBJECT_TYPE_FONT:
                switch (param)
                    {
                    case VG_FONT_NUM_GLYPHS:
                        size = 1;
                        break;

                    default:
                        VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);
                        break;
                    }
                break;

            case OBJECT_TYPE_PAINT:
                pPaint = (paint_t*)object;
                switch (param)
                    {
                    case VG_PAINT_COLOR_RAMP_PREMULTIPLIED:
                    case VG_PAINT_COLOR_RAMP_SPREAD_MODE:
                    case VG_PAINT_PATTERN_TILING_MODE:
                    case VG_PAINT_TYPE:
                        size = 1;
                        break;

                    case VG_PAINT_COLOR:
                    case VG_PAINT_LINEAR_GRADIENT:
                        size = 4;
                        break;

                    case VG_PAINT_RADIAL_GRADIENT:
                        size = 5;
                        break;

                    case VG_PAINT_COLOR_RAMP_STOPS:
                        size = pPaint->numStops;
                        break;

                    default:
                        VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);
                        break;
                    }
                break;
            }
        }

zz: return (size);
    }

/*******************************************************************************
 *
 * vgGetString
 *
 */
VG_API_CALL const VGubyte * VG_API_ENTRY vgGetString
    (
    VGStringID name
    ) VG_API_EXIT
    {
    const char*                     string;

    switch (name)
        {
        case VG_VENDOR:
            string = vgVendor;
            break;

        case VG_VERSION:
            string = vgVersion;
            break;

        case VG_RENDERER:
            string = vgRenderer;
            break;

        case VG_EXTENSIONS:
            string = vgExtensions;
            break;

        default:
            string = NULL;
            break;
        }

    return ((const VGubyte*)string);
    }

/*******************************************************************************
 *
 * vgGetVectorSize
 *
 */
VG_API_CALL VGint VG_API_ENTRY vgGetVectorSize
    (
    VGParamType type
    ) VG_API_EXIT
    {
    VGint                           size = 0;   /* vector size */

    GET_GC();
    if (pGc != NULL)
        {
        /* Return number of elements */
        switch (type)
            {
            /* Scalars */
            case VG_FILL_RULE:
            case VG_IMAGE_MODE:
            case VG_MATRIX_MODE:
            case VG_MAX_FLOAT:
            case VG_MAX_IMAGE_BYTES:
            case VG_MAX_IMAGE_HEIGHT:
            case VG_MAX_IMAGE_PIXELS:
            case VG_MAX_IMAGE_WIDTH:
            case VG_MAX_SCISSOR_RECTS:
            case VG_MAX_COLOR_RAMP_STOPS:
            case VG_SCREEN_LAYOUT:
            case VG_SCISSORING:
                size = 1;
                break;

            /* Vectors */
            case VG_CLEAR_COLOR:
                size = 4;
                break;

            case VG_GLYPH_ORIGIN:
                size = 2;
                break;

            case VG_SCISSOR_RECTS:
                size = pGc->vg.numScissorRects;
                break;

            default:
                VG_FAIL(VG_ILLEGAL_ARGUMENT_ERROR);
                break;
            }
        }

zz: return (size);
    }
