# Makefile - DRM src Makefile
#
# Copyright (c) 2013-2015, Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 15jul15,yat  Port original drm_sysfs.c (US60452)
# 22jan15,qsn  Port DRM to VxWorks 7 (US50702)
# 20nov13,mgc  create

BUILD_DIRS = vxoal show

LIB_BASE_NAME = gfxDrm
include $(WIND_KRNL_MK)/defs.library.mk

ifdef _WRS_CONFIG_DRM_DEBUG_FLAG
EXTRA_DEFINE += -DGFX_USE_DEBUG
endif
EXTRA_DEFINE += -DCONFIG_DRM_I915_FBDEV -DCONFIG_DRM_I915_KMS -DCONFIG_INTEL_IOMMU -DCONFIG_VGA_ARB -D__ARCH_HAS_VGA_DEFAULT_DEVICE -DCONFIG_DMI -DCONFIG_PCI
ifdef _WRS_CONFIG_LP64
EXTRA_DEFINE += -DACPI_MACHINE_WIDTH=64
else
EXTRA_DEFINE += -DACPI_MACHINE_WIDTH=32
endif

C_SOURCE = $(wildcard *.c)
OBJS = $(C_SOURCE:.c=.o)

include $(WIND_KRNL_MK)/rules.library.mk

#
# drm is considered 3rd party code
# all compiler warnings will be suppressed by the following
#
CC_WARNINGS = $(CC_WARNINGS_NONE)
