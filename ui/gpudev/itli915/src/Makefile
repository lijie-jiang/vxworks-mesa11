# Makefile - I915 driver src Makefile
#
# Copyright (c) 2013-2015, Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 01jun15,rpc  porting needed files (US59495)
# 01a,20nov13,mgc  created (US48907)



TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME=gfxItlI915

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

BUILD_DIRS = show

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h
EXTRA_INCLUDE += -I../h
ifdef _WRS_CONFIG_DRM_DEBUG_FLAG
EXTRA_DEFINE += -DGFX_USE_DEBUG
endif
EXTRA_DEFINE += -std=gnu99 -DCONFIG_DRM_I915_FBDEV -DCONFIG_DRM_I915_KMS -DCONFIG_INTEL_IOMMU -DCONFIG_VGA_ARB -D__ARCH_HAS_VGA_DEFAULT_DEVICE -DCONFIG_DMI -DCONFIG_PCI
ifdef _WRS_CONFIG_LP64
EXTRA_DEFINE += -DACPI_MACHINE_WIDTH=64
else
EXTRA_DEFINE += -DACPI_MACHINE_WIDTH=32
endif

C_SOURCE = $(wildcard *.c)
OBJS = $(C_SOURCE:.c=.o)
include $(TGT_DIR)/h/make/rules.library

#CC_OPTIM=$(CC_OPTIM_NORMAL) -fno-strict-aliasing -Wno-unused-function -std=gnu9x -w
CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers

