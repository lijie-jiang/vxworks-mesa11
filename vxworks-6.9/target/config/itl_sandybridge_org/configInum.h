/* configInum.h - Sandy Bridge Interrupt Number configuration header */

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
01c,06jan12,sem  WIND00322011 - Wrap APIC_TIMER_CLOCK_HZ for cdf compatibility
01b,07mar11,j_z  sync to itl_nehalem/configInum.h 01b version.
01a,13jan11,j_z  initial creation based on itl_hanlanCreek version 01b 
*/

/*
This module contains the configuration parameters for the
Intel Sandy Bridge BSP
*/

#ifndef	INCconfigInumh
#define	INCconfigInumh

#ifdef __cplusplus
extern "C" {
#endif


/* interrupt mode selection (P6, P7 specific) */

#if defined(INCLUDE_VIRTUAL_WIRE_MODE) || defined(INCLUDE_SYMMETRIC_IO_MODE)
#if !defined (CDF_OVERRIDE) && !defined (PRJ_BUILD)
#define APIC_TIMER_CLOCK_HZ  100000000 /* frequency ~  100 Mhz */
#endif /* !CDF_OVERRIDE && !PRJ_BUILD */
#define TIMER_CLOCK_HZ       APIC_TIMER_CLOCK_HZ /* obsolete */
#define PIT0_FOR_AUX		/* use channel 0 as an Aux Timer */
#endif	/* defined(INCLUDE_VIRTUAL_WIRE_MODE) || defined(INCLUDE_SYMMETRIC_IO_MODE) */

/*
 * interrupt (vector) numbers for PIC(i8259a), Local APIC and IO APIC
 * reserved/used vector numbers are:
 *   0x00 - 0x1f : used by the CPU exceptions
 *   0x40        : reserved by VxWorks(AE syscall)
 */

#define INT_NUM_GET(irq)	(sysInumTbl[(int)irq])

#ifdef	INCLUDE_SYMMETRIC_IO_MODE

/*
 * Interrupt priority is implied by its vector number, according to
 * the following relationship:
 *   priority = vectorNo / 16
 * The lowest priority is 1 and 15 is the highest.  To avoid losing
 * interrupts, software should allocate no more than 2 interrupt
 * vectors per priority.  The recommended priority class are:
 *   priority class 15   : system event interrupt (power fail, etc)
 *   priority class 14   : inter processor interrupt (IPI)
 *   priority class 13   : local interrupt (LINT[1:0] events)
 *   priority class 12   : timer interrupt (local timer and error interrupt)
 *   priority class 11-3 : I/O interrupt
 *   priority class 2    : I/O interrupt (CPU self interrupt)
 *   priority class 1-0  : reserved
 */

#undef	PIT0_INT_LVL
#define	PIT0_INT_LVL		0x2	/* PIT0 interrupt level */
#define	INT_NUM_IRQ0		0xbc	/* vector number for IOAPIC IRQ0 */
#define INT_NUM_LOAPIC_IPI	0xe8	/* Local APIC IPI vec no */
#define INT_NUM_LOAPIC_SM	0xe4	/* Local APIC SM vec no */
#define INT_NUM_LOAPIC_SPURIOUS	0xe0	/* Local APIC SPURIOUS vec no */
#define INT_NUM_LOAPIC_THERMAL	0xdc	/* Local APIC THERMAL vec no */
#define INT_NUM_LOAPIC_PMC	0xd8	/* Local APIC PMC vec no */
#define INT_NUM_LOAPIC_LINT1	0xd4	/* Local APIC LINT1 vec no */
#define INT_NUM_LOAPIC_LINT0	0xd0	/* Local APIC LINT0 vec no */
#define INT_NUM_LOAPIC_ERROR	0xcc	/* Local APIC ERROR vec no */
#define INT_NUM_LOAPIC_TIMER	0xc4	/* Local APIC TIMER vec no */
#define INT_NUM_IOAPIC_IRQ0	0xbc	/* IO APIC IRQ 0 vec no */
#define INT_NUM_IOAPIC_IRQ1	0xb4	/* IO APIC IRQ 1 vec no */
#define INT_NUM_IOAPIC_IRQ2	0xac	/* IO APIC IRQ 2 vec no */
#define INT_NUM_IOAPIC_IRQ3	0xa4	/* IO APIC IRQ 3 vec no */
#define INT_NUM_IOAPIC_IRQ4	0x9c	/* IO APIC IRQ 4 vec no */
#define INT_NUM_IOAPIC_IRQ5	0x94	/* IO APIC IRQ 5 vec no */
#define INT_NUM_IOAPIC_IRQ6	0x8c	/* IO APIC IRQ 6 vec no */
#define INT_NUM_IOAPIC_IRQ7	0x84	/* IO APIC IRQ 7 vec no */
#define INT_NUM_IOAPIC_IRQ8	0x7c	/* IO APIC IRQ 8 vec no */
#define INT_NUM_IOAPIC_IRQ9	0x74	/* IO APIC IRQ 9 vec no */
#define INT_NUM_IOAPIC_IRQA	0x6c	/* IO APIC IRQ A vec no */
#define INT_NUM_IOAPIC_IRQB	0x64	/* IO APIC IRQ B vec no */
#define INT_NUM_IOAPIC_IRQC	0x5c	/* IO APIC IRQ C vec no */
#define INT_NUM_IOAPIC_IRQD	0x54	/* IO APIC IRQ D vec no */
#define INT_NUM_IOAPIC_IRQE	0x4c	/* IO APIC IRQ E vec no */
#define INT_NUM_IOAPIC_IRQF	0x44	/* IO APIC IRQ F vec no */
#define INT_NUM_IOAPIC_PIRQA	0x3e	/* IO APIC PIRQ A vec no */
#define INT_NUM_IOAPIC_PIRQB	0x3c	/* IO APIC PIRQ B vec no */
#define INT_NUM_IOAPIC_PIRQC	0x3a	/* IO APIC PIRQ C vec no */
#define INT_NUM_IOAPIC_PIRQD	0x38	/* IO APIC PIRQ D vec no */
#define INT_NUM_IOAPIC_PIRQE	0x36	/* IO APIC PIRQ E vec no */
#define INT_NUM_IOAPIC_PIRQF	0x34	/* IO APIC PIRQ F vec no */
#define INT_NUM_IOAPIC_PIRQG	0x32	/* IO APIC PIRQ G vec no */
#define INT_NUM_IOAPIC_PIRQH	0x30	/* IO APIC PIRQ H vec no */
#define INT_NUM_IOAPIC_PXIRQ0   0x2e    /* IO APIC PCIX IRQ0 vec no */
#define INT_NUM_IOAPIC_PXIRQ1   0x2c    /* IO APIC PCIX IRQ1 vec no */
#define INT_NUM_IOAPIC_PXIRQ2   0x2a    /* IO APIC PCIX IRQ2 vec no */
#define INT_NUM_IOAPIC_PXIRQ3   0x28    /* IO APIC PCIX IRQ3 vec no */
#define INT_NUM_PIT0		(INT_NUM_IOAPIC_IRQ2)
#define INT_NUM_COM1		(INT_NUM_IOAPIC_IRQ4)
#define INT_NUM_COM2		(INT_NUM_IOAPIC_IRQ3)
#define INT_NUM_COM3		(INT_NUM_IOAPIC_IRQ5)
#define INT_NUM_COM4		(INT_NUM_IOAPIC_IRQ9)
#define INT_NUM_RTC		(INT_NUM_IOAPIC_IRQ8)
#define INT_NUM_FD		(INT_NUM_IOAPIC_IRQ6)
#define INT_NUM_ATA0 		(INT_NUM_IOAPIC_IRQE)
#define INT_NUM_ATA1		(INT_NUM_IOAPIC_IRQ9)
#define INT_NUM_PCIC		(INT_NUM_IOAPIC_IRQA)
#define INT_NUM_TCIC		(INT_NUM_IOAPIC_IRQA)
#define INT_NUM_ELT0		(INT_NUM_IOAPIC_IRQ5)
#define INT_NUM_ELT1		(INT_NUM_IOAPIC_IRQ9)
#define INT_NUM_LPT		(INT_NUM_IOAPIC_IRQ7)
#define INT_NUM_LPT0		(INT_NUM_IOAPIC_IRQ7)
#define INT_NUM_LPT1		(INT_NUM_IOAPIC_IRQ5)
#define INT_NUM_LPT2		(INT_NUM_IOAPIC_IRQ9)
#define INT_NUM_KBD		(INT_NUM_IOAPIC_IRQ1)
#define INT_NUM_MSE		(INT_NUM_IOAPIC_IRQC)
#define INT_NUM_ELC		(INT_NUM_IOAPIC_IRQB)
#define INT_NUM_ULTRA		(INT_NUM_IOAPIC_IRQB)
#define INT_NUM_EEX		(INT_NUM_IOAPIC_IRQB)
#define INT_NUM_ELT		(INT_NUM_IOAPIC_IRQB)
#define INT_NUM_ENE		(INT_NUM_IOAPIC_IRQ5)
#define INT_NUM_ESMC		(INT_NUM_IOAPIC_IRQB)
#define INT_NUM_EI		(INT_NUM_IOAPIC_IRQB)

#else	/* ! INCLUDE_SYMMETRIC_IO_MODE = (INCLUDE_VIRTUAL_WIRE_MODE) || (PIC_MODE) */

#define INT_NUM_IRQ0		0x20	/* vector number for PIC IRQ0 */
#define INT_NUM_LOAPIC_IPI	0xe8	/* Local APIC IPI vec no */
#define INT_NUM_LOAPIC_SM	0xe4	/* Local APIC SM vec no */
#define INT_NUM_LOAPIC_SPURIOUS	0xe0	/* Local APIC SPURIOUS vec no */
#define INT_NUM_LOAPIC_THERMAL	0xdc	/* Local APIC THERMAL vec no */
#define INT_NUM_LOAPIC_PMC	0xd8	/* Local APIC PMC vec no */
#define INT_NUM_LOAPIC_LINT1	0xd4	/* Local APIC LINT1 vec no */
#define INT_NUM_LOAPIC_LINT0	0xd0	/* Local APIC LINT0 vec no */
#define INT_NUM_LOAPIC_ERROR	0xcc	/* Local APIC ERROR vec no */
#define INT_NUM_LOAPIC_TIMER	0xc4	/* Local APIC TIMER vec no */
#define INT_NUM_PIT0		(INT_NUM_IRQ0 + PIT0_INT_LVL)
#define INT_NUM_COM1		(INT_NUM_IRQ0 + COM1_INT_LVL)
#define INT_NUM_COM2		(INT_NUM_IRQ0 + COM2_INT_LVL)
#define INT_NUM_COM3		(INT_NUM_IRQ0 + COM3_INT_LVL)
#define INT_NUM_COM4		(INT_NUM_IRQ0 + COM4_INT_LVL)
#define INT_NUM_RTC		(INT_NUM_IRQ0 + RTC_INT_LVL)
#define INT_NUM_FD		(INT_NUM_IRQ0 + FD_INT_LVL)
#define INT_NUM_ATA0 		(INT_NUM_IRQ0 + ATA0_INT_LVL)
#define INT_NUM_ATA1		(INT_NUM_IRQ0 + ATA1_INT_LVL)
#define INT_NUM_PCIC		(INT_NUM_IRQ0 + PCIC_INT_LVL)
#define INT_NUM_TCIC		(INT_NUM_IRQ0 + TCIC_INT_LVL)
#define INT_NUM_ELT0		(INT_NUM_IRQ0 + ELT0_INT_LVL)
#define INT_NUM_ELT1		(INT_NUM_IRQ0 + ELT1_INT_LVL)
#define INT_NUM_LPT		(INT_NUM_IRQ0 + LPT_INT_LVL)
#define INT_NUM_LPT0		(INT_NUM_IRQ0 + LPT0_INT_LVL)
#define INT_NUM_LPT1		(INT_NUM_IRQ0 + LPT1_INT_LVL)
#define INT_NUM_LPT2		(INT_NUM_IRQ0 + LPT2_INT_LVL)
#define INT_NUM_KBD		(INT_NUM_IRQ0 + KBD_INT_LVL)
#define INT_NUM_MSE		(INT_NUM_IRQ0 + MSE_INT_LVL)
#define INT_NUM_ELC		(INT_NUM_IRQ0 + INT_LVL_ELC)
#define INT_NUM_ULTRA		(INT_NUM_IRQ0 + INT_LVL_ULTRA)
#define INT_NUM_EEX		(INT_NUM_IRQ0 + INT_LVL_EEX)
#define INT_NUM_ELT		(INT_NUM_IRQ0 + INT_LVL_ELT)
#define INT_NUM_ENE		(INT_NUM_IRQ0 + INT_LVL_ENE)
#define INT_NUM_ESMC		(INT_NUM_IRQ0 + INT_LVL_ESMC)
#define INT_NUM_EI		(INT_NUM_IRQ0 + INT_LVL_EI)

#endif	/* INCLUDE_SYMMETRIC_IO_MODE */


/* kept for backward compatibility.  obsolete in the next release */

#define INT_VEC_GET(irq)	INT_NUM_GET(irq)
#define PIT0_INT_VEC		(INT_NUM_PIT0)
#define COM1_INT_VEC		(INT_NUM_COM1)
#define COM2_INT_VEC		(INT_NUM_COM2)
#define RTC_INT_VEC		(INT_NUM_RTC)
#define FD_INT_VEC		(INT_NUM_FD)
#define ATA0_INT_VEC 		(INT_NUM_ATA0)
#define ATA1_INT_VEC		(INT_NUM_ATA1)
#define PCIC_INT_VEC		(INT_NUM_PCIC)
#define TCIC_INT_VEC		(INT_NUM_TCIC)
#define ELT0_INT_VEC		(INT_NUM_ELT0)
#define ELT1_INT_VEC		(INT_NUM_ELT1)
#define LPT_INT_VEC		(INT_NUM_LPT)
#define KBD_INT_VEC		(INT_NUM_KBD)
#define MSE_INT_VEC		(INT_NUM_MSE)
#define INT_VEC_ELC		(INT_NUM_ELC)
#define INT_VEC_ULTRA		(INT_NUM_ULTRA)
#define INT_VEC_EEX		(INT_NUM_EEX)
#define INT_VEC_ELT		(INT_NUM_ELT)
#define INT_VEC_ENE		(INT_NUM_ENE)
#define INT_VEC_ESMC		(INT_NUM_ESMC)
#define INT_VEC_EI		(INT_NUM_EI)


#ifdef __cplusplus
}
#endif

#endif	/* INCconfigInumh */

