# Makefile - Event Devices Framework Touch Screen Calibration Demo src Makefile
#
# Copyright 2013-2014, Wind River Systems, Inc.
#      
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# -------------------- 
# 26may14,y_f  removed LAYER_REQUIRES FBDEV_COMMON (V7GFX-165)
# 28oct13,y_f  create

DOC_FILES = evdevDemoTsCal.c

ifeq ($(SPACE), user)
EXE = evdevDemoTsCal.vxe
include $(WIND_USR_MK)/defs.vsbvars.mk
else
LIB_BASE_NAME = evdevDemoTsCal
include $(WIND_KRNL_MK)/defs.vsbvars.mk
endif

ifdef _WRS_CONFIG_FBDEV_COMMON
OBJS = evdevDemoTsCal.o
else
LIB_BASE_NAME =
EXE =
OBJS =
endif

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.rtp.mk
else
include $(WIND_KRNL_MK)/rules.library.mk
endif