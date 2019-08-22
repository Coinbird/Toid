#include <AccelStepper.h>

const float STEPS_PER_REV = 32; 
 
//  Amount of Gear Reduction
const float GEAR_RED = 64;
 
// Number of steps per geared output rotation
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;

#define MAX_STEPPER_SPEED 150
#define STEPPER_ACCELERATION 100

AccelStepper stepper(AccelStepper::FULL4WIRE, 8, 10, 9, 11); 
// Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

void setupStepper() {
   stepper.setMaxSpeed(MAX_STEPPER_SPEED);
   stepper.setAcceleration(STEPPER_ACCELERATION);
//  stepper.moveTo(STEPS_PER_OUT_REV );

}

void moveSmallAmountCW() {
  stepper.moveTo(getPos() + 100);
}

void moveSmallAmountCCW() {
  stepper.moveTo(getPos() - 100);
}

void stopAll() {
  stepper.stop();
}

void runStepper() {
  stepper.run();
}

long getPos() {
  return stepper.currentPosition();
}

void randomizeTarget() {
  stepper.moveTo(getPos() + (long)STEPS_PER_OUT_REV/4);
}

void moveToTidePosition(int mins, bool isHighTide) {
  Serial.print("move ");
  Serial.println(mins);
}
