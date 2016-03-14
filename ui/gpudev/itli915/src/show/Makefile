# Makefile - show Makefile
#
# Copyright (c) 2015, Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 31jul15,yat  Written (US60452)

LIB_BASE_NAME = gfxItlI915Show
include $(WIND_KRNL_MK)/defs.library.mk

EXTRA_INCLUDE += -I.. -I../../h

ifdef _WRS_CONFIG_DRM_DEBUG_FLAG
EXTRA_DEFINE += -DGFX_USE_DEBUG
endif
EXTRA_DEFINE += -std=gnu99 -DCONFIG_DRM_I915_FBDEV -DCONFIG_DRM_I915_KMS -DCONFIG_INTEL_IOMMU -DCONFIG_VGA_ARB -D__ARCH_HAS_VGA_DEFAULT_DEVICE -DCONFIG_DMI -DCONFIG_PCI
ifdef _WRS_CONFIG_LP64
EXTRA_DEFINE += -DACPI_MACHINE_WIDTH=64
else
EXTRA_DEFINE += -DACPI_MACHINE_WIDTH=32
endif

OBJS = i915_vxworks_show.o

include $(WIND_KRNL_MK)/rules.library.mk

#
# i915 is considered 3rd party code
# all compiler warnings will be suppressed by the following
#
CC_WARNINGS = $(CC_WARNINGS_NONE)
