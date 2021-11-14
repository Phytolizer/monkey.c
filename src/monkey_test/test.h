#pragma once

#include <stdbool.h>

#define test_assert(test, message)                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(test))                                                                                                   \
        {                                                                                                              \
            return message;                                                                                            \
        }                                                                                                              \
    } while (false)

#define test_run(test)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        char* message = test();                                                                                        \
        ++(*test_count);                                                                                               \
        if (message != NULL)                                                                                           \
        {                                                                                                              \
            return message;                                                                                            \
        }                                                                                                              \
    } while (false)
