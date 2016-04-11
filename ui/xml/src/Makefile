# Makefile - makefile for src/xml
#
# modification history
# --------------------
# 01b,17jun05,j_c  Fix apigen interface
# 01a,???????,???  Written
#
# This Makefile is for refgen doc generation only.
# It shall not be called from ubermake.


# used by defs.docs for vxworks KERNEL_MAJOR_VERSION = 5

TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME = xml

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=
EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h/web

OBJS          = xmlparse.o xmlrole.o xmltok.o

#EXTRA_INCLUDE += -I. -I../h
include $(TGT_DIR)/h/make/rules.library

CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers
