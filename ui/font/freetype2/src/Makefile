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

include $(TGT_DIR)/h/make/defs.library
LIB_BASE_NAME = gfxFreeType2
ifeq ($(CPU), ARMARCH7)
EXTRA_DEFINE += -mlong-calls
endif

# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = autofit base bdf cache cff cid gxvalid gzip lzw otvalid pcf pfr psaux pshinter psnames raster sfnt
SUBDIRS += smooth tools truetype type1 type42 winfonts

OBJS = $(C_SOURCE:.c=.o)

include $(TGT_DIR)/h/make/rules.library
CC_WARNINGS = $(CC_WARNINGS_NONE)