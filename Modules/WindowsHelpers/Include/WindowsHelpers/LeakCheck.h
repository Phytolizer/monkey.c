#pragma once

#ifdef _WIN32

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
// Order matters here.
// (https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2022#enable-memory-leak-detection)
#include <crtdbg.h>

#define LEAK_SET_STDERR()                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);                                                               \
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);                                                             \
    } while (0)

#define LEAK_CHECK() _CrtDumpMemoryLeaks()

#else

#define LEAK_SET_STDERR() (void)0
#define LEAK_CHECK() 0

#endif