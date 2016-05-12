# Makefile - Vector Graphics demos Layer Makefile
#
# Copyright (c) 2013-2014, 2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 05feb16,yat  Add BUILD_USER_DIRS with no directory
# 12jan16,mze  clean up
# 22dec14,yat  Add Intel and LP64 support (US50456)
# 18jun14,yat  Added main demo for dynamic RTP (US11227)
# 21feb14,pad  The RTP version of these demos must not be built when the VSB
#              does not enable the user space networking layer (issue
#              VXW7-1904).
# 24jan14,mgc  Modified for VxWorks 7 release
# 05oct12,mze  create
TGT_DIR=$(WIND_BASE)/target
DOC_OPTS = -mg -nostrict -missingok

include $(TGT_DIR)/h/make/defs.library
BUILD_DIRS = main vgswap flow image scroll pattern subway

include $(TGT_DIR)/h/make/rules.library
