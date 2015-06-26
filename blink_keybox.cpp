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
	if (!channel->isInitialized())
	{
		channel->setMasterMode();
		channel->begin(speed, 5); //hard code our address as 5
    
		channel->sendNodeStart(0); //make all connected nodes go active
		delay(500);
		channel->sendNodeStart(0); //make all connected nodes go active
	}
	keyboxID = id;
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
	unsigned char data[8];
	for (int y = 0; y < 8; y++) data[y] = 0;

	relayState[relay-1] = state;

	for (int x = 0; x < 12; x++)
	{
		if (relayState[x]) data[x / 8] |= (1 << (x % 8));
	}
	channel->sendPDOMessage(0x200 + keyboxID, 8, data);		
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