// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef Project_Snuff_H_
#define Project_Snuff_H_
//add your includes for the project Snuff_Robot_Arduino here
#include "HBridge2013.h"

//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//add your function definitions for the project Snuff_Robot_Arduino here

void loadAllCals();
void checkForNewData();
void updateAllServos();
void updateServos(bool arm, bool wrist, bool palm, bool gripper);
void updateArmServo();
void updateWristServo();
void updatePalmServo();
void updateGripperServo();
void updateAllActuators();
void updateActuators(bool base, bool shoulder, bool elbow);
void updateBaseActuator();
void updateShoulderActuator();
void updateElbowActuator();
void runActuatorDirectionTest(char selection);
void runActuatorPositionLimitTest(char selection);
bool runOneActuatorDirectionTest(HBridge2013 *hbridge, int potPin);
bool runOneActuatorPositionLimitTest(HBridge2013 *hbridge, int potPin);



//Do not add code below this line
#endif /* Eclipse-Arduino-Snuff_H_ */
