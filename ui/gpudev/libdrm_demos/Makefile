# Makefile for libDRM
#
# Copyright (c) 2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 16sep15,yat  Written (US24710)



TGT_DIR=$(WIND_BASE)/target

DOC_OPTS = -mg -nostrict -missingok
include $(TGT_DIR)/h/make/defs.library

# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS =  draw

include $(TGT_DIR)/h/make/rules.library
