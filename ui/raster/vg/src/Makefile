# Makefile - Graphics Framework VG Library Makefile
#
# Copyright (c) 2013-2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 22dec14,yat  Enable GFX_THREAD_VARIABLE for thread variable (US50977)
# 22dec14,yat  Remove mlong-calls for performance improvement (US46449)
# 24jan14,mgc  Modified for VxWorks 7 release
# 11may12,rfm  create

# Enable graphics context as thread variable instead of static variable
TGT_DIR=$(WIND_BASE)/target
LIB_BASE_NAME=gfxVg

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

EXTRA_DEFINE += -DGFX_THREAD_VARIABLE

C_SOURCE = gfxVgRaster.c

OBJS = $(C_SOURCE:.c=.o)

EXTRA_INCLUDE += -I. -IEGL -IVG -I$(TGT_DIR)/src/ui/h

SUBDIRS = EGL GLU VG

include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers
