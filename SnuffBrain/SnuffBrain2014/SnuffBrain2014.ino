#include <HBridge2014.h>

#define HBRIDGE1_IN1_PIN 37
#define HBRIDGE1_IN2_PIN 33
#define HBRIDGE1_PWM_PIN 4

#define HBRIDGE2_IN1_PIN 39
#define HBRIDGE2_IN2_PIN 43
#define HBRIDGE2_PWM_PIN 7

#define HBRIDGE3_IN1_PIN 35
#define HBRIDGE3_IN2_PIN 31
#define HBRIDGE3_PWM_PIN 5


#define OUT_ERROR 'E'
#define OUT_PACKET_CONFIRMED 'P'
#define OUT_PACKET_PROCESSED 'p'
#define OUT_PACKET_DROPPED 'D'
#define OUT_MESSAGE_UNKNOWN '?'
#define T_MAX_PACKET_WAIT 50

/////////////////////////
boolean packetInProgress = false;
boolean packetReceived = false;
signed char bytesNeeded = 5;
unsigned char packetID = 0;
unsigned long t = 0;
unsigned long t_packet = 0;

HBridge2014 baseHB, shoulderHB, elbowHB;

////////////////////////
void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(38400);
  
  baseHB.attach(HBRIDGE1_IN1_PIN, HBRIDGE1_IN2_PIN, HBRIDGE1_PWM_PIN);
  shoulderHB.attach(HBRIDGE2_IN1_PIN, HBRIDGE2_IN2_PIN, HBRIDGE2_PWM_PIN);
  elbowHB.attach(HBRIDGE3_IN1_PIN, HBRIDGE3_IN2_PIN, HBRIDGE3_PWM_PIN);
}

////////////////////////
void loop() {
  t = millis();
  receiveData();
  
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
      signed char consumedBytes = processMessage(Serial.read());
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
signed char processMessage(unsigned char messageID) {
  signed char consumedData = 0;
  switch (messageID) {
    
  case 1: {
    boolean isFWD = Serial.read();
    unsigned char pwm = Serial.read();
    consumedData += 2;
    baseHB.write(isFWD ? pwm : -pwm);
  } break;
  
  case 2: {
    boolean isFWD = Serial.read();
    unsigned char pwm = Serial.read();
    consumedData += 2;
    shoulderHB.write(isFWD ? pwm : -pwm);
  } break;

  case 3: {
    boolean isFWD = Serial.read();
    unsigned char pwm = Serial.read();
    consumedData += 2;
    elbowHB.write(isFWD ? pwm : -pwm);
  } break;

  case 'e':
    Serial.write(messageID);
    Serial.write(1);
    Serial.write(Serial.read());
    consumedData++;
    break;
    
  case 'E': {
    Serial.write(messageID);
    Serial.write(1);
    signed char echoSize = Serial.read();
    consumedData += 1 + echoSize;
    while (echoSize-- > 0)
      Serial.write(Serial.read());
  } break;
    
  case 'T':
    digitalWrite(13, !digitalRead(13));
    break;
  case 'S':
    digitalWrite(13, HIGH);
    break;
  case 'R':
    digitalWrite(13, LOW);
    break;
  case 'D':
    digitalWrite(13, Serial.read() <= 'Z');
    consumedData++;
    break;
  case 'W':
    delay(Serial.read() * 10);
    consumedData++;
    break;
  default:
    Serial.write(messageID);
    Serial.write(1);
    Serial.write(OUT_MESSAGE_UNKNOWN);
    return -1;
  }
  return consumedData;
}
