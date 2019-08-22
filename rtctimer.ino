#include <Wire.h>
#include <DS1307RTC.h>
#include <TimeLib.h>

#define DS1307_CTRL_ID 0x68

#define SQ_INTERRUPT_PIN 0

int tick = 0;
int old_tick = 0;

tmElements_t tm;
char curDate[11]; 
char curTime[11]; 

void setSQW(uint8_t value) {
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write(7);
  Wire.write(value);
  Wire.endTransmission();
}
 
void handleInt() {
  tick++;
}

int getTickCount() {
  return tick;
}

bool isCheckTideTriggered() {
  if (tick > 10) { // TODO 60
    tick = 0;
    old_tick = 0;
    return true;
  } else {
    return false;
  }
}

bool isRTCTriggered() {
  if (tick != old_tick) {
    old_tick = tick;   
    return true;
  } else {
    return false;
  }
}

void setupRTC() {
  attachInterrupt(digitalPinToInterrupt(SQ_INTERRUPT_PIN),handleInt,FALLING);
  // 1Hz
  setSQW(0x10);
}

int readRTC() {
  return RTC.read(tm);
}
void displayRTCTime() {
  if (readRTC()) {
     getCurDateStr(curDate, false);
     printDate();
     // Go to the next line
     lcd.gotoXY(0, 1);
     getCurTimeStr(curTime, true);
     printTime();
  }  
}

void getCurDateStr(char* date, bool isFullYear) {  
  snprintf(date, 12, "%d%c%d%c%d", tm.Month, '/', tm.Day, '/', tmYearToCalendar(isFullYear ? tm.Year : tm.Year % 100));
}

void getCurTimeStr(char* date, bool isFullTime) {
  if (isFullTime) {
    snprintf(date, 12, "%02d%c%02d%c%02d", tm.Hour, ':', tm.Minute, ':', tm.Second);    
  } else {
    snprintf(date, 12, "%02d%c%02d", tm.Hour, ':', tm.Minute);    
  }
}

void printDate() {
    lcd.print(curDate);
}

void printTime() {
    lcd.print(curTime);
}
