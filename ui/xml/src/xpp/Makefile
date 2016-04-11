# Makefile - makefile for src/xml/xpp


TGT_DIR=$(WIND_BASE)/target

LIB_BASE_NAME = xml

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

DOC_FILES=

EXTRA_INCLUDE=-I$(TGT_DIR)/src/ui/h/web -I../../shareh
OBJS          = xpp.o sax.o

#EXTRA_INCLUDE += -I. -I../h
include $(TGT_DIR)/h/make/rules.library

CC_OPTIM= -w
include  $(TGT_DIR)/h/make/defs.drivers

