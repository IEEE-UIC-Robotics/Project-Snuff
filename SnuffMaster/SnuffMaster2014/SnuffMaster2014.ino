#include <EEPROM.h>

/*
 * PINS
 * ============
 * base:      8
 * shoulder:  9
 * elbow:    10
 * arm:      11
 * wrist:    12
 * palm:     13
 * button:   14
 * gripper:  15
 */
 
#define OUT_ERROR 'E'
#define OUT_PACKET_CONFIRMED 'P'
#define OUT_PACKET_PROCESSED 'p'
#define OUT_PACKET_DROPPED 'D'
#define OUT_MESSAGE_UNKNOWN '?'
#define T_MAX_PACKET_WAIT 50

// JOINT INDEXES
#define BASE 0
#define SHOULDER 1
#define ELBOW 2
#define ARM 3
#define WRIST 4
#define PALM 5
#define GRIPPER 6
#define BUTTON 7

// EEPROM ADDRESSES
const unsigned int EE_PINS[] = {0, 1, 2, 3, 4, 5, 6, 7};
const unsigned int EE_MINS[] = {8, 10, 12, 14, 16, 18, 20};
const unsigned int EE_MAXS[] = {22, 24, 26, 28, 30, 32, 34};
const unsigned int EE_SENDING_PERIOD = 36;
const unsigned int EE_FLIP[] = {38, 39, 40, 41, 42, 43, 44, 45};
const unsigned int EE_MIDS[] = {47, 49, 51, 53, 55, 57, 59, 61};

// CURRENT SETTINGS (INITIALIZED WITH DEFAULT VALUES)
byte pins[] = {8, 9, 10, 11, 12, 13, 15, 14};
unsigned int maxs[] = {877, 557, 833, 945, 981, 933, 1023};
unsigned int mins[] = {494, 181, 313, 257, 156, 225, 0};
unsigned int sendingPeriod = 20; // 50 times per second
boolean flip[] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int mids[] = {0, 0, 0, 0, 0, 0, 0, 0,};

// CURRENT STATE
unsigned int positionData[] = {0, 0, 0, 0, 0, 0, 0};
byte mappedData[] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned long t_sent = 0;

// RECEIVING DATA
boolean packetInProgress = false;
boolean packetReceived = false;
char bytesNeeded = 5;
byte packetID = 0;
unsigned long t = 0;
unsigned long t_packet = 0;

// SENDING DATA
boolean sendingEnabled = true;

void setup() {
  loadSettingsFromEEPROM();
  Serial.begin(38400);
}

void loop() {
  t = millis();
  receivingDataLoop();
  unsigned int dt = t - t_sent;
  if (sendingEnabled && dt >= sendingPeriod) {
    t_sent += dt;
    readPositionData();
    sendPositionData();
  }
}

void readPositionData() {
  for (byte i = 0; i < 7; i++) {
    unsigned int actualMin = mins[i];
    unsigned int actualMax = maxs[i];
    if (actualMin > actualMax) {
      unsigned int temp = actualMin;
      actualMin = actualMax;
      actualMax = actualMin;
    }
    positionData[i] = constrain(analogRead(pins[i]), actualMin, actualMax);
    mappedData[i] = map(positionData[i], actualMin, actualMax, flip[i] ? 255 : 0, flip[i] ? 0 : 255);
    if (flip[i])
      mappedData[i] = 255 - mappedData[i];
  }
  mappedData[BUTTON] = analogRead(pins[BUTTON]) < 512 ? 255 : 0;
  if (mappedData[BUTTON] != 0)
    mappedData[GRIPPER] = 200;
}

void sendPositionData() {
  Serial.write('U');
  Serial.write('I');
  Serial.write('C');
  Serial.write('V'); // PACKET ID RESERVED FOR VEST
  Serial.write(9);   // PACKET PAYLOAD SIZE IN BYTES
  Serial.write('V'); // MESSAGE ID RESERVED FOR VEST
  Serial.write(mappedData[BASE]);
  Serial.write(mappedData[SHOULDER]);
  Serial.write(mappedData[ELBOW]);
  Serial.write(mappedData[ARM]);
  Serial.write(mappedData[WRIST]);
  Serial.write(mappedData[PALM]);
  Serial.write(mappedData[GRIPPER]);
  Serial.write(mappedData[BUTTON]);
}

void receivingDataLoop() {
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

char processMessage(byte messageID) {
  char consumedData = 0;
  switch (messageID) {
  case 'M': { // SET POT MIN LIMIT OF A JOINT (ex: MB)
    char jointIndex = getJointIndex(Serial.read());
    consumedData++;
    setJointMin(jointIndex, analogRead(pins[jointIndex]));
  } break;
  case 'X': { // SET POT MAX LIMIT OF A JOINT (ex: XB)
    char jointIndex = getJointIndex(Serial.read());
    consumedData++;
    setJointMax(jointIndex, analogRead(pins[jointIndex]));
  } break;
  case 't': { // TOGGLE JOINT INVERTING SETTING (ex: tB)
    char jointIndex = getJointIndex(Serial.read());
    consumedData++;
    flip[jointIndex] = !flip[jointIndex];
    EEPROM.write(EE_FLIP[jointIndex], flip[jointIndex] ? 255 : 0);
    Serial.write(messageID);
    Serial.write(1);
    Serial.write(flip[jointIndex] ? 'F' : 'N');
  } break;
  case 'p': { // GET POT POSITION OF A JOINT (ex: pB)
    char jointIndex = getJointIndex(Serial.read());
    consumedData++;
    Serial.write(messageID);
    Serial.write(2);
    unsigned int currentJointPosition = analogRead(pins[jointIndex]);
    Serial.write((byte)(currentJointPosition >> 8));
    Serial.write((byte)(currentJointPosition & 0xFF));
  } break;
  case 'm': { // GET POT MIN LIMIT OF A JOINT (ex: mB)
    char jointIndex = getJointIndex(Serial.read());
    consumedData++;
    Serial.write(messageID);
    Serial.write(2);
    Serial.write((byte)(mins[jointIndex] >> 8));
    Serial.write((byte)mins[jointIndex]);
  } break;
  case 'x': { // GET POT MAX LIMIT OF A JOINT (ex: xB)
    char jointIndex = getJointIndex(Serial.read());
    consumedData++;
    Serial.write(messageID);
    Serial.write(2);
    Serial.write((byte)(maxs[jointIndex] >> 8));
    Serial.write((byte)maxs[jointIndex]);
  } break;
  case 'F': { // SET SENDING FREQUENCY (ex: F + 0x00 + 0x64)
    unsigned int frequency = Serial.read() << 8;
    frequency += Serial.read();
    consumedData += 2;
    setSendingFrequency(frequency);
  } break;
  case 'T': { // SET SENDING PERIOD (ex: T + 0x00 + 0x0A)
    unsigned int period = Serial.read() << 8;
    period += Serial.read();
    consumedData += 2;
    setSendingPeriod(period);
  } break;
  case '':{ // SET MIDDLE VALUE
  }
  default:
    Serial.write(messageID);
    Serial.write(1);
    Serial.write(OUT_MESSAGE_UNKNOWN);
    return -1;
  }
  return consumedData;
}

void loadSettingsFromEEPROM() {
  for (int i = 0; i < 8; i++) {
    //pins[i] = EEPROM.read(EE_PINS[i]);
    mins[i] = loadINTfromEEPROM(EE_MINS[i]);
    maxs[i] = loadINTfromEEPROM(EE_MAXS[i]);
    int currentMid;
    if((currentMid = loadINTfromEEPROM(EE_MIDS[i])) == 0){
      currentMid = (mins[i] + maxs[i])/2;
      mids[i] = currentMid;
      storeINTtoEEPROM(EE_MIDS[i], currentMid);
    }else{
      mids[i] = currentMid;
    }
    flip[i] = EEPROM.read(EE_FLIP[i]);
  }
  sendingPeriod = loadINTfromEEPROM(EE_SENDING_PERIOD);
}

void setJointMin(char jointIndex, unsigned int minValue) {
  if (jointIndex < 0 || jointIndex > 6)
    return;
  mins[jointIndex] = minValue;
  storeINTtoEEPROM(EE_MINS[jointIndex], minValue);
}

void setJointMax(char jointIndex, unsigned int maxValue) {
  if (jointIndex < 0 || jointIndex > 6)
    return;
  maxs[jointIndex] = maxValue;
  storeINTtoEEPROM(EE_MAXS[jointIndex], maxValue);
}

char getJointIndex(byte jointChar) {
  switch (jointChar) {
    case 'B': return BASE;
    case 'S': return SHOULDER;
    case 'E': return ELBOW;
    case 'A': return ARM;
    case 'W': return WRIST;
    case 'P': return PALM;
    case 'G': return GRIPPER;
    default: return -1;
  }
}

void setSendingFrequency(unsigned int newFrequency) {
  if (newFrequency == 0)
    sendingEnabled = false;
  else
    setSendingPeriod(1000 / newFrequency);
}

void setSendingPeriod(unsigned int newPeriod) {
  sendingEnabled = true;
  sendingPeriod = newPeriod;
  storeINTtoEEPROM(EE_SENDING_PERIOD, sendingPeriod);
}

void storeINTtoEEPROM(unsigned int address, unsigned int value) {
  EEPROM.write(address, (byte)(value >> 8));
  EEPROM.write(address + 1, (byte)(value & 0xFF));
}

unsigned int loadINTfromEEPROM(unsigned int address) {
  unsigned int value = EEPROM.read(address) << 8;
  value += EEPROM.read(address + 1);
  return value;
}
