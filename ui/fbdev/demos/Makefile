# Makefile - Frame buffer driver demo Layer Makefile
#
# Copyright (c) 2014 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# -------------------- 
# 18jun14,yat  Added main demo for dynamic RTP (US11227)
# 24jan14,mgc  Modified for VxWorks 7 release



TGT_DIR=$(WIND_BASE)/target


DOC_OPTS = -mg -nostrict -missingok
include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

SUBDIRS = main draw

export _WRS_CONFIG_FBDEV_TASK=1

include $(TGT_DIR)/h/make/rules.library