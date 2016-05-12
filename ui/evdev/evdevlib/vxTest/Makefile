# Makefile - makefile for evdev test library
#
# Copyright (c) 2013, 2015 Wind River Systems, Inc.
#      
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# -------------------- 
# 19Nov15,jnl  create 
#
# DESCRIPTION
# This file contains the makefile rules for building the evdev test library
#

LIB_BASE_NAME = vxTest


# Including defs.default.mk to see if _WRS_CONFIG_xxx is defined.
# It must be after LIB_BASE_NAME
include $(WIND_KRNL_MK)/defs.default.mk



ifeq ($(SPACE), user)
EXCLUDE_SUBDIRS = user_src cdf
else
EXCLUDE_SUBDIRS = user_src cdf
endif


C_EXCLUDE = 
C_SOURCE = $(filter-out $(C_EXCLUDE),$(wildcard *.c))

OBJS = $(C_SOURCE:.c=.o)
RTP_TEST_OBJS = $(C_SOURCE:.c=.o)
KERNEL_TEST_OBJS = $(C_SOURCE:.c=.o)

include $(WIND_KRNL_MK)/rules.vxTest.mk

