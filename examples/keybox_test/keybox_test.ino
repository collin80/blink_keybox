//Sets up to become the CANOpen master node on CAN0 and control a Blink Keybox
#include "variant.h"'
#include <due_can.h>
#include <due_canopen.h>
#include <blink_keybox.h>

//Leave defined if you use native port, comment if using programming port
#define Serial SerialUSB

void setup()
{

	Serial.begin(115200);
	KeyBox0.begin(125000, 0x20);
}

void loop(){
	static uint8_t which;
	static bool state;
	
	which++;
	if (which == 12)
	{
		which = 0;
		state = !state;
	}
	
	KeyBox0.loop();
	KeyBox0.setRelayState(which + 1, state);
	delay(200);
}


