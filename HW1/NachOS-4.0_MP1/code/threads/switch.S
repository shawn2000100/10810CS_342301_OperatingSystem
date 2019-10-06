/* switch.s 
 *   	Machine dependent context switch routines.  DO NOT MODIFY THESE! 
 *
 *	Context switching is inherently machine dependent, since
 *	the registers to be saved, how to set up an initial
 *	call frame, etc, are all specific to a processor architecture.
 *
 * 	This file currently supports the following architectures:
 *	    DEC MIPS  (DECMIPS)
 *	    DEC Alpha  (ALPHA)
 *	    SUN SPARC (SPARC)
 *	    HP PA-RISC (PARISC)
 *	    Intel 386 (x86)
 *	    IBM RS6000 (PowerPC) -- I hope it will also work for Mac PowerPC
 *
 * We define two routines for each architecture:
 *
 * ThreadRoot(InitialPC, InitialArg, WhenDonePC, StartupPC)
 *	InitialPC  - The program counter of the procedure to run
 *			in this thread.
 *      InitialArg - The single argument to the thread.
 *	WhenDonePC - The routine to call when the thread returns.
 *	StartupPC  - Routine to call when the thread is started.
 *
 *	ThreadRoot is called from the SWITCH() routine to start
 *	a thread for the first time. 
 *
 * SWITCH(oldThread, newThread)
 * 	oldThread  - The current thread that was running, where the
 *		CPU register state is to be saved.
 * 	newThread  - The new thread to be run, where the CPU register
 *		state is to be loaded from.
 */

/*
 Copyright (c) 1992-1996 The Regents of the University of California.
 All rights reserved.  See copyright.h for copyright notice and limitation 
 of liability and disclaimer of warranty provisions.
 */

#include "copyright.h"
#include "switch.h"


#ifdef DECMIPS

/* Symbolic register names */
#define z       $0      /* zero register */
#define a0      $4      /* argument registers */
#define a1      $5
#define s0      $16     /* callee saved */
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define sp      $29     /* stack pointer */
#define fp      $30     /* frame pointer */
#define ra      $31     /* return address */

        .text   
        .align  2

	.globl ThreadRoot
	.ent	ThreadRoot,0
ThreadRoot:
	or	fp,z,z		# Clearing the frame pointer here
				# makes gdb backtraces of thread stacks 
				# end here (I hope!)

	jal	StartupPC	# call startup procedure
	move	a0, InitialArg
	jal	InitialPC	# call main procedure
	jal 	WhenDonePC	# when done, call clean up procedure

	# NEVER REACHED
	.end ThreadRoot

	# a0 -- pointer to old Thread
	# a1 -- pointer to new Thread
	.globl SWITCH
	.ent	SWITCH,0
SWITCH:
	sw	sp, SP(a0)		# save new stack pointer
	sw	s0, S0(a0)		# save all the callee-save registers
	sw	s1, S1(a0)
	sw	s2, S2(a0)
	sw	s3, S3(a0)
	sw	s4, S4(a0)
	sw	s5, S5(a0)
	sw	s6, S6(a0)
	sw	s7, S7(a0)
	sw	fp, FP(a0)		# save frame pointer
	sw	ra, PC(a0)		# save return address
	
	lw	sp, SP(a1)		# load the new stack pointer
	lw	s0, S0(a1)		# load the callee-save registers
	lw	s1, S1(a1)
	lw	s2, S2(a1)
	lw	s3, S3(a1)
	lw	s4, S4(a1)
	lw	s5, S5(a1)
	lw	s6, S6(a1)
	lw	s7, S7(a1)
	lw	fp, FP(a1)
	lw	ra, PC(a1)		# load the return address	

	j	ra
	.end SWITCH

#endif // DECMIPS



#ifdef SPARC

/* NOTE!  These files appear not to exist on Solaris --
 *  you need to find where (the SPARC-specific) MINFRAME, ST_FLUSH_WINDOWS, ...
 *  are defined.  (I don't have a Solaris machine, so I have no way to tell.)
 */
#ifdef SOLARIS
#include <sys/trap.h>
#include <sys/asm_linkage.h>
#else
#include <sun4/trap.h>
#include <sun4/asm_linkage.h>
#endif
.seg    "text"

/* SPECIAL to the SPARC:
 *	The first two instruction of ThreadRoot are skipped because
 *	the address of ThreadRoot is made the return address of SWITCH()
 *	by the routine Thread::StackAllocate.  SWITCH() jumps here on the
 *	"ret" instruction which is really at "jmp %o7+8".  The 8 skips the
 *	two nops at the beginning of the routine.
 */

#ifdef SOLARIS
.globl	ThreadRoot
ThreadRoot:
#else
.globl	_ThreadRoot
_ThreadRoot:
#endif
	nop  ; nop         /* These 2 nops are skipped because we are called
			    * with a jmp+8 instruction. */
	clr	%fp        /* Clearing the frame pointer makes gdb backtraces 
	                    * of thread stacks end here. */
			   /* Currently the arguments are in out registers we
			    * save them into local registers so they won't be 
			    * trashed during the calls we make. */
	mov	InitialPC, %l0  
	mov	InitialArg, %l1
	mov	WhenDonePC, %l2
			   /* Execute the code:
			   *	call StartupPC();
			   *	call InitialPC(InitialArg);
			   *	call WhenDonePC();
			   */
	call	StartupPC,0
	nop
	call	%l0, 1	
	mov	%l1, %o0   /* Using delay slot to setup argument to InitialPC */
	call	%l2, 0
	nop
			   /* WhenDonePC call should never return.  If it does
			    * we execute a trap into the debugger.  */
	ta	ST_BREAKPOINT


#ifdef SOLARIS
.globl	SWITCH
SWITCH:
#else
.globl	_SWITCH
_SWITCH:
#endif
	save	%sp, -SA(MINFRAME), %sp
	st	%fp, [%i0]
	st	%i0, [%i0+I0]
	st	%i1, [%i0+I1]
	st	%i2, [%i0+I2]
	st	%i3, [%i0+I3]
	st	%i4, [%i0+I4]
	st	%i5, [%i0+I5]
	st	%i7, [%i0+I7]
	ta	ST_FLUSH_WINDOWS
	nop
	mov	%i1, %l0
	ld	[%l0+I0], %i0
	ld	[%l0+I1], %i1
	ld	[%l0+I2], %i2
	ld	[%l0+I3], %i3
	ld	[%l0+I4], %i4
	ld	[%l0+I5], %i5
	ld	[%l0+I7], %i7
	ld	[%l0], %i6
	ret
	restore

#endif // SPARC



#ifdef PARISC

    ;rp = r2,   sp = r30
    ;arg0 = r26,  arg1 = r25,  arg2 = r24,  arg3 = r23

	.SPACE  $TEXT$
	.SUBSPA $CODE$
ThreadRoot
	.PROC
	.CALLINFO CALLER,FRAME=0
	.ENTER

	    .CALL
	    ble  0(%r6)             ;call StartupPC
	    stw  %r31, -24(%sp)     ;put return address in proper stack
				    ;location for StartupPC export stub.

	    or   %r4, 0, %arg0      ;load InitialArg
	    .CALL   ;in=26
	    ble  0(%r3)             ;call InitialPC
	    stw  %r31, -24(%sp)     ;put return address in proper stack
				    ;location for InitialPC export stub.

	    .CALL
	    ble  0(%r5)             ;call WhenDonePC
	    stw  %r31, -24(%sp)     ;put return address in proper stack
				    ;location for StartupPC export stub.
	    .LEAVE

	.PROCEND


SWITCH
	.PROC
	.CALLINFO CALLER,FRAME=0
	.ENTRY

    ; save process state of oldThread
	stw  %sp, SP(%arg0)	;save stack pointer
	stw  %r3, S0(%arg0)	;save callee-save registers
	stw  %r4, S1(%arg0)
	stw  %r5, S2(%arg0)
	stw  %r6, S3(%arg0)
	stw  %r7, S4(%arg0)
	stw  %r8, S5(%arg0)
	stw  %r9, S6(%arg0)
	stw  %r10, S7(%arg0)
	stw  %r11, S8(%arg0)
	stw  %r12, S9(%arg0)
	stw  %r13, S10(%arg0)
	stw  %r14, S11(%arg0)
	stw  %r15, S12(%arg0)
	stw  %r16, S13(%arg0)
	stw  %r17, S14(%arg0)
	stw  %r18, S15(%arg0)
	stw  %rp, PC(%arg0)	;save program counter

    ; restore process state of nextThread
	ldw  SP(%arg1), %sp	;restore stack pointer
	ldw  S0(%arg1), %r3	;restore callee-save registers
	ldw  S1(%arg1), %r4
	ldw  S2(%arg1), %r5
	ldw  S3(%arg1), %r6
	ldw  S4(%arg1), %r7
	ldw  S5(%arg1), %r8
	ldw  S6(%arg1), %r9
	ldw  S7(%arg1), %r10
	ldw  S8(%arg1), %r11
	ldw  S9(%arg1), %r12
	ldw  S10(%arg1), %r13
	ldw  S11(%arg1), %r14
	ldw  S12(%arg1), %r15
	ldw  S13(%arg1), %r16
	ldw  S14(%arg1), %r17
	ldw  PC(%arg1), %rp	;save program counter
	bv   0(%rp)
	.EXIT
	ldw  S15(%arg1), %r18

	.PROCEND

	.EXPORT SWITCH,ENTRY,PRIV_LEV=3,RTNVAL=GR
	.EXPORT ThreadRoot,ENTRY,PRIV_LEV=3,RTNVAL=GR

#endif // PARISC



#ifdef x86

        .text
        .align  2

        .globl  ThreadRoot
        .globl  _ThreadRoot	

/* void ThreadRoot( void )
**
** expects the following registers to be initialized:
**      eax     points to startup function (interrupt enable)
**      edx     contains inital argument to thread function
**      esi     points to thread function
**      edi     point to Thread::Finish()
*/
_ThreadRoot:	
ThreadRoot:
        pushl   %ebp
        movl    %esp,%ebp
        pushl   InitialArg
        call    *StartupPC
        call    *InitialPC
        call    *WhenDonePC

        # NOT REACHED
        movl    %ebp,%esp
        popl    %ebp
        ret



/* void SWITCH( thread *t1, thread *t2 )
**
** on entry, stack looks like this:
**      8(esp)  ->              thread *t2
**      4(esp)  ->              thread *t1
**       (esp)  ->              return address
**
** we push the current eax on the stack so that we can use it as
** a pointer to t1, this decrements esp by 4, so when we use it
** to reference stuff on the stack, we add 4 to the offset.
*/
        .comm   _eax_save,4

        .globl  SWITCH
	.globl  _SWITCH
_SWITCH:		
SWITCH:
        movl    %eax,_eax_save          # save the value of eax
        movl    4(%esp),%eax            # move pointer to t1 into eax
        movl    %ebx,_EBX(%eax)         # save registers
        movl    %ecx,_ECX(%eax)
        movl    %edx,_EDX(%eax)
        movl    %esi,_ESI(%eax)
        movl    %edi,_EDI(%eax)
        movl    %ebp,_EBP(%eax)
        movl    %esp,_ESP(%eax)         # save stack pointer
        movl    _eax_save,%ebx          # get the saved value of eax
        movl    %ebx,_EAX(%eax)         # store it
        movl    0(%esp),%ebx            # get return address from stack into ebx
        movl    %ebx,_PC(%eax)          # save it into the pc storage

        movl    8(%esp),%eax            # move pointer to t2 into eax

        movl    _EAX(%eax),%ebx         # get new value for eax into ebx
        movl    %ebx,_eax_save          # save it
        movl    _EBX(%eax),%ebx         # retore old registers
        movl    _ECX(%eax),%ecx
        movl    _EDX(%eax),%edx
        movl    _ESI(%eax),%esi
        movl    _EDI(%eax),%edi
        movl    _EBP(%eax),%ebp
        movl    _ESP(%eax),%esp         # restore stack pointer
        movl    _PC(%eax),%eax          # restore return address into eax
        movl    %eax,4(%esp)            # copy over the ret address on the stack
        movl    _eax_save,%eax

        ret

#endif // x86


#if defined(ApplePowerPC)

	/* The AIX PowerPC code is incompatible with the assembler on MacOS X
	 * and Linux.  So the SWITCH code was adapted for IBM 750 compatible
	 * processors, and ThreadRoot is modeled after the more reasonable
	 * looking ThreadRoot's in this file.
	 *
	 * Joshua LeVasseur <jtl@ira.uka.de>
	 */

	.align	2
	.globl	_SWITCH
_SWITCH:
	stw	r1, 0(r3)	/* Store stack pointer. */
	stmw	r13, 20(r3)	/* Store general purpose registers 13 - 31. */
	stfd	f14, 96(r3)	/* Store floating point registers 14 -31. */
	stfd	f15,  104(r3)
	stfd	f16,  112(r3)
	stfd	f17,  120(r3) 
	stfd	f18,  128(r3) 
	stfd	f19,  136(r3) 
	stfd	f20,  144(r3) 
	stfd	f21,  152(r3) 
	stfd	f22,  160(r3) 
	stfd	f23,  168(r3) 
	stfd	f24,  176(r3) 
	stfd	f25,  184(r3) 
	stfd	f26,  192(r3) 
	stfd	f27,  200(r3) 
	stfd	f28,  208(r3) 
	stfd	f29,  216(r3) 
	stfd	f30,  224(r3) 
	stfd	f31,  232(r3) 

	mflr	r0
	stw	r0, 244(r3)	/* Spill the link register. */

	mfcr	r12
	stw	r12, 240(r3)	/* Spill the condition register. */

	lwz	r1, 0(r4)	/* Load the incoming stack pointer. */

	lwz	r0, 244(r4)	/* Load the incoming link register. */
	mtlr	r0		/* Restore the link register. */

	lwz	r12, 240(r4)	/* Load the condition register value. */
	mtcrf	0xff, r12	/* Restore the condition register. */

	lmw	r13, 20(r4)	/* Restore registers r13 - r31. */

	lfd	f14,  96(r4)	/* Restore floating point register f14 - f31. */
	lfd	f15,  104(r4)
	lfd	f16,  112(r4)
	lfd	f17,  120(r4)
	lfd	f18,  128(r4) 
	lfd	f19,  136(r4) 
	lfd	f20,  144(r4) 
	lfd	f21,  152(r4) 
	lfd	f22,  160(r4) 
	lfd	f23,  168(r4) 
	lfd	f24,  176(r4) 
	lfd	f25,  184(r4) 
	lfd	f26,  192(r4) 
	lfd	f27,  200(r4) 
	lfd	f28,  208(r4) 
	lfd	f29,  216(r4) 
	lfd	f30,  224(r4) 
	lfd	f31,  232(r4) 

	/* When a thread first starts, the following blr instruction jumps
	 * to ThreadRoot.  ThreadRoot expects the incoming thread block
	 * in r4.
	 */
	blr	/* Branch to the address held in link register. */


	.align	2
	.globl	_ThreadRoot
_ThreadRoot:
	lwz	r20, 16(r4)	/* StartupPCState - ThreadBegin		*/
	lwz	r21, 8(r4)	/* InitialArgState - arg		*/
	lwz	r22, 4(r4)	/* InitialPCState - func		*/
	lwz	r23, 12(r4)	/* WhenDonePCState - ThreadFinish	*/

	/* Call ThreadBegin function. */
	mtctr	r20		/* The function pointer. */
	bctrl

	/* Call the target function. */
	mr	r3, r21		/* Function arg. */
	mtctr	r22		/* Function pointer. */
	bctrl

	/* Call the ThreadFinish function. */
	mtctr	r23
	bctrl

	/* We shouldn't execute here. */
1:	b	1b


#endif


#if defined(PowerPC) && !defined(ApplePowerPC)
                .globl branch[ds]
                .csect branch[ds]
                .long  .branch[PR]
                .long  TOC[tc0]
                .long  0
                .toc
 T.branch:      .tc    .branch[tc], branch[ds]
                .globl .branch[PR]
                .csect .branch[PR]

         l      0, 0x0(11)        #  load function address into r0
         mtctr  0                 #  move r0 into counter register
         l      2, 0x4(11)        #  move new TOC address into r2
         l      11, 0x8(11)       #  reset function address
         bctr                     #  branch to the counter register


                .globl ThreadRoot[ds]
                .csect ThreadRoot[ds]
                .long  .ThreadRoot[PR]
                .long  TOC[tc0]
                .long  0
                .toc
 T.ThreadRoot:  .tc    .ThreadRoot[tc], ThreadRoot[ds]
                .globl .ThreadRoot[PR]
                .csect .ThreadRoot[PR]
 
                .set argarea,      32
                .set linkarea,     24
                .set locstckarea,   0
                .set nfprs,        18
                .set ngprs,        19
                .set szdsa,        8*nfprs+4*ngprs+linkarea+argarea+locstckarea
                

         mflr    0
         mfcr    12
         bl      ._savef14
         cror   0xf, 0xf, 0xf
         stm    13, -8*nfprs-4*ngprs(1)
         st     0, 8(1)
         st     12, 4(1)
         st     4,  24(1)
         st     5,  28(1)
         st     6,  32(1)
         stu    1, -szdsa(1) 

         muli   11,3,1          #  copy contents of register r24 to r11
         bl     .branch[PR]      #  call function branch
         cror   0xf, 0xf, 0xf    #  no operation
         
         ai     1,1,szdsa
         lm     13, -8*nfprs-4*ngprs(1)
         bl     ._restf14
         cror   0xf, 0xf, 0xf
         l      0, 8(1)
         l      12, 4(1)
         mtlr   0
         mtcrf  0x38, 12
         l      4, 24(1)
         l      5, 28(1)
         l      6, 32(1)
  
         mflr    0
         mfcr    12
         bl      ._savef14
         cror   0xf, 0xf, 0xf
         stm    13, -8*nfprs-4*ngprs(1)
         st     0, 8(1)
         st     12, 4(1)
         st     6,  24(1)
         stu    1, -szdsa(1)
 
         muli   3, 4,1          #  load user function parameter r22 to r3                     
         muli   11,5,1          #  copy contents of register r21 to r11
         bl     .branch[PR]      #  call function branch
         cror   0xf, 0xf, 0xf    #  no operation

         ai     1,1,szdsa
         lm     13, -8*nfprs-4*ngprs(1)
         bl     ._restf14
         cror   0xf, 0xf, 0xf
         l      0, 8(1)
         l      12, 4(1)
         mtlr   0
         mtcrf  0x38, 12
         l      6, 24(1)

         muli   11,6,1          #  copy contents of register r23 to r11
         bl     .branch[PR]      #  call function branch
         cror   0xf, 0xf, 0xf    #  no operation 
         brl                     #  the programme should not return here.

         .extern ._savef14
         .extern ._restf14



         
            .globl SWITCH[ds]
            .csect SWITCH[ds]
            .long  .SWITCH[PR]
            .long  TOC[tc0]
            .long  0
            .toc
 T.SWITCH:  .tc    .SWITCH[tc], SWITCH[ds]
            .globl .SWITCH[PR]
            .csect .SWITCH[PR]

         st     1,   0(3)       # store stack pointer
         stm    13,  20(3)      # store general purpose registers (13 -31)
         stfd   14,  96(3)      # store floating point registers (14 -31) 
         stfd   15,  104(3)     # there is no single instruction to do for
         stfd   16,  112(3)     # floating point registers. so do one by one
         stfd   17,  120(3) 
         stfd   18,  128(3) 
         stfd   19,  136(3) 
         stfd   20,  144(3) 
         stfd   21,  152(3) 
         stfd   22,  160(3) 
         stfd   23,  168(3) 
         stfd   24,  176(3) 
         stfd   25,  184(3) 
         stfd   26,  192(3) 
         stfd   27,  200(3) 
         stfd   28,  208(3) 
         stfd   29,  216(3) 
         stfd   30,  224(3) 
         stfd   31,  232(3) 
         mflr   0               # move link register value to register 0
         st     0,   244(3)     # store link register value
       
         mfcr   12              # move condition register to register 12
         st     12,  240(3)     # store condition register value


         l      1,   0(4)       # load stack pointer
         l      0,   244(4)     # load link register value
         mtlr   0         
         l      12,  240(4)     # load condition register value
         mtcrf  0x38,  12



         lm     13,  20(4)      # load into general purpose registers (13 -31) 
         lfd    14,  96(4)      # load into floating point registers (14 -31) 
         lfd    15,  104(4)     # there is no single instruction for
         lfd    16,  112(4)     # loading into more than one floating point 
         lfd    17,  120(4)     # registers. so do one by one.
         lfd    18,  128(4) 
         lfd    19,  136(4) 
         lfd    20,  144(4) 
         lfd    21,  152(4) 
         lfd    22,  160(4) 
         lfd    23,  168(4) 
         lfd    24,  176(4) 
         lfd    25,  184(4) 
         lfd    26,  192(4) 
         lfd    27,  200(4) 
         lfd    28,  208(4) 
         lfd    29,  216(4) 
         lfd    30,  224(4) 
         lfd    31,  232(4) 
         l      3,   16(4)
         l      5,   4(4)
         l      6,   12(4)
         l      4,   8(4)

         brl                    # branch to the address held in link register.
#endif // PowerPC


#ifdef ALPHA

/* 
 * Porting to Alpha was done by Shuichi Oikawa (shui@sfc.keio.ac.jp).
 */

/*
 *	Symbolic register names and register saving rules
 *
 *	Legend:
 *		T	Saved by caller (Temporaries)
 *		S	Saved by callee (call-Safe registers)
 */

#define	v0	$0	/* (T)		return value		*/
#define t0	$1	/* (T)		temporary registers	*/
#define s0	$9	/* (S)		call-safe registers	*/
#define s1	$10
#define s2	$11
#define s3	$12
#define s4	$13
#define s5	$14
#define s6	$15
#define a0	$16	/* (T)		argument registers	*/
#define a1	$17
#define ai	$25	/* (T)		argument information	*/
#define ra	$26	/* (T)		return address		*/
#define pv	$27	/* (T)		procedure value		*/
#define	gp	$29	/* (T)		(local) data pointer	*/
#define sp	$30	/* (S)		stack pointer		*/
#define zero	$31	/* 		wired zero		*/

	.set	noreorder	# unless overridden
	.align	3
	.text
 	
	.globl ThreadRoot
	.ent	ThreadRoot,0
ThreadRoot:
	.frame	sp,0,ra
	ldgp	gp,0(pv)

	mov	zero,s6		# Clearing the frame pointer here
				# makes gdb backtraces of thread stacks 
				# end here (I hope!)
	mov	StartupPC,pv
	jsr	ra,(pv)		# call startup procedure
	ldgp	gp,0(ra)

	mov	InitialArg,a0
	mov	InitialPC,pv
	jsr	ra,(pv)		# call main procedure
	ldgp	gp,0(ra)

	mov	WhenDonePC,pv
	jsr 	ra,(pv)		# when done, call clean up procedure
	ldgp	gp,0(ra)

	.end ThreadRoot		# NEVER REACHED
	
/* a0 -- pointer to old Thread *
 * a1 -- pointer to new Thread */
	.globl SWITCH
	.ent	SWITCH,0
SWITCH:
	.frame	sp,0,ra
	ldgp	gp,0(pv)
	
	stq	ra, PC(a0)		# save return address
	stq	gp, GP(a0)
	stq	sp, SP(a0)		# save new stack pointer
	stq	s0, S0(a0)		# save all the callee-save registers
	stq	s1, S1(a0)
	stq	s2, S2(a0)
	stq	s3, S3(a0)
	stq	s4, S4(a0)
	stq	s5, S5(a0)
	stq	s6, S6(a0)		# save frame pointer
	
	ldq	ra, PC(a1)		# load the return address	
	ldq	gp, GP(a1)
	ldq	sp, SP(a1)		# load the new stack pointer
	ldq	s0, S0(a1)		# load the callee-save registers
	ldq	s1, S1(a1)
	ldq	s2, S2(a1)
	ldq	s3, S3(a1)
	ldq	s4, S4(a1)
	ldq	s5, S5(a1)
	ldq	s6, S6(a1)

	mov	ra,pv
	ret	zero,(ra)

	.end SWITCH
	
#endif // ALPHA
