#include "SimpleString/String.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

String StringCopy(const char* data, int length)
{
    String result = {.data = calloc(length + 1, 1), .length = length};
    memcpy(result.data, data, length);
    return result;
}

String StringDuplicate(StringSpan s)
{
    return StringCopy(s.data, s.length);
}

String StringSubstring(StringSpan str, int start, int end)
{
    String result = {.data = calloc(end - start + 1, 1), .length = end - start};
    memcpy(result.data, str.data + start, end - start);
    return result;
}

String StringPrintf(FORMAT_STRING(const char* fmt), ...)
{
    va_list args;
    va_start(args, fmt);
    int length = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    if (length < 0)
    {
        return (String){0};
    }

    String result = {.data = calloc(length + 1, 1), .length = length};
    va_start(args, fmt);
    vsnprintf(result.data, length + 1, fmt, args);
    va_end(args);

    return result;
}

bool StringSpansEqual(StringSpan a, StringSpan b)
{
    return a.length == b.length && memcmp(a.data, b.data, a.length) == 0;
}
