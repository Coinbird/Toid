  #include <AStar32U4.h>

#include <SD.h>

AStar32U4LCD lcd;

AStar32U4ButtonB buttonB;
AStar32U4ButtonC buttonC;

uint8_t displayMode = 0;

void setup()
{
  Serial.begin(115200);
  uint8_t tmp = 0;
  for (tmp = 0; tmp < 255 && !Serial; tmp++) { // wait for serial, or time out
   delay(1); 
  }
  delay(200);  
  openSDFile(); // open and load current date row
  setupRTC();
  setupStepper();
}

void loop()
{
  if (buttonB.getSingleDebouncedPress()) {
    moveEighth(); // for re-zeroing without messing with the clock hand
  }
  if (buttonC.getSingleDebouncedPress()) {
    stopAll(); // for re-zeroing without messing with the clock hand
  }

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
