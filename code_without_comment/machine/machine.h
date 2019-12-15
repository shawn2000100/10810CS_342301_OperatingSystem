#include "utility.h"
#include "translate.h"

const int PageSize = 128;

const int NumPhysPages = 128;

const int MemorySize = (NumPhysPages * PageSize);
const int TLBSize = 4;

enum ExceptionType {
             NoException,
		     SyscallException,
		     PageFaultException,
		     ReadOnlyException,
		     BusErrorException,
		     AddressErrorException,
		     OverflowException,
		     IllegalInstrException,
		     NumExceptionTypes
};


#define StackReg	29	// User's stack pointer
#define RetAddrReg	31	// Holds return address for procedure calls
#define NumGPRegs	32	// 32 general purpose registers on MIPS
#define HiReg		32	// Double register to hold multiply result
#define LoReg		33
#define PCReg		34	// Current program counter
#define NextPCReg	35	// Next program counter (for branch delay)
#define PrevPCReg	36	// Previous program counter (for debugging)
#define LoadReg		37	// The register target of a delayed load.
#define LoadValueReg 	38	// The value to be loaded by a delayed load.
#define BadVAddrReg	39	// The failing virtual address on an exception

#define NumTotalRegs 	40

class Instruction;
class Interrupt;

class Machine {
  public:
    Machine(bool debug);
    ~Machine();

    void Run();

    int ReadRegister(int num);

    void WriteRegister(int num, int value);

    char *mainMemory;

    TranslationEntry *tlb;

    TranslationEntry *pageTable;

    unsigned int pageTableSize;

    bool ReadMem(int addr, int size, int* value);
    bool WriteMem(int addr, int size, int value);

  private:
    void DelayedLoad(int nextReg, int nextVal);

    void OneInstruction(Instruction *instr);

    ExceptionType Translate(int virtAddr, int* physAddr, int size,bool writing);

    void RaiseException(ExceptionType which, int badVAddr);

    void Debugger();
    void DumpState();

    int registers[NumTotalRegs];

    bool singleStep;

    int runUntilTime;

    friend class Interrupt;
};


extern void ExceptionHandler(ExceptionType which);

unsigned int WordToHost(unsigned int word);
unsigned short ShortToHost(unsigned short shortword);
unsigned int WordToMachine(unsigned int word);
unsigned short ShortToMachine(unsigned short shortword);
