# Makefile - Frame buffer console test Makefile
#
# Copyright (c) 2013-2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 22dec14,yat  Remove mlong-calls for performance improvement (US46449)
# 24jan14,mgc  Modified for VxWorks 7 release

ifeq ($(SPACE), user)
else
LIB_BASE_NAME = gfxFbTests
endif

C_SOURCE = gfxConsoleTest.c

OBJS = $(C_SOURCE:.c=.o)

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.rtp.mk
else
include $(WIND_KRNL_MK)/rules.library.mk
endif
