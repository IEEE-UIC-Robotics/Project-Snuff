#include "Arduino.h"
#include "Actuator2014.h"
#include "HBridge2013.h"

Actuator2014::Actuator2014(HBridge2013* hb, char potpin) {
	HB = hb;
	POT_PIN = potpin;
	position = 0;
	target = 0;
	prevPosition = 0;
	speed = 0;
	error = 0;
	prevError = 0;
	P = 5;
	loopCycleTimestamp = 0;
	dt = 0;
}

void Actuator2014::loopCycle() {
	loopCycle(millis() - loopCycleTimestamp);
}

void Actuator2014::loopCycle(int deltaTime) {
	dt = deltaTime;
	loopCycleTimestamp += dt;
	
	prevPosition = position;
	position = analogRead(POT_PIN);
	speed = (position - prevPosition) * 1000 / dt;
	prevError = error;
	error = target - position;
	
	HB->write(error * 5);
}

void Actuator2014::goTo(int newTarget) {
	target = newTarget;
}

void Actuator2014::setP(int newP) {
	P = newP;
}