# Makefile - I915 driver layer Makefile
#
# Copyright 2013-2015, Wind River Systems, Inc.
#      
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 14sep15,yat  Add cfg (US66034)
# 22jan15,qsn  Port Intel HD graphics driver to VxWorks 7 (US50702)
# 20nov13,mgc  created (US48907)


TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME=drm

OBJS=

DOC_OPTS = -mg -nostrict -missingok

include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = src



include $(TGT_DIR)/h/make/rules.library

