# Makefile - Graphics Framework FreeType2 Makefile
#
# Copyright (c) 2014, 2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 12jan16,mze  use new macros
# 24dec14,yya  Modified for VxWorks 7 release

# TODO: use DIAB to build freetype2
TGT_DIR=$(WIND_BASE)/target

include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = src

include $(TGT_DIR)/h/make/rules.library
