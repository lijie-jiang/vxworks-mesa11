# Makefile - DRM Layer Makefile
#
# Copyright 2013-2015, Wind River Systems, Inc.
#      
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 22jan15,qsn  Port DRM to VxWorks 7 (US50702)
# 20nov13,mgc  created

TGT_DIR=$(WIND_BASE)/target

export _WRS_CONFIG_COMPAT69=y
DOC_OPTS = -mg -nostrict -missingok

include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = src



include $(TGT_DIR)/h/make/rules.library
