#pragma once

#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>

#define test_assert(test, cleanup, ...)                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(test))                                                                                                   \
        {                                                                                                              \
            char* message;                                                                                             \
            asprintf(&message, __VA_ARGS__);                                                                           \
            cleanup;                                                                                                   \
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
