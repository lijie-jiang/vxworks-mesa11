# Makefile - libDRM Makefile
#
# Copyright (c) 2013-2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# -------------------- 
# 07jan15,mgc  Port libDRM to VxWorks 7 (US23263)
# 20nov13,mgc  create

TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME=gfxLibDRM

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h 

EXTRA_INCLUDE += -I../src
EXTRA_DEFINE += -DHAVE_CONFIG_H

C_SOURCE = xf86drm.c xf86drmHash.c xf86drmRandom.c xf86drmSL.c xf86drmMode.c
OBJS = $(C_SOURCE:.c=.o)

include $(TGT_DIR)/h/make/rules.library

#CC_OPTIM=$(CC_OPTIM_NORMAL) -fno-strict-aliasing -Wno-unused-function -std=gnu9x -w
CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers



