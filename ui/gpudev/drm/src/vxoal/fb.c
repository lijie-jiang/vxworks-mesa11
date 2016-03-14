/* fb.c - DRM frame buffer stub functions */

/*
 * Copyright (c) 2015 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
14sep15,yat  Clean up code (US66034)
06mar15,yat  Add drm_fb_set_options and fb_get_options
*/

/*
DESCRIPTION

This file provides DRM frame buffer stub functions.
FBDEV sets the option and DRM gets the option.
*/

/* includes */

#include <string.h> /* for strncpy */
#include <stdlib.h> /* for calloc */
#include <vxoal/krnl/device.h> /* for device */
#include <vxoal/krnl/fb.h> /* for extern */

/* defines */

#define FB_MAX_STR_CHARS 128

/* locals */

static char fb_name[FB_MAX_STR_CHARS] = "";
static char fb_option[FB_MAX_STR_CHARS] = "";

/*******************************************************************************
 *
 * drm_fb_set_options - set the frame buffer name and option
 *
 * RETURNS: N/A
 *
 */
void drm_fb_set_options(const char *name, const char *option)
    {
    (void)strncpy(fb_name, name, FB_MAX_STR_CHARS-1);
    fb_name[FB_MAX_STR_CHARS-1] = '\0';

    (void)strncpy(fb_option, option, FB_MAX_STR_CHARS-1);
    fb_option[FB_MAX_STR_CHARS-1] = '\0';
    }

/*******************************************************************************
 *
 * fb_get_options - get the option based on the frame buffer name
 *  
 * RETURNS:
 *  
 */
int fb_get_options(const char *name, char **option)
    {
    if ((strlen(fb_name) > 1) && strstr(name, fb_name))
        {
        *option = fb_option;
        return 0;
        }

    *option = NULL;
    return 1;
    }

/**
 * framebuffer_alloc - creates a new frame buffer info structure
 *
 * @size: size of driver private data, can be zero
 * @dev: pointer to the device for this fb, this can be NULL
 *
 * Returns the new structure, or NULL if an error occurred.
 *
 */
struct fb_info *framebuffer_alloc(size_t size, struct device *dev)
    {
    struct fb_info *info;

    info = calloc(1, sizeof (struct fb_info));
    if (info == NULL)
        return NULL;
    info->device = dev;
    return info;
    }

/**
 * framebuffer_release - frees the fb_info structure
 *
 * @info: frame buffer info structure
 *
 */
void framebuffer_release(struct fb_info *info)
    {
    if (!info)
        return;

    free(info);
    }


void cfb_fillrect(struct fb_info *info, const struct fb_fillrect *rect) {}
void cfb_copyarea(struct fb_info *info, const struct fb_copyarea *area) {}
void cfb_imageblit(struct fb_info *info, const struct fb_image *image)  {}

