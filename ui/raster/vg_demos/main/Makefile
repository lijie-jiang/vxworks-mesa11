# Makefile - Vector Graphics main demo Makefile
#
# Copyright (c) 2013-2014, 2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 25feb16,yat  Suppress warnings for 3rd party font (US74892)
# 22dec14,yat  Remove mlong-calls for performance improvement (US46449)
# 08dec14,yat  Consolidate main using EXTRA_DEFINE (US46449)
# 25jun13,mgc  Modified for VxWorks 7 release


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

C_SOURCE = gfxVgMainDemo.c

OBJS = $(C_SOURCE:.c=.o)


include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers
