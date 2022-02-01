#pragma once

#include <WindowsHelpers/LeakCheck.h>
// The above always comes first!

#include <stdbool.h>

typedef struct
{
    char* data;
    int length;
} String;

typedef struct
{
    const char* data;
    int length;
} StringSpan;

#define STRING_SPAN(x)                                                                                                 \
    (StringSpan)                                                                                                       \
    {                                                                                                                  \
        .data = x, .length = sizeof x - 1                                                                              \
    }

#define STRING_SPAN_INIT(x)                                                                                            \
    {                                                                                                                  \
        x, sizeof x - 1                                                                                                \
    }

#define STRING_AS_SPAN(s)                                                                                              \
    (StringSpan)                                                                                                       \
    {                                                                                                                  \
        .data = s.data, .length = s.length                                                                             \
    }

#ifdef _MSC_VER
#include <sal.h>
#define FORMAT_STRING(p) _Printf_format_string_ p
#define ATTR_PRINTF(...)
#else
#define FORMAT_STRING(p) p
#define ATTR_PRINTF(...) __attribute__((format(printf, __VA_ARGS__)))
#endif

String StringCopy(const char* data, int length);
String StringDuplicate(StringSpan s);
String StringSubstring(StringSpan str, int start, int end);
String StringPrintf(FORMAT_STRING(const char* fmt), ...) ATTR_PRINTF(1, 2);
bool StringSpansEqual(StringSpan a, StringSpan b);
