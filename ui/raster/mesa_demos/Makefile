# Makefile - Mesa demos Layer Makefile
#
# Copyright (c) 2014-2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 12jan16,mze  clean up
# 06oct15,yat  Remove OpenGL ES1 demos (US67554)
# 22dec14,yat  Create demo for Mesa (US24712)
# 24jan14,mgc  Modified for VxWorks 7 release


TGT_DIR=$(WIND_BASE)/target


DOC_OPTS = -mg -nostrict -missingok
include $(TGT_DIR)/h/make/defs.library



# Ensure the methods sub-directory comes to the top of the list, so that
# the rest of the sub-directories are built after vxbMethodDecl.h is
# updated if a new VxBus method file is added in the methods directory.

 
ifdef _WRS_CONFIG_MESA_GPUDEV_DRI_I915
_MESA_DRI=yes
endif
ifdef _WRS_CONFIG_MESA_GPUDEV_DRI_I965
_MESA_DRI=yes
endif

SUBDIRS = main swap fbo gear
ifdef _MESA_DRI
SUBDIRS += esmain es2swap es2fbo es2gear es2logo es3swap
endif

include $(TGT_DIR)/h/make/rules.library