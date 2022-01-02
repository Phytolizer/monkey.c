#include "nonstd/strtok.h"
#include "nonstd/strpbrk.h"
#include "nonstd/strspn.h"
#include <stddef.h>
#include <string.h>

char* NonstdStrtok(char* str, const char* delim)
{
    static char* olds;
    return NonstdStrtokR(str, delim, &olds);
}

char* NonstdStrtokR(char* str, const char* delim, char** savep)
{
    if (str == NULL)
    {
        str = *savep;
    }

    str += NonstdStrspn(str, delim);
    if (*str == '\0')
    {
        *savep = str;
        return NULL;
    }

    char* token = str;
    str = NonstdStrpbrk(token, delim);
    if (str == NULL)
    {
        *savep = strchr(token, '\0');
    }
    else
    {
        *str = '\0';
        *savep = str + 1;
    }
    return token;
}
