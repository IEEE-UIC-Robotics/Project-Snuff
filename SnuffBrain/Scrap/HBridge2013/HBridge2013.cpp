#include "Arduino.h"
#include "HBridge2013.h"

HBridge2014::HBridge2014() {
	IN1_PIN = IN2_PIN = PWM_PIN = 0;
	ATTACHED = false;
	BRAKE_ON_ZERO = false;
	FLIPPED_DIRECTION = false;
	state = 0;
}

void HBridge2014::attach(unsigned char in1pin, unsigned char in2pin, unsigned char pwmPin) {
	IN1_PIN = in1pin;
	IN2_PIN = in2pin;
	PWM_PIN = pwmPin;
	pinMode(IN1_PIN, OUTPUT);
	pinMode(IN2_PIN, OUTPUT);
	pinMode(PWM_PIN, OUTPUT);
	ATTACHED = true;
}

void HBridge2014::detach() {
	ATTACHED = false;
}

bool HBridge2014::isAttached() {
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
		FLIPPED_DIRECTION = flipped;
}

boolean HBridge2014::toggleDirection() {
	if (ATTACHED)
		FLIPPED_DIRECTION = !FLIPPED_DIRECTION;
	return FLIPPED_DIRECTION;
}

void HBridge2014::updatePins() {
	if (state > 0) {
		if (FLIPPED_DIRECTION) {
		analogWrite(PWM_PIN, BRAKE_ON_ZERO ? 255 : 0);
		digitalWrite(IN1_PIN, BRAKE_ON_ZERO);
		digitalWrite(IN2_PIN, BRAKE_ON_ZERO);
	else {
		analogWrite(PWM_PIN, BRAKE_ON_ZERO ? 255 : 0);
		digitalWrite(IN1_PIN, BRAKE_ON_ZERO);
		digitalWrite(IN2_PIN, BRAKE_ON_ZERO);
	}
	}
	else if (state < 0) {

	}
	else {
		analogWrite(PWM_PIN, BRAKE_ON_ZERO ? 255 : 0);
		digitalWrite(IN1_PIN, BRAKE_ON_ZERO);
		digitalWrite(IN2_PIN, BRAKE_ON_ZERO);
	}
	else if (FLIPPED_DIRECTION) {
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
