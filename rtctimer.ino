#include <Wire.h>
 
#define DS1307_CTRL_ID 0x68

int tick = 0;
int old_tick = 0;

void setSQW(uint8_t value) {
  Wire.beginTransmission(DS1307_CTRL_ID);
  Wire.write(7);
  Wire.write(value);
  Wire.endTransmission();
}
 
void handleInt() {
  tick++;
}

bool isRTCTriggered() {
  return tick != old_tick;
}

void setupRTC() {
//  attachInterrupt(0,handleInt,FALLING);
  // 1Hz
  //setSQW(0x10);
}

void displayRTCTime(tmElements_t& tm) {
  if (RTC.read(tm)) {
     printDate(tm);
     // Go to the next line
     lcd.gotoXY(0, 1);
     printTime(tm);
  }  
}

void printDate(const tmElements_t& tm) {
     lcd.print(tm.Month);
     lcd.print('/');
     printTwoChar(tm.Day);
     lcd.print('/');
     lcd.print(tmYearToCalendar(tm.Year) % 100);  
}

void printTime(const tmElements_t& tm) {
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
