# Makefile - Mesa Layer Makefile
#
# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 12jan16,mze  clean up
# 14sep15,yat  Add shared public h (US24710)
# 22dec14,yat  Port Mesa to VxWorks 7 (US24705)

include $(WIND_KRNL_MK)/defs.layers.mk

BUILD_TOOLS=$(_WRS_CONFIG_gnu_VERSION)
BUILD_USER_TOOLS=$(_WRS_CONFIG_gnu_VERSION)
RTP_TOOLS=$(_WRS_CONFIG_gnu_VERSION)

SHARED_PUBLIC_H_DIRS = h
BUILD_DIRS = src
BUILD_USER_DIRS = src
POST_NOBUILD_CDFDIRS = cdf

include $(WIND_KRNL_MK)/rules.layers.mk
