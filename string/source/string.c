#include "string/string.h"

#include <stddef.h>
#include <string.h>
#include <vec/vec.h>

StringView StringViewFromC(const char* cstr) {
  return (StringView){.begin = cstr, .end = cstr + strlen(cstr)};
}

String StringFromC(const char* cstr) {
  String s = {0};
  VEC_APPEND(&s, cstr, strlen(cstr));
  return s;
}

String StringFromSpan(StringView span) {
  String s = {0};
  VEC_APPEND(&s, span.begin, span.end - span.begin);
  return s;
}

String StringDuplicate(const String s) {
  String result = {0};
  VEC_APPEND(&result, s.data, s.size);
  return result;
}

bool StringEqual(const String a, const String b) {
  return a.size == b.size && memcmp(a.data, b.data, a.size) == 0;
}

bool StringEqualView(const String a, StringView b) {
  return a.size == SPAN_SIZE(&b) && memcmp(a.data, b.begin, a.size) == 0;
}
