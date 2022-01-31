#include "MonkeyRepl/WhoAmI.h"

#ifdef _WIN32
#include <Windows.h>
#include <lmcons.h>
#define SECURITY_WIN32
#include <security.h>
#else
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#endif

String WhoAmI(void)
{
    int nameSizeGuess = 32;
    String userName = {.data = calloc(nameSizeGuess, 1), .length = 0};
#ifdef _WIN32
    ULONG nameSize = nameSizeGuess;
    BOOLEAN ret = GetUserNameEx(NameDisplay, userName.data, &nameSize);
    if (ret == 0)
    {
        DWORD error = GetLastError();
        switch (error)
        {
            case ERROR_MORE_DATA:
                free(userName.data);
                userName.data = calloc(nameSize + 1, 1);
                ret = GetUserNameEx(NameDisplay, userName.data, &nameSize);
                break;
            case ERROR_NO_SUCH_DOMAIN:
                free(userName.data);
                userName.data = NULL;
                userName.length = 0;
                return userName;
            case ERROR_NONE_MAPPED: {
                DWORD nameSize = UNLEN + 1;
                free(userName.data);
                userName.data = calloc(nameSize, 1);
                ret = GetUserName(userName.data, &nameSize);
                break;
            }
        }
        if (ret == 0)
        {
            free(userName.data);
            userName.data = NULL;
            userName.length = 0;
        }
    }
    userName.length = nameSize;
#else
    while (true)
    {
        int ret = getlogin_r(userName.data, nameSizeGuess);
        if (ret == 0)
        {
            break;
        }
        if (errno != ERANGE)
        {
            free(userName.data);
            userName.data = NULL;
            userName.length = 0;
            return userName;
        }
        nameSizeGuess *= 2;
        free(userName.data);
        userName.data = calloc(nameSizeGuess, 1);
    }
    userName.length = strlen(userName.data);
#endif
    return userName;
}