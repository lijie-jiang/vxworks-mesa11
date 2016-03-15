# Makefile - Splash screen Makefile
#
# Copyright (c) 2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 24jan14,mgc  Modified for VxWorks 7 release


TGT_DIR=$(WIND_BASE)/target
LIB_BASE_NAME = gfxSplash

DOC_FILES=

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h 


OBJS = gfxSplash.o

include $(TGT_DIR)/h/make/rules.library

#CC_OPTIM=$(CC_OPTIM_NORMAL) -fno-strict-aliasing -Wno-unused-function -std=gnu9x -w
CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers
