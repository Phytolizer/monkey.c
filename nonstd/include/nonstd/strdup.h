#ifndef NONSTD_STRDUP_H_
#define NONSTD_STRDUP_H_

#include <stddef.h>

char* NonstdStrdup(const char* str);
char* NonstdStrndup(const char* str, size_t n);
void* NonstdMemdup(const void* mem, size_t nbytes);

#endif  // NONSTD_STRDUP_H_
