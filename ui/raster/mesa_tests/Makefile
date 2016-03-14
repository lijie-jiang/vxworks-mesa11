# Makefile - OpenGL ES tests Layer Makefile
#
# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 12jan16,mze  use new macros
# 06oct15,yat  Remove OpenGL ES1 demos (US67554)
# 22dec14,yat  Create test for Mesa (US24712)

include $(WIND_KRNL_MK)/defs.layers.mk

BUILD_TOOLS=$(_WRS_CONFIG_gnu_VERSION)
BUILD_USER_TOOLS=$(_WRS_CONFIG_gnu_VERSION)
RTP_TOOLS=$(_WRS_CONFIG_gnu_VERSION)

ifdef _WRS_CONFIG_MESA_GPUDEV_DRI_I915
_MESA_DRI=yes
endif
ifdef _WRS_CONFIG_MESA_GPUDEV_DRI_I965
_MESA_DRI=yes
endif

BUILD_DIRS = main polygon texturing
ifdef _MESA_DRI
BUILD_DIRS += esmain es2multigear es2multishader es2polygon es2texturing
endif
BUILD_USER_DIRS =
POSTBUILD_RTP_DIRS = main mainso polygon polygonso texturing texturingso
ifdef _MESA_DRI
POSTBUILD_RTP_DIRS += esmain esmainso es2multigear es2multigearso es2multishader es2multishaderso es2polygon es2polygonso es2texturing es2texturingso
endif
POST_NOBUILD_CDFDIRS = cdf

include $(WIND_KRNL_MK)/rules.layers.mk
