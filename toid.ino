#include <AStar32U4.h>

#include <SD.h>

AStar32U4LCD lcd;

AStar32U4ButtonB buttonB;
AStar32U4ButtonC buttonC;

uint8_t displayMode = 0;

void setup()
{
  Serial.begin(115200);
  while (!Serial) ; // wait for Arduino Serial Monitor
  delay(200);  
  openSDFile();
  setupRTC();
  setupStepper();
}

void loop()
{
  if (buttonB.getSingleDebouncedPress()) {
    randomizeTarget();
  }
  if (buttonC.getSingleDebouncedPress()) {
    stopAll();
    if (displayMode == 0) {
      displayMode = 1;
    } else {
      displayMode = 0;
    }
  }

  if (isRTCTriggered()) {
    // Clear the screen
    lcd.clear();
    
    switch(displayMode) {
      case 0:
        displayRTCTime();
        break;
      default:
        lcd.print(getNextTide());    
    }
  }

  if (isCheckTideTriggered()) {
    lcd.clear();
    lcd.print(getTimeToNextTideInfo());    
    readNextTide();
  }

  runStepper();
}
