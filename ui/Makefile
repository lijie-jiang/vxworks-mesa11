# Makefile - Raster Layer Makefile
#
# Copyright (c) 2013-2014, 2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 12jan16,mze  remove dead macros
# 24jan14,mgc  Modified for VxWorks 7 release

TGT_DIR=$(WIND_BASE)/target
export _WRS_CONFIG_MESA_GPUDEV_DRI_I965=y
export _WRS_CONFIG_MESA_MANGLE=y
export _WRS_CONFIG_COMPAT69=y
export _WRS_CONFIG_VXBUS_LEGACY=y
export _WRS_CONFIG_FBDEV_TASK=y
DOC_OPTS = -mg -nostrict -missingok

include $(TGT_DIR)/h/make/defs.library
SUBDIRS = evdev fbdev font image gpudev raster xml zlib

include $(TGT_DIR)/h/make/rules.library
