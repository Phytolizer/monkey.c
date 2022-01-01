#ifndef STRING_STRING_H_
#define STRING_STRING_H_

#include <span/span.h>
#include <stdbool.h>
#include <vec/vec.h>

typedef VEC_TYPE(char) String;
typedef SPAN_TYPE(char) StringView;

StringView StringViewFromC(const char* cstr);
String StringFromC(const char* cstr);
String StringFromSpan(StringView span);
bool StringEqual(const String a, const String b);
bool StringEqualView(const String a, StringView b);

#define STRING_PRINT(S) (int)(S).size, (S).data
#define STRING_FMT ".*s"

#endif  // STRING_STRING_H_
