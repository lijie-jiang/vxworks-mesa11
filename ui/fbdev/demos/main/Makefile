# Makefile - Frame buffer main demo Makefile
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



TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME = gfxFbDemos

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h 

ifdef _WRS_CONFIG_FBDEV_TASK
EXTRA_DEFINE += -DGFX_USE_FBDEV_TASK
endif


C_SOURCE = gfxFbMainDemo.c

OBJS = $(C_SOURCE:.c=.o)

#EXTRA_INCLUDE += -I. -I../h
include $(TGT_DIR)/h/make/rules.library

CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers
