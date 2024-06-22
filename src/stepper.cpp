#include <AccelStepper.h>
#include "stepper.h"

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
  stepper.setCurrentPosition(0);
//  stepper.stop();
}

void runStepper() {
  stepper.run();
}

long getPos() {
  return stepper.currentPosition();
}

void moveEighth() {
  stepper.moveTo(getPos() + (long)STEPS_PER_OUT_REV/8);
}

void moveToTidePosition(int mins, bool isHighTide) {
  if (mins <= 0) // not initialized yet, skip
    return;
  float pctMove = (float)mins / 360.0;
  Serial.print("mins: ");
  Serial.println(mins);
  if (isHighTide) { // Left Side
    long moveTarget = STEPS_PER_OUT_REV/2 - (long)(pctMove * float(STEPS_PER_OUT_REV / 2));
    stepper.moveTo(moveTarget);
    Serial.print(F("move hi "));
    Serial.println(moveTarget);
  } else {
    long moveTarget = STEPS_PER_OUT_REV - (long)(pctMove * float(STEPS_PER_OUT_REV / 2));
    stepper.moveTo(moveTarget);
    Serial.print(F("move lo "));
    Serial.println(moveTarget);
  }
}
