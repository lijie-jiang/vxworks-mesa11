# Makefile - Frame buffer copy test Makefile
#
# Copyright (c) 2013-2014, 2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 17feb16,yat  Add frame buffer copy test (US73758)
# 22dec14,yat  Remove mlong-calls for performance improvement (US46449)
# 24jan14,mgc  Modified for VxWorks 7 release

ifeq ($(SPACE), user)
EXE = gfxFbCopyTest.vxe
include $(WIND_USR_MK)/defs.library.mk
else
LIB_BASE_NAME = gfxFbTests
include $(WIND_KRNL_MK)/defs.library.mk
endif

EXTRA_DEFINE += -D_FBDEV
ifdef _WRS_CONFIG_EVDEV_LIB
EXTRA_DEFINE += -DGFX_USE_EVDEV
endif

C_SOURCE = gfxFbCopyTest.c

OBJS = $(C_SOURCE:.c=.o)

#ifeq ($(CPU), ARMARCH7)
#EXTRA_DEFINE += -mfpu=neon -mfloat-abi=softfp -DUSE_NEON
#OBJS += gfxNeonCopy.o
#endif

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.rtp.mk
else
include $(WIND_KRNL_MK)/rules.library.mk
endif
