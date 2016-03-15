# Makefile - Wind River Graphics frame buffer driver interface Makefile
#
# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 24feb16,yat  Remove unused BUILD_USER_DIRS (US75033)
# 16apr15,yat  Add BUILD_DIRS and BUILD_USER_DIRS for fbdev docs fix (V7GFX-245)
# 24jan14,mgc  Modified for VxWorks 7 release


TGT_DIR=$(WIND_BASE)/target

DOC_OPTS = -mg -nostrict -missingok

include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = src



include $(TGT_DIR)/h/make/rules.library
