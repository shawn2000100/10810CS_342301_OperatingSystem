// post.cc 
// 	Routines to deliver incoming network messages to the correct
//	"address" -- a mailbox, or a holding area for incoming messages.
//	This module operates just like the US postal service (in other
//	words, it works, but it's slow, and you can't really be sure if
//	your mail really got through!).
//
//	Note that once we prepend the MailHdr to the outgoing message data,
//	the combination (MailHdr plus data) looks like "data" to the Network 
//	device.
//
// 	The implementation synchronizes incoming messages with threads
//	waiting for those messages.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "post.h"

//----------------------------------------------------------------------
// Mail::Mail
//      Initialize a single mail message, by concatenating the headers to
//	the data.
//
//	"pktH" -- source, destination machine ID's
//	"mailH" -- source, destination mailbox ID's
//	"data" -- payload data
//----------------------------------------------------------------------

Mail::Mail(PacketHeader pktH, MailHeader mailH, char *msgData)
{
    ASSERT(mailH.length <= MaxMailSize);

    pktHdr = pktH;
    mailHdr = mailH;
    bcopy(msgData, data, mailHdr.length);
}

//----------------------------------------------------------------------
// MailBox::MailBox
//      Initialize a single mail box within the post office, so that it
//	can receive incoming messages.
//
//	Just initialize a list of messages, representing the mailbox.
//----------------------------------------------------------------------


MailBox::MailBox()
{ 
    messages = new SynchList<Mail *>(); 
}

//----------------------------------------------------------------------
// MailBox::~MailBox
//      De-allocate a single mail box within the post office.
//
//	Just delete the mailbox, and throw away all the queued messages 
//	in the mailbox.
//----------------------------------------------------------------------

MailBox::~MailBox()
{ 
    delete messages; 
}

//----------------------------------------------------------------------
// PrintHeader
// 	Print the message header -- the destination machine ID and mailbox
//	#, source machine ID and mailbox #, and message length.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//----------------------------------------------------------------------

static void 
PrintHeader(PacketHeader pktHdr, MailHeader mailHdr)
{
    cout << "From (" << pktHdr.from << ", " << mailHdr.from << ") to (" << 
	pktHdr.to << ", " << mailHdr.to << ") bytes " << mailHdr.length << "\n";
}

//----------------------------------------------------------------------
// MailBox::Put
// 	Add a message to the mailbox.  If anyone is waiting for message
//	arrival, wake them up!
//
//	We need to reconstruct the Mail message (by concatenating the headers
//	to the data), to simplify queueing the message on the SynchList.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//	"data" -- payload message data
//----------------------------------------------------------------------

void 
MailBox::Put(PacketHeader pktHdr, MailHeader mailHdr, char *data)
{ 
    Mail *mail = new Mail(pktHdr, mailHdr, data); 

    messages->Append(mail);		// put on the end of the list of 
					// arrived messages, and wake up 
					// any waiters
}

//----------------------------------------------------------------------
// MailBox::Get
// 	Get a message from a mailbox, parsing it into the packet header,
//	mailbox header, and data. 
//
//	The calling thread waits if there are no messages in the mailbox.
//
//	"pktHdr" -- address to put: source, destination machine ID's
//	"mailHdr" -- address to put: source, destination mailbox ID's
//	"data" -- address to put: payload message data
//----------------------------------------------------------------------

void 
MailBox::Get(PacketHeader *pktHdr, MailHeader *mailHdr, char *data) 
{ 
    DEBUG(dbgNet, "Waiting for mail in mailbox");
    Mail *mail = messages->RemoveFront();	// remove message from list;
						// will wait if list is empty

    *pktHdr = mail->pktHdr;
    *mailHdr = mail->mailHdr;
    if (debug->IsEnabled('n')) {
	cout << "Got mail from mailbox: ";
	PrintHeader(*pktHdr, *mailHdr);
    }
    bcopy(mail->data, data, mail->mailHdr.length);
					// copy the message data into
					// the caller's buffer
    delete mail;			// we've copied out the stuff we
					// need, we can now discard the message
}

//----------------------------------------------------------------------
// PostOfficeInput::PostOfficeInput
// 	Initialize the post office input queues as a collection of mailboxes.
//	Also initialize the network device, to allow post offices
//	on different machines to deliver messages to one another.
//
//      We use a separate thread "the postal worker" to wait for messages 
//	to arrive, and deliver them to the correct mailbox.  Note that
//	delivering messages to the mailboxes can't be done directly
//	by the interrupt handlers, because it requires a Lock.
//
//	"nBoxes" is the number of mail boxes in this Post Office
//----------------------------------------------------------------------

PostOfficeInput::PostOfficeInput(int nBoxes)
{
    messageAvailable = new Semaphore("message available", 0);

    numBoxes = nBoxes;
    boxes = new MailBox[nBoxes];

    network = new NetworkInput(this);

    Thread *t = new Thread("postal worker", 1);

    t->Fork(PostOfficeInput::PostalDelivery, this);
}

//----------------------------------------------------------------------
// PostOfficeInput::~PostOfficeInput
// 	De-allocate the post office data structures.
//	
//	Since the postal helper is waiting on the "messageAvail" semaphore,
//	we don't deallocate it!  This leaves garbage lying about,
//	but the alternative is worse!
//----------------------------------------------------------------------

PostOfficeInput::~PostOfficeInput()
{
    delete network;
    delete [] boxes;
}

//----------------------------------------------------------------------
// PostOffice::PostalDelivery
// 	Wait for incoming messages, and put them in the right mailbox.
//
//      Incoming messages have had the PacketHeader stripped off,
//	but the MailHeader is still tacked on the front of the data.
//----------------------------------------------------------------------

void
PostOfficeInput::PostalDelivery(void* data)
{
    PostOfficeInput* _this = (PostOfficeInput*)data;
    PacketHeader pktHdr;
    MailHeader mailHdr;
    char *buffer = new char[MaxPacketSize];

    for (;;) {
        // first, wait for a message
        _this->messageAvailable->P();	
        pktHdr = _this->network->Receive(buffer);

        mailHdr = *(MailHeader *)buffer;
        if (debug->IsEnabled('n')) {
	    cout << "Putting mail into mailbox: ";
	    PrintHeader(pktHdr, mailHdr);
        }

	// check that arriving message is legal!
	ASSERT(0 <= mailHdr.to && mailHdr.to < _this->numBoxes);
	ASSERT(mailHdr.length <= MaxMailSize);

	// put into mailbox
        _this->boxes[mailHdr.to].Put(pktHdr, mailHdr, buffer + sizeof(MailHeader));
    }
}

//----------------------------------------------------------------------
// PostOfficeInput::Receive
// 	Retrieve a message from a specific box if one is available, 
//	otherwise wait for a message to arrive in the box.
//
//	Note that the MailHeader + data looks just like normal payload
//	data to the Network.
//
//
//	"box" -- mailbox ID in which to look for message
//	"pktHdr" -- address to put: source, destination machine ID's
//	"mailHdr" -- address to put: source, destination mailbox ID's
//	"data" -- address to put: payload message data
//----------------------------------------------------------------------

void
PostOfficeInput::Receive(int box, PacketHeader *pktHdr, 
				MailHeader *mailHdr, char* data)
{
    ASSERT((box >= 0) && (box < numBoxes));

    boxes[box].Get(pktHdr, mailHdr, data);
    ASSERT(mailHdr->length <= MaxMailSize);
}

//----------------------------------------------------------------------
// PostOffice::CallBack
// 	Interrupt handler, called when a packet arrives from the network.
//
//	Signal the PostalDelivery routine that it is time to get to work!
//----------------------------------------------------------------------

void
PostOfficeInput::CallBack()
{ 
    messageAvailable->V(); 
}

//----------------------------------------------------------------------
// PostOfficeOutput::PostOfficeOutput
// 	Initialize the post office output queue.
//
//	"reliability" is the probability that a network packet will
//	  be delivered (e.g., reliability = 1 means the network never
//	  drops any packets; reliability = 0 means the network never
//	  delivers any packets)
//----------------------------------------------------------------------

PostOfficeOutput::PostOfficeOutput(double reliability)
{
    messageSent = new Semaphore("message sent", 0);
    sendLock = new Lock("message send lock");

    network = new NetworkOutput(reliability, this);
}

//----------------------------------------------------------------------
// PostOfficeOutput::~PostOfficeOutput
// 	De-allocate the post office data structures.
//----------------------------------------------------------------------

PostOfficeOutput::~PostOfficeOutput()
{
    delete network;
    delete messageSent;
    delete sendLock;
}

//----------------------------------------------------------------------
// PostOfficeOutput::Send
// 	Concatenate the MailHeader to the front of the data, and pass 
//	the result to the Network for delivery to the destination machine.
//
//	Note that the MailHeader + data looks just like normal payload
//	data to the Network.
//
//	"pktHdr" -- source, destination machine ID's
//	"mailHdr" -- source, destination mailbox ID's
//	"data" -- payload message data
//----------------------------------------------------------------------

void
PostOfficeOutput::Send(PacketHeader pktHdr, MailHeader mailHdr, char* data)
{
    char* buffer = new char[MaxPacketSize];	// space to hold concatenated
						// mailHdr + data

    if (debug->IsEnabled('n')) {
	cout << "Post send: ";
	PrintHeader(pktHdr, mailHdr);
    }
    ASSERT(mailHdr.length <= MaxMailSize);
    ASSERT(0 <= mailHdr.to);
    
    // fill in pktHdr, for the Network layer
    pktHdr.from = kernel->hostName;
    pktHdr.length = mailHdr.length + sizeof(MailHeader);

    // concatenate MailHeader and data
    bcopy((char *)&mailHdr, buffer, sizeof(MailHeader));
    bcopy(data, buffer + sizeof(MailHeader), mailHdr.length);

    sendLock->Acquire();   		// only one message can be sent
					// to the network at any one time
    network->Send(pktHdr, buffer);
    messageSent->P();			// wait for interrupt to tell us
					// ok to send the next message
    sendLock->Release();

    delete [] buffer;			// we've sent the message, so
					// we can delete our buffer
}

//----------------------------------------------------------------------
// PostOfficeOutput::CallBack
// 	Interrupt handler, called when the next packet can be put onto the 
//	network.
//
//	Called even if the previous packet was dropped.
//----------------------------------------------------------------------

void 
PostOfficeOutput::CallBack()
{ 
    messageSent->V();
}

