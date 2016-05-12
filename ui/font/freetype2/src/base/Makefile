# Makefile - Graphics Framework FreeType2 Makefile
#
# Copyright (c) 2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 24dec14,yya  Modified for VxWorks 7 release


TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME=gfxFreeType2

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library
ifeq ($(CPU), ARMARCH7)
EXTRA_DEFINE += -mlong-calls
endif
DOC_FILES=

EXTRA_INCLUDE+= -I$(TGT_DIR)/src/ui/font/freetype2/h

C_SOURCE = ftinit.c ftbase.c ftbbox.c ftsystem.c ftglyph.c ftbitmap.c

OBJS = $(C_SOURCE:.c=.o)

include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers
#
# freetype2 is considered 3rd party code
# all compiler warnings will be suppressed by the following
#
CC_OPTIM= -w
