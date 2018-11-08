
#ifndef UTIL_H
#define UTIL_H

#include <TGR.h>

char* concat(const char *s1, const char *s2);
void bin_dump(uint64_t u);
void dumpData(char *name, FileStruct file, int size, bool use_non, int start, int end);
void crop(char *dst, char *src, size_t mn, size_t mx);
FileStruct load(char *adress, int make);

#endif

