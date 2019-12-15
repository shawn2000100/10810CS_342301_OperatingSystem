#include "console.h"
#include "main.h"
#include "stdio.h"

ConsoleInput::ConsoleInput(char *readFile, CallBackObj *toCall)
{
    if (readFile == NULL)
	readFileNo = 0;					// keyboard = stdin
    else
    	readFileNo = OpenForReadWrite(readFile, TRUE);	// should be read-only

    // set up the stuff to emulate asynchronous interrupts
    callWhenAvail = toCall;
    incoming = EOF;

    // start polling for incoming keystrokes
    kernel->interrupt->Schedule(this, ConsoleTime, ConsoleReadInt);
}

ConsoleInput::~ConsoleInput()
{
    if (readFileNo != 0)
	Close(readFileNo);
}

void
ConsoleInput::CallBack()
{
  char c;
  int readCount;

    ASSERT(incoming == EOF);
    if (!PollFile(readFileNo)) { // nothing to be read
        // schedule the next time to poll for a packet
        kernel->interrupt->Schedule(this, ConsoleTime, ConsoleReadInt);
    } else {
    	// otherwise, try to read a character
    	readCount = ReadPartial(readFileNo, &c, sizeof(char));
	if (readCount == 0) {
	   // this seems to happen at end of file, when the
	   // console input is a regular file
	   // don't schedule an interrupt, since there will never
	   // be any more input
	   // just do nothing....
	}
	else {
	  // save the character and notify the OS that
	  // it is available
	  ASSERT(readCount == sizeof(char));
	  incoming = c;
	  kernel->stats->numConsoleCharsRead++;
	}
	callWhenAvail->CallBack();
    }
}

char
ConsoleInput::GetChar()
{
   char ch = incoming;

   if (incoming != EOF) {	// schedule when next char will arrive
       kernel->interrupt->Schedule(this, ConsoleTime, ConsoleReadInt);
   }
   incoming = EOF;
   return ch;
}

ConsoleOutput::ConsoleOutput(char *writeFile, CallBackObj *toCall)
{
    if (writeFile == NULL)
	writeFileNo = 1;				// display = stdout
    else
    	writeFileNo = OpenForWrite(writeFile);

    callWhenDone = toCall;
    putBusy = FALSE;
}

//----------------------------------------------------------------------
// ConsoleOutput::~ConsoleOutput
// 	Clean up console output emulation
//----------------------------------------------------------------------

ConsoleOutput::~ConsoleOutput()
{
    if (writeFileNo != 1)
	Close(writeFileNo);
}

//----------------------------------------------------------------------
// ConsoleOutput::CallBack()
// 	Simulator calls this when the next character can be output to the
//	display.
//----------------------------------------------------------------------

void
ConsoleOutput::CallBack()
{
	DEBUG(dbgTraCode, "In ConsoleOutput::CallBack(), " << kernel->stats->totalTicks);
    putBusy = FALSE;
    kernel->stats->numConsoleCharsWritten++;
    callWhenDone->CallBack();
}

//----------------------------------------------------------------------
// ConsoleOutput::PutChar()
// 	Write a character to the simulated display, schedule an interrupt
//	to occur in the future, and return.
//----------------------------------------------------------------------

void
ConsoleOutput::PutChar(char ch)
{
    ASSERT(putBusy == FALSE);
    WriteFile(writeFileNo, &ch, sizeof(char));
    putBusy = TRUE;
    kernel->interrupt->Schedule(this, ConsoleTime, ConsoleWriteInt);
}
