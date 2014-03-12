//==============================================================================
//============================================================[ LIBRARIES ]=====
  #include <HBridge2013.h>
  #include <Actuator2014.h>
  #include <Servo.h>
  #include <EEPROM.h>

//==============================================================================
//========================================================[ SOMETHING ]=====
#define BASE_POT_PIN 10
#define SHOULDER_POT_PIN 9
#define ELBOW_POT_PIN 8

#define HBRIDGE1_IN1_PIN 37
#define HBRIDGE1_IN2_PIN 33
#define HBRIDGE1_PWM_PIN 4

#define HBRIDGE2_IN1_PIN 39
#define HBRIDGE2_IN2_PIN 43
#define HBRIDGE2_PWM_PIN 7

#define HBRIDGE3_IN1_PIN 35
#define HBRIDGE3_IN2_PIN 31
#define HBRIDGE3_PWM_PIN 5

#define HBRIDGE4_IN1_PIN 41
#define HBRIDGE4_IN2_PIN 45
#define HBRIDGE4_PWM_PIN 6

//==============================================================================
//========================================================[ COMMUNICATION ]=====
// MESSAGE IDs
#define MID_SNUFF_MAIN_COMMANDS 0
#define MID_CONTROLS_ACTUATORS_AND_BUTTON 1
#define MID_CONTROLS_SERVOS 2
#define MID_CONTROLS_SERVOS_DEGREES 32 //DEBUGING DEPREICATED
#define MID_SAY_HELLO_TO_SNUFF 3
#define MID_RUN_SELF_CALIBRATIONS 10 // Byte1: 1 - Direction Test, 2 - Position Limit Test // Byte2: selection (XXXXXESB)
#define MID_EEPROM_READ 11 // 
#define MID_EEPROM_WRITE 7 // 
#define MID_ACTUATOR_POSITION_READ 12 // Byte1: 1 - Pot Position, 2 - Target Position
#define MID_FLIP_VEST_INPUT 6   //INVERT

// GOOD MESSAGES FROM SNUFF
#define GOOD_MSG_SNUFF_SAYS_IM_STILL_HERE 'g'
#define GOOD_MSG_MESSAGE_RECEIVED 'm'
#define GOOD_MSG_SNUFF_SAYS_HELLO 'H'
#define GOOD_MSG_DIRECTION_TEST_PASSED 'd'
#define GOOD_MSG_DIRECTION_TEST_FLIPPED_DIRECTION 'f'
#define GOOD_MSG_POSITION_LIMIT_TEST_DONE 12

// BAD MESSAGES FROM SNUFF
#define ERROR_MSG_UNKNOWN_MESSAGE_ID_RECEIVED 'm'
#define ERROR_MSG_CANNOT_MOVE_ACTUATOR 'a'

long TIME_lastOKStatus = 0;

//==============================================================================
//==============================================[ CALIBRATABLE PARAMETERS ]=====
#define EEPROM_CAL_BASE_POT_MAX 0
#define EEPROM_CAL_BASE_POT_MIN 1
#define EEPROM_CAL_SHOULDER_POT_MAX 2
#define EEPROM_CAL_SHOULDER_POT_MIN 3
#define EEPROM_CAL_ELBOW_POT_MAX 4
#define EEPROM_CAL_ELBOW_POT_MIN 5
#define EEPROM_CAL_AUTO_OK_STATUS_REPORTING 12
#define EEPROM_CAL_BASE_DIR_FLIPPED 13
#define EEPROM_CAL_SHOULDER_DIR_FLIPPED 14
#define EEPROM_CAL_ELBOW_DIR_FLIPPED 15
#define EEPROM_CAL_VEST_BASE_INPUT_FLIPPED 16
#define EEPROM_CAL_VEST_SHOULDER_INPUT_FLIPPED 17
#define EEPROM_CAL_VEST_ELBOW_INPUT_FLIPPED 18
#define EEPROM_CAL_VEST_ARM_INPUT_FLIPPED 19
#define EEPROM_CAL_VEST_WRIST_INPUT_FLIPPED 20
#define EEPROM_CAL_VEST_PALM_INPUT_FLIPPED 21
#define EEPROM_CAL_VEST_GRIPPER_INPUT_FLIPPED 22

int CAL_BASE_POT_MAX = 768;
int CAL_BASE_POT_MIN = 256;
boolean CAL_BASE_DIR_FLIPPED = false;

int CAL_SHOULDER_POT_MAX = 768;
int CAL_SHOULDER_POT_MIN = 256;
boolean CAL_SHOULDER_DIR_FLIPPED = false;

int CAL_ELBOW_POT_MAX = 768;
int CAL_ELBOW_POT_MIN = 256;
boolean CAL_ELBOW_DIR_FLIPPED = false;

boolean CAL_VEST_BASE_INPUT_FLIPPED = false;
boolean CAL_VEST_SHOULDER_INPUT_FLIPPED = false;
boolean CAL_VEST_ELBOW_INPUT_FLIPPED = false;
boolean CAL_VEST_ARM_INPUT_FLIPPED = false;
boolean CAL_VEST_WRIST_INPUT_FLIPPED = false;
boolean CAL_VEST_PALM_INPUT_FLIPPED = false;
boolean CAL_VEST_GRIPPER_INPUT_FLIPPED = false;

boolean CAL_AUTO_OK_STATUS_REPORTING = true;

//==============================================================================
//=======================================================[ H-BRIDGE SETUP ]=====
  HBridge2013 baseHB, shoulderHB, elbowHB;

//==============================================================================
//==========================================================[ SERVO SETUP ]=====
  Servo armServo, wristServo, palmServo, gripperServo;
  int armTargetPos, wristTargetPos, palmTargetPos, gripperTargetPos;
  int prevArmTargetPos, prevWristTargetPos, prevPalmTargetPos, prevGripperTargetPos;
  
  boolean ENABLE_DEGREE_SERVO_CONTROL = false;

//==============================================================================
//=======================================================[ ACTUATOR SETUP ]=====
  Actuator2014 base(&baseHB, BASE_POT_PIN);
  Actuator2014 shoulder(&shoulderHB, SHOULDER_POT_PIN);
  Actuator2014 elbow(&elbowHB, ELBOW_POT_PIN);

  int basePotValue, shoulderPotValue, elbowPotValue;
  byte baseCurrentPos, shoulderCurrentPos, elbowCurrentPos;
  byte baseOldPos, shoulderOldPos, elbowOldPos;
  int basePosChange, shoulderPosChange, elbowPosChange;
  byte baseTargetPos, shoulderTargetPos, elbowTargetPos;
  int baseTargetDistance, shoulderTargetDistance, elbowTargetDistance;

  long basePosTime, shoulderPosTime, elbowPosTime;
  long basePosChangeTime, shoulderPosChangeTime, elbowPosChangeTime;

//==============================================================================
//=================================================================[ MISC ]=====
int temp1;
int temp2;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void setup() {
  
  loadAllCals();
  baseHB.attach(HBRIDGE1_IN1_PIN, HBRIDGE1_IN2_PIN, HBRIDGE1_PWM_PIN);
  shoulderHB.attach(HBRIDGE2_IN1_PIN, HBRIDGE2_IN2_PIN, HBRIDGE2_PWM_PIN);
  elbowHB.attach(HBRIDGE3_IN1_PIN, HBRIDGE3_IN2_PIN, HBRIDGE3_PWM_PIN);
  base.goTo(500);
  shoulder.goTo(900);
  elbow.goTo(900);
  if (ENABLE_DEGREE_SERVO_CONTROL) {
    armTargetPos = 90;
    wristTargetPos = 90;
    palmTargetPos = 90;
    gripperTargetPos = 90;
    armServo.attach(9);
    wristServo.attach(11);
    palmServo.attach(8);
    gripperServo.attach(10);
  } else {
    armTargetPos = 1500;
    wristTargetPos = 1500;
    palmTargetPos = 1500;
    gripperTargetPos = 1500;
    armServo.attach(9);
    wristServo.attach(11);
    palmServo.attach(8);
    gripperServo.attach(10);
    updateAllServos();
  }
  Serial.begin(38400);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void loadAllCals() {
  
  CAL_BASE_POT_MAX = EEPROM.read(EEPROM_CAL_BASE_POT_MAX);
  CAL_BASE_POT_MIN = EEPROM.read(EEPROM_CAL_BASE_POT_MIN);
  CAL_SHOULDER_POT_MAX = EEPROM.read(EEPROM_CAL_SHOULDER_POT_MAX);
  CAL_SHOULDER_POT_MIN = EEPROM.read(EEPROM_CAL_SHOULDER_POT_MIN);
  CAL_ELBOW_POT_MAX = EEPROM.read(EEPROM_CAL_ELBOW_POT_MAX);
  CAL_ELBOW_POT_MIN = EEPROM.read(EEPROM_CAL_ELBOW_POT_MIN);
  CAL_AUTO_OK_STATUS_REPORTING = EEPROM.read(EEPROM_CAL_AUTO_OK_STATUS_REPORTING);
  CAL_BASE_DIR_FLIPPED = EEPROM.read(EEPROM_CAL_BASE_DIR_FLIPPED);
  CAL_SHOULDER_DIR_FLIPPED = EEPROM.read(EEPROM_CAL_SHOULDER_DIR_FLIPPED);
  CAL_ELBOW_DIR_FLIPPED = EEPROM.read(EEPROM_CAL_ELBOW_DIR_FLIPPED);
  CAL_VEST_BASE_INPUT_FLIPPED = EEPROM.read(EEPROM_CAL_VEST_BASE_INPUT_FLIPPED);
  CAL_VEST_SHOULDER_INPUT_FLIPPED = EEPROM.read(EEPROM_CAL_VEST_SHOULDER_INPUT_FLIPPED);
  CAL_VEST_ELBOW_INPUT_FLIPPED = EEPROM.read(EEPROM_CAL_VEST_ELBOW_INPUT_FLIPPED);
  CAL_VEST_ARM_INPUT_FLIPPED = EEPROM.read(EEPROM_CAL_VEST_ARM_INPUT_FLIPPED);
  CAL_VEST_WRIST_INPUT_FLIPPED = EEPROM.read(EEPROM_CAL_VEST_WRIST_INPUT_FLIPPED);
  CAL_VEST_PALM_INPUT_FLIPPED = EEPROM.read(EEPROM_CAL_VEST_PALM_INPUT_FLIPPED);
  CAL_VEST_GRIPPER_INPUT_FLIPPED = EEPROM.read(EEPROM_CAL_VEST_GRIPPER_INPUT_FLIPPED);
  baseHB.setFlippedDirection(CAL_BASE_DIR_FLIPPED);
  shoulderHB.setFlippedDirection(CAL_SHOULDER_DIR_FLIPPED);
  elbowHB.setFlippedDirection(CAL_ELBOW_DIR_FLIPPED);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void loop() {
  
  delay(5);
  
  if (CAL_AUTO_OK_STATUS_REPORTING) {
    if (millis() - TIME_lastOKStatus > 3000) {
      Serial.write('G');
      Serial.write((byte)GOOD_MSG_SNUFF_SAYS_IM_STILL_HERE);
      TIME_lastOKStatus = millis();
    }
  }
  checkForNewData();
  updateAllActuators();
  
  if (ENABLE_DEGREE_SERVO_CONTROL == false)
    updateAllServos();
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void checkForNewData() {
  if (Serial.available() > 20) {
    Serial.print("DATA CLOG");
    Serial.flush();
  }
  while (Serial.available() >= 8) {
    if (Serial.read() != 'U') {
      Serial.write('!');
    }
    else {
      if (Serial.read() != 'I') {
        Serial.write('!');
      }
      else {
        if (Serial.read() != 'C') {
          Serial.write('!');
        }
        else {
          byte messageID = Serial.read();
          byte messageByte1 = Serial.read();
          byte messageByte2 = Serial.read();
          byte messageByte3 = Serial.read();
          byte messageByte4 = Serial.read();
          boolean goodMessage = true;
          switch (messageID) {
            case MID_SNUFF_MAIN_COMMANDS:
              if (messageByte1 == 0) {
                CAL_AUTO_OK_STATUS_REPORTING = !CAL_AUTO_OK_STATUS_REPORTING;
                EEPROM.write(EEPROM_CAL_AUTO_OK_STATUS_REPORTING, CAL_AUTO_OK_STATUS_REPORTING);
              }
            break;
            case MID_CONTROLS_ACTUATORS_AND_BUTTON:
              base.goTo(4 *(CAL_VEST_BASE_INPUT_FLIPPED ? (255 - messageByte1) : messageByte1));
              shoulder.goTo(4 * (CAL_VEST_SHOULDER_INPUT_FLIPPED ? (255 - messageByte2) : messageByte2));
              elbow.goTo(4 * (CAL_VEST_ELBOW_INPUT_FLIPPED ? (255 - messageByte3) : messageByte3));
            break;
            case MID_CONTROLS_SERVOS:
              ENABLE_DEGREE_SERVO_CONTROL = false;
              //armServo.write(messageByte1);//map(messageByte1, 0, 255, 0, 180));
              //wristServo.write(messageByte2);//map(messageByte2, 0, 255, 0, 180));
              //palmServo.write(messageByte3);//map(messageByte3, 0, 255, 0, 180));
              //gripperServo.write(messageByte4);//map(messageByte4, 0, 255, 0, 180));
              armTargetPos = (CAL_VEST_ARM_INPUT_FLIPPED ? (255 - messageByte1) : messageByte1);
              wristTargetPos = (CAL_VEST_WRIST_INPUT_FLIPPED ? (255 - messageByte2) : messageByte2);
              palmTargetPos = (CAL_VEST_PALM_INPUT_FLIPPED ? (255 - messageByte3) : messageByte3);
              gripperTargetPos = (CAL_VEST_GRIPPER_INPUT_FLIPPED ? (255 - messageByte4) : messageByte4);
            break;
            case MID_CONTROLS_SERVOS_DEGREES:
              ENABLE_DEGREE_SERVO_CONTROL = true;
              armTargetPos = (CAL_VEST_ARM_INPUT_FLIPPED ? 255 - messageByte1 : messageByte1);
              wristTargetPos = (CAL_VEST_WRIST_INPUT_FLIPPED ? 255 - messageByte2 : messageByte2);
              palmTargetPos = (CAL_VEST_PALM_INPUT_FLIPPED ? 255 - messageByte3 : messageByte3);
              gripperTargetPos = (CAL_VEST_GRIPPER_INPUT_FLIPPED ? 255 - messageByte4 : messageByte4);
              armServo.write(armTargetPos);//map(messageByte1, 0, 255, 0, 180));
              wristServo.write(wristTargetPos);//map(messageByte2, 0, 255, 0, 180));
              palmServo.write(palmTargetPos);//map(messageByte3, 0, 255, 0, 180));
              gripperServo.write(gripperTargetPos);//map(messageByte4, 0, 255, 0, 180));
            break;
            case MID_SAY_HELLO_TO_SNUFF:
              Serial.write('G');
              Serial.write(GOOD_MSG_SNUFF_SAYS_HELLO);
            break;
            case MID_RUN_SELF_CALIBRATIONS:
              if (messageByte1 == 1)
                runActuatorDirectionTest(messageByte2);
              else if (messageByte1 == 2)
                runActuatorPositionLimitTest(messageByte2);
            break;
            case MID_EEPROM_READ:
              Serial.write('D');
              for (int i = messageByte1; i < messageByte1 + messageByte2; i++) {
                Serial.write(EEPROM.read(i));
              }
            break;
            case MID_EEPROM_WRITE:
              Serial.write('G');
              Serial.write('w');
              EEPROM.write(messageByte1, messageByte2);
              Serial.write(messageByte1);
              Serial.write(messageByte2);
            break;
            case MID_ACTUATOR_POSITION_READ:
              Serial.write('D');
              if (messageByte1 == 1) {
                if (messageByte2 % 2 == 1)
                  Serial.write(analogRead(BASE_POT_PIN) / 4);
                if (messageByte2 % 4 >= 2)
                  Serial.write(analogRead(SHOULDER_POT_PIN) / 4);
                if (messageByte2 % 8 >= 4)
                  Serial.write(analogRead(ELBOW_POT_PIN) / 4);
              }
              else if (messageByte1 == 2) {
                if (messageByte2 % 2 == 1)
                  Serial.write(analogRead(BASE_POT_PIN) / 4);
                if (messageByte2 % 4 >= 2)
                  Serial.write(analogRead(SHOULDER_POT_PIN) / 4);
                if (messageByte2 % 8 >= 4)
                  Serial.write(analogRead(ELBOW_POT_PIN) / 4);
              }
            break;
            case MID_FLIP_VEST_INPUT:
              if (messageByte1 == 1) {
                CAL_VEST_BASE_INPUT_FLIPPED = !CAL_VEST_BASE_INPUT_FLIPPED;
                EEPROM.write(EEPROM_CAL_VEST_BASE_INPUT_FLIPPED, CAL_VEST_BASE_INPUT_FLIPPED);
              }
              else if (messageByte1 == 2) {
                CAL_VEST_SHOULDER_INPUT_FLIPPED = !CAL_VEST_SHOULDER_INPUT_FLIPPED;
                EEPROM.write(EEPROM_CAL_VEST_SHOULDER_INPUT_FLIPPED, CAL_VEST_SHOULDER_INPUT_FLIPPED);
              }
              else if (messageByte1 == 3) {
                CAL_VEST_ELBOW_INPUT_FLIPPED = !CAL_VEST_ELBOW_INPUT_FLIPPED;
                EEPROM.write(EEPROM_CAL_VEST_ELBOW_INPUT_FLIPPED, CAL_VEST_ELBOW_INPUT_FLIPPED);
              }
              else if (messageByte1 == 4) {
                CAL_VEST_ARM_INPUT_FLIPPED = !CAL_VEST_ARM_INPUT_FLIPPED;
                EEPROM.write(EEPROM_CAL_VEST_ARM_INPUT_FLIPPED, CAL_VEST_ARM_INPUT_FLIPPED);
              }
              else if (messageByte1 == 5) {
                CAL_VEST_WRIST_INPUT_FLIPPED = !CAL_VEST_WRIST_INPUT_FLIPPED;
                EEPROM.write(EEPROM_CAL_VEST_WRIST_INPUT_FLIPPED, CAL_VEST_WRIST_INPUT_FLIPPED);
              }
              else if (messageByte1 == 6) {
                CAL_VEST_PALM_INPUT_FLIPPED = !CAL_VEST_PALM_INPUT_FLIPPED;
                EEPROM.write(EEPROM_CAL_VEST_PALM_INPUT_FLIPPED, CAL_VEST_PALM_INPUT_FLIPPED);
              }
              else if (messageByte1 == 7) {
                CAL_VEST_GRIPPER_INPUT_FLIPPED = !CAL_VEST_GRIPPER_INPUT_FLIPPED;
                EEPROM.write(EEPROM_CAL_VEST_GRIPPER_INPUT_FLIPPED, CAL_VEST_GRIPPER_INPUT_FLIPPED);
              }               
            break;
            default:
              goodMessage = false;
              Serial.write('E');
              Serial.write(ERROR_MSG_UNKNOWN_MESSAGE_ID_RECEIVED);
          }
          if (goodMessage) {
            //Serial.write('G');
            //Serial.write(GOOD_MSG_MESSAGE_RECEIVED);
          }
        }
      }
    }
  }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void updateAllServos() {
  updateArmServo();
  updateWristServo();
  updatePalmServo();
  updateGripperServo();
}
void updateServos(boolean arm, boolean wrist, boolean palm, boolean gripper) {
  if (arm)     updateArmServo();
  if (wrist)   updateWristServo();
  if (palm)    updatePalmServo();
  if (gripper) updateGripperServo();
}
void updateArmServo() {
  int newPOS, oldPOS = prevArmTargetPos;
  newPOS = map(armTargetPos, 0, 255, 900, 2100);

  if (newPOS - oldPOS > 200)
    newPOS = oldPOS + 200;
  else if (newPOS - oldPOS < -200)
    newPOS = oldPOS - 200;
  armServo.writeMicroseconds(newPOS);
  prevArmTargetPos = newPOS;
}
void updateWristServo() {
  int newPOS, oldPOS = prevWristTargetPos;
  newPOS = map(wristTargetPos, 0, 255, 900, 2100);

  if (newPOS - oldPOS > 200)
    newPOS = oldPOS + 200;
  else if (newPOS - oldPOS < -200)
    newPOS = oldPOS - 200;
  wristServo.writeMicroseconds(newPOS);
  prevWristTargetPos = newPOS;
}
void updatePalmServo() {
  int newPOS, oldPOS = prevPalmTargetPos;
  newPOS = map(palmTargetPos, 0, 255, 900, 2100);

  if (newPOS - oldPOS > 200)
    newPOS = oldPOS + 200;
  else if (newPOS - oldPOS < -200)
    newPOS = oldPOS - 200;
  palmServo.writeMicroseconds(newPOS);
  prevPalmTargetPos = newPOS;
}
void updateGripperServo() {
  int newPOS, oldPOS = prevGripperTargetPos;
  newPOS = map(gripperTargetPos, 0, 255, 900, 2100);

  if (newPOS - oldPOS > 200)
    newPOS = oldPOS + 200;
  else if (newPOS - oldPOS < -200)
    newPOS = oldPOS - 200;
  gripperServo.writeMicroseconds(newPOS);
  prevGripperTargetPos = newPOS;
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void updateAllActuators() {
  updateBaseActuator();
  updateShoulderActuator();
  updateElbowActuator();
}
void updateActuators(boolean base, boolean shoulder, boolean elbow) {
  if (base)     updateBaseActuator();
  if (shoulder) updateShoulderActuator();
  if (elbow)    updateElbowActuator();
}
void updateBaseActuator() {
  base.loopCycle();
  /*
  int newPWM, oldPWM = baseHB.read();
  if (baseTargetDistance > 10)
    newPWM = map(constrain(baseTargetDistance, 10, 50), 10, 50, 80, 255);
  else if (baseTargetDistance < -10)
    newPWM = map(constrain(baseTargetDistance, -50, -10), -50, -10, -255, -80);
  else
    newPWM = 0;
  if (newPWM - oldPWM > 10)
    newPWM = oldPWM + 10;
  else if (newPWM - oldPWM < -10)
    newPWM = oldPWM - 10;
  baseHB.write(newPWM);
  */
}
void updateShoulderActuator() {
  shoulder.loopCycle();
  /*
  int newPWM, oldPWM = shoulderHB.read();
  if (shoulderTargetDistance > 5)
    newPWM = map(constrain(shoulderTargetDistance, 5, 40), 5, 40, 80, 255);
  else if (shoulderTargetDistance < -5)
    newPWM = map(constrain(shoulderTargetDistance, -40, -5), -40, -5, -255, -80);
  else
    newPWM = 0;
  if (newPWM - oldPWM > 20)
    newPWM = oldPWM + 20;
  else if (newPWM - oldPWM < -20)
    newPWM = oldPWM - 20;
  shoulderHB.write(newPWM);
  */
}
void updateElbowActuator() {
  elbow.loopCycle();
  /*
  int newPWM, oldPWM = elbowHB.read();
  if (elbowTargetDistance > 5)
    newPWM = map(constrain(elbowTargetDistance, 5, 50), 5, 50, 60, 255);
  else if (elbowTargetDistance < -5)
    newPWM = map(constrain(elbowTargetDistance, -50, -5), -50, -5, -255, -60);
  else
    newPWM = 0;
  if (newPWM - oldPWM > 20)
    newPWM = oldPWM + 20;
  else if (newPWM - oldPWM < -20)
    newPWM = oldPWM - 20;
  elbowHB.write(newPWM);
  */
}


void runActuatorDirectionTest(byte selection) {
  if (selection % 2 == 1)
    if (runActuatorDirectionTest(baseHB, BASE_POT_PIN)) {
      CAL_BASE_DIR_FLIPPED = !CAL_BASE_DIR_FLIPPED;
      baseHB.setFlippedDirection(CAL_BASE_DIR_FLIPPED);
      EEPROM.write(EEPROM_CAL_BASE_DIR_FLIPPED, CAL_BASE_DIR_FLIPPED);
    }
  if (selection % 4 >= 2)
    if (runActuatorDirectionTest(shoulderHB, SHOULDER_POT_PIN)) {
      CAL_SHOULDER_DIR_FLIPPED = !CAL_SHOULDER_DIR_FLIPPED;
      shoulderHB.setFlippedDirection(CAL_SHOULDER_DIR_FLIPPED);
      EEPROM.write(EEPROM_CAL_SHOULDER_DIR_FLIPPED, CAL_SHOULDER_DIR_FLIPPED);
    }
  if (selection % 8 >= 4)
    if (runActuatorDirectionTest(elbowHB, ELBOW_POT_PIN)) {
      CAL_ELBOW_DIR_FLIPPED = !CAL_ELBOW_DIR_FLIPPED;
      elbowHB.setFlippedDirection(CAL_ELBOW_DIR_FLIPPED);
      EEPROM.write(EEPROM_CAL_ELBOW_DIR_FLIPPED, CAL_ELBOW_DIR_FLIPPED);
    }
}

boolean runActuatorDirectionTest(HBridge2013 hbridge, int potPin) {
  int startPOS = analogRead(potPin);
  int testFWD, testBWD;
 
  hbridge.write(150);
  delay(200);
  hbridge.write(0);
  delay(100);
  testFWD = analogRead(potPin) - startPOS;

  startPOS += testFWD;
  hbridge.write(-150);
  delay(200);
  hbridge.write(0);
  delay(100);
  testBWD = analogRead(potPin) - startPOS;
  
  if (testFWD < 0 || testBWD > 0) {
    Serial.write('G');
    Serial.write(GOOD_MSG_DIRECTION_TEST_FLIPPED_DIRECTION);
    return true;
  } else if (testFWD > 0 || testBWD < 0) {
    Serial.write('G');
    Serial.write(GOOD_MSG_DIRECTION_TEST_PASSED);
  } else {
    Serial.write('E');
    Serial.write(ERROR_MSG_CANNOT_MOVE_ACTUATOR);
  }
  return false;
}


void runActuatorPositionLimitTest(byte selection) {
  if (selection % 2 == 1)
    if (runActuatorPositionLimitTest(baseHB, BASE_POT_PIN)) {
      CAL_BASE_POT_MAX = temp1 / 4;
      CAL_BASE_POT_MIN = temp2 / 4;
      EEPROM.write(EEPROM_CAL_BASE_POT_MAX, (byte)CAL_BASE_POT_MAX);
      EEPROM.write(EEPROM_CAL_BASE_POT_MIN, (byte)CAL_BASE_POT_MIN);
      Serial.write('I');
      Serial.write(CAL_BASE_POT_MAX);
      Serial.write(CAL_BASE_POT_MIN);
    }
  if (selection % 4 >= 2)
    if (runActuatorPositionLimitTest(shoulderHB, SHOULDER_POT_PIN)) {
      CAL_SHOULDER_POT_MAX = temp1 / 4;
      CAL_SHOULDER_POT_MIN = temp2 / 4;
      
      EEPROM.write(EEPROM_CAL_SHOULDER_POT_MAX, (byte)CAL_SHOULDER_POT_MAX);
      EEPROM.write(EEPROM_CAL_SHOULDER_POT_MIN, (byte)CAL_SHOULDER_POT_MIN);
      Serial.write('I');
      Serial.write(CAL_SHOULDER_POT_MAX);
      Serial.write(CAL_SHOULDER_POT_MIN);
    }
  if (selection % 8 >= 4)
    if (runActuatorPositionLimitTest(elbowHB, ELBOW_POT_PIN)) {
      CAL_ELBOW_POT_MAX = temp1 / 4;
      CAL_ELBOW_POT_MIN = temp2 / 4;
      EEPROM.write(EEPROM_CAL_ELBOW_POT_MAX, (byte)CAL_ELBOW_POT_MAX);
      EEPROM.write(EEPROM_CAL_ELBOW_POT_MIN, (byte)CAL_ELBOW_POT_MIN);
      Serial.write('I');
      Serial.write(CAL_ELBOW_POT_MAX);
      Serial.write(CAL_ELBOW_POT_MIN);
    }
}

boolean runActuatorPositionLimitTest(HBridge2013 hbridge, int potPin) {
  //MAX POS TEST
  int pos = analogRead(potPin);
  int prevPos;
  int posChange;
  int pwm = 60;
  do {
    prevPos = pos;
    for (int i = 0; i < 20; i++) {
      hbridge.write(pwm);
      pwm = pwm < 250 ? pwm + 5 : 255;
      delay(20);
    }
    pos = analogRead(potPin);
    posChange = pos - prevPos;
  } while(posChange != 0); 
  hbridge.write(0);
  temp1 = analogRead(potPin);
  
  //MIN POS TEST
  pos = analogRead(potPin);
  pwm = -60;
  do {
    prevPos = pos;
    for (int i = 0; i < 20; i++) {
      hbridge.write(pwm);
      pwm = pwm > -250 ? pwm - 5 : -255;
      delay(20);
    }
    pos = analogRead(potPin);
    posChange = pos - prevPos;
  } while(posChange != 0); 
  hbridge.write(0);
  temp2 = analogRead(potPin);
  
  if (temp2 > temp1) {
    int temp = temp1;
    temp1 = temp2;
    temp2 = temp;
  }
  
  Serial.write('G');
  Serial.write(GOOD_MSG_POSITION_LIMIT_TEST_DONE);
  return true;
}
