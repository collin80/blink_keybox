#include "blink_keybox.h"

//trampoline functions to bounce back into a class member. Not as pretty or 
//proper as using a delegate scheme but nothing about delegates is that much fun
//in C++ so this is much easier to maintain.
void onPDOReceive0(CAN_FRAME *frame)
{
	KeyBox0.onPDOReceive(frame);
}

void onSDORxRequest0(SDO_FRAME *frame)
{
	KeyBox0.onSDORequest(frame);
}

void onSDORxResponse0(SDO_FRAME *frame)
{
	KeyBox0.onSDOResponse(frame);
}

void onPDOReceive1(CAN_FRAME *frame)
{
	KeyBox1.onPDOReceive(frame);
}

void onSDORxRequest1(SDO_FRAME *frame)
{
	KeyBox1.onSDORequest(frame);
}

void onSDORxResponse1(SDO_FRAME *frame)
{
	KeyBox1.onSDOResponse(frame);
}

KEYBOX::KEYBOX(int chan)
{
	if (chan == 0)
	{
		channel = &CanOpen0;
		channel->setPDOCallback(onPDOReceive0);
		channel->setSDOReqCallback(onSDORxRequest0);
		channel->setSDOReplyCallback(onSDORxResponse0);
	}
	else
	{
		channel = &CanOpen1;
		channel->setPDOCallback(onPDOReceive1);
		channel->setSDOReqCallback(onSDORxRequest1);
		channel->setSDOReplyCallback(onSDORxResponse1);
	}
	for (int x = 0; x < 12; x++) relayState[x] = false;
	keyboxID = 0x15; //the default ID
}

void KEYBOX::begin(int speed, int id)
{
	channel->setMasterMode();
	channel->begin(speed, id);
    
	channel->sendNodeReset(0); //reset all nodes
	delay(500);
	channel->sendNodeStart(0); //make all connected nodes go active
	delay(500);
}

bool KEYBOX::isConnected()
{
}

bool KEYBOX::isRelayActive(int relay)
{
	if (relay == 0) return false;
	if (relay > 12) return false;
	return relayState[relay - 1];	
}

void KEYBOX::setKeyboxID(int id)
{
	keyboxID = id;
}

//Not actually in existence yet...
int KEYBOX::findKeyboxID()
{
	return -1;
}

void KEYBOX::setRelayState(int relay, bool state)
{
	if (relay == 0) return;
	if (relay > 12) return;
	SDO_FRAME frame;
	frame.nodeID = keyboxID;
	frame.cmd = SDO_WRITE;
	frame.index = 0x2001;
	frame.subIndex = relay;
	frame.dataLength = 1;
	if (state)
		frame.data[0] = 1; //turn that relay on!
	else 
		frame.data[0] = 0; //turn that relay off!
	channel->sendSDORequest(&frame);
}

void KEYBOX::setDeviceCANSpeed(int speed)
{
}

void KEYBOX::loop()
{
	channel->loop();
}

//There shouldn't be PDO messages we need to process in this library...
void KEYBOX::onPDOReceive(CAN_FRAME* frame)
{
}

//There shouldn't be SDO requests to this library either
void KEYBOX::onSDORequest(SDO_FRAME* frame)
{
}

//This, however, we're going to get SDO responses.
void KEYBOX::onSDOResponse(SDO_FRAME* frame)
{
	if ((frame->index == 0x2001) && (frame->cmd == SDO_WRITEACK)) //response from relay setting
	{
		if (frame->data[0] == 1)
		{
			relayState[frame->subIndex] = true;
		}
		else 
			relayState[frame->subIndex] = false;
	}
}

KEYBOX KeyBox0(0);
KEYBOX KeyBox1(1);