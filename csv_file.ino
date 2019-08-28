#include <limits.h>

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

////------------------------------------------------------------------------------
//int csvReadUint32(File* file, uint32_t* num, char delim) {
//  char buf[20];
//  char* ptr;
//  int rtn = csvReadText(file, buf, sizeof(buf), delim);
//  if (rtn < 0) return rtn;
//  *num = strtoul(buf, &ptr, 10);
//  if (buf == ptr) return -3;
//  while(isspace(*ptr)) ptr++;
//  return *ptr == 0 ? rtn : -4;
//}
//
////------------------------------------------------------------------------------
//int csvReadUint16(File* file, uint16_t* num, char delim) {
//  uint32_t tmp;
//  int rtn = csvReadUint32(file, &tmp, delim);
//  if (rtn < 0) return rtn;
//  if (tmp > UINT_MAX) return -5;
//  *num = tmp;
//  return rtn;
//}

////------------------------------------------------------------------------------
//int csvReadDouble(File* file, double* num, char delim) {
//  char buf[20];
//  char* ptr;
//  int rtn = csvReadText(file, buf, sizeof(buf), delim);
//  if (rtn < 0) return rtn;
//  *num = strtod(buf, &ptr);
//  if (buf == ptr) return -3;
//  while(isspace(*ptr)) ptr++;
//  return *ptr == 0 ? rtn : -4;
//}
//
////------------------------------------------------------------------------------
//int csvReadFloat(File* file, float* num, char delim) {
//  double tmp;
//  int rtn = csvReadDouble(file, &tmp, delim);
//  if (rtn < 0)return rtn;
//  // could test for too large.
//  *num = tmp;
//  return rtn;
//}
