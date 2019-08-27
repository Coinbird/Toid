#include <AStar32U4.h>

#include <SD.h>

AStar32U4LCD lcd;

//AStar32U4ButtonB buttonB;
//AStar32U4ButtonC buttonC;

uint8_t displayMode = 0;

void setup()
{
  Serial.begin(115200);
//  while(!Serial) {} 
  delay(200);  
  openSDFile();
  setupRTC();
  setupStepper();
}

void loop()
{
//  if (buttonB.getSingleDebouncedPress()) {
//    moveEighth(); // for re-zeroing without messing with the clock hand
//  }
//  if (buttonC.getSingleDebouncedPress()) {
//    stopAll(); // for re-zeroing without messing with the clock hand
//  }

  if (isRTCTriggered()) {
    // Clear the screen
    lcd.clear();
    displayRTCTime();
        
  }

  if (isCheckTideTriggered()) {
    lcd.clear();
    lcd.print(getTimeToNextTideInfo());    
    lcd.gotoXY(0,1);
    lcd.print(getNextTideLevel());    
    readNextTide();

    moveToTidePosition( minsUntilNextTide(), isNextHighTide());      
  }

  runStepper();
}
