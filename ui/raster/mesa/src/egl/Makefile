# Makefile - Mesa Makefile
#
# Copyright (c) 2013-2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 22dec14,yat  Port Mesa to VxWorks 7 (US24705)

SUBDIRS = drivers main

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.library.mk 
else
include $(WIND_KRNL_MK)/rules.library.mk
endif
