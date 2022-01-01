#include "string/string.h"

#include <stddef.h>
#include <string.h>
#include <vec/vec.h>

String StringFromC(const char* cstr) {
  String s = {0};
  VEC_APPEND(&s, cstr, strlen(cstr));
  return s;
}

String StringFromSpan(const char* begin, const char* end) {
  String s = {0};
  VEC_APPEND(&s, begin, end - begin);
  return s;
}

bool StringEqual(const String a, const String b) {
  return a.size == b.size && memcmp(a.data, b.data, a.size) == 0;
}
