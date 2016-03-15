# Makefile - Intel Graphics and Memory Controller frame buffer driver Makefile
#
# Copyright (c) 2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 20dec14,qsn  Initial VxWorks 7 release (US48907)

BUILD_DIRS = src

TGT_DIR=$(WIND_BASE)/target

DOC_OPTS = -mg -nostrict -missingok

include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = src



include $(TGT_DIR)/h/make/rules.library
