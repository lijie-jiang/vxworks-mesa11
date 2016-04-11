/* romInit.s - Intel Sandy Bridge ROM initialization module */

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
01e,12dec12,rbc  WIND00381053 - Fix A20 enable move code to
                 target/src/hwif/cpu/arch/i86
01d,31aug12,g_x  make romWarmHigh as the entry from first boot 
                 stage (WIND00332348)
01c,31mar12,c_t  fix opcode problem for real mode.(WIND00332346)
01b,16aug11,jjk  WIND00263692, Multi-stage boot support
01a,11apr11,j_z  initial creation based on itl_nehalem version 01d
*/

/*
DESCRIPTION
This module contains the entry code for the VxWorks bootrom.

The routine sysToMonitor(2) jumps to the location XXX bytes
passed the beginning of romInit, to perform a "warm boot".

This code is intended to be generic accross i80x86 boards.
Hardware that requires special register setting or memory
mapping to be done immediately, may do so here.
*/

#define _ASMLANGUAGE
#include <vxWorks.h>
#include <sysLib.h>
#include <asm.h>
#include "config.h"

#ifndef INCLUDE_UEFI_BOOT_SUPPORT /* non-UEFI romInit code */

/**************************************************************
 * Original non-UEFI enabled romInit code starts here.
 **************************************************************/

	.data
	.globl	FUNC(copyright_wind_river)
	.long	FUNC(copyright_wind_river)


	/* internals */

	.globl	romInit			/* start of system code */
	.globl	_romInit		/* start of system code */

	.globl	sdata			/* start of data */
	.globl	_sdata			/* start of data */

        /* externals */

        .globl  FUNC(romA20on)         /* turn on A20 */

 sdata:
_sdata:
	.asciz	"start of data"


	.text
	.balign 16

/*******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*

* romInit (startType)
*     int startType;	/@ only used by 2nd entry point @/

*/

	/* cold start entry point in REAL MODE(16 bits) */

romInit:
_romInit:
	cli				/* LOCK INTERRUPT */
	jmp	cold			/* offset must be less than 128 */


	/* warm start entry point in PROTECTED MODE(32 bits) */

	.balign 16,0x90
romWarmHigh:				/* ROM_WARM_HIGH(0x10) is offset */
	cli				/* LOCK INTERRUPT */
	movl    SP_ARG1(%esp),%ebx	/* %ebx has the startType */
	jmp	warm

	/* warm start entry point in PROTECTED MODE(32 bits) */
	
	.balign 16,0x90
romWarmLow:				/* ROM_WARM_LOW(0x20) is offset */
	cli				/* LOCK INTERRUPT */
	cld				/* copy itself to ROM_TEXT_ADRS */
	movl	$ SYS_RAM_LOW_ADRS,%esi	/* get src addr (RAM_LOW_ADRS) */
	movl	$ ROM_TEXT_ADRS,%edi	/* get dst addr (ROM_TEXT_ADRS) */
	movl	$ ROM_SIZE,%ecx		/* get nBytes to copy */
	shrl	$2,%ecx			/* get nLongs to copy */
	rep				/* repeat next inst ECX time */
	movsl				/* copy nLongs from src to dst */
	movl    SP_ARG1(%esp),%ebx	/* %ebx has the startType */
	jmp	warm			/* jump to warm */

#ifdef GRUB_MULTIBOOT
        /* Multiboot Header allows GRUB to load kernel */
        .balign 4
multiboot_header:
        .long   MULTIBOOT_HEADER_MAGIC
        .long   MULTIBOOT_HEADER_FLAGS
        .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
        .long   multiboot_header + ROM_TEXT_ADRS - romInit
        .long   ROM_TEXT_ADRS
        .long   wrs_kernel_data_end + ROM_TEXT_ADRS - romInit
        .long   wrs_kernel_bss_end + ROM_TEXT_ADRS - romInit
        .long   ROM_TEXT_ADRS
#endif /* GRUB_MULTIBOOT */

	/* copyright notice appears at beginning of ROM (in TEXT segment) */

	.ascii   "Copyright 2011 Wind River Systems, Inc."


	/* cold start code in REAL MODE(16 bits) */

	.balign 16,0x90
cold:

#ifdef GRUB_MULTIBOOT
        mov     %eax,(MULTIBOOT_SCRATCH)     /* save magic */
        mov     %ebx,(MULTIBOOT_SCRATCH + 8) /* save multi-boot pointer */
#endif /* GRUB_MULTIBOOT */

        /*
         * Clear a couple of registers. This seems to be
         * necessary when booting via PXE.
         */

	.byte	0x67
	xorw	%ax, %ax
	.byte	0x67
	movw	%ax, %ds
	.byte	0x67
	movw	%ax, %es

#ifdef INCLUDE_BIOS_E820_MEM_AUTOSIZE
        .byte   0x67, 0x66
        call    FUNC(vxBiosE820MapQuery)  /* query the BIOS memory map */

#endif /* INCLUDE_BIOS_E820_MEM_AUTOSIZE */

	.byte	0x67, 0x66		/* next inst has 32bit operand */
        lidt	ROM_ADRS(romIdtr)	/* load temporary IDT */

	.byte	0x67, 0x66		/* next inst has 32bit operand */
	lgdt	ROM_ADRS(romGdtr)	/* load temporary GDT */

	/* switch to protected mode */

	mov	%cr0,%eax		/* move CR0 to EAX */
	.byte	0x66			/* next inst has 32bit operand */
	or	$0x00000001,%eax	/* set the PE bit */
	mov	%eax,%cr0		/* move EAX to CR0 */
	jmp	romInit1		/* near jump to flush a inst queue */

romInit1:
	.byte	0x66			/* next inst has 32bit operand */
	mov	$0x0010,%eax		/* set data segment 0x10 is 3rd one */
	mov	%ax,%ds			/* set DS */
	mov	%ax,%es			/* set ES */
	mov	%ax,%fs			/* set FS */
	mov	%ax,%gs			/* set GS */
	mov	%ax,%ss			/* set SS */
	.byte	0x66			/* next inst has 32bit operand */
	mov	$ ROM_STACK,%esp 	/* set lower mem stack pointer */
	.byte	0x67, 0x66		/* next inst has 32bit operand */
	ljmp	$0x08, $ ROM_TEXT_ADRS + romInit2 - romInit


	/* temporary IDTR stored in code segment in ROM */

romIdtr:
	.word	0x0000			/* size   : 0 */
	.long	0x00000000		/* address: 0 */


	/* temporary GDTR stored in code segment in ROM */

romGdtr:
	.word	0x0027			/* size   : 39(8 * 5 - 1) bytes */
	.long	(romGdt	- romInit + ROM_TEXT_ADRS) /* address: romGdt */


	/* temporary GDT stored in code segment in ROM */

	.balign 16,0x90
romGdt:
	/* 0(selector=0x0000): Null descriptor */
	.word	0x0000
	.word	0x0000
	.byte	0x00
	.byte	0x00
	.byte	0x00
	.byte	0x00

	/* 1(selector=0x0008): Code descriptor */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x9a			/* Code e/r, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */

	/* 2(selector=0x0010): Data descriptor */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x92			/* Data r/w, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */

	/* 3(selector=0x0018): Code descriptor, for the nesting interrupt */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x9a			/* Code e/r, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */

	/* 4(selector=0x0020): Code descriptor, for the nesting interrupt */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x9a			/* Code e/r, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */


	/* cold start code in PROTECTED MODE(32 bits) */

	.balign 16,0x90
romInit2:
	cli				/* LOCK INTERRUPT */
	movl	$ ROM_STACK,%esp	/* set a stack pointer */

#if	defined (TGT_CPU) && defined (INCLUDE_SYMMETRIC_IO_MODE)

        movl    $ MP_N_CPU, %eax
        lock
        incl    (%eax)

#endif	/* defined (TGT_CPU) && defined (INCLUDE_SYMMETRIC_IO_MODE) */

	/* WindML + VesaBIOS initialization */
	
#ifdef	INCLUDE_WINDML	
	movl	$ VESA_BIOS_DATA_PREFIX,%ebx	/* move BIOS prefix addr to EBX */
	movl	$ VESA_BIOS_KEY_1,(%ebx)	/* store "BIOS" */
	addl	$4,%ebx				/* increment EBX */
	movl	$ VESA_BIOS_KEY_2,(%ebx)	/* store "DATA" */
	movl	$ VESA_BIOS_DATA_SIZE,%ecx	/* load ECX with nBytes to copy */
	shrl    $2,%ecx				/* get nLongs to copy */
	movl	$0,%esi				/* load ESI with source addr */
	movl	$ VESA_BIOS_DATA_ADDRESS,%edi	/* load EDI with dest addr */
	rep
	movsl					/* copy BIOS data to VRAM */
#endif	/* INCLUDE_WINDML */

	call	FUNC(romA20on)		/* enable A20 */
	cmpl	$0, %eax		/* is A20 enabled? */
	jne	romInitHlt		/*   no: jump romInitHlt */

	movl    $ BOOT_COLD,%ebx	/* %ebx has the startType */

	/* copy bootrom image to dst addr if (romInit != ROM_TEXT_ADRS) */

warm:

#ifdef INCLUDE_BIOS_E820_MEM_AUTOSIZE

    cmpl    $BOOT_COLD, %ebx
    jne     1f
    call    FUNC(vxBiosE820MapCopy)
1:
#endif /* INCLUDE_BIOS_E820_MEM_AUTOSIZE */

	ARCH_REGS_INIT			/* initialize DR[0-7] CR0 EFLAGS */

	/* ARCH_CR4_INIT		/@ initialize CR4 for P5,6,7 */

	xorl	%eax, %eax		/* zero EAX */
	movl	%eax, %cr4		/* initialize CR4 */

	movl	$romGdtr,%eax		/* load the original GDT */
	subl	$ FUNC(romInit),%eax
	addl	$ ROM_TEXT_ADRS,%eax
	pushl	%eax	
	call	FUNC(romLoadGdt)

	movl	$ STACK_ADRS, %esp	/* initialise the stack pointer */
	movl    $ ROM_TEXT_ADRS, %esi	/* get src addr(ROM_TEXT_ADRS) */
	movl    $ romInit, %edi		/* get dst addr(romInit) */
	cmpl	%esi, %edi		/* is src and dst same? */
	je	romInit4		/*   yes: skip copying */
	movl    $ FUNC(end), %ecx	/* get "end" addr */
	subl    %edi, %ecx		/* get nBytes to copy */
	shrl    $2, %ecx		/* get nLongs to copy */
	cld 				/* clear the direction bit */
	rep				/* repeat next inst ECX time */
	movsl                           /* copy itself to the entry point */

	/* jump to romStart(absolute address, position dependent) */

romInit4:
	xorl	%ebp, %ebp		/* initialize the frame pointer */
	pushl	$0			/* initialise the EFLAGS */
	popfl
	pushl	%ebx			/* push the startType */
	movl	$ FUNC(romStart),%eax	/* jump to romStart */
	call	*%eax

	/* just in case, if there's a problem in romStart or romA20on */

romInitHlt:
	pushl	%eax
	call	FUNC(romEaxShow)	/* show EAX on your display device */
	hlt	
	jmp	romInitHlt

#ifdef INCLUDE_BIOS_E820_MEM_AUTOSIZE

/* MUST include the assembly file at this specific location in 
   romInit.s to ensure correct linkage to 16 bit address space */

#include <fw/bios/vxBiosE820ALib.s>

#endif /* INCLUDE_BIOS_E820_MEM_AUTOSIZE */

/*******************************************************************************
*
* romLoadGdt - load the global descriptor table.
*
* RETURNS: N/A
*
* NOMANUAL

* void romLoadGdt (char *romGdtr)
 
*/

        .balign 16,0x90
FUNC_LABEL(romLoadGdt)
	movl	SP_ARG1(%esp),%eax
	lgdt	(%eax)
	movw	$0x0010,%ax		/* a selector 0x10 is 3rd one */
	movw	%ax,%ds	
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss
	ret

/*******************************************************************************
*
* romEaxShow - show EAX register 
*
* show EAX register in your display device 
*
* RETURNS: N/A

* void romEaxShow (void)
 
*/

	.balign 16,0x90
FUNC_LABEL(romEaxShow)

	/* show EAX register in your display device available */

	ret

#else /* INCLUDE_UEFI_BOOT_SUPPORT */

/***************************************
 * Start of UEFI-enabled romInit code
 ***************************************/

	.data
	.globl	FUNC(copyright_wind_river)
	.long	FUNC(copyright_wind_river)

	/* internals */

	.globl	romInit			/* start of system code */
	.globl	_romInit		/* start of system code */

	.globl	sdata			/* start of data */
	.globl	_sdata			/* start of data */
 	.globl  pRomUefiMemAddr   /* UEFI pointer value for memory block header */
 	.globl  pRomUefiAcpiAddr   /* UEFI pointer value for ACPI info */
	.globl  romUefiComplianceInfoP /* UEFI read-only compliance string */

 sdata:
_sdata:
	.asciz	"start of data"
	.balign 8, 0x00
pRomUefiMemAddr:
	.long 0x00000000
pRomUefiAcpiAddr:
	.long 0x00000000

	.text
	.balign 16

/*******************************************************************************
*
* romInit - entry point for VxWorks in ROM
*
* For cold boots:
* romInit (uefiBaseAddress, uefiMemoryblockAddr)
*     void *uefiBaseAddress;     /@ only used by 1st (cold boot) entry point @/
*     void *uefiMemoryBlockAddr; /@ only used by 1st (cold boot) entry point @/
*
* For warm boots:
* romInit (startType)
*     int startType;	/@ only used by 2nd entry point @/
*/

	/* cold start entry point in PROTECTED MODE (32 bits)
	   EFI will ensure that the GDT and segmentation select registers are
	   set to wide open 32 bit flat access.
	   
	   This code will start executing on a cold boot at the address given
	   by the first parameter, uefiBaseAddress.  It will copy itself to the ROM_TEXT_ADRS
	   location ASAP and then jump to there.  Note that even then the code is probably built
	   to run from yet another address, but since the bootrom image is probably compressed the code
	   will have to uncompress itself to the final destination address and then jump to that.
	 */

romInit:
_romInit:
	cli				/* LOCK INTERRUPT */
	jmp	cold


	/* warm start entry point in PROTECTED MODE(32 bits)
	 * These entry points will be offsets from the base address that
	 * the bootrom is built for.
	 */

	.balign 16,0x90
romWarmHigh:				/* ROM_WARM_HIGH(0x10) is offset */
	cli				/* LOCK INTERRUPT */
	movl    SP_ARG1(%esp),%ebx	/* %ebx has the startType */
	jmp	warmHigh

	/* warm start entry point in PROTECTED MODE(32 bits) */
	
	.balign 16,0x90
romWarmLow:				/* ROM_WARM_LOW(0x20) is offset */
	cli				/* LOCK INTERRUPT */
	
	cld				/* copy itself to ROM_TEXT_ADRS */
	movl	$ SYS_RAM_LOW_ADRS,%esi	/* get src addr (RAM_LOW_ADRS) */
	movl	$ ROM_TEXT_ADRS,%edi	/* get dst addr (ROM_TEXT_ADRS) */
	movl	$ ROM_SIZE,%ecx		/* get nBytes to copy */
	shrl	$2,%ecx			/* get nLongs to copy */
	rep				/* repeat next inst ECX time */
	movsl				/* copy nLongs from src to dst */

	movl    SP_ARG1(%esp),%ebx	/* %ebx has the startType */
	jmp	warm			/* jump to warm */

#ifdef GRUB_MULTIBOOT
        /* Multiboot Header allows GRUB to load kernel */
        .balign 4
multiboot_header:
        .long   MULTIBOOT_HEADER_MAGIC
        .long   MULTIBOOT_HEADER_FLAGS
        .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
        .long   multiboot_header + ROM_TEXT_ADRS - romInit
        .long   ROM_TEXT_ADRS
        .long   wrs_kernel_data_end + ROM_TEXT_ADRS - romInit
        .long   wrs_kernel_bss_end + ROM_TEXT_ADRS - romInit
        .long   ROM_TEXT_ADRS
#endif /* GRUB_MULTIBOOT */

	/* copyright notice appears at beginning of ROM (in TEXT segment) */

	.ascii   "Copyright 2010 Wind River Systems, Inc."
	.balign 4,0x00
romUefiComplianceInfoP:
	.asciz   "UEFI 001.10 Compliant" /* format is XXX.YY, where YY=1, 2, ..., 9, 10, 11, etc */


	/* UEFI cold start code in PROTECTED MODE (32 bits) 
	 * Executing at an unknown address, must use the first parameter to get it.
	 */

	.balign 16,0x90
cold:
#ifdef GRUB_MULTIBOOT
        mov     %eax,(MULTIBOOT_SCRATCH)     /* save magic */
        mov     %ebx,(MULTIBOOT_SCRATCH + 8) /* save multi-boot pointer */
#endif /* GRUB_MULTIBOOT */

	/* First argument is the location we're running from.
	 * Second argument is the UEFI memory block.
	 */

	/* get the base address */
	movl SP_ARG1(%esp), %edx  /* base address is arg 1 */

	/*
	 * get the memory block address ptr relative to our current execution
	 * address, and store the memory block address.  This in turn will be
	 * copied to ROM_TEXT_ADRS, and then probably somewhere else too, before
	 * bootInit picks it up and uses it.
	 */
	movl SP_ARG2(%esp), %ebx  /* memory block address is arg 2 */
	movl $FUNC(pRomUefiMemAddr), %eax  /* get the base address */
	subl $FUNC(romInit), %eax /* now has offset to base */
	addl %edx, %eax           /* add the base address we got*/
	movl %ebx, (%eax) /* store the value into the location*/

	
	/*
	 * get the ACPI address ptr relative to our current execution
	 * address, and store the memory block address.  This in turn will be
	 * copied to ROM_TEXT_ADRS, and then probably somewhere else too, before
	 * bootInit picks it up and uses it.
	 */
	movl SP_ARG3(%esp), %ebx  /* ACPI address is arg 3 */
	movl $FUNC(pRomUefiAcpiAddr), %eax  /* get the base address */
	subl $FUNC(romInit), %eax /* now has offset to base */
	addl %edx, %eax           /* add the base address we got*/
	movl %ebx, (%eax) /* store the value into the location*/

	
	/* 
	 * copy the bootrom image loaded by the 
	 * UEFI OS loader to ROM_TEXT_ADRS 
	 */
	cld
	movl	%edx, %esi  /* start at the base address we got */

	movl	$ROM_TEXT_ADRS,%edi	/* get dst addr (ROM_TEXT_ADRS) fixed */
	movl	$FUNC(end), %ecx	/* end of the image from linker */
	subl    $FUNC(romInit),%ecx		/* get nBytes to copy */
	shrl	$2,%ecx			/* get nLongs to copy */
	rep				/* repeat next inst ECX time */
	movsl				/* copy nLongs from src to dst */

	/* jump to romInitFixedLocation (absolute address, position dependent) */
	.balign 16,0x90
callroutine:
	movl $FUNC(romInitFixedLocation), %eax
	subl $FUNC(romInit), %eax
	addl $ROM_TEXT_ADRS, %eax
	
	/* push the memory pointer for use by romInit */
	call *%eax
	/* we should not return */
	ret

/*
 * This part of romInit is fixed in UEFI systems at ROM_TEXT_ADDR+(romInitFixedLocation-romInit)
 */
	.balign 16,0x90
romInitFixedLocation:
	movl $FUNC(romIdtr), %eax
	subl $FUNC(romInit), %eax
	addl $ROM_TEXT_ADRS, %eax
	lidt (%eax)  /* load temporary IDT */

	movl $FUNC(romGdtr), %eax
	subl $FUNC(romInit), %eax
	addl $ROM_TEXT_ADRS, %eax

	lgdt (%eax)	/* load temporary GDT */

	jmp	romInit1		/* near jump to flush a inst queue */

romInit1:
	movl $0x0010,%eax		/* set data segment 0x10 is 3rd one */

	movw	%ax,%ds			/* set DS */
	movw	%ax,%es			/* set ES */
	movw	%ax,%fs			/* set FS */
	movw	%ax,%gs			/* set GS */
	movw	%ax,%ss			/* set SS */

	/* EFI code ROM_STACK is at the end of
	 * the bootrom ROM buffer; its use isn't great.
	 */

	movl $ROM_STACK, %esp /* set stack to end of bootrom ROM buffer */
	ljmp	$0x08, $ROM_TEXT_ADRS+romInit2-romInit /* jump to romInit2, setting CS segment register */
	
	/* temporary IDTR stored in code segment in ROM */

romIdtr:
	.word	0x0000			/* size   : 0 */
	.long	0x00000000		/* address: 0 */


	/* temporary GDTR stored in code segment in ROM */

romGdtr:
	.word	0x0027			/* size   : 39(8 * 5 - 1) bytes */
	.long	(romGdt	- romInit + ROM_TEXT_ADRS) /* address: romGdt */
	

	/* temporary GDT stored in code segment in ROM */

	.balign 16,0x90
romGdt:
	/* 0(selector=0x0000): Null descriptor */
	.word	0x0000
	.word	0x0000
	.byte	0x00
	.byte	0x00
	.byte	0x00
	.byte	0x00

	/* 1(selector=0x0008): Code descriptor */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x9a			/* Code e/r, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */

	/* 2(selector=0x0010): Data descriptor */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x92			/* Data r/w, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */

	/* 3(selector=0x0018): Code descriptor, for the nesting interrupt */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x9a			/* Code e/r, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */

	/* 4(selector=0x0020): Code descriptor, for the nesting interrupt */
	.word	0xffff			/* limit: xffff */
	.word	0x0000			/* base : xxxx0000 */
	.byte	0x00			/* base : xx00xxxx */
	.byte	0x9a			/* Code e/r, Present, DPL0 */
	.byte	0xcf			/* limit: fxxxx, Page Gra, 32bit */
	.byte	0x00			/* base : 00xxxxxx */


	/* cold start code in PROTECTED MODE(32 bits) */

	.balign 16,0x90
romInit2:
	cli				/* LOCK INTERRUPT */

	/* UEFI code ROM_STACK is at end of bootrom ROM buffer */
	movl	$ ROM_STACK,%esp	/* set a stack pointer */
#if	defined (TGT_CPU) && defined (INCLUDE_SYMMETRIC_IO_MODE)

        movl    $ MP_N_CPU, %eax
        lock
        incl    (%eax)

#endif	/* defined (TGT_CPU) && defined (INCLUDE_SYMMETRIC_IO_MODE) */

	/* EFI code - no changes here, these old VESA addresses are in the RESERVED
	   video bios shadow area */

	/* WindML + VesaBIOS initialization */
	
#ifdef	INCLUDE_WINDML	
	movl	$ VESA_BIOS_DATA_PREFIX,%ebx	/* move BIOS prefix addr to EBX */
	movl	$ VESA_BIOS_KEY_1,(%ebx)	/* store "BIOS" */
	addl	$4,%ebx				/* increment EBX */
	movl	$ VESA_BIOS_KEY_2,(%ebx)	/* store "DATA" */
	movl	$ VESA_BIOS_DATA_SIZE,%ecx	/* load ECX with nBytes to copy */
	shrl    $2,%ecx				/* get nLongs to copy */
	movl	$0,%esi				/* load ESI with source addr */
	movl	$ VESA_BIOS_DATA_ADDRESS,%edi	/* load EDI with dest addr */
	rep
	movsl					/* copy BIOS data to VRAM */
#endif	/* INCLUDE_WINDML */

	/*
         * Don't call romA20on here anymore.  Done by EFI.
         */

	movl    $ BOOT_COLD,%ebx	/* %ebx has the startType */

	jmp warm /* jump over warmHigh debug output */

	/* handle warmHigh reset - send out debug statement */
warmHigh:
warm:
    /* copy bootrom image to dst addr if (romInit != ROM_TEXT_ADRS) */
	ARCH_REGS_INIT			/* initialize DR[0-7] CR0 EFLAGS */

	/* ARCH_CR4_INIT		/@ initialize CR4 for P5,6,7 */

	xorl	%eax, %eax		/* zero EAX */
	movl	%eax, %cr4		/* initialize CR4 */

	movl	$romGdtr,%eax		/* load the original GDT */
	subl	$ FUNC(romInit),%eax
	addl	$ ROM_TEXT_ADRS,%eax
	pushl	%eax
	call	FUNC(romLoadGdt)

	movl	$ STACK_ADRS, %esp	/* initialise the stack pointer */
	
	
	movl    $ ROM_TEXT_ADRS, %esi	/* get src addr(ROM_TEXT_ADRS) */
	movl    $ romInit, %edi		/* get dst addr(romInit) */
	cmpl	%esi, %edi		/* is src and dst same? */
	je	romInit4		/*   yes: skip copying */
	movl    $ FUNC(end), %ecx	/* get "end" addr */
	subl    %edi, %ecx		/* get nBytes to copy */
	shrl    $2, %ecx		/* get nLongs to copy */
	cld 				/* clear the direction bit */
	rep				/* repeat next inst ECX time */
	movsl                           /* copy itself to the entry point */

	/* jump to romStart(absolute address, position dependent) */

romInit4:
	xorl	%ebp, %ebp		/* initialize the frame pointer */
	pushl	$0			/* initialise the EFLAGS */
	popfl
	pushl	%ebx		 	/* push the startType */
	/* no need to push the memory pointer here too */
	movl	$ FUNC(romStart),%eax	/* jump to romStart */
	call	*%eax

	/* just in case, if there's a problem in romStart or romA20on */

romInitHlt:
	pushl	%eax
	call	FUNC(romEaxShow)	/* show EAX on your display device */
	hlt	
	jmp	romInitHlt


/*******************************************************************************
*
* romLoadGdt - load the global descriptor table.
*
* RETURNS: N/A
*
* NOMANUAL

* void romLoadGdt (char *romGdtr)
 
*/

        .balign 16,0x90
FUNC_LABEL(romLoadGdt)
	movl	SP_ARG1(%esp),%eax
	lgdt	(%eax)
	movw	$0x0010,%ax		/* a selector 0x10 is 3rd one */
	movw	%ax,%ds	
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%gs
	movw	%ax,%ss
	ret


/*******************************************************************************
*
* romEaxShow - show EAX register 
*
* show EAX register in your display device 
*
* RETURNS: N/A

* void romEaxShow (void)
 
*/

	.balign 16,0x90
FUNC_LABEL(romEaxShow)

	/* show EAX register in your display device available */

	ret

/**************************************************************
 * End of the UEFI enabled romInit code.
 **************************************************************/

#endif /* #ifndef INCLUDE_UEFI_BOOT_SUPPORT */
