#include <Wire.h>
#include "rtctimer.h"
#include "csv_file.h"

// #include <TimeLib.h>

int tick = 0;
int old_tick = 0;

DateTime tm;
char curDate[11];
char curTime[11];

RTC_DS3231 rtc;

void handleInt()
{
  tick++;
}

int getTickCount()
{
  return tick;
}

bool isCheckTideTriggered()
{
  if (tick > 10)
  { // TODO 60?
    tick = 0;
    old_tick = 0;
    return true;
  }
  else
  {
    return false;
  }
}

bool isRTCTriggered()
{
  if (tick != old_tick)
  {
    old_tick = tick;
    return true;
  }
  else
  {
    return false;
  }
}

void setupRTC()
{
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC3231.");
    Serial.flush();
    while (1)
      delay(10);
  }

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, setting time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  attachInterrupt(digitalPinToInterrupt(SQ_INTERRUPT_PIN), handleInt, FALLING);
  // 1Hz
  rtc.writeSqwPinMode(DS3231_SquareWave1Hz);
}

void readRTC()
{
  tm = rtc.now();
}

uint8_t getHour()
{
  return tm.hour();
}

uint8_t getMinute()
{
  return tm.minute();
}

void displayRTCTime(AStar32U4LCD *lcd)
{
  readRTC();
  getCurDateStr(curDate, true);
  lcd->print(curDate);
  // Go to the next linei
  lcd->gotoXY(0, 1);
  getCurTimeStr(curTime);
  lcd->print(curTime);
}

void getCurDateStr(char *date, bool short_date)
{
  if (short_date) {
    char yearbuf[] = "MM/DD/YY";
    snprintf(date, 9, tm.toString(yearbuf));
  } else {
    // Outputs with zero padding: 01/01/2019
    char yearbuf[] = "MM/DD/YYYY";
    snprintf(date, 11, tm.toString(yearbuf));
  }
}

void getCurTimeStr(char *date)
{
  char timebuf[] = "hh:mm:ss";
  snprintf(date, 9, tm.toString(timebuf));
}
