#include "Arduino.h"
#include "HBridge2013.h"

HBridge2014::HBridge2014() {
	IN1_PIN = IN2_PIN = PWM_PIN = 0;
	ATTACHED = false;
	BRAKE_ON_ZERO = true;
	DIRECTION = 1;
	state = 0;
}

void HBridge2014::attach(unsigned char in1pin, unsigned char in2pin, unsigned char pwmPin) {
	pinMode(in1pin, OUTPUT);
	pinMode(in2pin, OUTPUT);
	pinMode(pwmPin, OUTPUT);
	IN1_PIN = in1pin;
	IN2_PIN = in2pin;
	PWM_PIN = pwmPin;
	ATTACHED = true;
}

void HBridge2014::detach() {
	ATTACHED = false;
}

bool HBridge2014::attached() {
	return ATTACHED;
}

void HBridge2014::write(int newState) {
	if (ATTACHED) {
		state = constrain(newState, -255, 255);
		updatePins();
	}
}

int HBridge2014::read() {
	return state;
}

void HBridge2014::setBrakeOnZero(bool brakeOnZero) {
	if (ATTACHED)
		BRAKE_ON_ZERO = breakOnZero;
}

void HBridge2014::setFlippedDirection(bool flipped) {
	if (ATTACHED)
		DIRECTION = flipped ? -1 : 1;
}

signed char HBridge2014::toggleDirection() {
	if (ATTACHED)
		DIRECTION = 0 - DIRECTION;
	return DIRECTION;
}

void HBridge2014::updatePins() {
	if (state == 0) {
		analogWrite(PWM_PIN, 0);
		if (BRAKE_ON_ZERO) {
			digitalWrite(IN1_PIN, LOW);
			digitalWrite(IN2_PIN, LOW);
		}
		else {
			digitalWrite(IN1_PIN, HIGH);
			digitalWrite(IN2_PIN, HIGH);
		}
	}
	else {
		if (state * DIRECTION > 0) {
			digitalWrite(IN1_PIN, HIGH);
			digitalWrite(IN2_PIN, LOW);
		}
		else {
			digitalWrite(IN1_PIN, LOW);
			digitalWrite(IN2_PIN, HIGH);
		}
		analogWrite(PWM_PIN, abs(state));
	}
}
