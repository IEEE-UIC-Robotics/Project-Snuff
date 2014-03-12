#ifndef Actuator2014_h
#define Actuator2014_h

#include "HBridge2013.h"
#include "Arduino.h"

class Actuator2014 {
public:
	Actuator2014(HBridge2013*, char);
	void loopCycle();
	void loopCycle(int);
	void goTo(int);
	void setP(int);
	
private:
	HBridge2013* HB;
	char POT_PIN;
	
	int position;
	int target;
	int prevPosition;
	int prevTarget;
	int speed;
	int error;
	int prevError;
	int P;
	int loopCycleTimestamp;
	int dt;
};

#endif
