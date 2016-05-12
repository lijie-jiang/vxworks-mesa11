# Makefile - Event Devices Framework Libraries src Makefile
#
# Copyright 2013, Wind River Systems, Inc.
#      
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# -------------------- 
# 04jul13,y_f  create

TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME=evdevLib

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h -I../src


C_SOURCE = $(wildcard *.c)
DOC_FILES = $(C_SOURCE)
OBJS = $(C_SOURCE:.c=.o)

include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers