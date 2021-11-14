#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define test_asprintf_(dest, ...) \
    do { int len = snprintf(NULL, 0, __VA_ARGS__); *dest = malloc(len + 1); snprintf(*dest, len + 1, __VA_ARGS__); } while (false)

#define test_assert(test, cleanup, ...)                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(test))                                                                                                   \
        {                                                                                                              \
            char* message;                                                                                             \
            test_asprintf_(&message, __VA_ARGS__);                                                                           \
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
