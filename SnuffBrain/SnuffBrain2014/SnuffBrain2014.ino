#include <HBridge2013.h>
#include <Servo.h>
#include <EEPROM.h>

#define BASE_POT_PIN 10
#define SHOULDER_POT_PIN 9
#define ELBOW_POT_PIN 8

#define HBRIDGE1_IN1_PIN 33
#define HBRIDGE1_IN2_PIN 37
#define HBRIDGE1_PWM_PIN 4

#define HBRIDGE2_IN1_PIN 43
#define HBRIDGE2_IN2_PIN 39
#define HBRIDGE2_PWM_PIN 7

#define HBRIDGE3_IN1_PIN 31
#define HBRIDGE3_IN2_PIN 35
#define HBRIDGE3_PWM_PIN 5

#define HBRIDGE4_IN1_PIN 45
#define HBRIDGE4_IN2_PIN 41
#define HBRIDGE4_PWM_PIN 6


#define MAX_ACTUATOR_PWM_CHANGE 3
#define MAX_SERVO_POS_CHANGE 10


#define OUT_ERROR 'E'
#define OUT_PACKET_CONFIRMED 'P'
#define OUT_PACKET_PROCESSED 'p'
#define OUT_PACKET_DROPPED 'D'
#define OUT_MESSAGE_UNKNOWN '?'
#define T_MAX_PACKET_WAIT 50

/////////////////////////
boolean packetInProgress = false;
boolean packetReceived = false;
char bytesNeeded = 5;
unsigned char packetID = 0;


HBridge2013 baseHB, shoulderHB, elbowHB;

unsigned int baseMin = 200;
unsigned int baseMax = 800;
unsigned int shoulderMin = 200;
unsigned int shoulderMax = 800;
unsigned int elbowMin = 200;
unsigned int elbowMax = 800;

unsigned int basePotValue, shoulderPotValue, elbowPotValue;
unsigned int baseCurrentPos, shoulderCurrentPos, elbowCurrentPos;
unsigned int baseOldPos, shoulderOldPos, elbowOldPos;
int basePosChange, shoulderPosChange, elbowPosChange;
unsigned int baseTargetPos, shoulderTargetPos, elbowTargetPos;
int baseTargetDistance, shoulderTargetDistance, elbowTargetDistance;

Servo armServo, wristServo, palmServo, gripperServo;
unsigned int armTargetPos, wristTargetPos, palmTargetPos, gripperTargetPos;
unsigned int prevArmTargetPos, prevWristTargetPos, prevPalmTargetPos, prevGripperTargetPos;


unsigned long t = 0;
unsigned long t_packet = 0;

////////////////////////
void setup() {
  Serial.begin(38400);
  
  baseTargetPos = 120;
  shoulderTargetPos = 120;
  elbowTargetPos = 120;

  baseHB.attach(HBRIDGE1_IN1_PIN, HBRIDGE1_IN2_PIN, HBRIDGE1_PWM_PIN);
  shoulderHB.attach(HBRIDGE2_IN1_PIN, HBRIDGE2_IN2_PIN, HBRIDGE2_PWM_PIN);
  elbowHB.attach(HBRIDGE3_IN1_PIN, HBRIDGE3_IN2_PIN, HBRIDGE3_PWM_PIN);
  
  armTargetPos = 1500;
  wristTargetPos = 1500;
  palmTargetPos = 1500;
  gripperTargetPos = 1500;
  armServo.attach(9);
  wristServo.attach(11);
  palmServo.attach(8);
  gripperServo.attach(10);
}

////////////////////////
void loop() {
  t = millis();
  receiveData();
  updateAllActuators();
  updateAllServos();
}

void updateAllActuators() {
  updateBaseActuator();
  updateShoulderActuator();
  updateElbowActuator();
}

void updateBaseActuator() {
  baseOldPos = baseCurrentPos;
  basePotValue = analogRead(BASE_POT_PIN);
  baseCurrentPos = map(constrain(basePotValue, baseMin, baseMax), baseMin, baseMax, 0, 255);
  basePosChange = baseCurrentPos - baseOldPos;
  baseTargetDistance = baseTargetPos - baseCurrentPos;
  int newPWM, oldPWM = baseHB.read();
  if (baseTargetDistance > 5 || baseTargetDistance < -5)
    newPWM = constrain(map(baseTargetDistance, -50, 50, -255, 255), -255, 255);
  else
    newPWM = 0;
  if (newPWM - oldPWM > MAX_ACTUATOR_PWM_CHANGE)
    newPWM = oldPWM + MAX_ACTUATOR_PWM_CHANGE;
  else if (newPWM - oldPWM < -MAX_ACTUATOR_PWM_CHANGE)
    newPWM = oldPWM - MAX_ACTUATOR_PWM_CHANGE;
  baseHB.write(newPWM);
}

void updateShoulderActuator() {
  shoulderOldPos = shoulderCurrentPos;
  shoulderPotValue = analogRead(SHOULDER_POT_PIN);
  shoulderCurrentPos = map(constrain(shoulderPotValue, shoulderMin, shoulderMax), shoulderMin, shoulderMax, 0, 255);
  shoulderPosChange = shoulderCurrentPos - shoulderOldPos;
  shoulderTargetDistance = shoulderTargetPos - shoulderCurrentPos;
  int newPWM, oldPWM = shoulderHB.read();
  if (shoulderTargetDistance > 5 || shoulderTargetDistance < -5)
    newPWM = constrain(map(shoulderTargetDistance, -50, 50, -255, 255), -255, 255);
  else
    newPWM = 0;
  if (newPWM - oldPWM > MAX_ACTUATOR_PWM_CHANGE)
    newPWM = oldPWM + MAX_ACTUATOR_PWM_CHANGE;
  else if (newPWM - oldPWM < -MAX_ACTUATOR_PWM_CHANGE)
    newPWM = oldPWM - MAX_ACTUATOR_PWM_CHANGE;
  shoulderHB.write(newPWM);
}

void updateElbowActuator() {
  elbowOldPos = elbowCurrentPos;
  elbowPotValue = analogRead(ELBOW_POT_PIN);
  elbowCurrentPos = map(constrain(elbowPotValue, elbowMin, elbowMax), elbowMin, elbowMax, 0, 255);
  elbowPosChange = elbowCurrentPos - elbowOldPos;
  elbowTargetDistance = elbowTargetPos - elbowCurrentPos;
  int newPWM, oldPWM = elbowHB.read();
  if (elbowTargetDistance > 5 || elbowTargetDistance < -5)
    newPWM = constrain(map(elbowTargetDistance, -50, 50, -255, 255), -255, 255);
  else
    newPWM = 0;
  if (newPWM - oldPWM > MAX_ACTUATOR_PWM_CHANGE)
    newPWM = oldPWM + MAX_ACTUATOR_PWM_CHANGE;
  else if (newPWM - oldPWM < -MAX_ACTUATOR_PWM_CHANGE)
    newPWM = oldPWM - MAX_ACTUATOR_PWM_CHANGE;
  elbowHB.write(newPWM);
}

void updateAllServos() {
  updateArmServo();
  updateWristServo();
  updatePalmServo();
  updateGripperServo();
}

void updateArmServo() {
  int newPOS, oldPOS = prevArmTargetPos;
  newPOS = map(armTargetPos, 0, 255, 900, 2100);
  if (newPOS - oldPOS > MAX_SERVO_POS_CHANGE)
    newPOS = oldPOS + MAX_SERVO_POS_CHANGE;
  else if (newPOS - oldPOS < -MAX_SERVO_POS_CHANGE)
    newPOS = oldPOS - MAX_SERVO_POS_CHANGE;
  armServo.writeMicroseconds(newPOS);
  prevArmTargetPos = newPOS;
}

void updateWristServo() {
  int newPOS, oldPOS = prevWristTargetPos;
  newPOS = map(wristTargetPos, 0, 255, 900, 2100);
  if (newPOS - oldPOS > MAX_SERVO_POS_CHANGE)
    newPOS = oldPOS + MAX_SERVO_POS_CHANGE;
  else if (newPOS - oldPOS < -MAX_SERVO_POS_CHANGE)
    newPOS = oldPOS - MAX_SERVO_POS_CHANGE;
  wristServo.writeMicroseconds(newPOS);
  prevWristTargetPos = newPOS;
}

void updatePalmServo() {
  int newPOS, oldPOS = prevPalmTargetPos;
  newPOS = map(palmTargetPos, 0, 255, 900, 2100);
  if (newPOS - oldPOS > MAX_SERVO_POS_CHANGE)
    newPOS = oldPOS + MAX_SERVO_POS_CHANGE;
  else if (newPOS - oldPOS < -MAX_SERVO_POS_CHANGE)
    newPOS = oldPOS - MAX_SERVO_POS_CHANGE;
  palmServo.writeMicroseconds(newPOS);
  prevPalmTargetPos = newPOS;
}

void updateGripperServo() {
  int newPOS, oldPOS = prevGripperTargetPos;
  newPOS = map(gripperTargetPos, 0, 255, 900, 2100);
  if (newPOS - oldPOS > MAX_SERVO_POS_CHANGE)
    newPOS = oldPOS + MAX_SERVO_POS_CHANGE;
  else if (newPOS - oldPOS < -MAX_SERVO_POS_CHANGE)
    newPOS = oldPOS - MAX_SERVO_POS_CHANGE;
  gripperServo.writeMicroseconds(newPOS);
  prevGripperTargetPos = newPOS;
}

////////////////////////
void receiveData() {
  if (packetInProgress) {
    if (t - t_packet > T_MAX_PACKET_WAIT) {
      Serial.write(OUT_PACKET_DROPPED);
      Serial.write(packetID);
      bytesNeeded = 0;
    }
  }
  while (Serial.available() >= bytesNeeded) {
    if (bytesNeeded <= 0) {
      packetInProgress = false;
      packetReceived = false;
      bytesNeeded = 5;
    }
    else if (packetInProgress) {
      if (packetReceived == false) {
        Serial.write(OUT_PACKET_CONFIRMED);
        Serial.write(packetID);
        packetReceived = true;
      }
      bytesNeeded--;
      char consumedBytes = processMessage(Serial.read());
      if (consumedBytes == -1) {
        Serial.write(OUT_PACKET_DROPPED);
        Serial.write(packetID);
        while(bytesNeeded-- > 0)
          Serial.read();
      }
      else {
        bytesNeeded -= consumedBytes;
        if (bytesNeeded <= 0) {
          Serial.write(OUT_PACKET_PROCESSED);
          Serial.write(packetID);
        }
      }
    }
    else
      if (Serial.read() == 'U')
        if (Serial.read() == 'I')
          if (Serial.read() == 'C') {
            packetID = Serial.read();
            bytesNeeded = Serial.read();
            if (bytesNeeded > 0) {
              packetInProgress = true;
              t_packet = t;
            }
            else
              Serial.print(" FUCK YOU ");
          }
  }
}

//////////////////////////
char processMessage(unsigned char messageID) {
  char consumedData = 0;
  switch (messageID) {
    case 'V': {
      baseTargetPos = Serial.read();
      shoulderTargetPos = Serial.read();
      elbowTargetPos = Serial.read();
      armTargetPos = Serial.read();
      wristTargetPos = Serial.read();
      palmTargetPos = Serial.read();
      gripperTargetPos = Serial.read();
      if (Serial.read() > 0) {
        baseTargetPos = baseCurrentPos;
        shoulderTargetPos = shoulderCurrentPos;
        elbowTargetPos = elbowCurrentPos;
        armTargetPos = prevArmTargetPos;
        wristTargetPos = prevWristTargetPos;
        palmTargetPos = prevPalmTargetPos;
        gripperTargetPos = prevGripperTargetPos;
      }
      consumedData += 8;
    } break;
    case 'R': {
      Serial.write((byte)(analogRead(BASE_POT_PIN) >> 2));
      Serial.write((byte)(analogRead(SHOULDER_POT_PIN) >> 2));
      Serial.write((byte)(analogRead(ELBOW_POT_PIN) >> 2));
    } break;
    case 'O': {
      Serial.write((byte)(basePotValue >> 2));
      Serial.write((byte)(shoulderPotValue >> 2));
      Serial.write((byte)(elbowPotValue >> 2));
    } break;
    case 'C': {
      Serial.write((byte)baseCurrentPos);
      Serial.write((byte)shoulderCurrentPos);
      Serial.write((byte)elbowCurrentPos);
    } break;
    case 'T': {
      Serial.write((byte)baseTargetPos);
      Serial.write((byte)shoulderTargetPos);
      Serial.write((byte)elbowTargetPos);
    } break;
    case 'W': {
      Serial.write((byte)(baseHB.isFlipped() ? 'A' : 'B'));
      Serial.write((byte)abs(baseHB.read()));
      Serial.write((byte)(shoulderHB.isFlipped() ? 'A' : 'B'));
      Serial.write((byte)abs(shoulderHB.read()));
      Serial.write((byte)(elbowHB.isFlipped() ? 'A' : 'B'));
      Serial.write((byte)abs(elbowHB.read()));
    } break;
    case 'B': {
      baseHB.setFlippedDirection(true);
      Serial.write('B');
    } break;
    case 'S': {
      shoulderHB.setFlippedDirection(true);
      Serial.write('S');
    } break;
    case 'E': {
      elbowHB.setFlippedDirection(true);
      Serial.write('E');
    } break;
    case 'b': {
      baseHB.setFlippedDirection(true);
      Serial.write('b');
    } break;
    case 's': {
      shoulderHB.setFlippedDirection(true);
      Serial.write('s');
    } break;
    case 'e': {
      elbowHB.setFlippedDirection(true);
      Serial.write('e');
    } break;
    default:
      Serial.write(messageID);
      Serial.write(1);
      Serial.write(OUT_MESSAGE_UNKNOWN);
      return -1;
  }
  return consumedData;
}
