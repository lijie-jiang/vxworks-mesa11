# Makefile - Mesa demos Layer Makefile
#
# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 12jan16,mze  clean up
# 06oct15,yat  Remove OpenGL ES1 demos (US67554)
# 22dec14,yat  Create demo for Mesa (US24712)
# 24jan14,mgc  Modified for VxWorks 7 release

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

BUILD_DIRS = main swap fbo gear
ifdef _MESA_DRI
BUILD_DIRS += esmain es2swap es2fbo es2gear es2logo es3swap
endif
BUILD_USER_DIRS =
POSTBUILD_RTP_DIRS = main mainso swap swapso fbo fboso gear gearso
ifdef _MESA_DRI
POSTBUILD_RTP_DIRS += esmain esmainso es2swap es2swapso es2fbo es2fboso es2gear es2gearso es2logo es2logoso es3swap es3swapso
endif
POST_NOBUILD_CDFDIRS = cdf

include $(WIND_KRNL_MK)/rules.layers.mk
