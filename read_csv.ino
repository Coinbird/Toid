#include <AStar32U4.h>
#include <SPI.h>
#include <SD.h>

const uint8_t chipSelect = 4;

char currentTide[255] = "ASS";

void openSDFile() {
  // Initialize the SD card.
  lcd.clear();
  lcd.print("Init...");
  if (!SD.begin(chipSelect))
  {
    lcd.clear();
    lcd.print("Init err");
    while(1){}  // done
  }

  // Open the file.
  lcd.clear();
  lcd.print("Open...");
  File file = SD.open("tides.csv");
  if (!file)
  {
    lcd.clear();
    lcd.print("File Err");
    while(1){}  // done
  }

  file.close();
}

char * getCurrentTide() {
  return currentTide;
}
