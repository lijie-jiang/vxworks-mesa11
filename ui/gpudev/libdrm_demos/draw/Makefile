# Makefile for libDRM
#
# Copyright (c) 2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 16sep15,yat  Written (US24710)

ifeq ($(SPACE), user)
EXE = gfxKmsDrawDemo.vxe
ADDED_LIBS += -lgfxLibDRM
include $(WIND_USR_MK)/defs.library.mk
else
LIB_BASE_NAME = gfxLibDRMDemos
include $(WIND_KRNL_MK)/defs.library.mk
endif

ifdef _WRS_CONFIG_EVDEV_LIB
EXTRA_DEFINE += -DGFX_USE_EVDEV
endif

C_SOURCE = gfxKmsDrawDemo.c

OBJS = $(C_SOURCE:.c=.o)

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.rtp.mk
else
include $(WIND_KRNL_MK)/rules.library.mk
endif
