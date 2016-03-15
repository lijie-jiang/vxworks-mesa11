# Makefile - Intel Graphics and Memory Controller frame buffer driver Makefile
#
# Copyright (c) 2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 14sep15,yat  Rename OBJS file (US66034)
# 11mar15,yat  Add show support (US55410)


TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME=gfxItlGmcFbShow

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h 
EXTRA_INCLUDE += -I../../h

OBJS = gfxItlGmcShow.o 

include $(TGT_DIR)/h/make/rules.library

#CC_OPTIM=$(CC_OPTIM_NORMAL) -fno-strict-aliasing -Wno-unused-function -std=gnu9x -w
#CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers
