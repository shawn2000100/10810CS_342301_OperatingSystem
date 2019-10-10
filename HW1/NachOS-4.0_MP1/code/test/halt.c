/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"
// include <stdio.h> // 1910010[J]: 無法include <stdio.h> ...
// 1910010[J]: Halt()在usrProg/syscall.h裡呼叫，可能就是return 0 的意思，詳細定義的位置可能是在
int
main()
{
    PrintInt(666666); 
    Halt();
    /* not reached */
    PrintInt(777777);
}
