
/* segments.c
 *	Simple program to illustrate different segments and to show
 *      how parameters are passed and the syscall is generated.
 *
 * objdump below refers to:
 * /software/gcc_nachos/bin/decstation-ultrix-objdump
 *
 * Compile this "gmake segments"
 * Then use objdump to examine output.
 *          objdump -d segments.coff  - to disassemble
 *          objdump -s segments.coff  - to see contents of segments
 *          objdump -x segments.coff  - to see symbol table information
 *          nachos -d m -s -x segments
 */

#define N   (5)                      /* N is replaced by the preprocessor */

unsigned int initdata1 = 0xdeadbeef; /* initialized data put in .data segment */
int initdata2 = 0xbb;                /* same as above                         */
const int blah = 0xff;               /* into .rdata segment                   */
int uninitdata[N];                   /* allocate space in .bss segment        */

main()
{
	/* automatic variable stored on stack or in register */
        int i;
	int stack1 = 0xaa;                
	int stack2;
        const int stack3 = 0xee;     /* in reg or on stack not .rdata */
        char *str = "Hello World .rdata segment\n";

	/* str is stored on the stack or in a register
	 * but text that is initialized is stored in .rdata
	 */

        for (i=0; i<N; i++) {
          uninitdata[i] = i;
        }
        Halt();
}
