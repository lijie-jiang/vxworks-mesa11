# Makefile - Mesa Makefile
#
# Copyright (c) 2013-2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 14sep15,yat  Add support for Mesa GPU DRI (US24710)
# 22dec14,yat  Port Mesa to VxWorks 7 (US24705)

LIB_BASE_NAME = gfxMesa
TGT_DIR=$(WIND_BASE)/target
LIB_BASE_NAME = gfxMesa
# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library


EXTRA_INCLUDE += -I../../../include -I../../mesa -I../../mapi -I../.. -I../../gallium/auxiliary -I../../gallium/include \
               -I../../../../../khronos/h
EXTRA_DEFINE += -DHAVE_PTHREAD=1

ifdef _WRS_CONFIG_MESA_GPUDEV_DRI_I915
_MESA_DRI=yes
endif
ifdef _WRS_CONFIG_MESA_GPUDEV_DRI_I965
_MESA_DRI=yes
endif

ifdef _MESA_DRI
EXTRA_DEFINE += -D_EGL_BUILT_IN_DRIVER_DRI2 -DHAVE_DRM_PLATFORM
else
EXTRA_DEFINE += -D__NOT_HAVE_DRM_H
endif

ifdef _WRS_CONFIG_MESA_MANGLE
EXTRA_DEFINE += -DUSE_MGL_NAMESPACE
endif

C_SOURCE = $(wildcard *.c)
CPP_SOURCE = $(wildcard *.cpp)

OBJS = $(C_SOURCE:.c=.o)
OBJS += $(CPP_SOURCE:.cpp=.o)


include $(TGT_DIR)/h/make/rules.library

#CC_OPTIM=$(CC_OPTIM_NORMAL) -fno-strict-aliasing -Wno-unused-function -std=gnu9x -w
CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers
