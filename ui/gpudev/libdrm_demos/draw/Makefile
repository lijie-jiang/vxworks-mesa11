# Makefile for libDRM
#
# Copyright (c) 2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 16sep15,yat  Written (US24710)

TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME = gfxLibDRMDemos

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h 
ifdef _WRS_CONFIG_EVDEV_LIB
EXTRA_DEFINE += -DGFX_USE_EVDEV
endif
ifdef _WRS_CONFIG_FBDEV_TASK
EXTRA_DEFINE += -DGFX_USE_FBDEV_TASK
endif


C_SOURCE = gfxKmsDrawDemo.c

OBJS = $(C_SOURCE:.c=.o)

#EXTRA_INCLUDE += -I. -I../h
include $(TGT_DIR)/h/make/rules.library

CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers
