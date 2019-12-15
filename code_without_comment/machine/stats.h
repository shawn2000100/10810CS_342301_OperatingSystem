class Statistics {
  public:
    int totalTicks;
    int idleTicks;
    int systemTicks;
    int userTicks;

    int numDiskReads;
    int numDiskWrites;
    int numConsoleCharsRead;
    int numConsoleCharsWritten;
    int numPageFaults;
    int numPacketsSent;
    int numPacketsRecvd;

    Statistics();
    void Print();
};

const int UserTick = 	   1;
const int SystemTick =	  10;
const int RotationTime = 500;
const int SeekTime =	 500;
const int ConsoleTime =	 100;
const int NetworkTime =	 100;
const int TimerTicks = 	 100;
