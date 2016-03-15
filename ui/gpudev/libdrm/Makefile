# Makefile for libDRM
#
# Copyright (c) 2013-2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 16jun15,yat  Change to public directory h (US60452)
# 07jan15,yat  Port libDRM to VxWorks 7 (US24705)
# 20nov13,mgc  created

TGT_DIR=$(WIND_BASE)/target

DOC_OPTS = -mg -nostrict -missingok

include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = src



include $(TGT_DIR)/h/make/rules.library
