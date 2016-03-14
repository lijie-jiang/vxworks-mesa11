# Makefile - Intel Graphics Controller frame buffer driver src Makefile
#
# Copyright (c) 2013-2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 24jan14,mgc  Modified for VxWorks 7 release
# 11may12,rfm  create

OBJS = gfxItlgcIosDrv.o gfxItlgcVxbDrv.o  

EXTRA_INCLUDE += -I. -I../h

LIB_BASE_NAME = gfxItlgcFb

include $(WIND_KRNL_MK)/rules.library.mk
