/* errno.h 
 *	Error codes for system calls.          
 *      
 *	Do not modify the contents of this file.
 *
 *	Try to use the most descriptive error number for the error.
 *      You may find it helpful to see what errors equivalent UNIX
 *      system calls return under various error conditions.
 *      For example the man page for the write system call "man 2 write" 
 *      provides a list of different error number values for different
 *      conditions.
 *      NOTE: there are way more errors here than you should be supporting
 *            and many more error conditions listed in man pages than 
 *            Nachos can possibly generate. The list here is to give you
 *            some ideas and to hopefully standardize on some error numbers.
 *
 *      ALSO NOTE: These definitions may not correspond to Solaris definitions
 *      (for copyright reasons these are taken from Linux).
 */

#ifndef ERRNO_H
#define ERRNO_H

#include "copyright.h"		

#define EPERM            -1      /* Operation not permitted */
#define ENOENT           -2      /* No such file or directory */
#define ESRCH            -3      /* No such process */
#define EINTR            -4      /* Interrupted system call */
#define EIO              -5      /* I/O error */
#define ENXIO            -6      /* No such device or address */
#define E2BIG            -7      /* Arg list too long */
#define ENOEXEC          -8      /* Exec format error */
#define EBADF            -9      /* Bad file number */
#define ECHILD          -10      /* No child processes */
#define EAGAIN          -11      /* Try again */
#define ENOMEM          -12      /* Out of memory */
#define EACCES          -13      /* Permission denied */
#define EFAULT          -14      /* Bad address */
#define ENOTBLK         -15      /* Block device required */
#define EBUSY           -16      /* Device or resource busy */
#define EEXIST          -17      /* File exists */
#define EXDEV           -18      /* Cross-device link */
#define ENODEV          -19      /* No such device */
#define ENOTDIR         -20      /* Not a directory */
#define EISDIR          -21      /* Is a directory */
#define EINVAL          -22      /* Invalid argument */
#define ENFILE          -23      /* File table overflow */
#define EMFILE          -24      /* Too many open files */
#define ENOTTY          -25      /* Not a typewriter */
#define ETXTBSY         -26      /* Text file busy */
#define EFBIG           -27      /* File too large */
#define ENOSPC          -28      /* No space left on device */
#define ESPIPE          -29      /* Illegal seek */
#define EROFS           -30      /* Read-only file system */
#define EMLINK          -31      /* Too many links */
#define EPIPE           -32      /* Broken pipe */
#define EDOM            -33      /* Math argument out of domain of func */
#define ERANGE          -34      /* Math result not representable */
#define EDEADLK         -35      /* Resource deadlock would occur */
#define ENAMETOOLONG    -36      /* File name too long */
#define ENOLCK          -37      /* No record locks available */
#define ENOSYS          -38      /* Function not implemented */
#define ENOTEMPTY       -39      /* Directory not empty */
#define ELOOP           -40      /* Too many symbolic links encountered */
#define EWOULDBLOCK     EAGAIN  /* Operation would block */
#define ENOMSG          -42      /* No message of desired type */
#define EIDRM           -43      /* Identifier removed */
#define ECHRNG          -44      /* Channel number out of range */
#define EL2NSYNC        -45      /* Level 2 not synchronized */
#define EL3HLT          -46      /* Level 3 halted */
#define EL3RST          -47      /* Level 3 reset */
#define ELNRNG          -48      /* Link number out of range */
#define EUNATCH         -49      /* Protocol driver not attached */
#define ENOCSI          -50      /* No CSI structure available */
#define EL2HLT          -51      /* Level 2 halted */
#define EBADE           -52      /* Invalid exchange */
#define EBADR           -53      /* Invalid request descriptor */
#define EXFULL          -54      /* Exchange full */
#define ENOANO          -55      /* No anode */
#define EBADRQC         -56      /* Invalid request code */
#define EBADSLT         -57      /* Invalid slot */

#endif // ERRNO_H
