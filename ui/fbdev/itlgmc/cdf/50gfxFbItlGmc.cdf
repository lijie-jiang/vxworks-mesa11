/* 50gfxFbItlGmc.cdf - Intel Graphics and Memory Controller frame buffer driver */

/*
 * Copyright (c) 2015, Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River License agreement.
 */

/*
modification history
--------------------
02dec15,yat  Add support for INCLUDE_FBDEV_INIT (US71171)
*/

Component INCLUDE_FBDEV_CONSOLE {
    REQUIRES        INCLUDE_FBDEV_INIT
}

Component INCLUDE_FBDEV_SPLASH {
    REQUIRES        INCLUDE_FBDEV_INIT
}
