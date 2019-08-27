#include <AStar32U4.h>
#include <SPI.h>
#include <SD.h>

#define CHIPSELECT 4

char nextTideInfo[10];
char nextTideLevel[10];

uint8_t tideHour = 0;
uint8_t tideMinute = 0;
float tideLevel = 0.0;

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

  Serial.println("tides.csv open");
  readNextTide();
}

void parseHourMin(char* timeS, uint8_t& hour, uint8_t& min) {
  // Assume 24-hour time and 
  // zero-padded in this exact format:
  // HH:MM 
  hour = atoi(timeS);
  min = atoi(timeS + 3);   
}

char * getTimeToNextTideInfo() {
    sprintf(nextTideInfo, "%02d:%02d %s", tideHour, tideMinute, isCurrentHighTide ? "Hi " : "Lo");
    return nextTideInfo;
}

char * getNextTideLevel() {
    dtostrf(tideLevel,7, 2, nextTideLevel);
//    sprintf(nextTideLevel, "%.2f", ass);
    return nextTideLevel;
}

void readNextTide() {
  // from https://tidesandcurrents.noaa.gov/noaatideannual.html in 24 hour Clock export as TXT
  // Date     Day Time  Pred(Ft)  Pred(cm)  High/Low
  // Converted and exported as CSV in Google Sheets in this format:
  // 01/01/2019,Tue,07:11,6.31,192,L

  int16_t heightCM; 
  // Must be dimmed to allow for zero byte.
  char dateS[11], dayS[4], timeS[6], highLow[2];

  char searchDate[11]; 
  readRTC();

  bool loadedCurrentTide = false;
  // get current time
  curHour = getHour();
  curMinute = getMinute();


  // get current date
  getCurDateStr(searchDate, true);
//  Serial.print("c ");
//  Serial.print(curHour);
//  Serial.print(":");
//  Serial.println(curMinute);

  bool foundTime = false;
  if (pendingNextDayTide && strcmp(searchDate, dateS) != 0 ) {
      Serial.print("Wait tmw ");
      serialPrintTide();
      return;    
  }

  if (tideHour > curHour || (tideHour == curHour && tideMinute > curMinute)) {
      Serial.print("Wait ");
      serialPrintTide();
      loadedCurrentTide = false;
      return;
  }
  serialPrintTide();
  if (filePtr.available()) {
    Serial.println("Reading from file...");  
  }
  while (filePtr.available() && !foundTime) {    
    if (csvReadText(&filePtr, dateS, sizeof(dateS), CSV_DELIM) != CSV_DELIM 
      || csvReadText(&filePtr, dayS, sizeof(dayS), CSV_DELIM) != CSV_DELIM
      || csvReadText(&filePtr, timeS, sizeof(timeS), CSV_DELIM) != CSV_DELIM    
      || csvReadFloat(&filePtr, &tideLevel, CSV_DELIM) != CSV_DELIM
      || csvReadInt16(&filePtr, &heightCM, CSV_DELIM) != CSV_DELIM
      || csvReadText(&filePtr, highLow, sizeof(highLow), CSV_DELIM) != '\n') {
//      Serial.println(dateS);
//      Serial.println(dayS);
//      Serial.println(timeS);
//      Serial.println(heightFt);
//      Serial.println(heightCM);
//      Serial.println(highLow);
      Serial.println("READ ERROR - EOF?");
      filePtr.close();
      foundTime = true;
      while(true) {}
    } else if (strcmp(searchDate, dateS) == 0) {
      parseHourMin(timeS, tideHour, tideMinute);
      pendingNextDayTide = false;
      initialLoadTide = true; 
      if (tideHour < curHour || (tideHour == curHour && tideMinute <= curMinute)) {
        serialPrintTide();
        Serial.println("skipped");
      } else {
        if (strncmp(highLow, "H", 1) == 0) {
          isCurrentHighTide = true;
        } else {
          isCurrentHighTide = false;
        } 
        getTimeToNextTideInfo();
        foundTime = true;        
        loadedCurrentTide = true;
        // TODO: refactor to simplify - LoadNextTide
      }
    } else if (initialLoadTide) {
      parseHourMin(timeS, tideHour, tideMinute);
      serialPrintTide();
      Serial.println(dateS);      
      Serial.println("Tmw Tide");      
      if (strncmp(highLow, "H", 1) == 0) {
        isCurrentHighTide = true;
      } else {
        isCurrentHighTide = false;
      } 
      getTimeToNextTideInfo();
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
      Serial.println(tideLevel);
}

int minsUntilNextTide() {
  curHour = getHour();
  curMinute = getMinute();
  int totalMins;
  if (pendingNextDayTide) {
    int curMins = (23 - curHour) * 60 + (60 - curMinute);
    totalMins = curMins + (tideHour * 60) + tideMinute;
  } else {
    totalMins = (tideHour - curHour) * 60 + tideMinute - curMinute;
  }
  return totalMins > 360 ? 360 : totalMins;
}

bool isNextHighTide() {
  return isCurrentHighTide;
}
