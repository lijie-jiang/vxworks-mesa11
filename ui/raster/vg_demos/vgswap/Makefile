# Makefile - Vector Graphics Demo Makefile
#
# Copyright (c) 2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 13nov14,yat  Created Vector Graphics swap demo (US46449)
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
C_SOURCE = gfxVgSwapDemo.c

OBJS = $(C_SOURCE:.c=.o)


include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers
