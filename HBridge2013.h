#ifndef HBridge2013_h
#define HBridge2013_h

#include "Arduino.h"

class HBridge2013
{
  public:
    HBridge2013();
    void attach(const byte, const byte, const byte);
    void detach();
    boolean attached();
    void write(int);
    int read();
    void setBrakeOnZero(boolean);
    void setFlippedDirection(boolean);

  private:
    void updatePins();

    byte IN1_PIN;
    byte IN2_PIN;
    byte PWM_PIN;

    boolean ATTACHED;
    boolean BRAKE_ON_ZERO;
    short DIRECTION;

    int state;
};

#endif
