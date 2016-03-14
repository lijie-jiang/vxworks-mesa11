# Makefile - Mesa Makefile
#
# Copyright (c) 2013-2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 15sep15,yat  Add Mesa DRI i965 (US24710)
# 22dec14,yat  Port Mesa to VxWorks 7 (US24705)

SUBDIRS = common swrast i965

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.library.mk 
else
include $(WIND_KRNL_MK)/rules.library.mk
endif
