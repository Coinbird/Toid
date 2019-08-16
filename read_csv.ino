#include <AStar32U4.h>
#include <SPI.h>
#include <SD.h>

#include <limits.h>

const uint8_t chipSelect = 4;

char currentTide[255] = "---";
File filePtr;

const char CSV_DELIM = ',';

void openSDFile() {
  // Initialize the SD card.
  lcd.clear();
  lcd.print("Init...");
  if (!SD.begin(chipSelect))
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

  // 1/1/2019,Tue,7:11 AM,6.31,192,L

  int16_t heightCM; 
  float heightFt;
  // Must be dim 9 to allow for zero byte.
  char dateS[11], dayS[4], timeS[9], highLow[2];

  char searchDate[20]; 
  readRTC();
  getCurDate(searchDate);
  while (filePtr.available() && strcmp(searchDate, dateS) != 0) {    
    if (csvReadText(&filePtr, dateS, sizeof(dateS), CSV_DELIM) != CSV_DELIM 
      || csvReadText(&filePtr, dayS, sizeof(dayS), CSV_DELIM) != CSV_DELIM
      || csvReadText(&filePtr, timeS, sizeof(timeS), CSV_DELIM) != CSV_DELIM    
      || csvReadFloat(&filePtr, &heightFt, CSV_DELIM) != CSV_DELIM
      || csvReadInt16(&filePtr, &heightCM, CSV_DELIM) != CSV_DELIM
      || csvReadText(&filePtr, highLow, sizeof(highLow), CSV_DELIM) != '\n') {
      sprintf(currentTide, "Read Err");      
    } else {
//      String heightS = String(heightFt, 2);
      sprintf(currentTide, timeS);          
    }
  }
  filePtr.close(); // TODO : do not close - keep file handle to seek
}

char * getCurrentTide() {
  return currentTide;
}



/*
 * Read a file one field at a time.
 *
 * file - File to read.
 *
 * str - Character array for the field.
 *
 * size - Size of str array.
 *
 * delim - csv delimiter.
 *
 * return - negative value for failure.
 *          delimiter, '\n' or zero(EOF) for success.           
 */
int csvReadText(File* file, char* str, size_t size, char delim) {
  char ch;
  int rtn;
  size_t n = 0;
  while (true) {
    // check for EOF
    if (!file->available()) {
      rtn = 0;
      break;
    }
    if (file->read(&ch, 1) != 1) {
      // read error
      rtn = -1;
      break;
    }
    // Delete CR.
    if (ch == '\r') {
      continue;
    }
    if (ch == delim || ch == '\n') {
      rtn = ch;
      break;
    }
    if ((n + 1) >= size) {
      // string too long
      rtn = -2;
      n--;
      break;
    }
    str[n++] = ch;
  }
  str[n] = '\0';
  return rtn;
}

//------------------------------------------------------------------------------
int csvReadInt32(File* file, int32_t* num, char delim) {
  char buf[20];
  char* ptr;
  int rtn = csvReadText(file, buf, sizeof(buf), delim);
  if (rtn < 0) return rtn;
  *num = strtol(buf, &ptr, 10);
  if (buf == ptr) return -3;
  while(isspace(*ptr)) ptr++;
  return *ptr == 0 ? rtn : -4;
}

//------------------------------------------------------------------------------
int csvReadInt16(File* file, int16_t* num, char delim) {
  int32_t tmp;
  int rtn = csvReadInt32(file, &tmp, delim);
  if (rtn < 0) return rtn;
  if (tmp < INT_MIN || tmp > INT_MAX) return -5;
  *num = tmp;
  return rtn;
}

//------------------------------------------------------------------------------
int csvReadUint32(File* file, uint32_t* num, char delim) {
  char buf[20];
  char* ptr;
  int rtn = csvReadText(file, buf, sizeof(buf), delim);
  if (rtn < 0) return rtn;
  *num = strtoul(buf, &ptr, 10);
  if (buf == ptr) return -3;
  while(isspace(*ptr)) ptr++;
  return *ptr == 0 ? rtn : -4;
}

//------------------------------------------------------------------------------
int csvReadUint16(File* file, uint16_t* num, char delim) {
  uint32_t tmp;
  int rtn = csvReadUint32(file, &tmp, delim);
  if (rtn < 0) return rtn;
  if (tmp > UINT_MAX) return -5;
  *num = tmp;
  return rtn;
}

//------------------------------------------------------------------------------
int csvReadDouble(File* file, double* num, char delim) {
  char buf[20];
  char* ptr;
  int rtn = csvReadText(file, buf, sizeof(buf), delim);
  if (rtn < 0) return rtn;
  *num = strtod(buf, &ptr);
  if (buf == ptr) return -3;
  while(isspace(*ptr)) ptr++;
  return *ptr == 0 ? rtn : -4;
}

//------------------------------------------------------------------------------
int csvReadFloat(File* file, float* num, char delim) {
  double tmp;
  int rtn = csvReadDouble(file, &tmp, delim);
  if (rtn < 0)return rtn;
  // could test for too large.
  *num = tmp;
  return rtn;
}
