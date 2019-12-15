// network.cc 
//	Routines to simulate a network interface, using UNIX sockets
//	to deliver packets between multiple invocations of nachos.
//
//  DO NOT CHANGE -- part of the machine emulation
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "network.h"
#include "main.h"

//-----------------------------------------------------------------------
// NetworkInput::NetworkInput
// 	Initialize the simulation for the network input
//
//   	"toCall" is the interrupt handler to call when packet arrives
//-----------------------------------------------------------------------

NetworkInput::NetworkInput(CallBackObj *toCall)
{
    // set up the stuff to emulate asynchronous interrupts
    callWhenAvail = toCall;
    packetAvail = FALSE;
    inHdr.length = 0;
    
    sock = OpenSocket();
    sprintf(sockName, "SOCKET_%d", kernel->hostName);
    AssignNameToSocket(sockName, sock);		 // Bind socket to a filename 
						 // in the current directory.

    // start polling for incoming packets
    kernel->interrupt->Schedule(this, NetworkTime, NetworkRecvInt);
}

//-----------------------------------------------------------------------
// NetworkInput::NetworkInput
// 	Deallocate the simulation for the network input
//		(basically, deallocate the input mailbox)
//-----------------------------------------------------------------------

NetworkInput::~NetworkInput()
{
    CloseSocket(sock);
    DeAssignNameToSocket(sockName);
}

//-----------------------------------------------------------------------
// NetworkInput::CallBack
//	Simulator calls this when a packet may be available to
//	be read in from the simulated network.
//
//      First check to make sure packet is available & there's space to
//	pull it in.  Then invoke the "callBack" registered by whoever 
//	wants the packet.
//-----------------------------------------------------------------------

void
NetworkInput::CallBack()
{
    // schedule the next time to poll for a packet
    kernel->interrupt->Schedule(this, NetworkTime, NetworkRecvInt);

    if (inHdr.length != 0) 	// do nothing if packet is already buffered
	return;		
    if (!PollSocket(sock)) 	// do nothing if no packet to be read
	return;

    // otherwise, read packet in
    char *buffer = new char[MaxWireSize];
    ReadFromSocket(sock, buffer, MaxWireSize);

    // divide packet into header and data
    inHdr = *(PacketHeader *)buffer;
    ASSERT((inHdr.to == kernel->hostName) && (inHdr.length <= MaxPacketSize));
    bcopy(buffer + sizeof(PacketHeader), inbox, inHdr.length);
    delete [] buffer ;

    DEBUG(dbgNet, "Network received packet from " << inHdr.from << ", length " << inHdr.length);
    kernel->stats->numPacketsRecvd++;

    // tell post office that the packet has arrived
    callWhenAvail->CallBack();
}

//-----------------------------------------------------------------------
// NetworkInput::Receive
// 	Read a packet, if one is buffered
//-----------------------------------------------------------------------

PacketHeader
NetworkInput::Receive(char* data)
{
    PacketHeader hdr = inHdr;

    inHdr.length = 0;
    if (hdr.length != 0) {
    	bcopy(inbox, data, hdr.length);
    }
    return hdr;
}

//-----------------------------------------------------------------------
// NetworkOutput::NetworkOutput
// 	Initialize the simulation for sending network packets
//
//   	"reliability" says whether we drop packets to emulate unreliable links
//   	"toCall" is the interrupt handler to call when next packet can be sent
//-----------------------------------------------------------------------

NetworkOutput::NetworkOutput(double reliability, CallBackObj *toCall)
{
    if (reliability < 0) chanceToWork = 0;
    else if (reliability > 1) chanceToWork = 1;
    else chanceToWork = reliability;

    // set up the stuff to emulate asynchronous interrupts
    callWhenDone = toCall;
    sendBusy = FALSE;
    sock = OpenSocket();
}

//-----------------------------------------------------------------------
// NetworkOutput::~NetworkOutput
// 	Deallocate the simulation for sending network packets
//-----------------------------------------------------------------------

NetworkOutput::~NetworkOutput()
{
    CloseSocket(sock);
}

//-----------------------------------------------------------------------
// NetworkOutput::CallBack
// 	Called by simulator when another packet can be sent.
//-----------------------------------------------------------------------

void
NetworkOutput::CallBack()
{
    sendBusy = FALSE;
    kernel->stats->numPacketsSent++;
    callWhenDone->CallBack();
}

//-----------------------------------------------------------------------
// NetworkOutput::Send
// 	Send a packet into the simulated network, to the destination in hdr.
// 	Concatenate hdr and data, and schedule an interrupt to tell the user 
// 	when the next packet can be sent 
//
// 	Note we always pad out a packet to MaxWireSize before putting it into
// 	the socket, because it's simpler at the receive end.
//-----------------------------------------------------------------------

void
NetworkOutput::Send(PacketHeader hdr, char* data)
{
    char toName[32];

    sprintf(toName, "SOCKET_%d", (int)hdr.to);
    
    ASSERT((sendBusy == FALSE) && (hdr.length > 0) && 
	(hdr.length <= MaxPacketSize) && (hdr.from == kernel->hostName));
    DEBUG(dbgNet, "Sending to addr " << hdr.to << ", length " << hdr.length);

    kernel->interrupt->Schedule(this, NetworkTime, NetworkSendInt);

    if (RandomNumber() % 100 >= chanceToWork * 100) { // emulate a lost packet
	DEBUG(dbgNet, "oops, lost it!");
	return;
    }

    // concatenate hdr and data into a single buffer, and send it out
    char *buffer = new char[MaxWireSize];
    *(PacketHeader *)buffer = hdr;
    bcopy(data, buffer + sizeof(PacketHeader), hdr.length);
    SendToSocket(sock, buffer, MaxWireSize, toName);
    delete [] buffer;
}
