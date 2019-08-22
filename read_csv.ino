#include <AStar32U4.h>
#include <SPI.h>
#include <SD.h>

#define CHIPSELECT 4

char currentTide[20] = "---";
uint8_t tideHour = 0;
uint8_t tideMin = 0;

uint8_t curHour = 0;
uint8_t curMinute = 0;

bool isCurrentHighTide = false;
File filePtr;

#define CSV_DELIM ','

void openSDFile() {
  // Initialize the SD card.
  lcd.clear();
  lcd.print("Init...");
  if (!SD.begin(CHIPSELECT))
  {
    lcd.clear();
    lcd.print("Init err");
    while(1){}  // done
  }

  // Open the file.
  lcd.clear();
  lcd.print("Open...");
  filePtr = SD.open("tides.csv");
  if (!filePtr)
  {
    lcd.clear();
    lcd.print("File Err");
    while(1){}  // done
  }

  readNextTide();
}

void parseHourMin(char* timeS, uint8_t& hour, uint8_t& min) {
  // Convert to 24-hour time
  // Assumes EXACT format:
  // HH:MM PM
  hour = atoi(timeS);
  min = atoi(timeS + 2); 
  char* am_pm = timeS + 5;
  hour += bool(strcmp(am_pm, "PM") == 0) ? 12 : 0; // 24 hour time
  // 24h - if PM then add 12    
}

char * getCurrentTide() {
  return currentTide;
}

void readNextTide() {
    // 1/1/2019,Tue,7:11 AM,6.31,192,L

  int16_t heightCM; 
  float heightFt;
  // Must be dimmed to allow for zero byte.
  char dateS[11], dayS[4], timeS[9], highLow[2];

  char searchDate[11]; 
  readRTC();

  // get current time
  curHour = getHour();
  curMinute = getMinute();

  // get current date
  getCurDateStr(searchDate, true);

  bool foundTime = false;
  while (filePtr.available() && !foundTime) {    
    if (csvReadText(&filePtr, dateS, sizeof(dateS), CSV_DELIM) != CSV_DELIM 
      || csvReadText(&filePtr, dayS, sizeof(dayS), CSV_DELIM) != CSV_DELIM
      || csvReadText(&filePtr, timeS, sizeof(timeS), CSV_DELIM) != CSV_DELIM    
      || csvReadFloat(&filePtr, &heightFt, CSV_DELIM) != CSV_DELIM
      || csvReadInt16(&filePtr, &heightCM, CSV_DELIM) != CSV_DELIM
      || csvReadText(&filePtr, highLow, sizeof(highLow), CSV_DELIM) != '\n') {
      sprintf(currentTide, "Read Err");      
      foundTime = true;
    } else if (strcmp(searchDate, dateS) != 0) {
      parseHourMin(timeS, tideHour, tideMin);
      if (tideHour <= curHour && tideMin <= curMinute) {
        
      } else {
        if (strcmp(highLow, 'H') == 0) {
          isCurrentHighTide = true;
        } else {
          isCurrentHighTide = false;
        } 
        sprintf(currentTide, "%d %d %s", tideHour, tideMin, highLow);
        foundTime = true;        
      }
    }
  }
  filePtr.close(); // TODO : do not close - keep file handle to seek
}
