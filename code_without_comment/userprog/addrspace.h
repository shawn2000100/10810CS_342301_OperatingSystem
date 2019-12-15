#include "filesys.h"

#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    AddrSpace();			// Create an address space.
    ~AddrSpace();			// De-allocate an address space

    bool Load(char *fileName);

    void Execute(char *fileName);

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch

    ExceptionType Translate(unsigned int vaddr, unsigned int *paddr, int mode);

  private:
    TranslationEntry *pageTable;

    unsigned int numPages;

    void InitRegisters();
};
