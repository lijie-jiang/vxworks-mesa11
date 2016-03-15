# Makefile - Mesa Makefile
#
# Copyright (c) 2013-2015 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 15sep15,yat  Add Mesa DRI i965 (US24710)
# 22dec14,yat  Port Mesa to VxWorks 7 (US24705)



TGT_DIR=$(WIND_BASE)/target


DOC_OPTS = -mg -nostrict -missingok

include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = common swrast i965



include $(TGT_DIR)/h/make/rules.library

