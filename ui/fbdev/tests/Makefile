# Makefile - Frame buffer driver tests Layer Makefile
#
# Copyright (c) 2012, 2014, 2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 17feb16,yat  Add frame buffer copy test (US73758)
# 22dec14,yat  Added frame buffer capture test (US50456)
# 18jun14,yat  Added main demo for dynamic RTP (US11227)
# 24jan14,mgc  Modified for VxWorks 7 release
# 11may12,rfm  create

BUILD_DIRS = main console ioctl capture copy
POSTBUILD_RTP_DIRS = main mainso ioctl capture copy
POST_NOBUILD_CDFDIRS = cdf

include $(WIND_KRNL_MK)/rules.layers.mk
