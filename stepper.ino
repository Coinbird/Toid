#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::FULL4WIRE, 8, 10, 9, 11); 
// Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

void setupStepper() {
   stepper.setMaxSpeed(150);
   stepper.setAcceleration(10);
//   stepper.setSpeed(150);  
    randomizeTarget();

}

void runStepper() {
  stepper.run();
}

void randomizeTarget() {
  stepper.moveTo(1000 - (rand() % 2000));
}
