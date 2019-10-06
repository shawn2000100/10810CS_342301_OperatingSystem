/* switch.h
 *	Definitions needed for implementing context switching.
 *
 *	Context switching is inherently machine dependent, since
 *	the registers to be saved, how to set up an initial
 *	call frame, etc, are all specific to a processor architecture.
 *
 * 	This file currently supports the DEC MIPS, DEC Alpha, SUN SPARC,
 *  HP PARISC, IBM PowerPC, and Intel x86 architectures.
 */

/*
 Copyright (c) 1992-1996 The Regents of the University of California.
 All rights reserved.  See copyright.h for copyright notice and limitation 
 of liability and disclaimer of warranty provisions.
 */

#ifndef SWITCH_H
#define SWITCH_H

#include "copyright.h"

#ifdef DECMIPS

/* Registers that must be saved during a context switch. 
 * These are the offsets from the beginning of the Thread object, 
 * in bytes, used in switch.s
 */
#define SP 0
#define S0 4
#define S1 8
#define S2 12
#define S3 16
#define S4 20
#define S5 24
#define S6 28
#define S7 32
#define FP 36
#define PC 40

/* To fork a thread, we set up its saved register state, so that
 * when we switch to the thread, it will start running in ThreadRoot.
 *
 * The following are the initial registers we need to set up to
 * pass values into ThreadRoot (for instance, containing the procedure
 * for the thread to run).  The first set is the registers as used
 * by ThreadRoot; the second set is the locations for these initial
 * values in the Thread object -- used in Thread::AllocateStack().
 */

#define InitialPC	s0
#define InitialArg	s1
#define WhenDonePC	s2
#define StartupPC	s3

#define PCState		(PC/4-1)
#define FPState		(FP/4-1)
#define InitialPCState	(S0/4-1)
#define InitialArgState	(S1/4-1)
#define WhenDonePCState	(S2/4-1)
#define StartupPCState	(S3/4-1)

#endif 	// DECMIPS

#ifdef SPARC

/* Registers that must be saved during a context switch.  See comment above. */ 
#define I0 4
#define I1 8
#define I2 12
#define I3 16
#define I4 20
#define I5 24
#define I6 28
#define I7 32

/* Aliases used for clearing code.  */
#define FP I6
#define PC I7

/* Registers for ThreadRoot.  See comment above. */
#define InitialPC       %o0
#define InitialArg      %o1
#define WhenDonePC      %o2
#define StartupPC       %o3

#define PCState         (PC/4-1)
#define InitialPCState  (I0/4-1)
#define InitialArgState (I1/4-1)
#define WhenDonePCState (I2/4-1)
#define StartupPCState  (I3/4-1)

#endif 	// SPARC

#ifdef PARISC

/* Registers that must be saved during a context switch.  See comment above. */ 
#define   SP   0
#define   S0   4
#define   S1   8
#define   S2   12
#define   S3   16
#define   S4   20
#define   S5   24
#define   S6   28
#define   S7   32
#define   S8   36
#define   S9   40
#define   S10  44
#define   S11  48
#define   S12  52
#define   S13  56
#define   S14  60
#define   S15  64
#define   PC   68

/* Registers for ThreadRoot.  See comment above. */
#define InitialPC       %r3		/* S0 */
#define InitialArg      %r4
#define WhenDonePC      %r5
#define StartupPC       %r6

#define PCState         (PC/4-1)
#define InitialPCState  (S0/4-1)
#define InitialArgState (S1/4-1)
#define WhenDonePCState (S2/4-1)
#define StartupPCState  (S3/4-1)

#endif 	// PARISC

#ifdef x86

/* the offsets of the registers from the beginning of the thread object */
#define _ESP     0
#define _EAX     4
#define _EBX     8
#define _ECX     12
#define _EDX     16
#define _EBP     20
#define _ESI     24
#define _EDI     28
#define _PC      32

/* These definitions are used in Thread::AllocateStack(). */
#define PCState         (_PC/4-1)
#define FPState         (_EBP/4-1)
#define InitialPCState  (_ESI/4-1)
#define InitialArgState (_EDX/4-1)
#define WhenDonePCState (_EDI/4-1)
#define StartupPCState  (_ECX/4-1)

#define InitialPC       %esi
#define InitialArg      %edx
#define WhenDonePC      %edi
#define StartupPC       %ecx

#endif // x86

#ifdef PowerPC 

 #define	SP	  0    // stack pointer 
 #define	P1	  4    // parameters
 #define	P2	  8
 #define	P3	 12
 #define 	P4	 16 
 #define	GP13	 20    // general purpose registers 13-31
 #define	GP14	 24
 #define	GP15	 28     
 #define	GP16	 32
 #define	GP17	 36
 #define	GP18	 40  
 #define	GP19	 44
 #define	GP20	 48
 #define	GP21	 52
 #define	GP22	 56
 #define	GP23	 60
 #define	GP24	 64
 #define	GP25	 68     
 #define	GP26	 72
 #define	GP27	 76
 #define	GP28	 80  
 #define	GP29	 84
 #define	GP30	 88
 #define	GP31	 92
 #define	FP13	 96    // floating point registers 14-31
 #define	FP15	104     
 #define	FP16	112
 #define	FP17	120
 #define	FP18	128  
 #define	FP19	136
 #define	FP20	144
 #define	FP21	152
 #define	FP22	160
 #define	FP23	168
 #define	FP24	176
 #define	FP25	184     
 #define	FP26	192
 #define	FP27	200
 #define	FP28	208  
 #define	FP29	216
 #define	FP30	224
 #define	FP31	232
 #define	CR	240   // control register
 #define	LR	244   // link register
 #define	TOC	248   // Table Of Contents


 // for ThreadRoot assembly function 

 #define	InitialPCState	0  //  (P1/4 - 1)  // user function address 
 #define	InitialArgState	1  //  (P2/4 - 1)  // user function argument       
 #define	WhenDonePCState	2  //  (P3/4 - 1)  // clean up function addr 
 #define	StartupPCState	3  //  (P4/4 - 1)  // start up function addr 
 #define	PCState		60 //  (LR/4 - 1)  // ThreadRoot addr (first time).
                                                   // Later PC addr when SWITCH 
                                                   // occured
                   
 #define	InitialLR	21
 #define	InitialArg	22
 #define	WhenDoneLR	23
 #define	StartupLR	24

#endif  // PowerPC 

#ifdef ALPHA

/* 
 * Porting to Alpha was done by Shuichi Oikawa (shui@sfc.keio.ac.jp).
 */
/* Registers that must be saved during a context switch. 
 * These are the offsets from the beginning of the Thread object, 
 * in bytes, used in switch.s
 */
#define	SP	(0*8)
#define S0	(1*8)
#define S1	(2*8)
#define S2	(3*8)
#define S3	(4*8)
#define S4	(5*8)
#define S5	(6*8)
#define S6	(7*8)		/* used as FP (Frame Pointer) */
#define GP	(8*8)
#define PC	(9*8)

/* To fork a thread, we set up its saved register state, so that
 * when we switch to the thread, it will start running in ThreadRoot.
 *
 * The following are the initial registers we need to set up to
 * pass values into ThreadRoot (for instance, containing the procedure
 * for the thread to run).  The first set is the registers as used
 * by ThreadRoot; the second set is the locations for these initial
 * values in the Thread object -- used in Thread::StackAllocate().
 */
#define InitialPC	s0
#define InitialArg	s1
#define WhenDonePC	s2
#define StartupPC	s3

#define PCState		(PC/8-1)
#define FPState		(S6/8-1)
#define InitialPCState	(S0/8-1)
#define InitialArgState	(S1/8-1)
#define WhenDonePCState	(S2/8-1)
#define StartupPCState	(S3/8-1)

#endif // HOST_ALPHA
 
#endif // SWITCH_H
