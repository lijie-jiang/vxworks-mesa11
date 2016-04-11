# Makefile - makefile for target/config/itl_sandybridge
#
# Copyright (c) 2010-2012 Wind River Systems, Inc.
#
# The right to copy, distribute, modify, or otherwise make use
# of this software may be licensed only pursuant to the terms
# of an applicable Wind River license agreement.
#
# modification history
# --------------------
# 01e,27mar12,wyt  WIND00340694 - Change to new AHCI and ICH component.
# 01d,07mar12,wyt  remove mkboot.o
# 01c,16aug11,jjk  WIND00263692, Multi-stage boot support
# 01b,07mar11,j_z  change CPU to NEHALEM.
# 01a,14dec10,j_z  initial creation based on itl_nehalem version 01a
#
# DESCRIPTION
# This file contains rules for building VxWorks for
# itl_sandybridge.
#
# NOTE
# The constants ROM_TEXT_ADRS and RAM_HIGH_ADRS
# must be identical to those in config.h.
#
# INCLUDES
#     makeTarget
#*/

CPU	= NEHALEM
TOOL	= gnu 

TGT_DIR = $(WIND_BASE)/target

include $(TGT_DIR)/h/make/defs.bsp

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.

TARGET_DIR = itl_sandybridge
VENDOR     =
BOARD      = Sandy Bridge

FIRST_TEXT_ADRS    = 00008000

FIRST_ENTRY        = firstBoot  
FIRST_OBJ          = vxStage1Boot.o
FIRST_HIGH_FLAGS   = -Ttext $(FIRST_TEXT_ADRS)
LD_FIRST_CMP_FLAGS = $(ROM_LDFLAGS) $(RES_LDFLAGS) $(FIRST_HIGH_FLAGS)

#
# The constants ROM_TEXT_ADRS, ROM_SIZE, and RAM_HIGH_ADRS are defined
# in config.h, MakeSkel, Makefile, and Makefile.*
# All definitions for these constants must be identical.
#

RAM_LOW_ADRS       = 00408000   # VxWorks image entry point
RAM_HIGH_ADRS      = 10008000   # Boot image entry point

#EXTRA_DEFINE       = -DGRUB_MULTIBOOT
# AMP: BP & AP and SMP M-N images built with:
#EXTRA_DEFINE  = -D_WRS_VX_AMP -fno-zero-initialized-in-bss
# AMP/SMP M-N Bootrom
EXTRA_DEFINE  = -fno-zero-initialized-in-bss

######################################################################
# Use these for non-multi-stage boot
ROM_TEXT_ADRS      = 00008000   # ROM entry address
ROM_SIZE           = 00090000   # number of bytes of ROM space
######################################################################
# Use these for multi-stage boot
# EXTRA_DEFINE options:
#   -DINCLUDE_MULTI_STAGE_BOOT -DINCLUDE_FAST_REBOOT
#       Capable of warm reboots by copying and restoring second stage
#       to RAM.  Some RAM is used for the second stage image copy.
#   -DINCLUDE_MULTI_STAGE_BOOT
#       Cold reboots only.  No RAM is used for second stage image copy.
#   -DINCLUDE_MULTI_STAGE_BOOT -DINCLUDE_MULTI_STAGE_WARM_REBOOT
#       No RAM is used for second stage image copy.  Warm reboots are
#       possible (except PXE boots); however, care should be taken
#       not to initialize the boot device in bootApp/bootrom and
#       vxWorks.  For example, if SATA drive is used to store first
#       stage file, bootApp/bootrom and vxWorks should not be built
#       with INCLUDE_DRV_STORAGE_AHCI.
#EXTRA_DEFINE      += -DINCLUDE_MULTI_STAGE_BOOT -DINCLUDE_FAST_REBOOT
#EXTRA_DEFINE      += -DINCLUDE_MULTI_STAGE_BOOT -DINCLUDE_MULTI_STAGE_WARM_REBOOT
#ROM_TEXT_ADRS      = 00408000 # ROM entry address
#ROM_SIZE           = 00200000 # number of bytes of ROM space
######################################################################

MACH_EXTRA         = 

RELEASE += bootrom.bin

RELEASE += bootrom.pxe

%.pxe:  $(BSP_DIR)/pxeBoot.bin %.bin
	cat $+ > $@
bootrom.pxe: bootrom.bin pxeBoot.bin
	cat pxeBoot.bin bootrom.bin > bootrom.pxe

# These are for multi-stage boot
$(FIRST_OBJ)     : vxStage1Boot.s $(CFG_GEN_FILES) ipcom_ipdomain$(OPT)
vxStage1Boot.bin : vxStage1Boot
vxStage1Boot: $(FIRST_OBJ)
	$(LD) $(LDFLAGS) $(RES_LDFLAGS) $(LD_ENTRY_OPT) $(FIRST_ENTRY) \
		$(LD_FIRST_CMP_FLAGS) -defsym _VX_DATA_ALIGN=1 $(FIRST_OBJ) \
		$(LD_SCRIPT_RAM) -o $@

## Only redefine make definitions above this point, or the expansion of
## makefile target dependencies may be incorrect.

include $(TGT_DIR)/h/make/rules.bsp

# Following bootable vxWorks should be placed in bootable diskette
# by "mkboot" or "mkbootFd, mkbootAta" utility.
#
#   vxWorks.st_rom	- bootable vxWorks.st(compressed): upper mem
#   bootrom		- bootrom(compressed):		   upper mem
#   bootrom_uncmp	- bootrom:			   upper mem
#
# The boot image is either bootrom_uncmp or vxWorks.st_rom.
# If the EPROM is 27020 (256 Kbytes)
#	$(BINHEX) -a 3fe00 -b romcard >> $@
# If the EPROM is 27040 (512 Kbytes)
#	$(BINHEX) -a 7fe00 -b romcard >> $@

SED	= sed

romcard_bootrom_512.hex:	depend.$(BSP_NAME) romcard.o bootrom_uncmp
	- @ $(RM) $@
	$(LD) -o romcard -X -N -e _romcard romcard.o
	$(ROMSIZEPROG) -b 7fe00 bootrom_uncmp
	$(BINHEX) -a 0 bootrom_uncmp | $(SED) -e "/S9/d" > $@
	$(BINHEX) -a 7fe00 -b romcard >> $@
	- @ $(RM) romcard.o romcard
	$(POST_HEX_BUILD_RULE)

romcard_bootrom_256.hex:	depend.$(BSP_NAME) romcard.o bootrom_uncmp
	- @ $(RM) $@
	$(LD) -o romcard -X -N -e _romcard romcard.o
	$(ROMSIZEPROG) -b 3fe00 bootrom_uncmp
	$(BINHEX) -a 0 bootrom_uncmp | $(SED) -e "/S9/d" > $@
	$(BINHEX) -a 3fe00 -b romcard >> $@
	- @ $(RM) romcard.o romcard
	$(POST_HEX_BUILD_RULE)

romcard_vxWorks_st_512.hex:	depend.$(BSP_NAME) romcard.o vxWorks.st_rom
	- @ $(RM) $@
	$(LD) -o romcard -X -N -e _romcard romcard.o
	$(ROMSIZEPROG) -b 7fe00 vxWorks.st_rom
	$(BINHEX) -a 0 vxWorks.st_rom | $(SED) -e "/S9/d" > $@
	$(BINHEX) -a 7fe00 -b romcard >> $@
	- @ $(RM) romcard.o romcard
	$(POST_HEX_BUILD_RULE)

