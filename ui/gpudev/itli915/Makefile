# Makefile - I915 driver layer Makefile
#
# Copyright 2013-2015, Wind River Systems, Inc.
#      
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 14sep15,yat  Add cfg (US66034)
# 22jan15,qsn  Port Intel HD graphics driver to VxWorks 7 (US50702)
# 20nov13,mgc  created (US48907)

include $(WIND_KRNL_MK)/defs.layers.mk

BUILD_TOOLS=$(_WRS_CONFIG_gnu_VERSION)

BUILD_DIRS = src
POST_NOBUILD_CFGDIRS = cfg
POST_NOBUILD_CDFDIRS = cdf

include $(WIND_KRNL_MK)/rules.layers.mk
