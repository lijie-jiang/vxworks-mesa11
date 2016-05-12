# Makefile - Font Layer Makefile
#
# Copyright (c) 2014, 2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 23feb16,mze  updated macros and includes
# 24dec14,yya  created


TGT_DIR=$(WIND_BASE)/target

include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = freetype2

include $(TGT_DIR)/h/make/rules.library
