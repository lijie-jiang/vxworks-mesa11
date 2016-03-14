# Makefile - Intel Graphics and Memory Controller frame buffer driver src Makefile
#
# Copyright (c) 2014-2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 14sep15,yat  Rename OBJS files (US66034)
# 11mar15,yat  Add show and FBDEV task support (US55410)
# 20dec14,qsn  Initial VxWorks 7 release (US48907)

BUILD_DIRS = src

LIB_BASE_NAME = gfxItlGmcFb
include $(WIND_KRNL_MK)/defs.library.mk

ifdef _WRS_CONFIG_FBDEV_TASK
EXTRA_DEFINE += -DGFX_USE_FBDEV_TASK
endif

OBJS = gfxItlGmcIosDrv.o gfxItlGmc.o

EXTRA_INCLUDE += -I. -I../h

include $(WIND_KRNL_MK)/rules.library.mk
