#include "nonstd/strtok.h"
#include "nonstd/strpbrk.h"
#include "nonstd/strspn.h"
#include <stddef.h>
#include <string.h>

char* nonstd_strtok(char* str, const char* delim)
{
    static char* olds;
    return nonstd_strtok_r(str, delim, &olds);
}

char* nonstd_strtok_r(char* str, const char* delim, char** savep)
{
    if (str == NULL)
    {
        str = *savep;
    }

    str += nonstd_strspn(str, delim);
    if (*str == '\0')
    {
        *savep = str;
        return NULL;
    }

    char* token = str;
    str = nonstd_strpbrk(token, delim);
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
