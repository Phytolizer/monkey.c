#include "SimpleString/String.h"

#include <stdlib.h>
#include <string.h>

String StringCopy(const char* data, int length)
{
    String result = {.data = calloc(length + 1, 1), .length = length};
    memcpy(result.data, data, length);
    return result;
}

bool StringSpansEqual(StringSpan a, StringSpan b)
{
    return a.length == b.length && memcmp(a.data, b.data, a.length) == 0;
}
