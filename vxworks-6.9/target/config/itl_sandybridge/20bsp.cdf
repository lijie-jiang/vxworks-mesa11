/* 20bsp.cdf - BSP component description file */

/*
 * Copyright (c) 2010-2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01s,28jan13,pee  WIND00400541 disable ROM resident build spec in VIP
01r,08nov12,cfm  WIND00375416 - Added I8253 and MC146818 by default.
01q,22oct12,swu  WIND00382711 - Add SELECTION for INCLUDE_FAST_REBOOT and 
                 INCLUDE_MULTI_STAGE_WARM_REBOOT
01p,13aug12,yjw  Fix BootApp can not load image from USB disk
                 (WIND00367537)
01o,01jun12,wyt  WIND00351904 - Remove INCLUDE_USR_BOOT_OP from INCLUDE_AMP_CPU.
01n,12may12,yjw  Add INCLUDE_SYS_WARM_USB no longer require USB GEN1
                 mass storage.(WIND00344759)
01m,10jul12,wyt  WIND00353922 - Force unsupported component INCLUDE_ATA
01l,27mar12,wyt  WIND00340694 - Change to new AHCI and ICH component.
01k,20mar12,jlv  WIND00288091 - Added multiple Shumway PCI controllers
01j,13mar12,jjk  WIND00226834 - Support for Stargo board bundle
01i,29feb12,wyt  add Emerald Lake II board component.
01h,23feb12,jjk  WIND00335210 - Make board selection selectable
01g,01feb12,jjk  WIND00328835 - Probe AP startup
01f,10jan12,sem  WIND00284000 - Remove I8253 exclusion.
                 WIND00322011 - Update APIC_TIMER_CLOCK_HZ.
                 WIND00327276 - Use selections for interrupt modes and
                 memory autosize.
01e,14dec11,jjk  WIND00322333 - Bundles support for Shumway
01d,06sep11,jjk  WIND00263692, Multi-stage boot support
01c,30aug11,jb   WIND00296767 - SMT fails on Emerson and Emerald Lake
01b,07mar11,j_z  sync to itl_nehalem/20bsp.cdf 01l version.
01a,14dec10,j_z  initial creation based on itl_nehalem version 01a
*/

Bsp itl_sandybridge {
    NAME        board support package
    CPU         NEHALEM
    REQUIRES    INCLUDE_KERNEL \
                INCLUDE_PCI \
                INCLUDE_PENTIUM_PCI \
                INCLUDE_PCI_OLD_CONFIG_ROUTINES \
                INCLUDE_SANDYBRIDGE_PARAMS \
                INCLUDE_TIMER_SYS \
                INCLUDE_TIMER_SYS_DELAY \
                INCLUDE_MMU_P6_32BIT \
                SELECT_INTERRUPT_MODE \
                SELECT_SYS_WARM_TYPE \
                SELECT_MEM_AUTOSIZE \
                SELECT_SANDYBRIDGE_BOARD \
                INCLUDE_BOARD_CONFIG
    FP          hard
    MP_OPTIONS  UP SMP
    BUILD_SPECS default_rom default_romCompress
}

Component INCLUDE_BOARD_CONFIG {
    NAME        Board Configuration Component
    SYNOPSIS    Fundamental Board Configuration Component. Included by a Board Bundle
    REQUIRES    SANDYBRIDGE_TARGET_NAME \
                DEFAULT_BOOT_LINE \
                SYS_MODEL
#if (defined _WRS_CONFIG_SMP)
    CFG_PARAMS  SANDYBRIDGE_TARGET_NAME \
                DEFAULT_BOOT_LINE \
                SYS_AP_TIMEOUT \
                SYS_AP_LOOP_COUNT \
                SYS_MODEL \
                SYS_PCI_BUS_CTRL_NUM \
                SYS_FIRST_PCI_MIN_BUS \
                SYS_FIRST_PCI_MAX_BUS
#else
    CFG_PARAMS  SANDYBRIDGE_TARGET_NAME \
                DEFAULT_BOOT_LINE \
                SYS_MODEL \
                SYS_PCI_BUS_CTRL_NUM \
                SYS_FIRST_PCI_MIN_BUS \
                SYS_FIRST_PCI_MAX_BUS
#endif /* _WRS_CONFIG_SMP */
    _INCLUDE_WHEN \
                DRV_TIMER_I8253 \
                DRV_TIMER_MC146818
}

Selection SELECT_SANDYBRIDGE_BOARD {
    NAME        Sandybridge Target Selection
    SYNOPSIS    Selects Target Board for a build
    COUNT       1-1
    CHILDREN    INCLUDE_EMERALD_LAKE \
                INCLUDE_EMERALD_LAKE_II \
                INCLUDE_SHUMWAY \
                INCLUDE_STARGO
    DEFAULTS    INCLUDE_EMERALD_LAKE
    _CHILDREN   FOLDER_BSP_CONFIG
}

Component INCLUDE_EMERALD_LAKE {
    NAME        Emerald Lake Board Component
    SYNOPSIS    Provides Component Parameters for Osage Board
    REQUIRES    INCLUDE_BOARD_CONFIG
}

Component INCLUDE_EMERALD_LAKE_II {
    NAME        Emerald Lake II Board Component
    SYNOPSIS    Provides Component Parameters for Emerald Lake II Board
    REQUIRES    INCLUDE_BOARD_CONFIG
}

Component INCLUDE_SHUMWAY {
    NAME        Shumway Board Component
    SYNOPSIS    Provides Component Parameters for Shumway Board
    REQUIRES    INCLUDE_BOARD_CONFIG
    CFG_PARAMS  SYS_SECOND_PCI_MIN_BUS \
                SYS_SECOND_PCI_MAX_BUS \
                SYS_THIRD_PCI_MIN_BUS \
                SYS_THIRD_PCI_MAX_BUS
}

Component INCLUDE_STARGO {
    NAME        Stargo Board Component
    SYNOPSIS    Provides Component Parameters for Stargo Board
    REQUIRES    INCLUDE_BOARD_CONFIG
}

Parameter SANDYBRIDGE_TARGET_NAME {
    NAME     Sandybridge Target
    SYNOPSIS Sandybridge Target Board Name, leading space necessary.
    TYPE     string
    DEFAULT  (INCLUDE_EMERALD_LAKE && INCLUDE_SMP_SCHED_SMT_POLICY)::("/SMT Emerald Lake") \
             (INCLUDE_EMERALD_LAKE)::(" Emerald Lake") \
             (INCLUDE_EMERALD_LAKE_II && INCLUDE_SMP_SCHED_SMT_POLICY)::("/SMT Emerald Lake II") \
             (INCLUDE_EMERALD_LAKE_II)::(" Emerald Lake II") \
             (INCLUDE_SHUMWAY && INCLUDE_SMP_SCHED_SMT_POLICY)::("/SMT Shumway") \
             (INCLUDE_SHUMWAY)::(" Shumway") \
             (INCLUDE_STARGO && INCLUDE_SMP_SCHED_SMT_POLICY)::("/SMT Stargo") \
             (INCLUDE_STARGO)::(" Stargo") \
             " No Bundle Selected"
}

/* Network Boot Devices for a BSP
 * The REQUIRES line should be modified for a BSP.
 */

Component       INCLUDE_BOOT_NET_DEVICES {
    REQUIRES    INCLUDE_FEI8255X_VXB_END \
                INCLUDE_GEI825XX_VXB_END
}

/* Specify boot rom console device for this BSP */

Component       INCLUDE_BOOT_SHELL {
    REQUIRES += DRV_SIO_NS16550
}

/* Filesystem Boot Devices for the Sandybridge BSP
 * The REQUIRES line should be modified for a BSP.
 */

Component       INCLUDE_BOOT_FS_DEVICES_SANDYBRIDGE {
    REQUIRES    INCLUDE_BOOT_USB_FS_LOADER \
                INCLUDE_DOSFS \
                INCLUDE_USB \
                INCLUDE_USB_INIT \
                INCLUDE_EHCI \
                INCLUDE_EHCI_INIT \
                INCLUDE_UHCI \
                INCLUDE_UHCI_INIT
}


/* Filesystem Boot Devices for a Emerald Lake Board
 * The REQUIRES line should be modified for a BSP.
 */

Component       INCLUDE_BOOT_FS_DEVICES_EMERALD_LAKE {
    REQUIRES	INCLUDE_BOOT_FS_DEVICES_SANDYBRIDGE
    INCLUDE_WHEN INCLUDE_EMERALD_LAKE INCLUDE_BOOT_APP
}

Component       INCLUDE_BOOT_FS_DEVICES_EMERALD_LAKE_II {
    REQUIRES    INCLUDE_BOOT_FS_DEVICES_SANDYBRIDGE
    INCLUDE_WHEN INCLUDE_EMERALD_LAKE_II INCLUDE_BOOT_APP
}


/* Filesystem Boot Devices for a Stargo Board
 * The REQUIRES line should be modified for a BSP.
 */

Component       INCLUDE_BOOT_FS_DEVICES_STARGO {
    REQUIRES    INCLUDE_BOOT_FS_DEVICES_SANDYBRIDGE
    INCLUDE_WHEN INCLUDE_STARGO INCLUDE_BOOT_APP
}

/*
 * Warm boot device selection required for
 * NVRAM support
 */

Selection SELECT_SYS_WARM_TYPE {
    NAME        Warm start device selection
    COUNT       1-1
    CHILDREN    INCLUDE_SYS_WARM_BIOS \
                INCLUDE_SYS_WARM_FD \
                INCLUDE_SYS_WARM_TFFS \
                INCLUDE_SYS_WARM_USB \
                INCLUDE_SYS_WARM_ICH \
                INCLUDE_SYS_WARM_AHCI
    DEFAULTS    INCLUDE_SYS_WARM_BIOS
    _CHILDREN   FOLDER_BSP_CONFIG
}

/*
 * Warm boot device components
 */

Component INCLUDE_SYS_WARM_BIOS {
    NAME        BIOS warm start device component
    CFG_PARAMS  SYS_WARM_TYPE \
                NV_RAM_SIZE
}

Component INCLUDE_SYS_WARM_FD {
    NAME        FD warm start device component
    CFG_PARAMS  SYS_WARM_TYPE \
                NV_RAM_SIZE \
                BOOTROM_DIR
}

Component INCLUDE_SYS_WARM_TFFS {
    NAME        TFFS warm start device component
    CFG_PARAMS  SYS_WARM_TYPE \
                NV_RAM_SIZE \
                BOOTROM_DIR
}

Component INCLUDE_SYS_WARM_USB {
    NAME        USB warm start device component
    REQUIRES    INCLUDE_USB \
                INCLUDE_USB_INIT \
                INCLUDE_EHCI \
                INCLUDE_EHCI_INIT \
                INCLUDE_DOSFS
    CFG_PARAMS  SYS_WARM_TYPE \
                NV_RAM_SIZE \
                BOOTROM_DIR
}

Component INCLUDE_SYS_WARM_ICH {
    NAME        ICH warm start device component
    REQUIRES    INCLUDE_DRV_STORAGE_PIIX \
                INCLUDE_DOSFS
    CFG_PARAMS  SYS_WARM_TYPE \
                NV_RAM_SIZE \
                BOOTROM_DIR
}

Component INCLUDE_SYS_WARM_AHCI {
    NAME        AHCI warm start device component
    REQUIRES    INCLUDE_DRV_STORAGE_AHCI \
                INCLUDE_DOSFS
    CFG_PARAMS  SYS_WARM_TYPE \
                NV_RAM_SIZE \
                BOOTROM_DIR
}

Parameter SYS_WARM_TYPE {
    NAME        Warm start device parameter
    DEFAULT     (INCLUDE_SYS_WARM_BIOS)::(SYS_WARM_BIOS) \
                (INCLUDE_SYS_WARM_FD)::(SYS_WARM_FD) \
                (INCLUDE_SYS_WARM_TFFS)::(SYS_WARM_TFFS) \
                (INCLUDE_SYS_WARM_USB)::(SYS_WARM_USB) \
                (INCLUDE_SYS_WARM_ICH)::(SYS_WARM_ICH) \
                (INCLUDE_SYS_WARM_AHCI)::(SYS_WARM_AHCI) \
                (SYS_WARM_BIOS)
}

Parameter NV_RAM_SIZE {
    DEFAULT     (INCLUDE_SYS_WARM_BIOS)::(NONE) \
                (0x1000)
}

Parameter BOOTROM_DIR {
    DEFAULT     (INCLUDE_SYS_WARM_FD)::("/fd0") \
                (INCLUDE_SYS_WARM_USB)::("/bd0") \
                (INCLUDE_SYS_WARM_ICH)::("/ata0:1") \
                (INCLUDE_SYS_WARM_AHCI)::("/ata0:1") \
                (NULL)
}

/*
 * VX_SMP_NUM_CPUS is a SMP parameter only and only available for SMP
 * builds. Due to a limitation of the project tool at the time this
 * parameter is created where the tool can not recognize the ifdef SMP
 * selection, this parameter is set up such that _CFG_PARAMS is not
 * specified here. In the 00vxWorks.cdf file, where the parameter
 * VX_SMP_NUM_CPUS is defined, the _CFG_PARAMS is specified only for
 * VxWorks SMP. Hence the redefining of VX_SMP_NUM_CPUS here should only
 * override the value and not the rest of the properties. And for UP, the
 * parameter is ignored since the parameter is not tied to any component
 * (_CFG_PARAMS is not specified).
 */

Parameter VX_SMP_NUM_CPUS {
	NAME		Number of CPUs available to be enabled for VxWorks SMP
	TYPE		UINT
    DEFAULT     (INCLUDE_EMERALD_LAKE && INCLUDE_SMP_SCHED_SMT_POLICY)::(8) \
                (INCLUDE_EMERALD_LAKE)::(4) \
                (INCLUDE_EMERALD_LAKE_II && INCLUDE_SMP_SCHED_SMT_POLICY)::(8) \
                (INCLUDE_EMERALD_LAKE_II)::(4) \
                (INCLUDE_SHUMWAY && INCLUDE_SMP_SCHED_SMT_POLICY)::(32) \
                (INCLUDE_SHUMWAY)::(16) \
                (INCLUDE_STARGO && INCLUDE_SMP_SCHED_SMT_POLICY)::(8) \
                (INCLUDE_STARGO)::(4) \
                (8)
}

/*
 * This component enables GEI ownership filtering for AMP and SMP M-N
 * configurations running on systems with the Cougar Point chipset.
 */

Component INCLUDE_COUGARPOINT_AMP {
    NAME            Cougar Point Chip Set Support - Sandy Bridge
    SYNOPSIS        Enables Cougar Point AMP/SMP M-N
    REQUIRES        INCLUDE_AMP_CPU
}


/*
 * Required component for AMP or SMP M-N.
 */
Component INCLUDE_AMP_CPU {
    NAME            AMP/SMP M-N CPU Support
    SYNOPSIS        Per-cpu configuration for SMP M-N
    REQUIRES        INCLUDE_DEVICE_MANAGER \
                    INCLUDE_VXIPI \
                    INCLUDE_ERF \
                    INCLUDE_RAWFS \
                    INCLUDE_SYMMETRIC_IO_MODE \
                    SELECT_AMP_CPU_NUMBER \
                    INCLUDE_NO_MEM_AUTOSIZE \
                    RAM_HIGH_ADRS \
                    RAM_LOW_ADRS \
                    LOCAL_MEM_LOCAL_ADRS \
                    LOCAL_MEM_SIZE \
                    DEFAULT_BOOT_LINE \
                    SYS_MODEL
    CFG_PARAMS      DEFAULT_BOOT_LINE \
                    SYS_MODEL
    _CHILDREN       FOLDER_BSP_CONFIG
}

Selection SELECT_AMP_CPU_NUMBER {
    NAME            vxWorks AMP/SMP M-N CPU to build
    COUNT           1-1
    REQUIRES        INCLUDE_AMP_CPU
    INCLUDE_WHEN    INCLUDE_AMP_CPU
    _CHILDREN       FOLDER_BSP_CONFIG
    DEFAULTS        INCLUDE_AMP_CPU_00
    CHILDREN        INCLUDE_AMP_CPU_00 \
                    INCLUDE_AMP_CPU_01 \
                    INCLUDE_AMP_CPU_02 \
                    INCLUDE_AMP_CPU_03 \
                    INCLUDE_AMP_CPU_04 \
                    INCLUDE_AMP_CPU_05 \
                    INCLUDE_AMP_CPU_06 \
                    INCLUDE_AMP_CPU_07
}

/*
 * This is minimal, does not include MOB/MCB/TIP/WRLOAD or enough for
 * a development system.
 */
Component INCLUDE_AMP_CPU_00  {
    NAME            AMP/SMP M-N kernel configured for CPU 00
    REQUIRES        INCLUDE_SHELL_BANNER \
                    INCLUDE_WRLOAD \
                    INCLUDE_GEI825XX_VXB_END
}

/*
 * Override the default (empty) component definitions with custom options
 */
Component INCLUDE_AMP_CPU_01  {
    NAME            AMP/SMP M-N kernel configured for CPU 01
    REQUIRES        INCLUDE_SHELL_BANNER \
                    SELECT_MPAPIC_MODE \
                    INCLUDE_SYS_WARM_BIOS \
                    INCLUDE_WRLOAD_IMAGE_BUILD
}

/*
 * Override the default (empty) component definitions with custom options
 */
Component INCLUDE_AMP_CPU_02  {
    NAME            AMP/SMP M-N kernel configured for CPU 02
    REQUIRES        INCLUDE_SHELL_BANNER \
                    SELECT_MPAPIC_MODE \
                    INCLUDE_SYS_WARM_BIOS \
                    INCLUDE_WRLOAD_IMAGE_BUILD
}

/*
 * Override the default (empty) component definitions with custom options
 */
Component INCLUDE_AMP_CPU_03  {
    NAME            AMP/SMP M-N kernel configured for CPU 03
    REQUIRES        INCLUDE_SHELL_BANNER \
                    SELECT_MPAPIC_MODE \
                    INCLUDE_SYS_WARM_BIOS \
                    INCLUDE_WRLOAD_IMAGE_BUILD
}

/*
 * Override the default (empty) component definitions with custom options
 */
Component INCLUDE_AMP_CPU_04  {
    NAME            AMP/SMP M-N kernel configured for CPU 04
    REQUIRES        INCLUDE_SHELL_BANNER \
                    SELECT_MPAPIC_MODE \
                    INCLUDE_SYS_WARM_BIOS \
                    INCLUDE_WRLOAD_IMAGE_BUILD
}

/*
 * Override the default (empty) component definitions with custom options
 */
Component INCLUDE_AMP_CPU_05  {
    NAME            AMP/SMP M-N kernel configured for CPU 05
    REQUIRES        INCLUDE_SHELL_BANNER \
                    SELECT_MPAPIC_MODE \
                    INCLUDE_SYS_WARM_BIOS \
                    INCLUDE_WRLOAD_IMAGE_BUILD
}

/*
 * Override the default (empty) component definitions with custom options
 */
Component INCLUDE_AMP_CPU_06  {
    NAME            AMP/SMP M-N kernel configured for CPU 06
    REQUIRES        INCLUDE_SHELL_BANNER \
                    SELECT_MPAPIC_MODE \
                    INCLUDE_SYS_WARM_BIOS \
                    INCLUDE_WRLOAD_IMAGE_BUILD
}

/*
 * Override the default (empty) component definitions with custom options
 */
Component INCLUDE_AMP_CPU_07  {
    NAME            AMP/SMP M-N kernel configured for CPU 07
    REQUIRES        INCLUDE_SHELL_BANNER \
                    SELECT_MPAPIC_MODE \
                    INCLUDE_SYS_WARM_BIOS \
                    INCLUDE_WRLOAD_IMAGE_BUILD
}

/* AMP/SMP M-N Memory definitions */

/* 8 CPU Memory Configuration */
Parameter RAM_HIGH_ADRS {
    NAME     Bootrom Copy region
    DEFAULT  (INCLUDE_AMP_CPU_00)::(0x10008000) \
             (INCLUDE_AMP_CPU_01)::(0x10F68000) \
             (INCLUDE_AMP_CPU_02)::(0x20EC8000) \
             (INCLUDE_AMP_CPU_03)::(0x30E28000) \
             (INCLUDE_AMP_CPU_04)::(0x40D88000) \
             (INCLUDE_AMP_CPU_05)::(0x50CE8000) \
             (INCLUDE_AMP_CPU_06)::(0x60C48000) \
             (INCLUDE_AMP_CPU_07)::(0x70BA8000) \
             (INCLUDE_BOOT_APP)::(0x00608000) \
             (0x10008000)
}

Parameter RAM_LOW_ADRS {
    NAME     Runtime kernel load address
    DEFAULT  (INCLUDE_AMP_CPU_00)::(0x00408000) \
             (INCLUDE_AMP_CPU_01)::(0x10368000) \
             (INCLUDE_AMP_CPU_02)::(0x202C8000) \
             (INCLUDE_AMP_CPU_03)::(0x30228000) \
             (INCLUDE_AMP_CPU_04)::(0x40188000) \
             (INCLUDE_AMP_CPU_05)::(0x500E8000) \
             (INCLUDE_AMP_CPU_06)::(0x60048000) \
             (INCLUDE_AMP_CPU_07)::(0x6FFA8000) \
             (INCLUDE_BOOT_RAM_IMAGE)::(0x15008000) \
             (INCLUDE_BOOT_APP)::(0x10008000) \
             (0x00408000)
}

Parameter LOCAL_MEM_LOCAL_ADRS {
    NAME     system memory start address
    DEFAULT  (INCLUDE_AMP_CPU_00)::(0x00100000) \
             (INCLUDE_AMP_CPU_01)::(0x0FF60000) \
             (INCLUDE_AMP_CPU_02)::(0x1FEC0000) \
             (INCLUDE_AMP_CPU_03)::(0x2FE20000) \
             (INCLUDE_AMP_CPU_04)::(0x3FD80000) \
             (INCLUDE_AMP_CPU_05)::(0x4FCE0000) \
             (INCLUDE_AMP_CPU_06)::(0x5FC40000) \
             (INCLUDE_AMP_CPU_07)::(0x6FBA0000) \
             (INCLUDE_BOOT_APP)::(0x00100000) \
             (0x00100000)
}
/* End of 8 CPU Memory Configuration */

Parameter ROM_TEXT_ADRS {
    NAME        ROM text address
    SYNOPSIS    ROM text address
    DEFAULT     (ROM_BASE_ADRS)
}

Parameter ROM_SIZE {
    NAME        ROM size
    SYNOPSIS    ROM size
    DEFAULT     (INCLUDE_MULTI_STAGE_BOOT)::(0x00200000) \
                (0x00090000)
}

Parameter ROM_BASE_ADRS {
    NAME        ROM base address
    SYNOPSIS    ROM base address
    DEFAULT     (INCLUDE_MULTI_STAGE_BOOT)::(0x00408000) \
                (0x00008000)
}

Component INCLUDE_MULTI_STAGE_BOOT {
    NAME        Multi-stage boot support
    SYNOPSIS    Use multi-stage fast or warm reboot mechanism
    REQUIRES    ROM_BASE_ADRS \
                ROM_TEXT_ADRS \
                ROM_SIZE
}

Selection SELECT_MULTI_STAGE_REBOOT_TYPE {
    NAME        Multi-stage boot type select
    COUNT       1-1
    CHILDREN    INCLUDE_FAST_REBOOT \
                INCLUDE_MULTI_STAGE_WARM_REBOOT
}

Component INCLUDE_FAST_REBOOT {
    NAME        Multi-stage fast reboot type
    SYNOPSIS    Use multi-stage fast reboot mechanism
    REQUIRES    INCLUDE_MULTI_STAGE_BOOT
    EXCLUDES    INCLUDE_MULTI_STAGE_WARM_REBOOT
}

Component INCLUDE_MULTI_STAGE_WARM_REBOOT {
    NAME        Multi-stage warm reboot type
    SYNOPSIS    Use multi-stage warm reboot mechanism
    REQUIRES    INCLUDE_MULTI_STAGE_BOOT
    EXCLUDES    INCLUDE_FAST_REBOOT
}

Parameter LOCAL_MEM_SIZE {
    NAME     system memory size
    DEFAULT  (INCLUDE_AMP_CPU_00)::(0x0FE60000) \
             (INCLUDE_AMP_CPU_01)::(0x0FE60000) \
             (INCLUDE_AMP_CPU_02)::(0x0FE60000) \
             (INCLUDE_AMP_CPU_03)::(0x0FE60000) \
             (INCLUDE_AMP_CPU_04)::(0x0FE60000) \
             (INCLUDE_AMP_CPU_05)::(0x0FE60000) \
             (INCLUDE_AMP_CPU_06)::(0x0FE60000) \
             (INCLUDE_AMP_CPU_07)::(0x0FE60000) \
             (INCLUDE_BOOT_APP)::(0x30000000) \
             (0x30000000)
}

Parameter SYS_MODEL {
    NAME     System Model
    SYNOPSIS System Model string
    TYPE     string
#if (!defined _WRS_CONFIG_SMP)
    DEFAULT  (INCLUDE_AMP_CPU_00)::("Intel(R) SandyBridge Processor AMP BP"  SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_01)::("Intel(R) SandyBridge Processor AMP AP1" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_02)::("Intel(R) SandyBridge Processor AMP AP2" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_03)::("Intel(R) SandyBridge Processor AMP AP3" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_04)::("Intel(R) SandyBridge Processor AMP AP4" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_05)::("Intel(R) SandyBridge Processor AMP AP5" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_06)::("Intel(R) SandyBridge Processor AMP AP6" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_07)::("Intel(R) SandyBridge Processor AMP AP7" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_SYMMETRIC_IO_MODE && !INCLUDE_AMP_CPU)::("Intel(R) SandyBridge Processor SYMMETRIC IO" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_VIRTUAL_WIRE_MODE)::("Intel(R) SandyBridge Processor VIRTUAL WIRE" SANDYBRIDGE_TARGET_NAME) \
             ("Intel(R) SandyBridge Processor" SANDYBRIDGE_TARGET_NAME)
#else
    DEFAULT  (INCLUDE_AMP_CPU_00)::("Intel(R) SandyBridge Processor SMP BP"  SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_01)::("Intel(R) SandyBridge Processor SMP AP1" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_02)::("Intel(R) SandyBridge Processor SMP AP2" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_03)::("Intel(R) SandyBridge Processor SMP AP3" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_04)::("Intel(R) SandyBridge Processor SMP AP4" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_05)::("Intel(R) SandyBridge Processor SMP AP5" SANDYBRIDGE_TARGET_NAME) \
             (INCLUDE_AMP_CPU_06)::("Intel(R) SandyBridge Processor SMP AP6" SANDYBRIDGE_TARGET_NAME) \
             ("Intel(R) SandyBridge Processor SYMMETRIC IO SMP" SANDYBRIDGE_TARGET_NAME)
#endif
}

Parameter DEFAULT_BOOT_LINE {
    NAME     default boot line
    SYNOPSIS Default boot line string
    TYPE     string
    DEFAULT  (INCLUDE_AMP_CPU_00)::("gei(0,0)host:itl_AMP0/vxWorks h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=target_bp  n=0") \
             (INCLUDE_AMP_CPU_01)::("gei(0,0)host:itl_AMP1/vxWorks h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=target_ap1 n=1") \
             (INCLUDE_AMP_CPU_02)::("gei(0,0)host:itl_AMP2/vxWorks h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=target_ap2 n=2") \
             (INCLUDE_AMP_CPU_03)::("gei(0,0)host:itl_AMP3/vxWorks h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=target_ap3 n=3") \
             (INCLUDE_AMP_CPU_04)::("gei(0,0)host:itl_AMP4/vxWorks h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=target_ap4 n=4") \
             (INCLUDE_AMP_CPU_05)::("gei(0,0)host:itl_AMP5/vxWorks h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=target_ap5 n=5") \
             (INCLUDE_AMP_CPU_06)::("gei(0,0)host:itl_AMP6/vxWorks h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=target_ap6 n=6") \
             (INCLUDE_AMP_CPU_07)::("gei(0,0)host:itl_AMP7/vxWorks h=90.0.0.3 e=90.0.0.50:ffffff00 u=target tn=target_ap7 n=7") \
             ("gei(0,0)host:vxWorks h=90.0.0.3 e=90.0.0.50 u=target")
}

/*
 * SYS_AP_LOOP_COUNT is a SMP parameter used in function sysCpuStart.
 * It is used to set the count of times to check if an application
 * processor succeeded in starting up.
 */

Parameter SYS_AP_LOOP_COUNT {
    NAME     System AP startup checks
    SYNOPSIS Maximum times to check if an application processor started
    TYPE     uint
    DEFAULT  (200000)
}

/*
 * SYS_AP_TIMEOUT is a SMP parameter used in function sysCpuStart.
 * It is used to set the time in microseconds to wait between checking
 * if an application processor succeeded in starting up.
 * The time is specified in microseconds and should be short in duration.
 * SYS_AP_LOOP_COUNT * SYS_AP_TIMEOUT gives the total time to wait for an AP
 * before giving up and moving on to the next application processor.
 */

Parameter SYS_AP_TIMEOUT {
    NAME     System AP startup timeout
    SYNOPSIS Time between each check to see if application processor started
    TYPE     uint
    DEFAULT  (10)
}

/*******************************************************************************
*
* HWCONF component
*
*/

Component INCLUDE_HWCONF {
    NAME		hardware configuration
    SYNOPSIS	        hardware configuration support
    CONFIGLETTES        hwconf.c
    _CHILDREN	        FOLDER_NOT_VISIBLE
    _REQUIRES           INCLUDE_VXBUS
}

/*******************************************************************************
*
* Interrupt Mode Configuration
*
*/

Selection SELECT_INTERRUPT_MODE {
    NAME        Interrupt mode selection
    COUNT       1-1
    CHILDREN    INCLUDE_VIRTUAL_WIRE_MODE \
                INCLUDE_SYMMETRIC_IO_MODE \
                INCLUDE_PIC_MODE
    DEFAULTS    INCLUDE_SYMMETRIC_IO_MODE
    _REQUIRES   INCLUDE_VXBUS
    _CHILDREN   FOLDER_BSP_CONFIG
}

Component INCLUDE_VIRTUAL_WIRE_MODE {
    NAME        Virtual wire mode
    SYNOPSIS    Virtual wire interrupt mode
    REQUIRES    INCLUDE_INTCTLR_DYNAMIC_LIB \
                DRV_INTCTLR_MPAPIC \
                DRV_INTCTLR_LOAPIC \
                DRV_TIMER_LOAPIC \
                DRV_TIMER_I8253
    CFG_PARAMS  APIC_TIMER_CLOCK_HZ
}

Component INCLUDE_SYMMETRIC_IO_MODE {
    NAME        Symmetric I/O Mode
    SYNOPSIS    Symmetric I/O interrupt mode
    REQUIRES    INCLUDE_INTCTLR_DYNAMIC_LIB \
                DRV_INTCTLR_MPAPIC \
                DRV_INTCTLR_LOAPIC \
                DRV_TIMER_LOAPIC \
                DRV_INTCTLR_IOAPIC \
                DRV_TIMER_IA_TIMESTAMP
    CFG_PARAMS  APIC_TIMER_CLOCK_HZ
}

Component INCLUDE_PIC_MODE {
    NAME        PIC Mode
    SYNOPSIS    PIC interrupt mode
    REQUIRES    DRV_TIMER_I8253 \
                INCLUDE_NO_BOOT_OP
}

Parameter APIC_TIMER_CLOCK_HZ  {
    NAME        APIC timer clock rate configuration parameter
    SYNOPSIS    APIC timer clock rate (0 is auto-calculate)
    TYPE        uint
    DEFAULT     (0)
}

/*******************************************************************************
*
* MP Table Configuration Options
*
*/

Component INCLUDE_ACPI_MPAPIC {
    NAME        ACPI MP APIC
    SYNOPSIS    ACPI MP APIC component
    REQUIRES    INCLUDE_ACPI_BOOT_OP
}

Selection SELECT_MPAPIC_MODE {
    NAME        MP APIC boot options
    SYNOPSIS    Selects MP APIC struct creation method
    COUNT       1-1
    CHILDREN    INCLUDE_ACPI_BOOT_OP \
                INCLUDE_USR_BOOT_OP \
                INCLUDE_NO_BOOT_OP \
                INCLUDE_MPTABLE_BOOT_OP
    DEFAULTS    INCLUDE_MPTABLE_BOOT_OP
    _CHILDREN   FOLDER_BSP_CONFIG
}

Component INCLUDE_ACPI_BOOT_OP {
    NAME        ACPI MP APIC boot
    SYNOPSIS    ACPI MP APIC boot option
    REQUIRES    SELECT_INTERRUPT_MODE \
                INCLUDE_ACPI_MPAPIC \
                INCLUDE_ACPI_TABLE_MAP \
                INCLUDE_USR_MPAPIC
    EXCLUDES    INCLUDE_PIC_MODE
}

Component INCLUDE_USR_BOOT_OP {
    NAME        User defined MP APIC boot
    SYNOPSIS    User defined MP APIC option
    REQUIRES    SELECT_INTERRUPT_MODE
    EXCLUDES    INCLUDE_PIC_MODE
}

Component INCLUDE_MPTABLE_BOOT_OP {
    NAME        BIOS MP APIC boot
    SYNOPSIS    BIOS MP APIC boot option
    REQUIRES    SELECT_INTERRUPT_MODE
    EXCLUDES    INCLUDE_PIC_MODE
}

Component INCLUDE_NO_BOOT_OP {
    NAME        No MP APIC boot
    SYNOPSIS    No MP APIC boot option
    REQUIRES    SELECT_INTERRUPT_MODE
    EXCLUDES    INCLUDE_MPTABLE_BOOT_OP \
                INCLUDE_ACPI_BOOT_OP \
                INCLUDE_USR_BOOT_OP
}

/*******************************************************************************
*
* PCI Bus controller configuration
*
* On shumway board, the first PCI bus controller starts from bus 0 and
* the second PCI bus controller starts from bus 128 but they are not bridge
* connected.  In order to search the bus 128 or above, we create the second
* VxBus PCI bus controller device instance with the start bus number 128.
*
* The sub-class of the PCI bus controller, that bridges to bus 130, on
* the bus 128 is not configured as a PCI-to-PCI bridge on some shumway boards.
* To search the bus 130 or above, we create another VxBus PCI bus controller
* device instance with the start bus number 130.
*
* Note:
* The PCI_BUS_CTRL_NUM configuration currently supported is 1, 2 or 3 only.
*/

Parameter SYS_PCI_BUS_CTRL_NUM {
    NAME     PCI Bus Controllers
    SYNOPSIS Number of PCI Bus Controllers
    TYPE     uint
    DEFAULT  (INCLUDE_SHUMWAY)::(3) \
             (1)
}

Parameter SYS_FIRST_PCI_MIN_BUS {
    NAME     First Controller Bus start
    SYNOPSIS Start bus for first PCI bus controller
    TYPE     UINT
    DEFAULT  (0)
}

Parameter SYS_FIRST_PCI_MAX_BUS {
    NAME     First Controller Bus start
    SYNOPSIS End bus for first PCI bus controller
    TYPE     UINT
    DEFAULT  (INCLUDE_SHUMWAY)::(127) \
             (PCI_MAX_BUS)
}

Parameter SYS_SECOND_PCI_MIN_BUS {
    NAME     Second Controller Bus start
    SYNOPSIS Start bus for second PCI bus controller
    TYPE     UINT
    DEFAULT  (INCLUDE_SHUMWAY)::(128) \
             (0)
}

Parameter SYS_SECOND_PCI_MAX_BUS {
    NAME     Second Controller Bus end
    SYNOPSIS End bus for second PCI bus controller
    TYPE     UINT
    DEFAULT  (INCLUDE_SHUMWAY)::(129) \
             (0)
}

Parameter SYS_THIRD_PCI_MIN_BUS {
    NAME     Third Controller Bus start
    SYNOPSIS Start bus for third PCI bus controller
    TYPE     UINT
    DEFAULT  (INCLUDE_SHUMWAY)::(130) \
             (0)
}

Parameter SYS_THIRD_PCI_MAX_BUS {
    NAME     Third Controller Bus start
    SYNOPSIS Start bus for third PCI bus controller
    TYPE     UINT
    DEFAULT  (INCLUDE_SHUMWAY)::(PCI_MAX_BUS) \
             (0)
}

/*
 * Force unsupported components to be unavailable.
 *
 * The following component definition(s) forces the named component(s)
 * to be "unavailable" as far as the project build facility (vxprj) is
 * concerned. The required component (COMPONENT_NOT_SUPPORTED) should
 * never be defined, and hence the named component(s) will never be
 * available. This little trick is used by the BSPVTS build scripts
 * (suiteBuild, bspBuildTest.tcl) to automatically exclude test modules
 * that are not applicable to a BSP because the BSP does not support a
 * given component and/or test module. If and when support is added,
 * the following definition(s) should be removed.
 */

Component INCLUDE_ATA {
    REQUIRES    COMPONENT_NOT_SUPPORTED
}
