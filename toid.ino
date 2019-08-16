#include <AStar32U4.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

AStar32U4LCD lcd;

AStar32U4ButtonA buttonA;

uint8_t displayMode = 0;

void setup()
{
  setupRTC();
//  setupStepper();
}

void loop()
{

  tmElements_t tm;
  if (displayMode == 0 && buttonA.getSingleDebouncedPress()) {
    lcd.clear();
    displayMode = 1; 
  } else if (displayMode ==1 && buttonA.getSingleDebouncedPress()) {
    lcd.clear();
    displayMode = 0;
  }

  if (isRTCTriggered()) {
    // Clear the screen
    lcd.clear();
    
    switch(displayMode) {
      case 0:
        displayRTCTime(tm);
        break;
      default:
        lcd.print(getTickCount());    
    }
  }
//  runStepper();
//  delay(200);
}
