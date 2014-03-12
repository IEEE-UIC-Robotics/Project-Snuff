#define basePin 8
#define shoulderPin 9
#define elbowPin 10
#define armPin 11
#define wristPin 12
#define palmPin 13
#define buttonPin 14
#define gripperPin 15

#define baseIndex 0
#define shoulderIndex 1
#define elbowIndex 2
#define armIndex 3
#define wristIndex 4
#define palmIndex 5
#define buttonIndex 6
#define gripperIndex 7

const int pins[] = {8, 9, 10, 11, 12, 13, 14, 15};
const int potMax[] = {877, 557, 833, 945, 981, 933, 1023, 1023};
const int potMin[] = {494, 181, 313, 257, 156, 225, 0, 0};
int potReading[] = {0, 0, 0, 0, 0, 0, 0, 0};
byte mappedData[] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
  Serial.begin(38400);
}

void loop() {
  for (int i = 0; i < 8; i++) {
    //potReading[i] = analogRead(pins[i]);
    //mappedData[i] = potReading[i];
    potReading[i] = constrain(analogRead(pins[i]), potMin[i], potMax[i]);
    mappedData[i] = (byte)map(potReading[i], potMin[i], potMax[i], 0, 255);
    delay(1);
  }
  
  Serial.write('U');
  Serial.write('I');
  Serial.write('C');
  Serial.write(1);
  Serial.write(mappedData[baseIndex]);
  Serial.write(mappedData[shoulderIndex]);
  Serial.write(mappedData[elbowIndex]);
  Serial.write(mappedData[buttonIndex]);
  
  delay(8);
  
  Serial.write('U');
  Serial.write('I');
  Serial.write('C');
  Serial.write(2);
  Serial.write(mappedData[armIndex]);
  Serial.write(mappedData[wristIndex]);
  Serial.write(mappedData[palmIndex]);
  Serial.write(mappedData[gripperIndex]); 
}
