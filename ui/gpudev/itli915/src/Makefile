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

BUILD_DIRS = show

LIB_BASE_NAME = gfxItlI915
include $(WIND_KRNL_MK)/defs.library.mk

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

include $(WIND_KRNL_MK)/rules.library.mk

#
# i915 is considered 3rd party code
# all compiler warnings will be suppressed by the following
#
CC_WARNINGS = $(CC_WARNINGS_NONE)
