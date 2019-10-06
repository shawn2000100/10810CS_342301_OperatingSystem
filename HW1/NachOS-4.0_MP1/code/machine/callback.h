// callback.h 
//	Data structure to allow an object to register a "callback".
//	On an asynchronous operation, the call to start the operation
//	returns immediately.  When the operation completes, the called 
//	object must somehow notify the caller of the completion.
//	In the general case, the called object doesn't know the type
//	of the caller.
//
//	We implement this using virtual functions in C++.  An object
//	that needs to register a callback is set up as a derived class of
//	the abstract base class "CallbackObj".  When we pass a
//	pointer to the object to a lower level module, that module
//	calls back via "obj->CallBack()", without knowing the
//	type of the object being called back.
//
// 	Note that this isn't a general-purpose mechanism, 
//	because a class can only register a single callback.
//
//  DO NOT CHANGE -- part of the machine emulation
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
//

#ifndef CALLBACK_H
#define CALLBACK_H 

#include "copyright.h"

// Abstract base class for objects that register callbacks

class CallBackObj {
   public:
     virtual void CallBack() = 0;
   protected:
     CallBackObj() {};	// to prevent anyone from creating
				// an instance of this class.  Only
				// allow creation of instances of
				// classes derived from this class.
     virtual ~CallBackObj() {};
};

#endif
