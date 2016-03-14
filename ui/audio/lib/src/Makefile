# Makefile - Audio Libraries src Makefile
#
# Copyright 2013-2014, Wind River Systems, Inc.
#      
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# -------------------- 
# 20mar14,y_f  add user space build
# 30oct13,y_f  written

ifeq ($(SPACE), user)
C_SOURCE = audioLibWav.c
else
C_SOURCE = audioLibCore.c audioLibWav.c
endif

OBJS = $(C_SOURCE:.c=.o)
LIB_FORMAT = both
LIB_BASE_NAME = audioLib

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.library.mk 
else
include $(WIND_KRNL_MK)/rules.library.mk
endif