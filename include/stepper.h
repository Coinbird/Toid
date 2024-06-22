#include <AccelStepper.h>

#ifndef __STEPPER__
#define __STEPPER__

const float STEPS_PER_REV = 32; 
 
//  Amount of Gear Reduction
const float GEAR_RED = 64;
 
// Number of steps per geared output rotation
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;

#define MAX_STEPPER_SPEED 150
#define STEPPER_ACCELERATION 100

void setupStepper();
void moveSmallAmountCW();
void moveSmallAmountCCW();
void stopAll();
void runStepper();
long getPos();
void moveEighth();
void moveToTidePosition(int mins, bool isHighTide);
long getPos();

#endif