# Makefile - makefile for evdev test lib
#
# Copyright (c) 2015 Wind River Systems, Inc.
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
#*/

LIB_BASE_NAME = vxTest_EVDEV_LIB

# Defined SDA setting here as the SDA flags will be computed when including
# following make rules.

SDA_DISABLE = TRUE

# Including defs.default.mk to see if _WRS_CONFIG_xxx is defined.
# It must be after LIB_BASE_NAME
include $(WIND_KRNL_MK)/defs.default.mk

EXCLUDE_DIRS = tmp
SUBDIRS = $(filter-out $(EXCLUDE_DIRS),$(basename $(patsubst ./%,%,$(shell find . -maxdepth 1 -type d))))

C_EXCLUDE = 


C_SOURCE = $(filter-out $(C_EXCLUDE),$(wildcard *.c))

OBJS = $(C_SOURCE:.c=.o)
RTP_TEST_OBJS = $(C_SOURCE:.c=.o)
KERNEL_TEST_OBJS = $(C_SOURCE:.c=.o)

include $(WIND_KRNL_MK)/rules.vxTest.mk

