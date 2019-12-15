#include "utility.h"
#include "sysdep.h"

const char dbgAll = '+';		// turn on all debug messages
const char dbgThread = 't';		// threads
const char dbgSynch = 's';		// locks, semaphores, condition vars
const char dbgInt = 'i'; 		// interrupt emulation
const char dbgMach = 'm'; 		// machine emulation
const char dbgDisk = 'd'; 		// disk emulation
const char dbgFile = 'f'; 		// file system
const char dbgAddr = 'a'; 		// address spaces
const char dbgNet = 'n'; 		// network emulation
const char dbgSys = 'u';                // systemcall
const char dbgTraCode = 'c';

class Debug {
  public:
    Debug(char *flagList);

    bool IsEnabled(char flag);

  private:
    char *enableFlags;		// controls which DEBUG messages are printed
};

extern Debug *debug;

#define DEBUG(flag, expr)                                                     \
    if (!debug->IsEnabled(flag)) {} else { 				\
        cerr << expr << "\n";   				        \
    }


#define ASSERT(condition)                                               \
    if (condition) {} else { 						\
	cerr << "Assertion failed: line " << __LINE__ << " file " << __FILE__ << "\n";      \
        Abort();                                                              \
    }


#define ASSERTNOTREACHED()                                             \
    { 						\
	cerr << "Assertion failed: line " << __LINE__ << " file " << __FILE__ << "\n";      \
        Abort();                                                              \
    }


#define UNIMPLEMENTED()                                                      \
{                                                                            \
  cerr << "Reached UNIMPLEMENTED function " << __FUNCTION__ << " in file: "  \
       << __FILE__ << " line: " << __LINE__ << ".\n";                        \
}
