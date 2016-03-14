# Makefile - makefile for ui/audio test library
#
# Copyright (c) 2013-2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 18jan15,zjl   update for vxTest
# 06aug14,yyl   Remove IO from EXCLUDE_SUBDIRS for CERT
# 06aug14,l_w   disable edr for CERT
# 17jun14,b_c   rtp builds depends on _WRS_CONFIG_RTP(VXW7-3152)
# 22apr14,b_c   Define SDA_DISABLE
# 10mar14,elp   Fixed SAFETY typo and vxsim build
# 20jan14,b_c   Update for safety test library
# 22jul13,l_w   create
#
# DESCRIPTION
# This file contains the makefile rules for building the core OS test library
#
#*/

LIB_BASE_NAME = vxTest


# Including defs.default.mk to see if _WRS_CONFIG_xxx is defined.
# It must be after LIB_BASE_NAME
include $(WIND_KRNL_MK)/defs.default.mk



ifeq ($(SPACE), user)
EXCLUDE_SUBDIRS = src h
else
EXCLUDE_SUBDIRS = user_src  h
endif


C_EXCLUDE =
C_SOURCE = $(filter-out $(C_EXCLUDE),$(wildcard *.c))

OBJS = $(C_SOURCE:.c=.o)
RTP_TEST_OBJS = $(C_SOURCE:.c=.o)
KERNEL_TEST_OBJS = $(C_SOURCE:.c=.o)

include $(WIND_KRNL_MK)/rules.vxTest.mk

