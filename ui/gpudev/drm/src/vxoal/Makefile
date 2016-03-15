# Makefile - vxoal Makefile
#
# Copyright (c) 2013-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 25feb16,yat  Add GFX_USE_PMAP for non COMPAT69 (US76256)
# 03jun15,rpc  porting DRM/i915 3.18 (US52495)
# 09mar15,qsn  Updated (US50613)
# 24jan14,mgc  Modified for VxWorks 7 release
# 25apr13,mgc  create

TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME=gfxDrm

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h 


ifdef _WRS_CONFIG_DRM_DEBUG_FLAG
EXTRA_DEFINE += -DGFX_USE_DEBUG
endif
EXTRA_DEFINE += -DCONFIG_DRM_I915_FBDEV -DCONFIG_DRM_I915_KMS -DCONFIG_INTEL_IOMMU -DCONFIG_VGA_ARB -D__ARCH_HAS_VGA_DEFAULT_DEVICE -DCONFIG_DMI -DCONFIG_PCI
ifdef _WRS_CONFIG_LP64
EXTRA_DEFINE += -DACPI_MACHINE_WIDTH=64
else
EXTRA_DEFINE += -DACPI_MACHINE_WIDTH=32
endif
ifndef _WRS_CONFIG_COMPAT69
EXTRA_DEFINE += -DGFX_USE_PMAP
endif

C_SOURCE = $(wildcard *.c)
OBJS = $(C_SOURCE:.c=.o)

include $(TGT_DIR)/h/make/rules.library

#CC_OPTIM=$(CC_OPTIM_NORMAL) -fno-strict-aliasing -Wno-unused-function -std=gnu9x -w
CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers


