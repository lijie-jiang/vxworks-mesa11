/* 25bundle_targets.cdf - itl_sandybridge BSP bundles for target boards */

/*
 * Copyright (c) 2011-2012 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01g,01jun12,wyt  WIND00315705 - Change to INCLUDE_ACPI_BOOT_OP for
                 EMERALD_LAKE_II bundles
01f,13mar12,jjk  WIND00226834 - Support for Stargo board bundle
01e,28feb12,wyt  bundles support for Emerald Lake II
01d,14dec11,jjk  WIND00322333 - Bundles support for Shumway
01c,06sep11,jjk  WIND00263692, Multi-stage boot support
01b,30aug11,jb   WIND00296767 - SMT fails on Emerson and Emerald Lake
01a,29mar11,j_z  initial creation based on itl_nehalem version 01c
*/

Bundle BUNDLE_EMERALD_LAKE {
    NAME        Intel Emerald Lake Board bundle
    SYNOPSIS    Configures itl_sandybridge vxWorks Image builds for the Intel Emerald Lake target.
    COMPONENTS  INCLUDE_EMERALD_LAKE INCLUDE_ACPI_CPU_CONFIG INCLUDE_ACPI_BOOT_OP
}

Bundle BUNDLE_EMERALD_LAKE_SMT {
    NAME        Intel Emerald Lake Board bundle
    SYNOPSIS    Configures itl_sandybridge vxWorks Image builds for the Intel Emerald Lake target with SMT.
    COMPONENTS  INCLUDE_EMERALD_LAKE INCLUDE_ACPI_CPU_CONFIG INCLUDE_ACPI_BOOT_OP INCLUDE_SMP_SCHED_SMT_POLICY
}

Bundle BUNDLE_EMERALD_LAKE_BOOTAPP {
    NAME        Intel Emerald Lake Board bundle
    SYNOPSIS    Configures itl_sandybridge BOOTAPP builds for the Intel Emerald Lake target.
    COMPONENTS  INCLUDE_EMERALD_LAKE INCLUDE_ACPI_BOOT_OP INCLUDE_SYS_WARM_USB
}

Bundle BUNDLE_EMERALD_LAKE_II {
    NAME        Intel Emerald Lake II Board bundle
    SYNOPSIS    Configures itl_sandybridge vxWorks Image builds for the Intel Emerald Lake II target.
    COMPONENTS  INCLUDE_EMERALD_LAKE_II INCLUDE_ACPI_CPU_CONFIG INCLUDE_ACPI_BOOT_OP
}

Bundle BUNDLE_EMERALD_LAKE_II_SMT {
    NAME        Intel Emerald Lake II Board bundle
    SYNOPSIS    Configures itl_sandybridge vxWorks Image builds for the Intel Emerald Lake II target with SMT.
    COMPONENTS  INCLUDE_EMERALD_LAKE_II INCLUDE_ACPI_CPU_CONFIG INCLUDE_ACPI_BOOT_OP INCLUDE_SMP_SCHED_SMT_POLICY
}

Bundle BUNDLE_EMERALD_LAKE_II_BOOTAPP {
    NAME        Intel Emerald Lake II Board bundle
    SYNOPSIS    Configures itl_sandybridge BOOTAPP builds for the Intel Emerald Lake II target.
    COMPONENTS  INCLUDE_EMERALD_LAKE_II INCLUDE_ACPI_BOOT_OP INCLUDE_SYS_WARM_USB
}

Bundle BUNDLE_SHUMWAY {
    NAME        Intel Shumway Board bundle
    SYNOPSIS    Configures itl_sandybridge vxWorks builds for the Intel Shumway target.
    COMPONENTS  INCLUDE_SHUMWAY INCLUDE_ACPI_CPU_CONFIG INCLUDE_ACPI_BOOT_OP
}

Bundle BUNDLE_SHUMWAY_SMT {
    NAME        Intel Shumway Board bundle
    SYNOPSIS    Configures itl_sandybridge vxWorks Image builds for the Intel Shumway target with SMT.
    COMPONENTS  INCLUDE_SHUMWAY INCLUDE_ACPI_CPU_CONFIG INCLUDE_ACPI_BOOT_OP INCLUDE_SMP_SCHED_SMT_POLICY
}

Bundle BUNDLE_SHUMWAY_BOOTAPP {
    NAME        Intel Shumway Board bundle
    SYNOPSIS    Configures itl_sandybridge BOOTAPP builds for the Intel Shumway target.
    COMPONENTS  INCLUDE_SHUMWAY INCLUDE_ACPI_BOOT_OP
}

Bundle BUNDLE_STARGO {
    NAME        Intel Stargo Board bundle
    SYNOPSIS    Configures itl_sandybridge vxWorks builds for the Intel Stargo target.
    COMPONENTS  INCLUDE_STARGO INCLUDE_ACPI_BOOT_OP INCLUDE_ACPI_CPU_CONFIG
}

Bundle BUNDLE_STARGO_SMT {
    NAME        Intel Stargo Board bundle
    SYNOPSIS    Configures itl_sandybridge vxWorks Image builds for the Intel Stargo target with SMT.
    COMPONENTS  INCLUDE_STARGO INCLUDE_ACPI_BOOT_OP INCLUDE_ACPI_CPU_CONFIG INCLUDE_SMP_SCHED_SMT_POLICY
}

Bundle BUNDLE_STARGO_BOOTAPP {
    NAME        Intel Stargo Board bundle
    SYNOPSIS    Configures itl_sandybridge BOOTAPP builds for the Intel Stargo target.
    COMPONENTS  INCLUDE_STARGO
}

Bundle BUNDLE_MSB_FAST_REBOOT {
    NAME        Multi Stage Boot Fast Reboot bundle
    SYNOPSIS    Configures itl_sandybridge BOOTAPP to do Fast reboots.
    COMPONENTS  INCLUDE_MULTI_STAGE_BOOT INCLUDE_FAST_REBOOT
}

Bundle BUNDLE_MSB_WARM_REBOOT {
    NAME        Multi Stage Boot Warm Reboot bundle
    SYNOPSIS    Configures itl_sandybridge BOOTAPP to do warm reboots.
    COMPONENTS  INCLUDE_MULTI_STAGE_BOOT INCLUDE_MULTI_STAGE_WARM_REBOOT
}
