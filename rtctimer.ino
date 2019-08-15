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
