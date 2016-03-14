# Makefile - DRM Layer Makefile
#
# Copyright 2013-2015, Wind River Systems, Inc.
#      
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 22jan15,qsn  Port DRM to VxWorks 7 (US50702)
# 20nov13,mgc  created

include $(WIND_KRNL_MK)/defs.layers.mk

BUILD_TOOLS=$(_WRS_CONFIG_gnu_VERSION)

KERNEL_PUBLIC_H_DIRS = h
BUILD_DIRS = src
POST_NOBUILD_CDFDIRS = cdf
POST_NOBUILD_CFGDIRS = cfg

include $(WIND_KRNL_MK)/rules.layers.mk
