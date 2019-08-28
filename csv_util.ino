#include <AStar32U4.h>
#include <SPI.h>
#include <SD.h>

#define CHIPSELECT 4

char nextTideInfo[10];
char nextTideLevel[10];
char tideLevel[6];

uint8_t tideHour = 0;
uint8_t tideMinute = 0;

uint8_t curHour = 0;
uint8_t curMinute = 0;

char curTideDate[11];

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
    return tideLevel;
}

void readNextTide() {
  // from https://tidesandcurrents.noaa.gov/noaatideannual.html in 24 hour Clock export as TXT
  // Date     Day Time  Pred(Ft)  Pred(cm)  High/Low
  // Converted and exported as CSV in Google Sheets in this format:
  // 01/01/2019,Tue,07:11,6.31,192,L

  int16_t heightCM; 
  // Must be dimmed to allow for zero byte.
  char dayS[4], timeS[6], highLow[2];
  char searchDate[11]; 
  
  readRTC();  
  // get current time
  curHour = getHour();
  curMinute = getMinute();
  // get current date
  getCurDateStr(searchDate, true);

  bool foundTime = false;
  if (pendingNextDayTide && strcmp(searchDate, curTideDate) != 0 ) {
      serialPrintTide();
      Serial.println("Wait until date:");
      Serial.println(curTideDate); 
      return;    
  } 
  
  if (tideHour > curHour || (tideHour == curHour && tideMinute > curMinute)) {
      Serial.print("Waiting until ");
      serialPrintTide();
      return;
  }
  
  if (filePtr.available()) {
    Serial.println("Reading from file...");  
  }
  
  // Read 1 or more lines from file
  while (filePtr.available() && !foundTime) {    
    if (csvReadText(&filePtr, curTideDate, sizeof(curTideDate), CSV_DELIM) != CSV_DELIM 
      || csvReadText(&filePtr, dayS, sizeof(dayS), CSV_DELIM) != CSV_DELIM
      || csvReadText(&filePtr, timeS, sizeof(timeS), CSV_DELIM) != CSV_DELIM    
      || csvReadText(&filePtr, tideLevel, sizeof(tideLevel), CSV_DELIM) != CSV_DELIM
      || csvReadInt16(&filePtr, &heightCM, CSV_DELIM) != CSV_DELIM
      || csvReadText(&filePtr, highLow, sizeof(highLow), CSV_DELIM) != '\n') {
      Serial.println("Read Error - EOF/ RTC err?");
      filePtr.close();
      while(true) {delay(100);}
    } else if (strcmp(searchDate, curTideDate) == 0) { // Read OK, this line matches our date
      parseHourMin(timeS, tideHour, tideMinute);
      pendingNextDayTide = false;
      initialLoadTide = true; // Found first date that matches (initial load)
      if (tideHour < curHour || (tideHour == curHour && tideMinute <= curMinute)) {
        Serial.print("Skipped ");
        serialPrintTide();
      } else {
        setHiLoTide(highLow);
        getTimeToNextTideInfo();
        foundTime = true;        
      }
    } else if (initialLoadTide) { // we have already loaded the first matching date, so we can assume this is the next tide
      Serial.println("Loading tmw tide");      
      parseHourMin(timeS, tideHour, tideMinute);
      serialPrintTide();
      setHiLoTide(highLow);
      getTimeToNextTideInfo();
      foundTime = true;
      pendingNextDayTide = true;
    }
  } 
}

void setHiLoTide(char * highLow) {
  if (strncmp(highLow, "H", 1) == 0) {
    isCurrentHighTide = true;
  } else {
    isCurrentHighTide = false;
  }
}

void serialPrintTide() {
  char* strTide = "t xx:yy Lo"; 
  snprintf(strTide, 10, "t %02d:%02d", tideHour, tideMinute, tideLevel);
  Serial.println(strTide);
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
