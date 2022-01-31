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

#define STRING_AS_SPAN(s)                                                                                              \
    (StringSpan)                                                                                                       \
    {                                                                                                                  \
        .data = s.data, .length = s.length                                                                             \
    }

String StringCopy(const char* data, int length);
bool StringSpansEqual(StringSpan a, StringSpan b);
