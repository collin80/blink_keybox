#ifndef BLINK_KB_H_
#define BLINK_KB_H_

#include <Arduino.h>
#include <due_canopen.h> //keybox connects over canopen link

#define DEFAULT_ID	0x20

class KEYBOX
{
public:
	KEYBOX(int); //canbus channel to use

	void begin(int speed = 125000, int id = DEFAULT_ID); //begin the comm, pass desired canbus speed - defaults to the default keybox speed
	bool isConnected(); //is a keybox responding to us?
	bool isRelayActive(int); //get status of relay (active = currently latched)
	void setRelayState(int, bool); //false = off, true = active/on/latched
	void setDeviceCANSpeed(int); //set a new comm speed. Updates the keybox with this new value
	void setKeyboxID(int id = 0x15); //that is the actual default ID they come with.
	int findKeyboxID(); //listen on the bus to find the keybox and return it's ID
	void loop();
	void onPDOReceive(CAN_FRAME* frame);
	void onSDORequest(SDO_FRAME* frame);
	void onSDOResponse(SDO_FRAME* frame);
private:
	CANOPEN *channel;
	int keyboxID; //which ID is the keybox on?
	bool relayState[12];
};

extern KEYBOX KeyBox0;
extern KEYBOX KeyBox1;

#endif