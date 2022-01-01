#ifndef STRING_STRING_H_
#define STRING_STRING_H_

#include <stdbool.h>
#include <vec/vec.h>

typedef VEC_TYPE(char) String;

String StringFromC(const char* cstr);
String StringFromSpan(const char* begin, const char* end);
bool StringEqual(const String a, const String b);

#endif  // STRING_STRING_H_
