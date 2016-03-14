# Makefile - Sample frame buffer driver src Makefile
#
# Copyright (c) 2013-2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 22dec14,yat  Rework pmap of frame buffer for RTP restart (US46449)
# 24jan14,mgc  Modified for VxWorks 7 release

EXTRA_INCLUDE += -I. -I../h

LIB_BASE_NAME = gfxSampleFb
include $(WIND_KRNL_MK)/defs.library.mk

ifdef _WRS_CONFIG_COMPAT69
EXTRA_DEFINE += -D_COMPAT69
endif

OBJS = gfxSampleIosDrv.o gfxSample.o

include $(WIND_KRNL_MK)/rules.library.mk
