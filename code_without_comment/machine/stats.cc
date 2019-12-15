#include "stats.h"

Statistics::Statistics()
{
    totalTicks = idleTicks = systemTicks = userTicks = 0;
    numDiskReads = numDiskWrites = 0;
    numConsoleCharsRead = numConsoleCharsWritten = 0;
    numPageFaults = numPacketsSent = numPacketsRecvd = 0;
}

void Statistics::Print()
{
    cout << "Ticks: total " << totalTicks << ", idle " << idleTicks;
    cout << ", system " << systemTicks << ", user " << userTicks <<"\n";
    cout << "Disk I/O: reads " << numDiskReads;
    cout << ", writes " << numDiskWrites << "\n";
    cout << "Console I/O: reads " << numConsoleCharsRead;
    cout << ", writes " << numConsoleCharsWritten << "\n";
    cout << "Paging: faults " << numPageFaults << "\n";
    cout << "Network I/O: packets received " << numPacketsRecvd;
    cout << ", sent " << numPacketsSent << "\n";
}
