# Makefile - Vector Graphics EGL Makefile
#
# Copyright (c) 2013-2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 22dec14,yat  Remove mlong-calls for performance improvement (US46449)
# 24jan14,mgc  Modified for VxWorks 7 release
# 11may12,rfm  create
TGT_DIR=$(WIND_BASE)/target
LIB_BASE_NAME = gfxVg
# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

C_SOURCE = egl.c 

OBJS = $(C_SOURCE:.c=.o)     

EXTRA_INCLUDE += -I.. -I../EGL -I../VG -I$(TGT_DIR)/src/ui/h
 

include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers
