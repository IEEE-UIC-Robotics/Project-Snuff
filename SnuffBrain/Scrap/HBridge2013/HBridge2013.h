#ifndef HBridge2014_h
#define HBridge2014_h

#include "Arduino.h"

class HBridge2014
{
public:
	HBridge2014();
	void attach(unsigned char, unsigned char, unsigned char);
	void detach();
	bool attached();
	void write(int);
	int read();
	void setBrakeOnZero(bool);
	void setFlippedDirection(bool);
	signed char toggleDirection();
	
private:
	void updatePins();
	
	unsigned char IN1_PIN;
	unsigned char IN2_PIN;
	unsigned char PWM_PIN;
	
	bool ATTACHED;
	bool BRAKE_ON_ZERO;
	signed char DIRECTION;
	
	int state;
};

#endif
