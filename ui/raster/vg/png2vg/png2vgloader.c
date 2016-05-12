/* png2vgloader.c - PNG library interface for Wind River VG */

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
20jul09,m_c  Written
*/

/*
DESCRIPTION
These routine assist in interface with libpng in loading images and converting
them into a VG format.
*/

/* includes */

#include <png2vgloader.h>
/* forward declarations */

/* Endianness helper */
LOCAL union { char c; int i; }     byteOrder = {1};

/*******************************************************************************
 *
 * png_load_vg - load a png image into a VGImage format
 *
 * RETURNS: An image handle if successful, otherwise VG_INVALID_HANDLE
 *
 * ERRNO: VG_INVALID_HANDLE
 *
 * SEE ALSO:
 *
 */
VGImage png_load_vg
    (
    const char* filename,
    VGImageFormat format
    )
    {
    int         bitDepth, colorType;    /* PNG information */
    FILE *      fp;                     /* file object */
    int         i;
    VGImage     image;                  /* image */
    png_infop   info;                   /* PNG info object */
    png_structp png;                    /* PNG object */
    png_bytep   pBuf;                   /* buffer */
    png_uint_32 width, height;          /* width and height */

    /* Check arguments */
    if (filename == NULL)
        {
        errno = EINVAL;
        return (VG_INVALID_HANDLE);
        }

    /* Open the file for reading */
    fp = fopen(filename, "rb");
    if (fp == NULL)
        return (VG_INVALID_HANDLE);

    /*
     * REQUIRED - Create and initialize the png_struct with the desired error
     * handler funtions
     */
    png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png == NULL)
        {
       fclose(fp);
       return (VG_INVALID_HANDLE);
        }

    /* REQUIRED - Allocate/initialize the memory for image information */
    info = png_create_info_struct(png);
    if (info == NULL)
        {
zz:    png_destroy_read_struct(&png, NULL, NULL);
       fclose(fp);
       return (VG_INVALID_HANDLE);
        }

    /* REQUIRED - Set error handling */
    if (setjmp(png_jmpbuf(png)))
        goto zz;

    /* REQUIRED - Set up the input control */
    png_init_io(png, fp);

    /*
     * REQUIRED - The call to png_read_info() gives us all of the information
     * from the PNG file before the first IDAT (image data chunk)
     */
    png_read_info(png, info);
    png_get_IHDR(png, info, &width, &height, &bitDepth, &colorType, NULL, NULL, NULL);

    /* Add filler (or alpha) byte (before/after each RGB triplet) */
    png_set_filler(png, 0xff, PNG_FILLER_AFTER);

    /* Strip 16 bit/color files down to 8 bits/color */
    png_set_strip_16(png);

    /* Expand paletted colors into true RGB triplets */
    if (colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    /*
     * Expand paletted or RGB images with transparency to full alpha channels
     * so the data is available as RGBA quartets
     */
    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    /* Allocate temporay buffer */
    pBuf = malloc(width * 4);
    if (pBuf == NULL)
        goto zz;

    /* Create the image */
    image = vgCreateImage(format, width, height, VG_IMAGE_QUALITY_FASTER);
    if (image == VG_INVALID_HANDLE)
        {
        free(pBuf);
        switch (vgGetError())
            {
            case VG_ILLEGAL_ARGUMENT_ERROR:
            case VG_UNSUPPORTED_IMAGE_FORMAT_ERROR:
                errno = EINVAL;
                break;

            case VG_OUT_OF_MEMORY_ERROR:
                errno = ENOMEM;
                break;

            case VG_NO_CONTEXT_ERROR:
                errno = EACCES;
                break;

            default:
                errno = EFAULT;
            }
        goto zz;
        }

    /* Load the image */
    for (i = height - 1; i >= 0; i--)
        {
        png_read_row(png, pBuf, NULL);
        vgImageSubData(image, pBuf, 0, (byteOrder.c == 0) ? (VG_sRGBA_8888) : (VG_sABGR_8888), 0, i, width, 1);
        }

    /* Free temporary buffer */
    free(pBuf);

    /* REQUIRED - Clean up after the read, and free any memory allocated */
    png_destroy_read_struct(&png, &info, NULL);

    /* Close the file */
    fclose(fp);

    /* Return the image handle */
    return (image);
    }

/*******************************************************************************
 *
 * png_validate - check whether or not the supplied file is a valid PNG
 *
 * RETURNS: Zero if the file has the correct signature
 *
 * ERRNO: N/A
 *
 * SEE ALSO:
 *
 */
int png_validate
    (
    const char* filename
    )
    {
    png_byte                        buf[8];
    FILE*                           fp;
    int                             rc;

    /* Check arguments */
    if (filename == NULL)
        {
        errno = EINVAL;
        return (1);
        }

    /* Open file for reading */
    fp = fopen(filename, "rb");
    if (fp == NULL)
        return (1);

    /* Read 8 bytes and check signature */
    rc = (fread(buf, 1, 8, fp) == 8) ? (png_sig_cmp(buf, 0, 8)) : (1);

    /* Close the file */
    fclose(fp);

    return (rc);
    }
