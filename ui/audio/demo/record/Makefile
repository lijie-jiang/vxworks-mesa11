# Makefile - Audio Driver Framework Record Demo Makefile
#
# Copyright 2014, Wind River Systems, Inc.
#      
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# -------------------- 
# 14feb14,y_f  create

ifeq ($(SPACE), user)
EXE = audioDemoRecord.vxe
ADDED_LIBS += -laudioLib
else
LIB_BASE_NAME = audioDemoRecord
endif

OBJS = audioDemoRecord.o

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.rtp.mk
else
include $(WIND_KRNL_MK)/rules.library.mk
endif