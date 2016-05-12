# Makefile - Graphics Framework PNG Makefile
#
# Copyright (c) 2013-2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 22dec14,yat  Remove mlong-calls for performance improvement (US46449)
# 24jan14,mgc  Modified for VxWorks 7 release
# 11may12,rfm  create


TGT_DIR=$(WIND_BASE)/target
LIB_BASE_NAME=gfxPng

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

EXTRA_INCLUDE += -I$(TGT_DIR)/src/ui/h

C_SOURCE = png.c pngerror.c pngget.c pngmem.c pngpread.c pngread.c pngrio.c pngrtran.c pngrutil.c pngset.c pngtrans.c pngwio.c pngwrite.c pngwtran.c pngwutil.c

OBJS = $(C_SOURCE:.c=.o)   


include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers
