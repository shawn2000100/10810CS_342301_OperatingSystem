#include "utility.h"
#include "callback.h"
#include "timer.h"

class Alarm : public CallBackObj {
  public:
    Alarm(bool doRandomYield);

    ~Alarm() { delete timer; }

    void WaitUntil(int x);

  private:
    Timer *timer;

    void CallBack();
};
