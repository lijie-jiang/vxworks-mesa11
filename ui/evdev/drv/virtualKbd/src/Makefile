# Makefile - VxWorks Virtual Keyboard Driver src Makefile
#
# Copyright 2014, Wind River Systems, Inc.
#      
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# -------------------- 
# 16may14,yat  Add virtual keyboard driver. (US24741)
TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME=evdevDrvVirtualKbd

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h -I../src


OBJS = evdevDrvVirtualKbd.o

include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers