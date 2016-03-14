# Makefile - OpenGL Fbo Demo Makefile
#
# Copyright (c) 2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 13jan16,yat  Written (US24710)

ifeq ($(SPACE), user)
EXTRA_INCLUDE += -I../fbo
EXE = gfxMesaFboSoDemo.vxe
ADDED_LIBS += -lgfxMesaEGL -lgfxMesaGL -lgfxMesa -lgfxMesaSw -lxml
include $(WIND_USR_MK)/defs.library.mk
EXE_FORMAT = dynamic
else
LIB_BASE_NAME = gfxMesaDemos
include $(WIND_KRNL_MK)/defs.library.mk
endif

EXTRA_DEFINE += -D_MESA
ifdef _WRS_CONFIG_MESA_MANGLE
EXTRA_DEFINE += -DUSE_MGL_NAMESPACE
endif

ifdef _WRS_CONFIG_MESA_GPUDEV_DRI_I915
_MESA_DRI=yes
ifndef _WRS_CONFIG_MESA_GPUDEV_DRI_I965
ifeq ($(SPACE), user)
ADDED_LIBS += -lgfxMesaDriI915
endif
endif
endif
ifdef _WRS_CONFIG_MESA_GPUDEV_DRI_I965
_MESA_DRI=yes
ifeq ($(SPACE), user)
ADDED_LIBS += -lgfxMesaDriI965
endif
endif

ifdef _MESA_DRI
EXTRA_DEFINE += -DGFX_USE_GBM
ifeq ($(SPACE), user)
ADDED_LIBS += -lgfxLibDRM
endif
endif

ifdef _WRS_CONFIG_EVDEV_LIB
EXTRA_DEFINE += -DGFX_USE_EVDEV
endif

C_SOURCE = gfxMesaFboSoDemo.c

OBJS = $(C_SOURCE:.c=.o)

ifeq ($(SPACE), user)
include $(WIND_USR_MK)/rules.rtp.mk
else
include $(WIND_KRNL_MK)/rules.library.mk
endif
