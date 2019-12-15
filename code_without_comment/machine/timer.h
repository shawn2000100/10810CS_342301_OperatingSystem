#include "utility.h"
#include "callback.h"

// The following class defines a hardware timer.
class Timer : public CallBackObj {
  public:
    Timer(bool doRandom, CallBackObj *toCall);

    virtual ~Timer() {}

    void Disable() { disable = TRUE; }

  private:
    bool randomize;		// set if we need to use a random timeout delay
    CallBackObj *callPeriodically; // call this every TimerTicks time units
    bool disable;

    void CallBack();

    void SetInterrupt();
};
