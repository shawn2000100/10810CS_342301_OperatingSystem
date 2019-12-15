#include "list.h"
#include "callback.h"

enum IntStatus {IntOff, IntOn};

enum MachineStatus {IdleMode, SystemMode, UserMode};

enum IntType {TimerInt, DiskInt, ConsoleWriteInt, ConsoleReadInt,
              NetworkSendInt, NetworkRecvInt};

typedef int OpenFileId;

class PendingInterrupt {
  public:
    PendingInterrupt(CallBackObj *callOnInt, int time, IntType kind);

    CallBackObj *callOnInterrupt;

    int when;
    IntType type;
};

class Interrupt {
  public:
    Interrupt();
    ~Interrupt();

    IntStatus SetLevel(IntStatus level);

    void Enable() { (void) SetLevel(IntOn); }

    IntStatus getLevel() {return level;}

    void Idle();

    void Halt();

    void PrintInt(int number);
    int CreateFile(char *filename);
    OpenFileId OpenFile(char *name);
    int WriteFile(char *buffer, int size, OpenFileId id);
    int ReadFile(char *buffer, int size, OpenFileId id);
    int CloseFile(OpenFileId id);

    void YieldOnReturn();
    MachineStatus getStatus() { return status; }
    void setStatus(MachineStatus st) { status = st; }
    void DumpState();

    void Schedule(CallBackObj *callTo, int when, IntType type);
    void OneTick();

  private:
    IntStatus level;
    SortedList<PendingInterrupt *> *pending;

    //int writeFileNo;
    bool inHandler;
    //bool putBusy;
    bool yieldOnReturn;
    MachineStatus status;
    bool CheckIfDue(bool advanceClock);
    void ChangeLevel(IntStatus old, IntStatus now);
};
