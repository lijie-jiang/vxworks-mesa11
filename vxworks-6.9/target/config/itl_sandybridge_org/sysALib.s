/* sysALib.s - system-dependent routines */

/*
 * Copyright (c) 2011-2013 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
01c,28jan13,scm  WIND00229494 - SMP dedicated segment register
01b,20nov12,scm  WIND00376020 - fast system call support...
01a,11apr11,j_z  initial creation based on itl_nehalem version 02q
*/

/*
DESCRIPTION
This module contains system-dependent routines written in assembly
language.

This module must be the first specified in the \f3ld\f1 command used to
build the system.  The sysInit() routine is the system start-up code.

INTERNAL
Many routines in this module doesn't use the "c" frame pointer %ebp@ !
This is only for the benefit of the stacktrace facility to allow it 
to properly trace tasks executing within these routines.

SEE ALSO: 
.I "i80386 32-Bit Microprocessor User's Manual"
*/



#define _ASMLANGUAGE
#include <vxWorks.h>
#include <vsbConfig.h>
#include <asm.h>
#include <regs.h>
#include <sysLib.h>
#include <config.h>

#ifdef _WRS_CONFIG_SMP
#include <private/vxSmpP.h>
#endif /* _WRS_CONFIG_SMP */

        .data
	.globl  FUNC(copyright_wind_river)
	.long   FUNC(copyright_wind_river)

	/* externals */

	.globl	FUNC(usrInit)		/* system initialization routine */
	.globl	VAR(sysProcessor)	/* initialized to NONE(-1) in sysLib.c */

#ifdef _WRS_CONFIG_SMP
        _IA32_ASM_EXTERN_KERNEL_GLOBALS
#endif /* _WRS_CONFIG_SMP */
                                                                               
#if defined(_WRS_CONFIG_SMP) || defined(INCLUDE_AMP_CPU)
        .globl  FUNC(sysInitCpuStartup) /* the cpu startup */
        .globl  FUNC(cpuInit1)          /* the cpu startup initialization */
        .globl  FUNC(sysCpuInit)        /* the cpu initialization */
#endif /* defined(_WRS_CONFIG_SMP) || defined(INCLUDE_AMP_CPU) */

	/* internals */

	.globl	sysInit			/* start of system code */
	.globl	_sysInit		/* start of system code */
        .globl  GTEXT(sysInitGDT)
	.globl	GTEXT(sysInByte)
	.globl	GTEXT(sysInWord)
	.globl	GTEXT(sysInLong)
	.globl	GTEXT(sysInWordString)
	.globl	GTEXT(sysInLongString)
	.globl	GTEXT(sysOutByte)
	.globl	GTEXT(sysOutWord)
	.globl	GTEXT(sysOutLong)
	.globl	GTEXT(sysOutWordString)
	.globl	GTEXT(sysOutLongString)
	.globl	GTEXT(sysReboot)
	.globl	GTEXT(sysWait)
	.globl	GTEXT(sysLoadGdt)
	.globl	GTEXT(sysGdtr)
	.globl	GTEXT(sysGdt)

	.globl  GDATA(sysCsSuper)	/* code selector: supervisor mode */
	.globl  GDATA(sysCsExc)		/* code selector: exception */
	.globl  GDATA(sysCsInt)		/* code selector: interrupt */


	/* locals */

FUNC_LABEL(vendorIdIntel)		/* CPUID vendor ID - Intel */
	.ascii	"GenuineIntel"


#ifdef INCLUDE_UEFI_BOOT_SUPPORT
	.globl  pSysUefiMemAddr   /* UEFI pointer value for memory block header */
	.globl  pSysUefiAcpiAddr   /* UEFI pointer value for ACPI info */
	.globl  uefiComplianceInfoP /* UEFI read-only compliance string */

	.balign 8, 0x00
pSysUefiMemAddr:
		.long 0x00000000
pSysUefiAcpiAddr:
		.long 0x00000000

#endif /* INCLUDE_UEFI_BOOT_SUPPORT */


	.text
	.balign 16

/*******************************************************************************
*
* sysInit - start after boot
*
* This routine is the system start-up entry point for VxWorks in RAM, the
* first code executed after booting.  It disables interrupts, sets up
* the stack, and jumps to the C routine usrInit() in usrConfig.c.
*
* The initial stack is set to grow down from the address of sysInit().  This
* stack is used only by usrInit() and is never used again.  Memory for the
* stack must be accounted for when determining the system load address.
*
* NOTE: This routine should not be called by the user.
*
* RETURNS: N/A

* sysInit ()              /@ THIS IS NOT A CALLABLE ROUTINE @/
 
*/

sysInit:
_sysInit:
	cli				/* LOCK INTERRUPT */

#ifdef GRUB_MULTIBOOT
        jmp     multiboot_entry

        /* Multiboot Header ---allows GRUB to load kernel */
        .align 8

        /* MULTIBOOT ---ELF images */
multiboot_header:
        .long   MULTIBOOT_HEADER_MAGIC
        .long   MULTIBOOT_HEADER_FLAGS_ELF
        .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS_ELF)

multiboot_entry:

        cmp     $MULTIBOOT_BOOTLOADER_MAGIC, %eax
        jne     multiPrologEnd

        movl    %eax,(MULTIBOOT_SCRATCH)     /* save magic */

        movl    %ebx,(MULTIBOOT_SCRATCH + 8) /* save multi-boot pointer */

        movl    $BOOT_CLEAR,%ebx
multiPrologEnd:
#else
	movl    SP_ARG1(%esp),%ebx
#endif /* GRUB_MULTIBOOT */

#ifdef INCLUDE_UEFI_BOOT_SUPPORT

	jmp overcomplianceinfo
	/* UEFI read-only compliance string */
uefiComplianceInfoP:
		.asciz   "UEFI 001.10 Compliant" /* format is XXX.YY, where YY=1, 2, ..., 9, 10, 11, etc */
overcomplianceinfo:

	/* the second argument on the stack is going to be the memory address of the UEFI
	 * memory data block, pushed by the "go" routine in bootConfig.c
	 */ 
	movl $FUNC(pSysUefiMemAddr), %eax  /* get the memory map data buffer ptr address */
	movl SP_ARG2(%esp), %ecx
	movl %ecx, (%eax)		 /* store the value into the location*/

	/* the third argument on the stack is going to be the memory address of the UEFI
	 * ACPI data block, pushed by the "go" routine in bootConfig.c
	 */ 
	movl $FUNC(pSysUefiAcpiAddr), %eax  /* get the ACPI data ptr address */
	movl SP_ARG3(%esp), %ecx
	movl %ecx, (%eax)		 /* store the value into the location*/

#endif /* INCLUDE_UEFI_BOOT_SUPPORT */

	movl	$ FUNC(sysInit),%esp	/* initialize stack pointer */
	movl	$0,%ebp			/* initialize frame pointer */

	ARCH_REGS_INIT			/* initialize DR[0-7] CR0 EFLAGS */

	/* ARCH_CR4_INIT		/@ initialize CR4 for P5,6,7 */

	xorl	%eax, %eax		/* zero EAX */
	movl	%eax, %cr4		/* initialize CR4 */

#ifdef INCLUDE_UEFI_BOOT_SUPPORT
	pushl  	FUNC(pSysUefiAcpiAddr) /* push the pointer to the ACPI Table */
	pushl  	FUNC(pSysUefiMemAddr) /* push the pointer to the UEFI memory data block */
#endif
	pushl	%ebx			/* push the startType */
	movl	$ FUNC(usrInit),%eax
	movl	$ FUNC(sysInit),%edx	/* push return address */
	pushl	%edx			/*   for emulation for call */
	pushl	$0			/* push EFLAGS, 0 */
	pushl	$0x0008			/* a selector 0x08 is 2nd one */
	pushl	%eax			/* push EIP,  FUNC(usrInit) */
	iret				/* iret */

#if defined (_WRS_CONFIG_SMP) || defined (INCLUDE_AMP_CPU)
/*******************************************************************************
*
* sysInitCpuStartup - istartup initialization for APs...
*
* void sysInitCpuStartup (void)
*
* scratch memory usage:
*
*    scratchMem (scratch memory offset)  scratchPoint
*
*    Standard GDT Entries:
*
*    null descriptor                     scratchMem + 0x04
*
*    kernel text descriptor              scratchMem + 0x0c
*
*    kernel data descriptor              scratchMem + 0x14
*
*    exception text descriptor           scratchMem + 0x1C
*
*    interrupt data descriptor           scratchMem + 0x24
*
*    gdt limit << 16                     scratchMem + LIM_GDT_OFFSET
*    gdt base                            scratchMem + BASE_GDT_OFFSET
*    address of page directory           scratchMem + CR3_OFFSET
*
*    idt limit                           scratchMem + LIM_IDT_OFFSET
*    idt base address                    scratchMem + BASE_IDT_OFFSET
*
*    initial AP stack addr               scratchMem + AP_STK_OFFSET
*/
        .balign 16,0x90
FUNC_LABEL(sysInitCpuStartup)

        /* cold start code in REAL MODE (16 bits) */

        cli                              /* LOCK INTERRUPT */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        xor     %eax, %eax

        mov     %ax, %ds
        mov     %ax, %ss

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     $CPU_SCRATCH_POINT, %eax /* find the location of scratch mem */
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     (%eax), %ecx
        mov     %cx, %sp

        /* load intial IDT */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     (%eax), %ecx
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        add     $CPU_SCRATCH_IDT, %ecx   /* base of initial idt */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        lidt    (%ecx)

        /* load intial GDT */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     (%eax), %ecx
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        add     $CPU_SCRATCH_GDT, %ecx   /* base of initial gdt */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        lgdt    (%ecx)

#if defined (_WRS_CONFIG_SMP)
        /* Enable MMU for SMP only */
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     (%eax), %ecx
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        add     $CR3_OFFSET, %ecx        /* stored value for cr3 */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     (%ecx), %edx
        mov     %edx, %cr3

        /* Check for Logical CPU 0 Boot */

        mov     %cr0,%ecx                /* move CR0 to ECX */
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        or      $0x00000001,%ecx         /* set the PE bit */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        cmp     $0, %edx
        je      skipMmuInit


#ifdef  INCLUDE_MMU_P6_36BIT
        /* setup PAE, etc... */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     $0x000006F0,%ecx /* set PAE,PSE,OSXMMEXCPT,OSFXSR,PGE,MCE bits */
#else
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     $0x000006D0,%ecx /* set the OSXMMEXCPT,OSFXSR,PGE,PSE,MCE bits */
#endif  /* INCLUDE_MMU_P6_36BIT */
        mov     %ecx,%cr4                /* move ECX to CR4 */

        /* turn on paging & switch to protected mode */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     $0x80010033,%ecx         /* set the PE bit */

skipMmuInit:
        mov     %ecx,%cr0                /* move ECX to CR0 */

#else   /* defined (_WRS_CONFIG_SMP) */

        /* AMP Mode */

        /* switch to protected mode */

        mov     %cr0,%ecx               /* move CR0 to ECX */
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        or      $0x00000001,%ecx        /* set the PE bit */
        mov     %ecx,%cr0               /* move ECX to CR0 */

#endif /* defined (_WRS_CONFIG_SMP) */

        jmp     clearSegs                /* near jump to flush a inst queue */

clearSegs:
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     $0x0010,%ecx             /* a selector 0x10 is 3rd one */
        mov     %cx,%ds
        mov     %cx,%es
        mov     %cx,%fs
        mov     %cx,%gs
        mov     %cx,%ss

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        mov     (%eax), %ecx
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        add     $AP_STK_OFFSET, %ecx     /* base of initial stk */

        /* jump to protected mode code in high mem */

#ifndef _WRS_CONFIG_SMP
ampEntry:
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        movl    (%ecx), %esp             /* initialise the stack pointer */
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        pushl   $BOOT_COLD
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        pushl   $FUNC(sysInit)
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        ljmp    $0x08,  $FUNC(sysInit)
#else
smpEntry:

        /* Check for Logical CPU 0 Boot */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        cmpl    $0, %edx
        jne     1f

        /* Logical CPU0 Boot */

        .byte   0x67, 0x66               /* next inst has 32bit operand */
        movl    (%ecx), %esp             /* initialise the stack pointer */
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        pushl   $BOOT_COLD
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        pushl   $FUNC(sysInit)
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        ljmp    $0x08,  $FUNC(sysInit)
1:
        .byte   0x67, 0x66               /* next inst has 32bit operand */
        ljmp    $0x08, $RAM_LOW_ADRS + cpuInit1 - sysInit
#endif /* !_WRS_CONFIG_SMP */

        .balign 16,0x90
FUNC_LABEL(cpuInit1)

        /* cold start code in PROTECTED MODE (32 bits) */

        movl    (%ecx), %esp             /* initialise the stack pointer */

        xorl    %eax, %eax               /* zero EAX */
        movl    %eax, %edx               /* zero EDX */

        xorl    %ebp, %ebp               /* initialize the frame pointer */
        pushl   $0                       /* initialise the EFLAGS */
        popfl

        movl    $FUNC(sysCpuInit),%eax   /* jump to cpu init to complete */

        call    *%eax

        /* just in case there's a problem */

cpuInitHlt1:
        hlt
        jmp     cpuInitHlt1
#endif /* defined (_WRS_CONFIG_SMP) || defined (INCLUDE_AMP_CPU) */

/*******************************************************************************
*
* sysInitGDT - load the brand new GDT into RAM
*
* void sysInitGDT (void)
*/
        .balign 16,0x90
FUNC_LABEL(sysInitGDT)

        pushal
        movl    %esp,%ebp

        movl    $FUNC(sysGdt),%esi      /* set src addr (&sysGdt) */

#ifdef _WRS_CONFIG_SMP
        _IA32_PER_CPU_VALUE_GET(%edi,%eax,pSysGdt)
#else
	movl    FUNC(pSysGdt), %edi
#endif

        movl    %edi,%eax
        movl    $ GDT_ENTRIES,%ecx      /* number of GDT entries */
        movl    %ecx,%edx
        shll    $1,%ecx                 /* set (nLongs of GDT) to copy */
        cld
        rep
        movsl                           /* copy GDT from src to dst */

        pushl   %eax                    /* push the (GDT base addr) */
        shll    $3,%edx                 /* get (nBytes of GDT) */
        decl    %edx                    /* get (nBytes of GDT) - 1 */
        shll    $16,%edx                /* move it to the upper 16 */
        pushl   %edx                    /* push the nBytes of GDT - 1 */
        leal    2(%esp),%eax            /* get the addr of (size:addr) */
        pushl   %eax                    /* push it as a parameter */
        call    FUNC(sysLoadGdt)        /* load the brand new GDT in RAM */

        movl    %ebp,%esp
        popal                           /* re-initialize frame pointer */
        ret

/*******************************************************************************
*
* sysInByte - input one byte from I/O space
*
* RETURNS: Byte data from the I/O port.

* UCHAR sysInByte (address)
*     int address;	/@ I/O port address @/
 
*/

	.balign 16,0x90
FUNC_LABEL(sysInByte)
	movl	SP_ARG1(%esp),%edx
	movl	$0,%eax
	inb	%dx,%al
	jmp	sysInByte0
sysInByte0:
	ret

/*******************************************************************************
*
* sysInWord - input one word from I/O space
*
* RETURNS: Word data from the I/O port.

* USHORT sysInWord (address)
*     int address;	/@ I/O port address @/
 
*/

	.balign 16,0x90
FUNC_LABEL(sysInWord)
	movl	SP_ARG1(%esp),%edx
	movl	$0,%eax
	inw	%dx,%ax
	jmp	sysInWord0
sysInWord0:
	ret

/*******************************************************************************
*
* sysInLong - input one long-word from I/O space
*
* RETURNS: Long-Word data from the I/O port.

* USHORT sysInLong (address)
*     int address;	/@ I/O port address @/
 
*/

	.balign 16,0x90
FUNC_LABEL(sysInLong)
	movl	SP_ARG1(%esp),%edx
	movl	$0,%eax
	inl	%dx,%eax
	jmp	sysInLong0
sysInLong0:
	ret

/*******************************************************************************
*
* sysOutByte - output one byte to I/O space
*
* RETURNS: N/A

* void sysOutByte (address, data)
*     int address;	/@ I/O port address @/
*     char data;	/@ data written to the port @/
 
*/

	.balign 16,0x90
FUNC_LABEL(sysOutByte)
	movl	SP_ARG1(%esp),%edx
	movl	SP_ARG2(%esp),%eax
	outb	%al,%dx
	jmp	sysOutByte0
sysOutByte0:
	ret

/*******************************************************************************
*
* sysOutWord - output one word to I/O space
*
* RETURNS: N/A

* void sysOutWord (address, data)
*     int address;	/@ I/O port address @/
*     short data;	/@ data written to the port @/
 
*/

	.balign 16,0x90
FUNC_LABEL(sysOutWord)
	movl	SP_ARG1(%esp),%edx
	movl	SP_ARG2(%esp),%eax
	outw	%ax,%dx
	jmp	sysOutWord0
sysOutWord0:
	ret

/*******************************************************************************
*
* sysOutLong - output one long-word to I/O space
*
* RETURNS: N/A

* void sysOutLong (address, data)
*     int address;	/@ I/O port address @/
*     long data;	/@ data written to the port @/
 
*/

	.balign 16,0x90
FUNC_LABEL(sysOutLong)
	movl	SP_ARG1(%esp),%edx
	movl	SP_ARG2(%esp),%eax
	outl	%eax,%dx
	jmp	sysOutLong0
sysOutLong0:
	ret

/*******************************************************************************
*
* sysInWordString - input word string from I/O space
*
* RETURNS: N/A

* void sysInWordString (port, address, count)
*     int port;		/@ I/O port address @/
*     short *address;	/@ address of data read from the port @/
*     int count;	/@ count @/
 
*/

	.balign 16,0x90
FUNC_LABEL(sysInWordString)
	pushl	%edi
	movl	SP_ARG1+4(%esp),%edx
	movl	SP_ARG2+4(%esp),%edi
	movl	SP_ARG3+4(%esp),%ecx
	cld
	rep
	insw	%dx,(%edi)
	movl	%edi,%eax
	popl	%edi
	ret

/*******************************************************************************
*
* sysInLongString - input long string from I/O space
*
* RETURNS: N/A

* void sysInLongString (port, address, count)
*     int port;		/@ I/O port address @/
*     long *address;	/@ address of data read from the port @/
*     int count;	/@ count @/
 
*/

	.balign 16,0x90
FUNC_LABEL(sysInLongString)
	pushl	%edi
	movl	SP_ARG1+4(%esp),%edx
	movl	SP_ARG2+4(%esp),%edi
	movl	SP_ARG3+4(%esp),%ecx
	cld
	rep
	insl	%dx,(%edi)
	movl	%edi,%eax
	popl	%edi
	ret

/*******************************************************************************
*
* sysOutWordString - output word string to I/O space
*
* RETURNS: N/A

* void sysOutWordString (port, address, count)
*     int port;		/@ I/O port address @/
*     short *address;	/@ address of data written to the port @/
*     int count;	/@ count @/
 
*/

	.balign 16,0x90
FUNC_LABEL(sysOutWordString)
	pushl	%esi
	movl	SP_ARG1+4(%esp),%edx
	movl	SP_ARG2+4(%esp),%esi
	movl	SP_ARG3+4(%esp),%ecx
	cld
	rep
	outsw	(%esi),%dx
	movl	%esi,%eax
	popl	%esi
	ret

/*******************************************************************************
*
* sysOutLongString - output long string to I/O space
*
* RETURNS: N/A

* void sysOutLongString (port, address, count)
*     int port;		/@ I/O port address @/
*     long *address;	/@ address of data written to the port @/
*     int count;	/@ count @/
 
*/

	.balign 16,0x90
FUNC_LABEL(sysOutLongString)
	pushl	%esi
	movl	SP_ARG1+4(%esp),%edx
	movl	SP_ARG2+4(%esp),%esi
	movl	SP_ARG3+4(%esp),%ecx
	cld
	rep
	outsl	(%esi),%dx
	movl	%esi,%eax
	popl	%esi
	ret

/*******************************************************************************
*
* sysWait - wait until the input buffer become empty
*
* wait until the input buffer become empty
*
* RETURNS: N/A

* void sysWait (void)
 
*/

	.balign 16,0x90
FUNC_LABEL(sysWait)
	xorl	%ecx,%ecx
sysWait0:
	movl	$0x64,%edx		/* Check if it is ready to write */
	inb	%dx,%al
	andb	$2,%al
	loopnz	sysWait0
	ret

/*******************************************************************************
*
* sysReboot - reboot system
*
* RETURNS: N/A
*
* NOMANUAL
*
* void sysReboot ()
*
*/

	.balign 16,0x90
FUNC_LABEL(sysReboot)

	pushl   $0x2
	call	FUNC(sysToMonitor)		/* initiate cold boot */
	ret

/*******************************************************************************
*
* sysLoadGdt - load the global descriptor table.
*
* RETURNS: N/A
*
* NOMANUAL
* void sysLoadGdt (char *sysGdtr)
*/

        .balign 16,0x90
FUNC_LABEL(sysLoadGdt)
	movl	4(%esp),%eax
	lgdt	(%eax)
	movw	$0x0010,%ax		/* a selector 0x10 is 3rd one */
	movw	%ax,%ds	
	movw	%ax,%es
	movw	%ax,%fs
	movw	%ax,%ss

#ifndef _WRS_CONFIG_SMP
        movw    %ax,%gs
#endif /* !_WRS_CONFIG_SMP */

	ret

/*******************************************************************************
*
* sysGdt - the global descriptor table.
*
* RETURNS: N/A
*
* NOMANUAL
*
*/

        .text
        .balign 16,0x90
FUNC_LABEL(sysGdtr)
        .word   0x004f                  /* size   : 79(8 * 10 - 1) bytes */
        .long   FUNC(sysGdt)

        .balign 16,0x90
FUNC_LABEL(sysGdt)

        /* 0 (selector=0x0000): Null descriptor */

        .word   0x0000
        .word   0x0000
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00

        /*
         * The global descriptor table will be populated with some, if not
         * all, of the following entries based on the image configuration:
         *
         *    Index         Usage
         *    -----         -----
         *      0            RESERVED
         *                  
         *      1            Supervisor Code Segment  (0x00000008)
         *      2            Supervisor Data Segment  (0x00000010)
         *                                             
         *     (3 - 5 exist if RTPs enabled...)
         *
         *      3            User Code Segment        (0x00000018)
         *      4            User Data Segment        (0x00000020)
         *      5            RTP TSS                  (0x00000028)
         *
         *     (5 - 7 exist if OSM enabled...)
         *
         *      5            OSM/RTP Save TSS         (0x00000028)
         *      6            OSM Restore TSS          (0x00000030)
         *      7            OSM Task Gate            (0x00000038)
         *
         *     (8 - 9 allways present...)
         *
         *      8            Trap Segment             (0x00000040)
         *      9            Interrupt Segment        (0x00000048)
         *
         * Note GDT entries 1,2,8,and 9 always present.
         */

        /* 1 (selector=0x0008): Code descriptor, for the supervisor mode task */

        .word   0xffff  /* limit: xffff */
        .word   0x0000  /* base : xxxx0000 */
        .byte   0x00    /* base : xx00xxxx */
        .byte   0x9b    /* (P) Segment Present = 0x1, 
                         * (DPL) Privilege Level = 0x00 (DPL0),
                         * (S) Descriptor Type = 0x1 (code/data),
                         * (TYPE) Segement type = 0xB (exec, read, non-conforming)
                         */
        .byte   0xcf    /* (G) Granularity = 0x1,
                         * (D/B) Default Op Size = 0x1 (32-bit segment),
                         * (L) 64 bit Code Segment = 0x0 (IA-32e mode only), 
                         * (AVL) Available for use - 0x0,
                         * Seg Limit 19:16 = 0xf
                         */
        .byte   0x00    /* base : 00xxxxxx */

        /* 2 (selector=0x0010): Data descriptor */

        .word   0xffff  /* limit: xffff */
        .word   0x0000  /* base : xxxx0000 */
        .byte   0x00    /* base : xx00xxxx */
        .byte   0x93    /* (P) Segment Present = 0x1, 
                         * (DPL) Privilege Level = 0x00 (DPL0),
                         * (S) Descriptor Type = 0x1 (code/data),
                         * (TYPE) Segement type = 0x3 (expand-up, read, write)
                         */
        .byte   0xcf    /* (G) Granularity = 0x1,
                         * (D/B) Default Op Size = 0x1 (32-bit segment),
                         * (L) 64 bit Code Segment = 0x0 (IA-32e mode only), 
                         * (AVL) Available for use - 0x0,
                         * Seg Limit 19:16 = 0xf
                         */
        .byte   0x00    /* base : 00xxxxxx */

        /* 3 (selector=0x0018): place holder for User Code Segment*/

        .word   0x0000
        .word   0x0000
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00

        /* 4 (selector=0x0020): place holder for User Data Segment */

        .word   0x0000
        .word   0x0000
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00

        /* 5 (selector=0x0028): place holder for OSM/RTP Save TSS */

        .word   0x0000
        .word   0x0000
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00

        /* 6 (selector=0x0030): place holder for OSM Restore TSS */

        .word   0x0000
        .word   0x0000
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00

        /* 7 (selector=0x0038): place holder for OSM Task Gate */

        .word   0x0000
        .word   0x0000
        .byte   0x00
        .byte   0x00
        .byte   0x00
        .byte   0x00

        /* 8 (selector=0x0040): Code descriptor, for the exception */

        .word   0xffff  /* limit: xffff */
        .word   0x0000  /* base : xxxx0000 */
        .byte   0x00    /* base : xx00xxxx */
        .byte   0x9B    /* (P) Segment Present = 0x1, 
                         * (DPL) Privilege Level = 0x00 (DPL0),
                         * (S) Descriptor Type = 0x1 (code/data),
                         * (TYPE) Segement type = 0xB (exec, read, non-conforming)
                         */
        .byte   0xcf    /* (G) Granularity = 0x1,
                         * (D/B) Default Op Size = 0x1 (32-bit segment),
                         * (L) 64 bit Code Segment = 0x0 (IA-32e mode only), 
                         * (AVL) Available for use - 0x0,
                         * Seg Limit 19:16 = 0xf
                         */
        .byte   0x00    /* base : 00xxxxxx */

        /* 9 (selector=0x0048): Code descriptor, for the interrupt */

        .word   0xffff  /* limit: xffff */
        .word   0x0000  /* base : xxxx0000 */
        .byte   0x00    /* base : xx00xxxx */
        .byte   0x9B    /* (P) Segment Present = 0x1, 
                         * (DPL) Privilege Level = 0x00 (DPL0),
                         * (S) Descriptor Type = 0x1 (code/data),
                         * (TYPE) Segement type = 0xB (exec, read, non-conforming)
                         */
        .byte   0xcf    /* (G) Granularity = 0x1,
                         * (D/B) Default Op Size = 0x1 (32-bit segment),
                         * (L) 64 bit Code Segment = 0x0 (IA-32e mode only), 
                         * (AVL) Available for use - 0x0,
                         * Seg Limit 19:16 = 0xf
                         */
        .byte   0x00    /* base : 00xxxxxx */

        .data
        .balign 32,0x90
FUNC_LABEL(sysCsSuper)
        .long   0x00000008              /* CS for supervisor mode task */
FUNC_LABEL(sysCsExc)
        .long   0x00000040              /* CS for exception */
FUNC_LABEL(sysCsInt)
        .long   0x00000048              /* CS for interrupt */

