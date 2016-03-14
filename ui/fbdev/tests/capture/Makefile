# Makefile - Frame buffer capture test Makefile
#
# Copyright (c) 2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 22dec14,yat  Created frame buffer capture test (US50456)

ifeq ($(SPACE), user)
EXE = gfxFbCaptureTest.vxe
else
LIB_BASE_NAME = gfxFbTests
endif

C_SOURCE = gfxFbCaptureTest.c

OBJS = $(C_SOURCE:.c=.o)

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.rtp.mk
else
include $(WIND_KRNL_MK)/rules.library.mk
endif
