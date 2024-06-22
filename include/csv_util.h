#include <Arduino.h>
#include <SD.h>
#include <AStar32U4LCD.h>

#ifndef __CSV_UTIL_H__
#define __CSV_UTIL_H__

#define CHIPSELECT 4

#define CSV_DELIM ','


void openSDFile(AStar32U4LCD* lcd);
void parseHourMin(char* timeS, uint8_t& hour, uint8_t& min);
char * getTimeToNextTideInfo();
char * getNextTideLevel();
void readNextTide();
void setHiLoTide(char * highLow);
void serialPrintTide();
int minsUntilNextTide();
bool isNextHighTide();

#endif