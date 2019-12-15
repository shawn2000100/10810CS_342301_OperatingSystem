#include "utility.h"
#include "callback.h"

class ConsoleInput : public CallBackObj {
  public:
    ConsoleInput(char *readFile, CallBackObj *toCall);

    ~ConsoleInput();

    char GetChar();

    void CallBack();

  private:
    int readFileNo;			// UNIX file emulating the keyboard
    CallBackObj *callWhenAvail;
    char incoming;
};

class ConsoleOutput : public CallBackObj {
  public:
    ConsoleOutput(char *writeFile, CallBackObj *toCall);
    ~ConsoleOutput();
    void PutChar(char ch);
    void CallBack();
    void PutInt(int n);

  private:
    int writeFileNo;			// UNIX file emulating the display
    CallBackObj *callWhenDone;
    bool putBusy;
};
