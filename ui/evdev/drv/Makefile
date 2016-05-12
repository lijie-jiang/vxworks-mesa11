# Makefile - Event Devices Drivers Makefile
#
# Copyright 2013, 2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 12jan16,mze  remove dead macros
# 03jul13,y_f  create

TGT_DIR=$(WIND_BASE)/target


include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = virtualKbd virtualPtr


include $(TGT_DIR)/h/make/rules.library