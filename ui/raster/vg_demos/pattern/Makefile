# Makefile - Vector Graphics Demo Makefile
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
# 18jun14,yat  Remove unused image library (US11227)
# 21feb14,pad  The RTP version of this demo must be linked against libnet
#              (issue VXW7-1984).
# 25jun13,mgc  Modified for VxWorks 7 release
# 11may12,rfm  create

TGT_DIR=$(WIND_BASE)/target
LIB_BASE_NAME=gfxVgDemos

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library
ifdef _WRS_CONFIG_EVDEV_LIB
EXTRA_DEFINE += -DGFX_USE_EVDEV
endif
DOC_FILES=
EXTRA_INCLUDE += -I$(TGT_DIR)/src/ui/h

C_SOURCE = gfxVgPatternDemo.c

OBJS = $(C_SOURCE:.c=.o)


include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers

