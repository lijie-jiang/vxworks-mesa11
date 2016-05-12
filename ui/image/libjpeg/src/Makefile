# Makefile - Graphics Framework JPEG Makefile
#
# Copyright (c) 2013-2014,2016 Wind River Systems, Inc.
#
# The right to copy, distribute, modify or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River License agreement.
#
# modification history
# --------------------
# 03feb16,yat  Add EXTRA_DEFINE with NO_GETENV to avoid using getenv (US73271)
# 22dec14,yat  Remove mlong-calls for performance improvement (US46449)
# 24jan14,mgc  Modified for VxWorks 7 release
# 11may12,rfm  create

TGT_DIR=$(WIND_BASE)/target
LIB_BASE_NAME=gfxJpeg

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

EXTRA_INCLUDE += -I$(TGT_DIR)/src/ui/h
EXTRA_DEFINE += -DNO_GETENV

C_SOURCE = jcapimin.c jcapistd.c jccoefct.c jccolor.c jcdctmgr.c jchuff.c jcinit.c jcmainct.c
C_SOURCE += jcmarker.c jcmaster.c jcomapi.c jcparam.c jcphuff.c jcprepct.c jcsample.c jctrans.c
C_SOURCE += jdapimin.c jdapistd.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c jdhuff.c
C_SOURCE += jdinput.c jdmainct.c jdmarker.c jdmaster.c jdmerge.c jdphuff.c jdpostct.c jdsample.c
C_SOURCE += jdtrans.c jerror.c jfdctflt.c jfdctfst.c jfdctint.c jidctflt.c jidctfst.c jidctint.c
C_SOURCE += jidctred.c jmemansi.c jmemmgr.c jmemnobs.c jquant1.c jquant2.c jutils.c

OBJS = $(C_SOURCE:.c=.o)     


include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers
