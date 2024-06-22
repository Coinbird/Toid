#include <SD.h>

#ifndef __CSV_FILE_H__
#define __CSV_FILE_H__


int csvReadText(File* file, char* str, size_t size, char delim);
int csvReadInt32(File* file, int32_t* num, char delim);
int csvReadInt16(File* file, int16_t* num, char delim);

#endif