#include "main.h"
#include "filesys.h"
#include "openfile.h"
#include "sysdep.h"

Kernel *kernel;
Debug *debug;

static void Cleanup(int x)
{
    cerr << "\nCleaning up after signal " << x << "\n";
    delete kernel;
}

static const int TransferSize = 128;

void Print(char *name)
{
    OpenFile *openFile;
    int i, amountRead;
    char *buffer;

    if ((openFile = kernel->fileSystem->Open(name)) == NULL) {
        printf("Print: unable to open file %s\n", name);
        return;
    }

    buffer = new char[TransferSize];
    while ((amountRead = openFile->Read(buffer, TransferSize)) > 0)
        for (i = 0; i < amountRead; i++)
            printf("%c", buffer[i]);
    delete [] buffer;

    delete openFile;
    return;
}

int main(int argc, char **argv)
{
    int i;
    char *debugArg = "";
    char *userProgName = NULL;
    bool threadTestFlag = false;
    bool consoleTestFlag = false;
    bool networkTestFlag = false;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
      	    ASSERT(i + 1 < argc);
            debugArg = argv[i + 1];
	          i++;
        }
      	else if (strcmp(argv[i], "-z") == 0) {
            cout << copyright << "\n";
      	}
      	else if (strcmp(argv[i], "-x") == 0) {
            ASSERT(i + 1 < argc);
       	    userProgName = argv[i + 1];
       	    i++;
      	}
      	else if (strcmp(argv[i], "-K") == 0) {
      	    threadTestFlag = TRUE;
      	}
      	else if (strcmp(argv[i], "-C") == 0) {
      	    consoleTestFlag = TRUE;
      	}
      	else if (strcmp(argv[i], "-N") == 0) {
      	    networkTestFlag = TRUE;
      	}
        else if (strcmp(argv[i], "-u") == 0) {
            cout << "Partial usage: nachos [-z -d debugFlags]\n";
            cout << "Partial usage: nachos [-x programName]\n";
            cout << "Partial usage: nachos [-K] [-C] [-N]\n";
        }
    }

    debug = new Debug(debugArg);

    kernel = new Kernel(argc, argv);

    kernel->Initialize();

    CallOnUserAbort(Cleanup);

    if (threadTestFlag) {
      kernel->ThreadSelfTest();
    }
    if (consoleTestFlag) {
      kernel->ConsoleTest();
    }
    if (networkTestFlag) {
      kernel->NetworkTest();
    }

    kernel->ExecAll();

    ASSERTNOTREACHED();
}

