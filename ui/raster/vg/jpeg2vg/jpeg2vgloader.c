/* jpeg2vgloader.c - JPEG library interface for Wind River VG */

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
25jun13,mgc  Modified for VxWorks 7 release
15feb12,m_c  Written
*/

/*
DESCRIPTION
These routine assist in interface with libjpeg in loading images and converting
them into a VG format.
*/

/* includes */

#include <jpeg2vgloader.h>

/*******************************************************************************
 *
 * jpeg_load_vg - load a jpeg image into a VGImage format
 *
 * RETURNS: An image handle if successful, otherwise VG_INVALID_HANDLE
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
VGImage jpeg_load_vg
    (
    const char* filename,
    VGImageFormat format
    )
    {
    JSAMPARRAY                      buf;
    struct jpeg_decompress_struct   cinfo;
    unsigned int*                   data;
    FILE*                           fp;
    int                             i, j, k;
    VGImage                         image;
    struct jpeg_error_mgr           jerr;
    VGint                           width;

    data = NULL;
    image = VG_INVALID_HANDLE;

    /* Check arguments */
    if (filename == NULL)
        return (VG_INVALID_HANDLE);

    /* Open the file for reading */
    fp = fopen(filename, "rb");
    if (fp == NULL)
        return (VG_INVALID_HANDLE);

    /* REQUIRED - Initialize JPEG decompression object */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    /* REQUIRED - Specify the data source */
    jpeg_stdio_src(&cinfo, fp);

    /* REQUIRED - Read the file info */
    jpeg_read_header(&cinfo, TRUE);

    /* Set the decompression parameters */
    cinfo.out_color_space = JCS_RGB;

    /* REQUIRED - Start decompressor */
    jpeg_start_decompress(&cinfo);

    /* REQUIRED - Allocate memory for one row */
    buf = cinfo.mem->alloc_sarray((j_common_ptr)&cinfo, JPOOL_IMAGE, cinfo.output_width * cinfo.output_components, 1);
    data = malloc(cinfo.output_width * 4);
    if ((buf == NULL) || (data == NULL))
        goto zz;
    width = cinfo.output_width;

    /* Create a new VG image */
    image = vgCreateImage(format, width, cinfo.output_height, VG_IMAGE_QUALITY_FASTER);
    if (image == VG_INVALID_HANDLE)
        goto zz;

    /* Load the image */
    for (i = cinfo.output_height - 1; cinfo.output_scanline < cinfo.output_height; i--)
        {
        jpeg_read_scanlines(&cinfo, buf, 1);

        for (j = k = 0; j < width; j++, k += 3)
            data[j] = (buf[0][k + 0] << 24) + (buf[0][k + 1] << 16) + (buf[0][k + 2] << 8) + 0xff;

        vgImageSubData(image, data, 0, VG_sRGBA_8888, 0, i, width, 1);
        }

    /* REQUIRED - Finish decompression */
    jpeg_finish_decompress(&cinfo);

    /* REQUIRED - Release JPEG decompression object */
zz: jpeg_destroy_decompress(&cinfo);

    /* Close the file */
    fclose(fp);

    /* Free temporary buffer */
    free(data);

    return (image);
    }
