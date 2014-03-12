#define OUT_ERROR 'E'
#define OUT_PACKET_CONFIRMED 'P'
#define OUT_PACKET_PROCESSED 'p'
#define OUT_PACKET_DROPPED 'D'
#define T_MAX_PACKET_WAIT 50

/////////////////////////
boolean packetInProgress = false;
boolean packetReceived = false;
signed char bytesNeeded = 5;
unsigned char packetID = 0;

long t;
long t_packet;

////////////////////////
void setup(){
  pinMode(13, OUTPUT);
  Serial.begin(38400);
}

////////////////////////
void loop(){
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
      Serial.write(OUT_PACKET_PROCESSED);
      Serial.write(packetID);
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
      else
        bytesNeeded -= consumedBytes;
    }
    else
      if (Serial.read() == 'U')
        if (Serial.read() == 'I')
          if (Serial.read() == 'C') {
            packetID = Serial.read();
            bytesNeeded = Serial.read();
            packetInProgress = true;
            t_packet = t;
          }
  }
}

//////////////////////////
signed char processMessage(unsigned char messageID) {
  signed char consumedData = 0;
  Serial.print("MSG: ");
  Serial.println(messageID);
  switch (messageID) {
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
    Serial.write('?');
    return -1; // error
  }
  Serial.write('G');
  Serial.print(messageID);
  return consumedData;
}


