#include <RTC3231.h>
#include <AStar32U4LCD.h>

#ifndef __RTCTIMER_H__
#define __RTCTIMER_H__

// UPDATED FOR DS3231! For DS1307, see the old DS1307 branch in the repo.
// Also required: Adafruit BusIO Library

#define SQ_INTERRUPT_PIN 0
 
void handleInt();
int getTickCount();
bool isCheckTideTriggered();
bool isRTCTriggered();
void setupRTC();
void readRTC();
uint8_t getHour();
uint8_t getMinute();

void displayRTCTime(AStar32U4LCD* lcd);
void getCurDateStr(char* date, bool short_date);
void getCurTimeStr(char* date);

#endif