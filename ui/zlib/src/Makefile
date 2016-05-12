# Makefile - makefile for zlib
#
# modification history
# --------------------
# 11dec14,my_  remove option _WRS_CONFIG_ENV (V7PRO-1559)
# 05jun14,my_  added bootloader support
# 10jan14,dee  Suppress all build warnings.  (3rd party code)
# 25jul13,f_f  Created
#

LIB_EXT = 1

TGT_DIR=$(WIND_BASE)/target
LIB_BASE_NAME=zlib

# Including defs.library to see if _WRS_CONFIG_SMP is defined.
# It must be after LIB_BASE_NAME
include $(TGT_DIR)/h/make/defs.library

EXTRA_INCLUDE += -I$(TGT_DIR)/src/ui/h
ifeq ($(CPU), ARMARCH7)
EXTRA_DEFINE += -mlong-calls
endif
OBJS          = adler32.o  compress.o  crc32.o  deflate.o  gzclose.o  gzlib.o  \
                gzread.o  gzwrite.o  infback.o  inffast.o  inflate.o  inftrees.o \
                trees.o  uncompr.o  zutil.o


include $(TGT_DIR)/h/make/rules.library

include  $(TGT_DIR)/h/make/defs.drivers

