# Makefile - Frame buffer main demo Makefile
#
# Copyright (c) 2013-2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 14sep15,yat  Remove build restriction for ARCH_i86 dynamic RTP (US24710)
# 07jul15,yat  Change IA CPU to use ARCH_i86 (V7GFX-267)
# 22dec14,yat  Enable static RTP only for Intel (US50456)
# 22dec14,yat  Remove mlong-calls for performance improvement (US46449)
# 18jun14,yat  Added main demo for dynamic RTP (US11227)
# 24jan14,mgc  Modified for VxWorks 7 release

ifeq ($(SPACE), user)
EXTRA_INCLUDE += -I../draw
EXE = gfxFbMainSoDemo.vxe
include $(WIND_USR_MK)/defs.library.mk
EXE_FORMAT = dynamic
else
LIB_BASE_NAME = gfxFbDemos
include $(WIND_KRNL_MK)/defs.library.mk
endif

ifdef _WRS_CONFIG_EVDEV_LIB
EXTRA_DEFINE += -DGFX_USE_EVDEV
endif

C_SOURCE = gfxFbMainSoDemo.c

OBJS = $(C_SOURCE:.c=.o)

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.rtp.mk
else
include $(WIND_KRNL_MK)/rules.library.mk
endif
