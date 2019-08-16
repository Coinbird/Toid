#include <Wire.h>
#include <DS1307RTC.h>
#include <TimeLib.h>

#define DS1307_CTRL_ID 0x68

#define SQ_INTERRUPT_PIN 0

int tick = 0;
int old_tick = 0;

tmElements_t tm;

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
     printDate();
     // Go to the next line
     lcd.gotoXY(0, 1);
     printTime();
  }  
}

void getCurDate(char* date) {
  snprintf(date, 12, "%d%c%d%c%d", tm.Month, '/', tm.Day, '/', tmYearToCalendar(tm.Year));
}

void printDate() {
     lcd.print(tm.Month);
     lcd.print('/');
     printTwoChar(tm.Day);
     lcd.print('/');
     lcd.print(tmYearToCalendar(tm.Year) % 100);  
}

void printTime() {
     lcd.print(tm.Hour);
     lcd.print(':');
     printTwoChar(tm.Minute);
     lcd.print(':');
     printTwoChar(tm.Second);
}

void printTwoChar(uint8_t val){
  if (val < 10){
      lcd.print('0');
  }
  lcd.print(val);   
}
