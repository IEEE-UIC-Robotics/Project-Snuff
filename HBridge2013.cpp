#include "HBridge2013.h"
#include "Arduino.h"

HBridge2013::HBridge2013() {
  IN1_PIN = IN2_PIN = PWM_PIN = 0;
  ATTACHED = false;
  BRAKE_ON_ZERO = true;
  DIRECTION = 1;
  state = 0;
}

void HBridge2013::attach(const byte pin1, const byte pin2, const byte pinPWM) {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pinPWM, OUTPUT);
  IN1_PIN = pin1;
  IN2_PIN = pin2;
  PWM_PIN = pinPWM;
  ATTACHED = true;
}

void HBridge2013::detach() {
  ATTACHED = false;
}

boolean HBridge2013::attached() {
  return ATTACHED;
}

void HBridge2013::write(int newState) {
  state = constrain(newState, -255, 255);
  if (ATTACHED)
    updatePins();
}

int HBridge2013::read() {
  return state;
}

void HBridge2013::setBrakeOnZero(boolean newParam) {
  BRAKE_ON_ZERO = newParam;
}

void HBridge2013::setFlippedDirection(boolean flipped) {
  if (flipped)
    DIRECTION = -1;
  else
    DIRECTION = 1;
}

void HBridge2013::updatePins() {
  if (state * DIRECTION > 0) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
    analogWrite(PWM_PIN, abs(state));
  }
  else if (state == 0) {
    analogWrite(PWM_PIN, 0);
    if (BRAKE_ON_ZERO) {
      digitalWrite(IN1_PIN, LOW);
      digitalWrite(IN2_PIN, LOW); }
    else {
      digitalWrite(IN1_PIN, HIGH);
      digitalWrite(IN2_PIN, HIGH); }
  }
  else {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
    analogWrite(PWM_PIN, abs(state));
  }
}
