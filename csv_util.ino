#include <AStar32U4.h>
#include <SPI.h>
#include <SD.h>

#define CHIPSELECT 4

char nextTide[10];
char nextTideInfo[10];
uint8_t tideHour = 0;
uint8_t tideMinute = 0;

uint8_t curHour = 0;
uint8_t curMinute = 0;

// tide load states for next days
bool initialLoadTide = false; 
bool pendingNextDayTide = false;


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
  // Assumes exact format:
  // HH:MM PM
  hour = atoi(timeS);
  min = atoi(timeS + 2); 
  char* am_pm = timeS + 5;
  hour += bool(strcmp(am_pm, "PM") == 0) ? 12 : 0; // 24 hour time
  // 24h - if PM then add 12    
}

char * getNextTide() {
  return nextTide;
}

char * getTimeToNextTideInfo() {
     // "04 94 L"
    sprintf(nextTideInfo, "%d %d %d", tideHour, tideMinute, minsUntilNextTide()); //isCurrentHighTide ? "Hi" : "Lo");
    return nextTideInfo;
}

void readNextTide() {
    // 1/1/2019,Tue,7:11 AM,6.31,192,L

  int16_t heightCM; 
  float heightFt;
  // Must be dimmed to allow for zero byte.
  char dateS[11], dayS[4], timeS[9], highLow[2];

  char searchDate[11]; 
  readRTC();

  bool loadedCurrentTide = false;
  // get current time
  curHour = getHour();
  curMinute = getMinute();

  // get current date
  getCurDateStr(searchDate, true);
  Serial.print("c ");
  Serial.print(curHour);
  Serial.print(":");
  Serial.println(curMinute);

  bool foundTime = false;
  if (pendingNextDayTide && strcmp(searchDate, dateS) != 0 ) {
      Serial.print("wait tmw ");
      serialPrintTide();
      return;    
  }

  if (tideHour > curHour || (tideHour == curHour && tideMinute > curMinute)) {
      Serial.print("wait til ");
      serialPrintTide();
      loadedCurrentTide = false;
      return;
  }
  serialPrintTide();
  Serial.println("Read");
  while (filePtr.available() && !foundTime) {    
    if (csvReadText(&filePtr, dateS, sizeof(dateS), CSV_DELIM) != CSV_DELIM 
      || csvReadText(&filePtr, dayS, sizeof(dayS), CSV_DELIM) != CSV_DELIM
      || csvReadText(&filePtr, timeS, sizeof(timeS), CSV_DELIM) != CSV_DELIM    
      || csvReadFloat(&filePtr, &heightFt, CSV_DELIM) != CSV_DELIM
      || csvReadInt16(&filePtr, &heightCM, CSV_DELIM) != CSV_DELIM
      || csvReadText(&filePtr, highLow, sizeof(highLow), CSV_DELIM) != '\n') {
      sprintf(nextTide, "Read Err");      
      filePtr.close();
      foundTime = true;
    } else if (strcmp(searchDate, dateS) == 0) {
      parseHourMin(timeS, tideHour, tideMinute);
      pendingNextDayTide = false;
      initialLoadTide = true; 
      if (tideHour < curHour || (tideHour == curHour && tideMinute <= curMinute)) {
        serialPrintTide();
        Serial.println("skipped");
      } else {
        if (strcmp(highLow, 'H') == 0) {
          isCurrentHighTide = true;
        } else {
          isCurrentHighTide = false;
        } 
        sprintf(nextTide, "%d %d %s", tideHour, tideMinute, highLow);
        Serial.println(nextTide);
        foundTime = true;        
        loadedCurrentTide = true;
        // TODO: refactor to LoadNextTide instead
      }
    } else if (initialLoadTide) {
      parseHourMin(timeS, tideHour, tideMinute);
      serialPrintTide();
      Serial.println(dateS);      
      Serial.println("Tomorrow Tide");      
      foundTime = true;
      pendingNextDayTide = true;
      loadedCurrentTide = true;
    }
  }
}

void serialPrintTide() {
      Serial.print("t ");
      Serial.print(tideHour);
      Serial.print(":");
      Serial.println(tideMinute);
}

int minsUntilNextTide() {

  curHour = getHour();
  curMinute = getMinute();
  if (pendingNextDayTide) {
    int curMins = (23 - curHour) * 60 + (60 - curMinute);
//    Serial.println(curMins);
    int tmwMins = (tideHour * 60) + tideMinute;
//    Serial.println(tmwMins);
    return curMins + tmwMins;
  } else {
    return (tideHour - curHour) * 60 + tideMinute - curMinute;
  }
}

bool isNextHighTide() {
  return isCurrentHighTide;
}
