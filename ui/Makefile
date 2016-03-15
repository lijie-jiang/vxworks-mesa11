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

LIB_BASE_NAME=ui

OBJS=
export _WRS_CONFIG_MESA_GPUDEV_DRI_I965=y
export _WRS_CONFIG_COMPAT69=y
export _WRS_CONFIG_VXBUS_LEGACY=y
DOC_OPTS = -mg -nostrict -missingok

include $(TGT_DIR)/h/make/defs.library
SUBDIRS = fbdev gpudev raster

include $(TGT_DIR)/h/make/rules.library
