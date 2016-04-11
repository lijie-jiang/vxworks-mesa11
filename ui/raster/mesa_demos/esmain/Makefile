# Makefile - Main demo Makefile
#
# Copyright (c) 2014-2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 14sep15,yat  Add support for Mesa GPU DRI (US24710)
# 22dec14,yat  Create OpenGL demo for Mesa (US24712)

TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME = gfxMesaDemos

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h 

EXTRA_DEFINE += -D_MESA -D_ES2 -D_ES3
ifdef _WRS_CONFIG_MESA_MANGLE
EXTRA_DEFINE += -DUSE_MGL_NAMESPACE
endif

ifdef _WRS_CONFIG_MESA_GPUDEV_DRI_I915
_MESA_DRI=yes
ifndef _WRS_CONFIG_MESA_GPUDEV_DRI_I965
ifeq ($(SPACE), user)
ADDED_LIBS += -lgfxMesaDriI915
endif
endif
endif
ifdef _WRS_CONFIG_MESA_GPUDEV_DRI_I965
_MESA_DRI=yes
ifeq ($(SPACE), user)
ADDED_LIBS += -lgfxMesaDriI965
endif
endif

ifdef _MESA_DRI
EXTRA_DEFINE += -DGFX_USE_GBM
ifeq ($(SPACE), user)
ADDED_LIBS += -lgfxLibDRM
endif
endif

ifdef _WRS_CONFIG_EVDEV_LIB
EXTRA_DEFINE += -DGFX_USE_EVDEV
endif

C_SOURCE = gfxMesaEsMainDemo.c

OBJS = $(C_SOURCE:.c=.o)


#EXTRA_INCLUDE += -I. -I../h
include $(TGT_DIR)/h/make/rules.library

CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers



