/* vxStage1Boot.s - IA First Stage Boot */

/*
 * Copyright (c) 2011, 2012 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01f,31aug12,g_x  Jump to ROM_WARM_HIGH as the entry point of the second
                 stage. (WIND00332348)
01e,30jul12,g_x  Use .code16 instead of 0x66 prefix before call to avoid 
                 operand sizes of call and ret mismatch. (WIND00364527)
01d,28jun12,wyt  Add 0x66 operand size prefix (WIND00355697).
01c,16jan12,wyt  Skip bootA20On if > 1MB memory accessible (WIND00326992).
01b,16aug11,jjk  WIND00263692, Multi-stage boot support - merged
01a,30jun11,j_l  written based on itl_nehalem_64/vxStage1Boot.s (01c).
*/

/*
DESCRIPTION
This module contains the first stage boot code for IA /bootapps.
It reads the bootapp via int 13 and stores the file at RAM_LOW_ADRS
in 32bit space.  The bootapp is then entered at RAM_LOW_ADRS +
0x50 (RAM_WARM_BOOT).
This allows very large bootapp images to be used.

For PXE (Pre-boot Execution Environment) boot, first stage boot loader
uses PXE's underyling UNDI (Universal Network Device
Interface) API to load the second stage bootapp.sys.
'int 0x1A' PXE BIOS interface is used to
determine whether bootApp has been loaded via PXE.  If
it is a PXE load, "PXENV+" structure is retrieved.  Next,
"!PXE" is retrieved (offset from beginning of "PXENV+"
structure).  Subsequent PXENV_GET_CACHED_INFO PXE API call
(using additional information from "!PXE") is used to
retrieve the TFTP host and TFTP directory information.
Finally, PXENV_TFTP_READ_FILE PXE API call is used to load
the second stage bootapp.sys file.  First stage boot only
supports accessing PXE API functions using "!PXE" structure as
specified above.
*/

#define _ASMLANGUAGE
#include <vxWorks.h>
#include <sysLib.h>
#include <asm.h>
#include <version.h>
#include <config.h>
#ifdef _WRS_CONFIG_LP64
#include <arch/i86/x86_64/regsX86_64.h>
#else
#include <arch/i86/regsI86.h>
#endif /* _WRS_CONFIG_LP64 */

#define SP_ARG1         4

#define F_ROM_ADRS(x) (FIRST_TEXT_ADRS + FUNC(x) - FUNC(firstBoot))

#ifndef CODE_SEGMENT
#define CODE_SEGMENT                    0x08
#endif /* CODE_SEGMENT */
#ifndef DATA_SEGMENT
#define DATA_SEGMENT                    0x10
#endif /* DATA_SEGMENT */
#define CODE_SEGMENT_UNREAL             0x28
#define DATA_SEGMENT_UNREAL             0x30

#define INCLUDE_PXE_BOOT

#ifdef  INCLUDE_PXE_BOOT

#define PXE_SEGMENT_OFFSET              0x2

#define PXE_VP                          0x5650
#define PXE_VN                          0x564E

#define PXE_PXENV_STRING                0x4E455850 /* "PXEN" */
#define PXE_PXE_STRING                  0x45585021 /* "!PXE" */

#define PXE_DIR_CHAR                    0x2f       /* "/"    */

#define PXE_BOOTFILE_NAME               "bootapp.sys"
#define PXE_BOOTFILE_NAME_LEN           0xC        /* including null end of str char */

/* PXENV+ Structure */
#define PXE_ENV_SIGNATURE_OFFSET        0x00
#define PXE_ENV_SIGNATURE_SIZE          0x06
#define PXE_ENV_VERSION_OFFSET          (PXE_ENV_SIGNATURE_OFFSET+PXE_ENV_SIGNATURE_SIZE)
#define PXE_ENV_VERSION_SIZE            0x02
#define PXE_ENV_LENGTH_OFFSET           (PXE_ENV_VERSION_OFFSET+PXE_ENV_VERSION_SIZE)
#define PXE_ENV_LENGTH_SIZE             0x01
#define PXE_ENV_CHECKSUM_OFFSET         (PXE_ENV_LENGTH_OFFSET+PXE_ENV_LENGTH_SIZE)
#define PXE_ENV_CHECKSUM_SIZE           0x01
#define PXE_ENV_RMENTRY_OFFSET          (PXE_ENV_CHECKSUM_OFFSET+PXE_ENV_CHECKSUM_SIZE)
#define PXE_ENV_RMENTRY_SIZE            0x04
#define PXE_ENV_PMOFFSET_OFFSET         (PXE_ENV_RMENTRY_OFFSET+PXE_ENV_RMENTRY_SIZE)
#define PXE_ENV_PMOFFSET_SIZE           0x04
#define PXE_ENV_PMSELECTOR_OFFSET       (PXE_ENV_PMOFFSET_OFFSET+PXE_ENV_PMOFFSET_SIZE)
#define PXE_ENV_PMSELECTOR_SIZE         0x02
#define PXE_ENV_STACKSEG_OFFSET         (PXE_ENV_PMSELECTOR_OFFSET+PXE_ENV_PMSELECTOR_SIZE)
#define PXE_ENV_STACKSEG_SIZE           0x02
#define PXE_ENV_STACKSIZE_OFFSET        (PXE_ENV_STACKSEG_OFFSET+PXE_ENV_STACKSEG_SIZE)
#define PXE_ENV_STACKSIZE_SIZE          0x02
#define PXE_ENV_BCCODESEG_OFFSET        (PXE_ENV_STACKSIZE_OFFSET+PXE_ENV_STACKSIZE_SIZE)
#define PXE_ENV_BCCODESEG_SIZE          0x02
#define PXE_ENV_BCCODESIZE_OFFSET       (PXE_ENV_BCCODESEG_OFFSET+PXE_ENV_BCCODESEG_SIZE)
#define PXE_ENV_BCCODESIZE_SIZE         0x02
#define PXE_ENV_BCDATASEG_OFFSET        (PXE_ENV_BCCODESIZE_OFFSET+PXE_ENV_BCCODESIZE_SIZE)
#define PXE_ENV_BCDATASEG_SIZE          0x02
#define PXE_ENV_BCDATASIZE_OFFSET       (PXE_ENV_BCDATASEG_OFFSET+PXE_ENV_BCDATASEG_SIZE)
#define PXE_ENV_BCDATASIZE_SIZE         0x02
#define PXE_ENV_UNDIDATASEG_OFFSET      (PXE_ENV_BCDATASIZE_OFFSET+PXE_ENV_BCDATASIZE_SIZE)
#define PXE_ENV_UNDIDATASEG_SIZE        0x02
#define PXE_ENV_UNDIDATASIZE_OFFSET     (PXE_ENV_UNDIDATASEG_OFFSET+PXE_ENV_UNDIDATASEG_SIZE)
#define PXE_ENV_UNDIDATASIZE_SIZE       0x02
#define PXE_ENV_UNDICODESEG_OFFSET      (PXE_ENV_UNDIDATASIZE_OFFSET+PXE_ENV_UNDIDATASIZE_SIZE)
#define PXE_ENV_UNDICODESEG_SIZE        0x02
#define PXE_ENV_UNDICODESIZE_OFFSET     (PXE_ENV_UNDICODESEG_OFFSET+PXE_ENV_UNDICODESEG_SIZE)
#define PXE_ENV_UNDICODESIZE_SIZE       0x02
#define PXE_ENV_PXEPTR_OFFSET           (PXE_ENV_UNDICODESIZE_OFFSET+PXE_ENV_UNDICODESIZE_SIZE)
#define PXE_ENV_PXEPTR_SIZE             0x04
#define PXE_ENV_LEN                     (PXE_ENV_PXEPTR_OFFSET+PXE_ENV_PXEPTR_SIZE)

/* !PXE Structure */
#define PXE_SIGNATURE_OFFSET            0x00
#define PXE_SIGNATURE_SIZE              0x04
#define PXE_STRUCTLENGTH_OFFSET         (PXE_SIGNATURE_OFFSET+PXE_SIGNATURE_SIZE)
#define PXE_STRUCTLENGTH_SIZE           0x01
#define PXE_STRUCTCKSUM_OFFSET          (PXE_STRUCTLENGTH_OFFSET+PXE_STRUCTLENGTH_SIZE)
#define PXE_STRUCTCKSUM_SIZE            0x01
#define PXE_STRUCTREV_OFFSET            (PXE_STRUCTCKSUM_OFFSET+PXE_STRUCTCKSUM_SIZE)
#define PXE_STRUCTREV_SIZE              0x01
#define PXE_RESERVED0_OFFSET            (PXE_STRUCTREV_OFFSET+PXE_STRUCTREV_SIZE)
#define PXE_RESERVED0_SIZE              0x01
#define PXE_UNDIROMID_OFFSET            (PXE_RESERVED0_OFFSET+PXE_RESERVED0_SIZE)
#define PXE_UNDIROMID_SIZE              0x04
#define PXE_BASEROMID_OFFSET            (PXE_UNDIROMID_OFFSET+PXE_UNDIROMID_SIZE)
#define PXE_BASEROMID_SIZE              0x04
#define PXE_ENTRYPOINTSP_OFFSET         (PXE_BASEROMID_OFFSET+PXE_BASEROMID_SIZE)
#define PXE_ENTRYPOINTSP_SIZE           0x04
#define PXE_ENTRYPOINTESP_OFFSET        (PXE_ENTRYPOINTSP_OFFSET+PXE_ENTRYPOINTSP_SIZE)
#define PXE_ENTRYPOINTESP_SIZE          0x04
#define PXE_STATUSCALLOUT_OFFSET        (PXE_ENTRYPOINTESP_OFFSET+PXE_ENTRYPOINTESP_SIZE)
#define PXE_STATUSCALLOUT_SIZE          0x04
#define PXE_RESERVED1_OFFSET            (PXE_STATUSCALLOUT_OFFSET+PXE_STATUSCALLOUT_SIZE)
#define PXE_RESERVED1_SIZE              0x01
#define PXE_SEGDESCCOUNT_OFFSET         (PXE_RESERVED1_OFFSET+PXE_RESERVED1_SIZE)
#define PXE_SEGDESCCOUNT_SIZE           0x01
#define PXE_FIRSTSELECTOR_OFFSET        (PXE_SEGDESCCOUNT_OFFSET+PXE_SEGDESCCOUNT_SIZE)
#define PXE_FIRSTSELECTOR_SIZE          0x02
#define PXE_STACK_OFFSET                (PXE_FIRSTSELECTOR_OFFSET+PXE_FIRSTSELECTOR_SIZE)
#define PXE_STACK_SIZE                  0x08
#define PXE_UNDIDATA_OFFSET             (PXE_STACK_OFFSET+PXE_STACK_SIZE)
#define PXE_UNDIDATA_SIZE               0x08
#define PXE_UNDICODE_OFFSET             (PXE_UNDIDATA_OFFSET+PXE_UNDIDATA_SIZE)
#define PXE_UNDICODE_SIZE               0x08
#define PXE_UNDICODEWRITE_OFFSET        (PXE_UNDICODE_OFFSET+PXE_UNDICODE_SIZE)
#define PXE_UNDICODEWRITE_SIZE          0x08
#define PXE_BCDATA_OFFSET               (PXE_UNDICODEWRITE_OFFSET+PXE_UNDICODEWRITE_SIZE)
#define PXE_BCDATA_SIZE                 0x08
#define PXE_BCCODE_OFFSET               (PXE_BCDATA_OFFSET+PXE_BCDATA_SIZE)
#define PXE_BCCODE_SIZE                 0x08
#define PXE_BCCODEWRITE_OFFSET          (PXE_BCCODE_OFFSET+PXE_BCCODE_SIZE)
#define PXE_BCCODEWRITE_SIZE            0x08
#define PXE_LEN                         (PXE_BCCODEWRITE_OFFSET+PXE_BCCODEWRITE_SIZE)

/* PXE API */
#define PXENV_UNLOAD_STACK              0x0070
#define PXENV_GET_CACHED_INFO           0x0071
#define PXENV_RESTART_TFTP              0x0073
#define PXENV_START_UNDI                0x0000
#define PXENV_STOP_UNDI                 0x0015
#define PXENV_START_BASE                0x0075
#define PXENV_STOP_BASE                 0x0076
#define PXENV_TFTP_OPEN                 0x0020
#define PXENV_TFTP_CLOSE                0x0021
#define PXENV_TFTP_READ                 0x0022
#define PXENV_TFTP_READ_FILE            0x0023
#define PXENV_TFTP_GET_FSIZE            0x0025
#define PXENV_UDP_OPEN                  0x0030
#define PXENV_UDP_CLOSE                 0x0031
#define PXENV_UDP_WRITE                 0x0033
#define PXENV_UDP_READ                  0x0032
#define PXENV_UNDI_STARTUP              0x0001
#define PXENV_UNDI_CLEANUP              0x0002
#define PXENV_UNDI_INITIALIZE           0x0003
#define PXENV_UNDI_RESET_ADAPTER        0x0004
#define PXENV_UNDI_SHUTDOWN             0x0005
#define PXENV_UNDI_OPEN                 0x0006
#define PXENV_UNDI_CLOSE                0x0007
#define PXENV_UNDI_TRANSMIT             0x0008
#define PXENV_UNDI_SET_MCAST_ADDRESS    0x0009
#define PXENV_UNDI_SET_STATION_ADDRESS  0x000A
#define PXENV_UNDI_SET_PACKET_FILTER    0x000B
#define PXENV_UNDI_GET_INFORMATION      0x000C
#define PXENV_UNDI_GET_STATISTICS       0x000D
#define PXENV_UNDI_CLEAR_STATISTICS     0x000E
#define PXENV_UNDI_INITIATE_DIAGS       0x000F
#define PXENV_UNDI_FORCE_INTERRUPT      0x0010
#define PXENV_UNDI_GET_MCAST_ADDRESS    0x0011
#define PXENV_UNDI_GET_NIC_TYPE         0x0012
#define PXENV_UNDI_GET_IFACE_INFO       0x0013
#define PXENV_UNDI_GET_STATE            0x0015
#define PXENV_UNDI_ISR                  0x0014

/* s_PXENV_GET_CACHED_INFO */
#define PXENV_GET_CACHED_INFO_STATUS             0x00
#define PXENV_GET_CACHED_INFO_STATUS_SIZE        0x02
#define PXENV_GET_CACHED_INFO_PACKETTYPE         (PXENV_GET_CACHED_INFO_STATUS+PXENV_GET_CACHED_INFO_STATUS_SIZE)
#define PXENV_GET_CACHED_INFO_PACKETTYPE_SIZE    0x02
#define PXENV_GET_CACHED_INFO_BUFFERSIZE         (PXENV_GET_CACHED_INFO_PACKETTYPE+PXENV_GET_CACHED_INFO_PACKETTYPE_SIZE)
#define PXENV_GET_CACHED_INFO_BUFFERSIZE_SIZE    0x02
#define PXENV_GET_CACHED_INFO_BUFFER             (PXENV_GET_CACHED_INFO_BUFFERSIZE+PXENV_GET_CACHED_INFO_BUFFERSIZE_SIZE)
#define PXENV_GET_CACHED_INFO_BUFFER_SIZE        0x04
#define PXENV_GET_CACHED_INFO_BUFFERLIMIT        (PXENV_GET_CACHED_INFO_BUFFER+PXENV_GET_CACHED_INFO_BUFFER_SIZE)
#define PXENV_GET_CACHED_INFO_BUFFERLIMIT_SIZE   0x02
#define PXENV_GET_CACHED_INFO_LEN                (PXENV_GET_CACHED_INFO_BUFFERLIMIT+PXENV_GET_CACHED_INFO_BUFFERLIMIT_SIZE)

#define PXENV_PACKET_TYPE_DHCP_DISCOVER 1
#define PXENV_PACKET_TYPE_DHCP_ACK      2
#define PXENV_PACKET_TYPE_CACHED_REPLY  3

/* bootph - cached packet format */
#define PXENV_GET_CACHED_PACKET_OPCODE           0x00
#define PXENV_GET_CACHED_PACKET_OPCODE_SIZE      0x01
#define PXENV_GET_CACHED_PACKET_HARDWARE         (PXENV_GET_CACHED_PACKET_OPCODE+PXENV_GET_CACHED_PACKET_OPCODE_SIZE)
#define PXENV_GET_CACHED_PACKET_HARDWARE_SIZE    0x01
#define PXENV_GET_CACHED_PACKET_HARDWARELEN      (PXENV_GET_CACHED_PACKET_HARDWARE+PXENV_GET_CACHED_PACKET_HARDWARE_SIZE)
#define PXENV_GET_CACHED_PACKET_HARDWARELEN_SIZE 0x01
#define PXENV_GET_CACHED_PACKET_GATEHOPS         (PXENV_GET_CACHED_PACKET_HARDWARELEN+PXENV_GET_CACHED_PACKET_HARDWARELEN_SIZE)
#define PXENV_GET_CACHED_PACKET_GATEHOPS_SIZE    0x01
#define PXENV_GET_CACHED_PACKET_IDENT            (PXENV_GET_CACHED_PACKET_GATEHOPS+PXENV_GET_CACHED_PACKET_GATEHOPS_SIZE)
#define PXENV_GET_CACHED_PACKET_IDENT_SIZE       0x04
#define PXENV_GET_CACHED_PACKET_SECONDS          (PXENV_GET_CACHED_PACKET_IDENT+PXENV_GET_CACHED_PACKET_IDENT_SIZE)
#define PXENV_GET_CACHED_PACKET_SECONDS_SIZE     0x02
#define PXENV_GET_CACHED_PACKET_FLAGS            (PXENV_GET_CACHED_PACKET_SECONDS+PXENV_GET_CACHED_PACKET_SECONDS_SIZE)
#define PXENV_GET_CACHED_PACKET_FLAGS_SIZE       0x02
#define PXENV_GET_CACHED_PACKET_CIP              (PXENV_GET_CACHED_PACKET_FLAGS+PXENV_GET_CACHED_PACKET_FLAGS_SIZE)
#define PXENV_GET_CACHED_PACKET_CIP_SIZE         0x04
#define PXENV_GET_CACHED_PACKET_YIP              (PXENV_GET_CACHED_PACKET_CIP+PXENV_GET_CACHED_PACKET_CIP_SIZE)
#define PXENV_GET_CACHED_PACKET_YIP_SIZE         0x04
#define PXENV_GET_CACHED_PACKET_SIP              (PXENV_GET_CACHED_PACKET_YIP+PXENV_GET_CACHED_PACKET_YIP_SIZE)
#define PXENV_GET_CACHED_PACKET_SIP_SIZE         0x04
#define PXENV_GET_CACHED_PACKET_GIP              (PXENV_GET_CACHED_PACKET_SIP+PXENV_GET_CACHED_PACKET_SIP_SIZE)
#define PXENV_GET_CACHED_PACKET_GIP_SIZE         0x04
#define PXENV_GET_CACHED_PACKET_CADDR            (PXENV_GET_CACHED_PACKET_GIP+PXENV_GET_CACHED_PACKET_GIP_SIZE)
#define PXENV_GET_CACHED_PACKET_CADDR_SIZE       0x10
#define PXENV_GET_CACHED_PACKET_SNAME            (PXENV_GET_CACHED_PACKET_CADDR+PXENV_GET_CACHED_PACKET_CADDR_SIZE)
#define PXENV_GET_CACHED_PACKET_SNAME_SIZE       0x40
#define PXENV_GET_CACHED_PACKET_BOOTFILE         (PXENV_GET_CACHED_PACKET_SNAME+PXENV_GET_CACHED_PACKET_SNAME_SIZE)
#define PXENV_GET_CACHED_PACKET_BOOTFILE_SIZE    0x80
#define PXENV_GET_CACHED_PACKET_LEN              (PXENV_GET_CACHED_PACKET_BOOTFILE+PXENV_GET_CACHED_PACKET_BOOTFILE_SIZE)

/* s_PXENV_TFTP_READ_FILE */
#define PXENV_TFTP_READ_FILE_STATUS              0x00
#define PXENV_TFTP_READ_FILE_STATUS_SIZE         0x02
#define PXENV_TFTP_READ_FILE_FILENAME            (PXENV_TFTP_READ_FILE_STATUS+PXENV_TFTP_READ_FILE_STATUS_SIZE)
#define PXENV_TFTP_READ_FILE_FILENAME_SIZE       PXENV_GET_CACHED_PACKET_BOOTFILE_SIZE
#define PXENV_TFTP_READ_FILE_BUFFERSIZE          (PXENV_TFTP_READ_FILE_FILENAME+PXENV_TFTP_READ_FILE_FILENAME_SIZE)
#define PXENV_TFTP_READ_FILE_BUFFERSIZE_SIZE     0x04
#define PXENV_TFTP_READ_FILE_BUFFER              (PXENV_TFTP_READ_FILE_BUFFERSIZE+PXENV_TFTP_READ_FILE_BUFFERSIZE_SIZE)
#define PXENV_TFTP_READ_FILE_BUFFER_SIZE         0x04
#define PXENV_TFTP_READ_FILE_SERVERIPADDRESS     (PXENV_TFTP_READ_FILE_BUFFER+PXENV_TFTP_READ_FILE_BUFFER_SIZE)
#define PXENV_TFTP_READ_FILE_SERVERIPADDRESS_SIZE 0x04
#define PXENV_TFTP_READ_FILE_GATEWAYIPADDRESS     (PXENV_TFTP_READ_FILE_SERVERIPADDRESS+PXENV_TFTP_READ_FILE_SERVERIPADDRESS_SIZE)
#define PXENV_TFTP_READ_FILE_GATEWAYIPADDRESS_SIZE 0x04
#define PXENV_TFTP_READ_FILE_MCASTIPADDRESS      (PXENV_TFTP_READ_FILE_GATEWAYIPADDRESS+PXENV_TFTP_READ_FILE_GATEWAYIPADDRESS_SIZE)
#define PXENV_TFTP_READ_FILE_MCASTIPADDRESS_SIZE 0x04
#define PXENV_TFTP_READ_FILE_TFTPCLNTPORT        (PXENV_TFTP_READ_FILE_MCASTIPADDRESS+PXENV_TFTP_READ_FILE_MCASTIPADDRESS_SIZE)
#define PXENV_TFTP_READ_FILE_TFTPCLNTPORT_SIZE   0x02
#define PXENV_TFTP_READ_FILE_TFTPSRVPORT         (PXENV_TFTP_READ_FILE_TFTPCLNTPORT+PXENV_TFTP_READ_FILE_TFTPCLNTPORT_SIZE)
#define PXENV_TFTP_READ_FILE_TFTPSRVPORT_SIZE    0x02
#define PXENV_TFTP_READ_FILE_TFTPOPENTIMEOUT     (PXENV_TFTP_READ_FILE_TFTPSRVPORT+PXENV_TFTP_READ_FILE_TFTPSRVPORT_SIZE)
#define PXENV_TFTP_READ_FILE_TFTPOPENTIMEOUT_SIZE 0x02
#define PXENV_TFTP_READ_FILE_TFTPREOPENDELAY     (PXENV_TFTP_READ_FILE_TFTPOPENTIMEOUT+PXENV_TFTP_READ_FILE_TFTPOPENTIMEOUT_SIZE)
#define PXENV_TFTP_READ_FILE_TFTPREOPENDELAY_SIZE 0x02
#define PXENV_TFTP_READ_FILE_LEN                 (PXENV_TFTP_READ_FILE_TFTPREOPENDELAY+PXENV_TFTP_READ_FILE_TFTPREOPENDELAY_SIZE)

/* Max size parameter between s_PXENV_GET_CACHED_INFO, s_PXENV_TFTP_READ_FILE, etc. */
#define PXENV_MAX_PARAMETER_LEN                  PXENV_TFTP_READ_FILE_LEN

#endif /* INCLUDE_PXE_BOOT */

        /* internals */

        .globl  firstBoot               /* start of system code */
        .globl  _firstBoot              /* start of system code */
        .globl  GTEXT(bootA20on)        /* turn on A20 */
        .globl  GTEXT(bootSecondary)
        .globl  GTEXT(strout)

        .text
        .balign 16

        .code32

/*******************************************************************************
*
* firstBoot - entry point for bootrom.bin
*
*/

        /* entry point in REAL MODE(16 bits) */

firstBoot:
_firstBoot:
        cli                             /* LOCK INTERRUPT */
        .byte   0x67, 0x66
        ljmp    $0x0, $ FIRST_TEXT_ADRS + cold - firstBoot

        /* Reentry point - PROTECTED MODE(32 bits) */
        
        .balign 16,0x90
rebootEntryHigh:
        cli                             /* LOCK INTERRUPT */

        mov     SP_ARG1(%esp),%ebx      /* %ebx has the startType */

        jmp     reBoot
        
        .balign 16,0x90
rebootEntryLow:
        /* This section not implemented */
        jmp     reBoot

        /* copyright notice appears at beginning of ROM (in TEXT segment) */

        .ascii   "Copyright 2011 Wind River Systems, Inc."

        /* cold start code in REAL MODE(16 bits) */

        .balign 16,0x90
cold:
        
        .byte   0x66
        mov     $BOOT_COLD,%ebx       /* %ebx has the startType */

        /* Init seg regs */

        mov     %cs,%ax
        mov     %ax,%ds

        xor     %ax,%ax
        mov     %ax,%ss
        mov     %ax,%es

        .byte   0x66                    /* next inst has 32bit operand */
        mov     $ROM_STACK,%esp         /* set lower mem stack pointer */

        .byte   0x67, 0x66              /* next inst has 32bit operand */
        sgdt    F_ROM_ADRS(vxinit_gdtr)

        .byte   0x67, 0x66              /* next inst has 32bit operand */
        sidt    F_ROM_ADRS(vxinit_idtr)

        /* switch to protected mode */

        .byte   0x67, 0x66              /* next inst has 32bit operand */
        lgdt    F_ROM_ADRS(bootGdtr)    /* load temporary GDT */

        mov     %cr0,%eax               /* move CR0 to EAX */
        or      $CR0_PE,%eax            /* set the PE bit */
        mov     %eax,%cr0               /* move EAX to CR0 */

        .byte   0x67, 0x66              /* next inst has 32bit operand */
        ljmp    $CODE_SEGMENT_UNREAL, $ FIRST_TEXT_ADRS + firstBoot1 - firstBoot

        .balign 16,0x90
firstBoot1:
        .byte   0x66                    /* next inst has 32bit operand */
        mov     $DATA_SEGMENT_UNREAL,%eax /* set data segment */
        mov     %ax,%ds                 /* set DS */
        mov     %ax,%es                 /* set ES */
        mov     %ax,%fs                 /* set FS */
        mov     %ax,%gs                 /* set GS */
        mov     %ax,%ss                 /* set SS */

        /* switch to unreal mode */

        mov     %cr0,%eax               /* move CR0 to EAX */
        and     $(~CR0_PE),%eax         /* unset the PE bit */
        mov     %eax,%cr0               /* move EAX to CR0 */
        
        .byte   0x67, 0x66              /* next inst has 32bit operand */
        ljmp    $0x0, $ FIRST_TEXT_ADRS + firstBoot2 - firstBoot

        .balign 16,0x90
firstBoot2:

        .byte   0x67, 0x66              /* next inst has 32bit operand */
        lgdt    F_ROM_ADRS(vxinit_gdtr)
        
        .byte   0x67, 0x66              /* next inst has 32bit operand */
        lidt    F_ROM_ADRS(vxinit_idtr)
        
        /* Init seg regs */

        xor     %ax,%ax
        mov     %ax,%ds
        mov     %ax,%es
        mov     %ax,%fs
        mov     %ax,%gs
        mov     %ax,%ss

        .byte   0x67, 0x66              /* next inst has 32bit operand */
        mov     %ebx, vxinit_start_type /* %ebx has the startType */
        .code16
        call    bootA20on
        .code32

        /* Initialize some variables */
        
        .byte   0x67, 0x66              /* next inst has 32bit operand */
        mov     $BOOT_IMAGE_SIZE,%eax
        .byte   0x67, 0x66              /* next inst has 32bit operand */
        movl    $0, (%eax)              /* size of binary image */

#ifdef INCLUDE_PXE_BOOT

        /* No PXE warm reboot support */

        .byte   0x67, 0x66              /* next inst has 32bit operand */
        cmpl    $BOOT_COLD, vxinit_start_type
        jne     1f

        .byte   0x67, 0x66              /* next inst has 32bit operand */
        movl    $0x0, vxpxe_pxe

        /* Check whether we are getting booted using PXE                */
        
        /* From PXE Specification:                                      */
        /* ----------------------                                       */
        /* Real mode (Int 1Ah Function 5650h)                           */
        /* Enter:                                                       */
        /*        AX := 5650h (VP)                                      */
        /* Exit:                                                        */
        /*        AX := 564Eh (VN)                                      */
        /*        ES := 16-bit segment address of the PXENV+ structure. */
        /*        BX := 16-bit offset of the PXENV+.                    */
        /*        EDX := may be trashed by the UNDI INT 1Ah handler.    */
        /*        All other register contents are preserved.            */
        /*        CF is cleared.                                        */
        /*        IF is preserved.                                      */
        /*        All other flags are undefined.                        */

        .byte   0x66                    /* next inst has 32bit operand */
        mov     $PXE_VP, %eax
        .byte   0x66                    /* next inst has 32bit operand */
        mov     $0, %ebx
        int     $0x1A

        .byte   0x66                    /* next inst has 32bit operand */
        cmp     $PXE_VN, %eax
        .byte   0x66                    /* next inst has 32bit operand */
        je      boot_pxe

        /* Not PXE boot */

1:
#endif /* INCLUDE_PXE_BOOT */
        
        .byte   0x66
        jmp     copyDevData

        .balign 16,0x90
copyDevData:

        /* Copy device data and MBR tmp values */

        .byte   0x67, 0x66
        mov     tempVarsAdr,%esi        /* Get address of MBR Temp Vars */
        .byte   0x67, 0x66
        mov     $ (FUNC(data_startl)),%edi     /* and where to copy */
        .byte   0x67, 0x66
        mov     tempVarsLen,%ecx               /* and how many bytes */
        cld
        rep
        movsb                                   /* copy bytes */

        jmp     copyMbrData

        .balign 16,0x90
copyMbrData:
        .byte   0x67, 0x66
        mov     bootDevAdr,%esi                /* Get address of MBR device info */
        .byte   0x67, 0x66
        mov     $ (FUNC(oemname)),%edi         /* and where to copy */
        .byte   0x67, 0x66
        mov     bootDevLen,%ecx                /* and how many bytes */
        cld
        rep
        movsb                                   /* copy bytes */

        jmp     firstBootSetupComplete

        .balign 16,0x90
firstBootSetupComplete:
        .byte   0x67, 0x66
        xor     %ebp,%ebp               /* initialize the frame pointer */
        .byte   0x67, 0x66
        push    $0                      /* initialise the EFLAGS */
        .byte   0x67, 0x66
        popf

        .byte   0x67, 0x66
        call    bootSecondary

        /* just in case, if there's a problem in bootSecondary */

firstBootHlt:
        hlt
        jmp     firstBootHlt

        .balign 16,0x90
reBoot:
        xor     %eax, %eax              /* zero EAX */
        mov     %cr0, %edx              /* get CR0 */
        and     $0x7ffafff1, %edx       /* clear PG, AM, WP, TS, EM, MP */
        mov     %edx, %cr0              /* set CR0 */

        pushl   %eax                    /* initialize EFLAGS */
        popfl 

        xor     %eax, %eax              /* zero EAX */
        mov     %eax, %cr4              /* initialize CR4 */
        mov     %eax, %cr3              /* initialize CR3 */
        
        mov     $BOOT_IMAGE_ADRS,%esi   /* Get address of bootapp binary */
        movl    (%esi),%esi
        or      %esi,%esi
        je      reBootReset             /* If zero then reset */

        mov     $BOOT_IMAGE_SIZE,%ecx
        movl    (%ecx),%ecx             /* Get size of binary image */
        or      %ecx,%ecx
        je      reBootReset             /* If zero then reset */
        shr     $2, %ecx                /* convert to longs */

        mov     $ROM_TEXT_ADRS,%edi      /* Get the to address */

        cld                             /* clear the direction bit */
        rep
        movsl                           /* Copy binary to RAM_LOW_ADRS */

        mov     $ROM_TEXT_ADRS,%eax
        mov     %eax,%esp               /* Set stack pointer */
        add     $ROM_WARM_HIGH,%eax
        pushl   %ebx                    /* %ebx has the startType */
        call    *%eax

reBootReset:

#ifdef INCLUDE_PXE_BOOT
        /* If PXE warm reboot, do cold reboot instead */

        cmp     $0x0, vxpxe_pxe
        je      1f
        mov     $RST_CNT_REG, %edx
        mov     $ (RST_CNT_FULL_RST | RST_CNT_CPU_RST | RST_CNT_SYS_RST), %eax
	outb	%al, %dx
rebootEntryHighHlt:
        hlt
        jmp     rebootEntryHighHlt

        /* Not PXE warm reboot */
1:
#endif /* INCLUDE_PXE_BOOT */

        /* Back to 16bit and go to the beginning */

        lgdt    F_ROM_ADRS(bootGdtr)

        mov     $ROM_STACK,%esp         /* set lower mem stack pointer */

        ljmp    $CODE_SEGMENT_UNREAL, $ FIRST_TEXT_ADRS + firstBoot1 - firstBoot

/*******************************************************************************
*
* bootA20on - enable A20
*
* enable A20
*
* RETURNS: N/A
* void bootA20on (void)
*/

        .balign 16,0x90
bootA20on:

        /* If memory above 1MB is already working, ignore A20 gate set */

        .byte   0x67, 0x66
        movl    $0x000000,%eax		
        .byte   0x67, 0x66
        movl    $0x100000,%edx
        .byte   0x67, 0x66
        pushl   (%eax)
        .byte   0x67, 0x66
        pushl   (%edx)
        .byte   0x67, 0x66
        movl    $0x0,(%eax)
        .byte   0x67, 0x66
        movl    $0x0,(%edx)
        .byte   0x67, 0x66
        movl    $0x01234567,(%eax)
        .byte   0x67, 0x66
        cmpl    $0x01234567,(%edx)
        .byte   0x67, 0x66
        popl    (%edx)
        .byte   0x67, 0x66
        popl    (%eax)
        .byte   0x67, 0x66
        jne     bootA20onRet

        /* 8042 keyboard controller pin */

        .code16
        call    bootA20Wait  
        .code32
        .byte   0x66
        movl    $0xd1,%eax		/* Write command */
        outb    %al,$0x64
        .code16
        call    bootA20Wait 
        .code32

        .byte   0x66
        movl    $0xdf,%eax		/* Enable A20 */
        outb    %al, $0x60
        .code16
        call    bootA20Wait 
        .code32

        .byte   0x66
        movl    $0xff,%eax		/* NULL command */
        outb    %al,$0x64
        .code16
        call    bootA20Wait
        .code32

        .byte   0x67, 0x66
        movl    $0x000000,%eax		/* Check if it worked */
        .byte   0x67, 0x66
        movl    $0x100000,%edx
        .byte   0x67, 0x66
        pushl   (%eax)
        .byte   0x67, 0x66
        pushl   (%edx)
        .byte   0x67, 0x66
        movl    $0x0,(%eax)
        .byte   0x67, 0x66
        movl    $0x0,(%edx)
        .byte   0x67, 0x66
        movl    $0x01234567,(%eax)
        .byte   0x67, 0x66
        cmpl    $0x01234567,(%edx)
        .byte   0x67, 0x66
        popl    (%edx)
        .byte   0x67, 0x66
        popl    (%eax)
        .byte   0x67, 0x66
        jne     bootA20onRet

        /* Fast gate A20 option */
   
        .byte   0x66
        movl    $0x02,%eax
        outb    %al,$0x92
        .byte   0x66
        movl    $100000, %ecx       /* Loop 100000 times */

bootA20on0:
        inb	    $0x92,%al
        andb    $0x02,%al
        loopz   bootA20on0

        .byte   0x67, 0x66
        movl    $0x000000,%eax		/* Check if it worked */
        .byte   0x67, 0x66
        movl    $0x100000,%edx
        .byte   0x67, 0x66
        pushl   (%eax)
        .byte   0x67, 0x66
        pushl   (%edx)
        .byte   0x67, 0x66
        movl    $0x0,(%eax)
        .byte   0x67, 0x66
        movl    $0x0,(%edx)
        .byte   0x67, 0x66
        movl    $0x01234567,(%eax)
        .byte   0x67, 0x66
        cmpl    $0x01234567,(%edx)
        .byte   0x67, 0x66
        popl    (%edx)
        .byte   0x67, 0x66
        popl    (%eax)
        .byte   0x67, 0x66
        jne     bootA20onRet

bootA20onFail:
        hlt
        jmp     bootA20onFail

bootA20onRet:
        ret

/*******************************************************************************
*
* bootA20Wait - wait until the input buffer become empty
*
* wait until the input buffer become empty
*
* RETURNS: N/A
* void bootA20Wait (void)
*/

        .balign 16,0x90
bootA20Wait:
        movl    $100000, %ecx   /* Loop 100000 times */
bootA20Wait0:
        .byte   0x66
        movl    $0x64,%edx		/* Check if it is ready to write */
        inb     %dx,%al
        andb    $2,%al
        loopnz  bootA20Wait0
        ret

/*******************************************************************************
*
* boot secondary boot image (original bootrom image)
*/

        .balign 16,0x90
bootSecondary:
        .byte   0x67, 0x66
        push    %bp
        mov     %sp,%bp

        /* Say we are here */
        .byte   0x67, 0x66
        mov     $firstBootId,%esi
        .code16
        call    strout
        .code32

        /* Setup Disc Params */

        /* Init some registers */
        
        .byte   0x67, 0x66
        xorl    %eax,%eax
        .byte   0x67, 0x66
        movl    %eax,%ebx
        .byte   0x67, 0x66
        movl    %eax,%ecx
        .byte   0x67, 0x66
        movl    %eax,%edx

        movb    $0x8,%ah       /* Read Drive Params Func */
        .byte   0x67
        movb    phy_drv,%dl    /* Drive */

        int     $0x13           /* Call the bios */
        cli                     /* Block interrupts */

        /* max sectors we can use w/BIOS in cx */
        and     $0x3F,%cx
        .code16
        movw    %cx, sec_per_track
        .code32

        /* max heads we can use w/BIOS in dx, add one for math */
        
        incb    %dh
        xor     %dl,%dl
        xchgb   %dl,%dh
        .code16
        movw    %dx, num_heads
        .code32

        /* Init some registers */
        
        .byte   0x67, 0x66
        xorl    %eax,%eax
        .byte   0x67, 0x66
        movl    %eax,%ebx
        .byte   0x67, 0x66
        movl    %eax,%ecx
        .byte   0x67, 0x66
        movl    %eax,%edx

        /* Find Boot Rom Image */
        
        .code16
        movb    num_fats,%al            /* How many FAT tables? */
        mulw    sec_per_fat             /* Total FAT secs now in AX. */
        movw    num_res_sec,%cx         /* How many reserved sectors in CX */
        addw    num_hid_sec,%cx         /* Hopefully hidden secs is equal */
        adcw    num_hid_sec2,%dx        /* to the partition value */
        add     %ax,%cx                /* CX has (rsec+(nfat*secperfat)) */
        adc     %bx,%dx                 /* BX = 0, we pick up any carry flag */

        /*
         * The number of sectors from the boot sector to the
         * root directory starting sector has been calculated
         * (works on FAT12/FAT16 only), we now store it
         */
        
        movw    %cx,data_startl
        movw    %dx,data_starth
        
        movw    res2,%ax               /* Use short File Names */
        mulw    root_size               /* res2 * # of root dir entries */
        movw    byt_per_sec,%si         /* bytes per sector in si */
        add     %si,%ax                 /* ax=(dirent_s * root_s)+byt_per_sec */
        dec     %ax                     /* ax--(this corrects the division) */
        div     %si                     /* ax=ax/si = number of sectors to read */
        mov     %ax,%cx                 /* number of root dir sectors to cx */

boot_next_root_sec:
        .byte   0x67, 0x66
        push    %cx                     /* store nSecs to read, note that cx is */
                                        /* decremented by the loop inst. below. */
        movw    data_startl,%ax         /* dx and ax together comprise the first */
        movw    data_starth,%dx         /* absolute sector read into the buffer */
        mov     $dir_buffer, %bx       /* bx = adrs of data buffer */

        call    boot_read_disk          /* get one sector into the buffer */
        jc      boot_read_error         /* report failure */

        /*
         * We have the sector data now, scan it for the file name:
         * FIND BOOTAPP.SYS
         */

        movw    byt_per_sec,%dx
        mov     $dir_buffer,%di        /* buffer to search */

boot_findfile:
        movw    $vxsys_name,%si         /* String to match */

        /*
         * scan for our boot file name in the data buffer
         * Loop through each entry in the sector
         */

        .byte   0x67, 0x66
        push    %di                     /* save di */
        mov     $11,%cx                 /* looking for 11 bytes */
        repe    cmpsb                   /* set zero flag on equal */
        .byte   0x67, 0x66
        pop     %di
        jz      boot_filefound          /* found it */

        /* point to next entry and try again */
        
        addw    res2,%di                /* next dirent */
        subw    res2,%dx                /* count processed */

        /* More to go in the sector? */
        
        jnz     boot_findfile

        addw    $1,data_startl          /* bump sector */
        adcw    $0,data_starth          /* get the carry */

        .byte   0x67, 0x66
        pop     %cx
        loop    boot_next_root_sec       /* try next sector */

        /* error no boot file found in root directory if we get here. */
        
        .byte   0x67, 0x66
        mov     $vxsys_error,%esi
        call    strout

        .code32
bootSecHlt:
        hlt
        jmp     bootSecHlt

        .byte   0x67, 0x66
        pop     %bp
        .byte   0x67, 0x66
        ret

        .balign 16, 0x90
boot_read_error:
        hlt
        jmp     boot_read_error

        .balign 16, 0x90
        .code16
boot_filefound:
        mov     $vxsys_name,%si
        call    strout

        .byte   0x67, 0x66
        pop     %cx                     /* Get remaining root sectors */
        addw    %cx,data_startl         /* add remaining root sectors. */
        adcw    $0,data_starth          /* and the carry */

        /* find the file size */
        
        xor     %cx,%cx                 /* cx = 0 */
        mov     $0x1c,%bx

        /* Check for long names */
        cmpw    $0x40,res2              /* Using long names? */
        jnz     boot_short_names
        add     $0x20,%bx               /* adjust for long names */

boot_short_names:
        add     %di,%bx                 /* points to lower file size */

        movl    (%ebx),%eax
        movl    %eax,vxsys_size         /* Save file size for FAST_BOOT */
        xor     %eax,%eax

        movw    (%bx),%ax               /* load lower file size */
        add     $2,%bx                  /* points to upper file size */
        movw    (%bx),%dx               /* File size in DX:AX */

        /*
         * We have the file size in ax & dx, now
         * work out number of sectors we need to load
         */
        
        divw    byt_per_sec             /* filesize / bytes per sec = nSecs */
        incw    %ax                     /* Get last sector */
        movw    %ax,num4sec             /* save number of sectors */

        /* Get the starting cluster for the data */
        
        sub     $4,%bx                  /* BX points to start cluster offset */
        movw    (%bx),%ax

        /* Adjust for two reserved fat entries */
        
        sub     $2,%ax

        /* Calculate the sector that matches start cluster */
        
        movb    sec_per_clu,%cl         /* Compute the starting sector */
        mulw    %cx                     /* CX, number of sectors to read */
        addw    data_startl, %ax        /* offset over already read secs */
        adc     data_starth, %dx        /* offset over already read secs */

        /*
         * dx:ax now has starting sector number
         * read the sectors sequentially into the data_buffer
         */

        xor     %bx,%bx                 /* In first segment, clear es */
        mov     %bx, %es

        movl    $ROM_TEXT_ADRS, vxsys_buffer_ptr

boot_next_sec:
        mov     $data_buffer,%bx        /* Get address of the data buffer */
        .byte   0x67, 0x66
        push    %ax                     /* save AX (sectors we read) */
        .byte   0x67, 0x66
        push    %dx                     /* save DX (sectors we read) */

        call    boot_read_disk          /* Read it */
        .byte   0x67, 0x66
        pop     %dx                     /* restore */
        .byte   0x67, 0x66
        pop     %ax                     /* restore */
        jc      boot_read_error         /* Error ? */

        mov     $dotstr,%si
        call    strout

        mov     $data_buffer,%esi       /* Get address of the data buffer */
        movl    vxsys_buffer_ptr,%edi   /* Get buffer address (RAM_LOW_ADRS+) */
        mov     $128,%ecx               /* Number of longs to copy */

        call    boot_copy_buffer        /* Copy buffer to RAM_LOW_ADDR+ */

        movl    %edi,vxsys_buffer_ptr   /* Save updated output buffer */

        decw    num4sec                 /* decr number of sector groups */
        jz      bootProtected           /* That's all there is */

        add     $1,%ax                  /* incr to next sector */
        adc     $0,%dx                  /* get the carry */
        jmp     boot_next_sec           /* Read next sector */

        .balign 16, 0x90
bootProtected:
        cli                             /* Make sure interrupts are disabled */

#ifdef INCLUDE_BIOS_E820_MEM_AUTOSIZE
        call    FUNC(vxBiosE820MapQuery)        /* Autosize while still in 16bit mode */
#endif /* INCLUDE_BIOS_E820_MEM_AUTOSIZE */

        .code32
        
        /* Switch to 32bit protected mode */
        
        xor     %ax, %ax
        mov     %ax, %ds
        mov     %ax, %es

	.byte	0x67, 0x66		/* next inst has 32bit operand */
        lidt    F_ROM_ADRS(bootIdtr)    /* load temporary IDT */

	.byte	0x67, 0x66		/* next inst has 32bit operand */
        lgdt    F_ROM_ADRS(bootGdtr)    /* load temporary GDT */

        /* switch to protected mode */

        mov     %cr0,%eax               /* move CR0 to EAX */
        or      $CR0_PE,%eax            /* set the PE bit */
        mov     %eax,%cr0               /* move EAX to CR0 */
        jmp     bootProtected1          /* near jump to flush a inst queue */

bootProtected1:
	.byte	0x66			/* next inst has 32bit operand */
        mov     $DATA_SEGMENT,%eax      /* set data segment 0x10 is 3rd one */
        mov     %ax,%ds                 /* set DS */
        mov     %ax,%es                 /* set ES */
        mov     %ax,%fs                 /* set FS */
        mov     %ax,%gs                 /* set GS */
        mov     %ax,%ss                 /* set SS */
	.byte	0x66			/* next inst has 32bit operand */
        mov     $ROM_STACK,%esp         /* set lower mem stack pointer */
	.byte	0x67, 0x66		/* next inst has 32bit operand */
        ljmp    $CODE_SEGMENT, $ FIRST_TEXT_ADRS + bootProtected2 - firstBoot

        .balign 16, 0x90
bootProtected2:
        
        /* Enter the Secondary bootrom at offset ROM_WARM_HIGH */
        
        mov     $BOOT_IMAGE_ADRS,%eax
        movl    $0, (%eax)              /* adrs of binary image */

        movl    vxsys_size,%eax         /* pass size of binary image */
        mov     $BOOT_IMAGE_SIZE,%ebx
        movl    %eax,(%ebx)             /* save size of binary image */
        mov     $ROM_TEXT_ADRS,%eax
        add     $ROM_WARM_HIGH,%eax
        pushl   vxinit_start_type
        call    *%eax

bootProtectedHalt:
        hlt
        jmp     bootProtectedHalt

/************************************************************************
 * boot_copy_buffer: copy buffer to high memory
 * esi = from address
 * edi = to address
 * ecx = number of long(4 byte) words  
 */
        
        .balign 16, 0x90
boot_copy_buffer:
        
        /* Save regs */
        
        .byte   0x67, 0x66
        push    %eax

boot_copy_buffer_loop:
        .byte   0x67, 0x66
        movl    (%esi),%eax             /* Load long from buffer */
        .byte   0x67, 0x66
        movl    %eax,(%edi)             /* Store the value */
        .byte   0x67, 0x66
        addl    $4,%esi                 /* increment input buffer ptr */
        .byte   0x67, 0x66
        addl    $4,%edi                 /* increment output buffer ptr */
        .byte   0x67, 0x66
        decl    %ecx                    /* decrement count */
        jne     boot_copy_buffer_loop

        /* restore regs */
        
        .byte   0x67, 0x66
        pop     %eax

        retl

/************************************************************************
 * Read disk
 * Entry: DX,AX - Absolute sector to read
 *        ES:BX - address of buffer.
 * 16 bit code
 * Exit:  CF - Clear if read successful.
 */
        
        .balign 16, 0x90
        .code16
boot_read_disk:
        movw    %bx,buffer_ptr          /* save adrs of buffer */
        movw    sec_per_track,%bx       /* Get sectors per track */
        divw    %bx                     /* data_sector/sec_per_track */
        incw    %dx                     /* increment remainder */
        movw    %dx,%bx                 /* Save high sector word */
        xorw    %dx,%dx                 /* clear dx */
        divw    num_heads               /* Compute head and cylinder */
        xchgb   %al,%ah                 /* Swap cyl low and high bytes */
        movb    $6,%cl
        shlb    %cl,%al                 /* Shift high cyl bits */
        xchgw   %ax,%cx                 /* Copy cyl into CX */
        orb     %bl,%cl                 /* Combine cyl with sector */
        movb    %dl,%dh                 /* Move head number */
        movw    $5,%bx                  /* retry 5 times */

boot_read_retry:
        .byte   0x67, 0x66
        push    %bx                     /* save N retries */
        movw    buffer_ptr,%bx          /* get buffer ptr */
        movb    phy_drv,%dl             /* Get disk to read */
        movw    $0x201,%ax              /* always read one sector */
        .byte   0x67, 0x66
        push    %cx
        .byte   0x67, 0x66
        push    %dx
        int     $0x13                   /* Read Disk */
        .byte   0x67, 0x66
        pop     %dx
        .byte   0x67, 0x66
        pop     %cx
        jc      boot_read_reset         /* Error, try again. */
        .byte   0x67, 0x66
        pop     %bx                     /* Restore retry count */
        clc
boot_read_exit:
        ret

boot_read_reset:
        xor     %ax,%ax                 /* Reset disk before reading */
        int     $0x13
        .byte   0x67, 0x66
        pop     %bx
        decw    %bx
        jnz     boot_read_retry
        stc
        jmp     boot_read_exit

#ifdef INCLUDE_PXE_BOOT
/*************************************************************************/
        .balign 16, 0x90
boot_pxe:
        /* ES := 16-bit segment address of the PXENV+ structure. */
        /* BX := 16-bit offset of the PXENV+.                    */

        mov     %es, vxpxe_segment
        mov     %ebx, vxpxe_offset

        /* Clean up */
        
        xor     %ebx,%ebx
        mov     %bx,%es
        
        /* Say we are here */

        mov     $firstBootId,%esi
        call    strout
        mov     $pxeBootId,%esi
        call    strout

        /* Find PXENV+ Structure location */
        
        mov     vxpxe_segment, %eax
        shl     $4,%eax
        mov     vxpxe_offset, %ebx
        add     %ebx,%eax

        /* Check for "PXEN" */
        
        cmpl    $PXE_PXENV_STRING, (%eax)
        je      1f
        mov     $vxpxe_pxenv_error,%esi
        call    strout
firstPxenvHlt:
        hlt
        jmp     firstPxenvHlt

        /* Save PXENV+ Structure location */

1:
        mov     %eax, vxpxe_pxeenv

        /* Find !PXE Structure location */
        
        xor     %ebx,%ebx
        xor     %ecx,%ecx
        add     $PXE_ENV_PXEPTR_OFFSET,%eax
        mov     PXE_SEGMENT_OFFSET(%eax),%bx /* PXEPtr segment */
        mov     (%eax),%cx                   /* PXEPtr offset  */
        shl     $4,%ebx
        add     %ecx,%ebx
        
        /* Check for "!PXE" */
        
        cmpl    $PXE_PXE_STRING, (%ebx)
        je      1f
        mov     $vxpxe_pxe_error,%esi
        call    strout
firstPxeHlt:
        hlt
        jmp     firstPxeHlt

        /* Save !PXE Structure location */
        
1:
        mov     %ebx, vxpxe_pxe

        /* Save PXE->EntryPointSP.offset */

        xor     %eax,%eax
        mov     (PXE_ENTRYPOINTSP_OFFSET)(%ebx), %ax
        mov     %ax, pxe_offset
        
        /* Save PXE->EntryPointSP.segment */
        
        mov     (PXE_ENTRYPOINTSP_OFFSET+PXE_SEGMENT_OFFSET)(%ebx), %ax
        mov     %ax, pxe_segment

        /* Get PXE cached info - PXENV_GET_CACHED_INFO */

        movl    $PXENV_GET_CACHED_INFO, vxpxe_api_function /* Try using local buffer */
        mov     $vxpxe_api_buffer, %esi
        movw    $0x0, (PXENV_GET_CACHED_INFO_STATUS)(%esi)
        movw    $PXENV_PACKET_TYPE_DHCP_ACK, (PXENV_GET_CACHED_INFO_PACKETTYPE)(%esi)
        movw    $PXENV_GET_CACHED_PACKET_LEN, (PXENV_GET_CACHED_INFO_BUFFERSIZE)(%esi)
        movl    $vxpxe_api_cached_packet, (PXENV_GET_CACHED_INFO_BUFFER)(%esi)
        movw    $0x0, (PXENV_GET_CACHED_INFO_BUFFERLIMIT)(%esi)
        call    pxe_api

        cmp     $0x0,%ax                /* Check return code for error */
        jne      1f

        /* Get resulting PXE cached info buffer - location of bootph structure */

        mov     $vxpxe_api_cached_packet, %eax
        jmp     firstGetCachedInfoDone

1:
        movl    $PXENV_GET_CACHED_INFO, vxpxe_api_function /* Try using PXE buffer */
        mov     $vxpxe_api_buffer, %esi
        movw    $0x0, (PXENV_GET_CACHED_INFO_STATUS)(%esi)
        movw    $PXENV_PACKET_TYPE_DHCP_ACK, (PXENV_GET_CACHED_INFO_PACKETTYPE)(%esi)
        movw    $0x0, (PXENV_GET_CACHED_INFO_BUFFERSIZE)(%esi)
        movl    $0x0, (PXENV_GET_CACHED_INFO_BUFFER)(%esi)
        movw    $0x0, (PXENV_GET_CACHED_INFO_BUFFERLIMIT)(%esi)
        call    pxe_api

        cmp     $0x0,%ax                /* Check return code for error */
        je      1f
        mov     $vxpxe_get_cached_info_error,%esi
        call    strout
firstGetCachedInfoHlt:
        hlt
        jmp     firstGetCachedInfoHlt

        /* Get resulting PXE cached info buffer - location of bootph structure */
1:
        xor     %eax,%eax
        xor     %ebx,%ebx
        mov     $vxpxe_api_buffer, %esi
        mov     (PXENV_GET_CACHED_INFO_BUFFER+PXE_SEGMENT_OFFSET)(%esi), %ax
        shl     $4,%eax
        mov     (PXENV_GET_CACHED_INFO_BUFFER)(%esi), %bx
        add     %ebx,%eax

firstGetCachedInfoDone:

        /* Get my ip address in bootph structure */

        mov     (PXENV_GET_CACHED_PACKET_YIP)(%eax), %ebx
        mov     %ebx, vxpxe_my_ip
        
        /* Get server ip address in bootph structure */
        
        mov     (PXENV_GET_CACHED_PACKET_SIP)(%eax), %ebx
        mov     %ebx, vxpxe_server_ip
        
        /* Get bootfile name in bootph structure */
        
        add     $PXENV_GET_CACHED_PACKET_BOOTFILE, %eax
        mov     $vxpxe_api_buffer, %ebx /* copy into buffer for next tftp command */
        add     $PXENV_TFTP_READ_FILE_FILENAME, %ebx
        xor     %ecx,%ecx
        xor     %edi,%edi
firstBootfile:
        cmpb    $0x0, (%eax)            /* check for end of string */
        je      2f
        mov     (%eax), %dl
        mov     %dl, (%ebx)             /* copy */
        cmp     $PXE_DIR_CHAR, %dl      /* look for last dir char */
        jne     1f
        mov     %ecx, %edi              /* %edi has position of last dir char */
1:
        inc     %eax
        inc     %ebx
        inc     %ecx
        cmp     $PXENV_GET_CACHED_PACKET_BOOTFILE_SIZE, %ecx
        jne     firstBootfile
        mov     $vxpxe_bootfile_error,%esi
        call    strout
firstBootfileHlt:
        hlt
        jmp     firstBootfileHlt
        
        /* Change bootfile name from bootrom.pxe to bootapp.sys */
2:
        movb    $0x0, (%ebx)            /* null terminate string */
        cmp     $0x0, %edi              /* %edi has position of last dir char */
        je      4f
        mov     %edi, %eax
        add     $PXE_BOOTFILE_NAME_LEN, %eax
        cmp     $PXENV_GET_CACHED_PACKET_BOOTFILE_SIZE, %eax
        jle     3f
        mov     $vxpxe_bootfile_len_error,%esi
        call    strout
firstBootfileLenHlt:
        hlt
        jmp     firstBootfileLenHlt
        
3:
        inc     %edi                    /* increment position if dir char was found */
4:
        xor     %ecx, %ecx
        mov     $vxpxe_api_buffer, %esi /* copy into buffer for next tftp command */
        add     $PXENV_TFTP_READ_FILE_FILENAME, %esi
        add     %edi, %esi              /* skip to char after dir char */
        mov     $vxpxe_bootfile_name, %ebx /* Location of new file name */
5:      
        mov     (%ebx), %al
        mov     %al, (%esi)             /* copy */
        inc     %ecx
        cmp     $PXE_BOOTFILE_NAME_LEN, %ecx
        jge     6f
        inc     %esi
        inc     %ebx
        jmp     5b

        /* TFTP read file - PXENV_TFTP_READ_FILE */

6:
        /* Print some status */
        mov     $vxpxe_api_buffer, %esi
        add     $PXENV_TFTP_READ_FILE_FILENAME, %esi
        call    strout
        mov     $vxsys_crlf, %esi
        call    strout

        movl    $PXENV_TFTP_READ_FILE, vxpxe_api_function
        mov     $vxpxe_api_buffer, %esi
        movw    $0x0, (PXENV_TFTP_READ_FILE_STATUS)(%esi)
        /* PXENV_TFTP_READ_FILE_FILENAME already set above */
        movl    $ROM_SIZE, (PXENV_TFTP_READ_FILE_BUFFERSIZE)(%esi)
        mov     $ROM_TEXT_ADRS, %eax
        mov     %eax, (PXENV_TFTP_READ_FILE_BUFFER)(%esi)
        mov     vxpxe_server_ip, %eax
        mov     %eax, (PXENV_TFTP_READ_FILE_SERVERIPADDRESS)(%esi)
        movl    $0x0, (PXENV_TFTP_READ_FILE_GATEWAYIPADDRESS)(%esi)
        movl    $0x0, (PXENV_TFTP_READ_FILE_MCASTIPADDRESS)(%esi)
        movw    $0x0, (PXENV_TFTP_READ_FILE_TFTPCLNTPORT)(%esi)
        movw    $0x0, (PXENV_TFTP_READ_FILE_TFTPSRVPORT)(%esi)
        movw    $0x0, (PXENV_TFTP_READ_FILE_TFTPOPENTIMEOUT)(%esi)
        movw    $0x0, (PXENV_TFTP_READ_FILE_TFTPREOPENDELAY)(%esi)
        call    pxe_api

        cmp     $0x0,%ax                /* Check return code for error */
        je      1f
        mov     $vxpxe_tftp_read_file_error,%esi
        call    strout
firstTftpReadFileHlt:
        hlt
        jmp     firstTftpReadFileHlt

1:
        /* Save size and boot */
        
        mov     (PXENV_TFTP_READ_FILE_BUFFERSIZE)(%esi), %eax
        mov     %eax, vxsys_size
        jmp     bootProtected

/************************************************************************
 * PXE API
 */
        .balign 16, 0x90
pxe_api:
        pushw   $0x0
        pushw   $vxpxe_api_buffer       /* buffer */
        pushw   vxpxe_api_function      /* PXE API */
        .byte   0x9a                    /* lcall */
pxe_offset:                             /* PXE->EntryPointSP.offset */
        .word   0x0000
pxe_segment:                            /* PXE->EntryPointSP.segment */
        .word   0x0000
        add     $6, %sp                 /* caller cleans up stack */
        ret                             /* ax has return status */
#endif /* INCLUDE_PXE_BOOT */

/************************************************************************
 * Print zero term string in ds:si
 */
        
        .balign 16, 0x90
strout:
        .byte   0x67, 0x66
        push    %bx
        .byte   0x67, 0x66
        push    %ax
        .byte   0x67, 0x66
        xor     %eax,%eax
        .byte   0x67, 0x66
        xor     %ebx,%ebx
nextc:  lodsb
        orb     %al,%al
        jz      end_str_loop
        mov     $0xe,%ah
        mov     $7,%bx
        int     $0x10
        cli
        jmp     nextc
end_str_loop:
        .byte   0x67, 0x66
        pop     %ax
        .byte   0x67, 0x66
        pop     %bx
        ret

/************************************************************************
 * boot_clear_local_mem - clears low memory
 * Clear LOCAL_MEM_LOCAL_ADRS to RAM_LOW_ADRS
 */
        
        .balign 16, 0x90
boot_clear_local_mem:
        .byte   0x67, 0x66
        push    %eax
        .byte   0x67, 0x66
        push    %edi
        .byte   0x67, 0x66
        push    %ecx

        mov     $ROM_TEXT_ADRS,%ecx
#ifdef _WRS_CONFIG_LP64
        sub     $ LOCAL_MEM_VIRT_TO_PHYS(LOCAL_MEM_LOCAL_ADRS),%ecx      /* Calculate bytes to clear */
#else
        sub     $LOCAL_MEM_LOCAL_ADRS,%ecx      /* Calculate bytes to clear */
#endif /* _WRS_CONFIG_LP64 */
        shr     $2,%ecx                         /* Convert to longs */

        xor     %eax,%eax                       /* Zero */

#ifdef _WRS_CONFIG_LP64
        mov     $ LOCAL_MEM_VIRT_TO_PHYS(LOCAL_MEM_LOCAL_ADRS),%edi
#else
        mov     $LOCAL_MEM_LOCAL_ADRS,%edi
#endif /* _WRS_CONFIG_LP64 */

boot_clear_local_mem_loop:
        movl    %eax,(%edi)             /* Store the value */
        addl    $4,%edi                 /* increment output buffer ptr */
        decl    %ecx                    /* decrement count */
        jne     boot_clear_local_mem_loop

        .byte   0x67, 0x66
        pop     %ecx
        .byte   0x67, 0x66
        pop     %edi
        .byte   0x67, 0x66
        pop     %eax
        ret

        .balign 16, 0x90
#ifdef INCLUDE_BIOS_E820_MEM_AUTOSIZE
        
/*
 * MUST include the assembly file at this specific location in
 * romInit.s to ensure correct linkage to 16 bit address space
 */

#include <fw/bios/vxBiosE820ALib.s>
#endif /* INCLUDE_BIOS_E820_MEM_AUTOSIZE */

        /* temporary IDTR stored in code segment in ROM */

bootIdtr:
        .word   0x0000                  /* size   : 0 */
        .long   0x00000000              /* address: 0 */


        /* temporary GDTR stored in code segment in ROM */

bootGdtr:
        .word   0x0037                  /* size   : 55(8 * 7 - 1) bytes */
        .long   (bootGdt - firstBoot + FIRST_TEXT_ADRS) /* address: bootGdt */
        

        /* temporary GDT stored in code segment in ROM */

        .balign 16,0x90
bootGdt:
        
        /* 0(selector=0x0000): Null descriptor */
        
        .word   0x0000
        .word   0x0000
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00

        /* 1(selector=0x0008): Code descriptor */
        
        .word   0xffff                  /* limit: xffff */
        .word   0x0000                  /* base : xxxx0000 */
        .byte   0x00                    /* base : xx00xxxx */
        .byte   0x9a                    /* Code e/r, Present, DPL0 */
        .byte   0xcf                    /* limit: fxxxx, Page Gra, 32bit */
        .byte   0x00                    /* base : 00xxxxxx */

        /* 2(selector=0x0010): Data descriptor */
        
        .word   0xffff                  /* limit: xffff */
        .word   0x0000                  /* base : xxxx0000 */
        .byte   0x00                    /* base : xx00xxxx */
        .byte   0x92                    /* Data r/w, Present, DPL0 */
        .byte   0xcf                    /* limit: fxxxx, Page Gra, 32bit */
        .byte   0x00                    /* base : 00xxxxxx */

        /* 3(selector=0x0018): Code descriptor, for the nesting interrupt */
        
        .word   0xffff                  /* limit: xffff */
        .word   0x0000                  /* base : xxxx0000 */
        .byte   0x00                    /* base : xx00xxxx */
        .byte   0x9a                    /* Code e/r, Present, DPL0 */
        .byte   0xcf                    /* limit: fxxxx, Page Gra, 32bit */
        .byte   0x00                    /* base : 00xxxxxx */

        /* 4(selector=0x0020): Code descriptor, for the nesting interrupt */
        
        .word   0xffff                  /* limit: xffff */
        .word   0x0000                  /* base : xxxx0000 */
        .byte   0x00                    /* base : xx00xxxx */
        .byte   0x9a                    /* Code e/r, Present, DPL0 */
        .byte   0xcf                    /* limit: fxxxx, Page Gra, 32bit */
        .byte   0x00                    /* base : 00xxxxxx */

        /* 5(selector=0x0028): Code descriptor */
        
        .word   0xffff                  /* limit: xffff */
        .word   0x0000                  /* base : xxxx0000 */
        .byte   0x00                    /* base : xx00xxxx */
        .byte   0x9b                    /* Code e/r, Present, DPL0 */
        .byte   0x8f                    /* limit: fxxxx, Page Gra, 32bit */
        .byte   0x00                    /* base : 00xxxxxx */

        /* 6(selector=0x0030): Data descriptor */
        
        .word   0xffff                  /* limit: xffff */
        .word   0x0000                  /* base : xxxx0000 */
        .byte   0x00                    /* base : xx00xxxx */
        .byte   0x93                    /* Data r/w, Present, DPL0 */
        .byte   0x8f                    /* limit: fxxxx, Page Gra, 32bit */
        .byte   0x00                    /* base : 00xxxxxx */

/* Temp vars. Copied from boot MBR */
        
        .data
        .balign 16, 0x00
tempVarsAdr:
        .long   0x7b00
tempVarsLen:
        .long   0xa

        .balign 4, 0x00
data_startl:
        .word 0x0000
data_starth:
        .word 0x0000
num4sec:
        .word 0x0000
buffer_ptr:
        .word 0x0000
seginc:
        .word 0x0000

/* Boot device info. Copied from boot MBR */

bootDevAdr:
        .long   0x7c03
bootDevLen:
        .long   0x3b

        .balign 16, 0x00
oemname:
        .byte   0x00 /* 8 */
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
byt_per_sec:
        .word   0x0000
sec_per_clu:
        .byte   0x00
num_res_sec:
        .word   0x0000
num_fats:
        .byte   0x00
root_size:
        .word   0x0000
total_sec:
        .word   0x0000
med_desc:
        .byte   0x00
sec_per_fat:
        .word   0x0000
sec_per_track:
        .word   0x0000
num_heads:
        .word   0x0000
num_hid_sec:
        .word   0x0000 /* 2 */
num_hid_sec2:
        .word   0x0000
total_sec32:
        .long   0x00000000
phy_drv:
        .byte   0x00
res1:
        .byte   0x00
signature:
        .byte   0x00
serial_num:
        .long   0x00000000
vol_label:
        .byte   0x00 /* 11 */
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00
res2:
        .word   0x0000 /* 4 */
        .word   0x0000
        .word   0x0000
        .word   0x0000

        .balign 4, 0x00
dotstr:
        .asciz  "."
        .balign 4, 0x00
firstBootId:
#ifdef _WRS_CONFIG_LP64
        .asciz "\n\rIA64 FSB V1.0\n\r"
#else
        .asciz "\n\rIA32 FSB V1.0\n\r"
#endif /* _WRS_CONFIG_LP64 */
        .balign 4, 0x00
vxsys_name:
        .asciz "BOOTAPP SYS"
        .balign 4, 0x00
vxsys_crlf:
        .asciz  "\n\r"
        .balign 4, 0x00
vxsys_error:
        .asciz "\n\rbootapp.sys not found\n\r"
        .balign 4, 0x00
vxsys_size:
        .long   0x00000000
        .balign 16, 0x00
vxsys_buffer_ptr:
        .long   ROM_TEXT_ADRS
        .balign 16, 0x00
vxinit_gdtr:
        .word   0x0000
        .long   0x00000000
        .balign 16, 0x00
vxinit_idtr:
        .word   0x0000
        .long   0x00000000
        .balign 16, 0x00
vxinit_start_type:
        .long   BOOT_COLD
#ifdef INCLUDE_PXE_BOOT
pxeBootId:
        .asciz "PXE Boot\n\r"
        .balign 4, 0x00
vxpxe_pxenv_error:
        .asciz "VxWorks - Cannot find PXENV+ structure\n\r"
        .balign 4, 0x00
vxpxe_pxe_error:
        .asciz "VxWorks - Cannot find !PXE structure\n\r"
        .balign 4, 0x00
vxpxe_get_cached_info_error:
        .asciz "VxWorks - PXENV_GET_CACHED_INFO error\n\r"
        .balign 4, 0x00
vxpxe_tftp_read_file_error:
        .asciz "VxWorks - PXENV_TFTP_READ_FILE error\n\r"
        .balign 4, 0x00
vxpxe_bootfile_error:
        .asciz "VxWorks - Cannot find bootfile name\n\r"
        .balign 4, 0x00
vxpxe_bootfile_len_error:
        .asciz "VxWorks - Bootfile path too long\n\r"
        .balign 4, 0x00
vxpxe_bootfile_name:
        .asciz PXE_BOOTFILE_NAME
        .byte   0x00
        .balign 4, 0x00
vxpxe_segment:
        .long   0x00000000
vxpxe_offset:
        .long   0x00000000
vxpxe_pxeenv:
        .long   0x00000000
vxpxe_pxe:
        .long   0x00000000
vxpxe_my_ip:
        .long   0x00000000
vxpxe_server_ip:
        .long   0x00000000
vxpxe_api_function:
        .long   0x00000000
        .balign 16, 0x00
vxpxe_api_cached_packet:
        .fill PXENV_GET_CACHED_PACKET_LEN
        .balign 16, 0x00
vxpxe_api_buffer:
        .fill PXENV_MAX_PARAMETER_LEN
#endif /* INCLUDE_PXE_BOOT */
        .balign 16, 0x00
dir_buffer:
        .fill 128,4,0
        .balign 512, 0x00
data_buffer:
        .fill 128,4,0
data_end:
        .long   0xFAFAFAFA
        .end
