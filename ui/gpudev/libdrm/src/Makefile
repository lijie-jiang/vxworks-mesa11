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

BUILD_DIRS = intel
BUILD_USER_DIRS = intel

EXTRA_INCLUDE += -I./
EXTRA_DEFINE += -DHAVE_CONFIG_H

C_SOURCE = xf86drm.c xf86drmHash.c xf86drmRandom.c xf86drmSL.c xf86drmMode.c
OBJS = $(C_SOURCE:.c=.o)

LIB_FORMAT = both
LIB_BASE_NAME = gfxLibDRM

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.library.mk 
else
include $(WIND_KRNL_MK)/rules.library.mk
endif

#
# libdrm is considered 3rd party code
# all compiler warnings will be suppressed by the following
#
CC_WARNINGS = $(CC_WARNINGS_NONE)
