#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define test_asprintf_(dest, ...)                                                                                      \
    do                                                                                                                 \
    {                                                                                                                  \
        int len = snprintf(NULL, 0, __VA_ARGS__);                                                                      \
        *dest = malloc(len + 1);                                                                                       \
        snprintf(*dest, len + 1, __VA_ARGS__);                                                                         \
        (*dest)[len] = '\0';                                                                                           \
    } while (false)

#define test_assert(test, cleanup, ...)                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(test))                                                                                                   \
        {                                                                                                              \
            char* message;                                                                                             \
            test_asprintf_(&message, __VA_ARGS__);                                                                     \
            cleanup;                                                                                                   \
            return message;                                                                                            \
        }                                                                                                              \
    } while (false)

#define test_suite(suite)                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        char* message = suite(test_count);                                                                             \
        if (message != NULL)                                                                                           \
        {                                                                                                              \
            return message;                                                                                            \
        }                                                                                                              \
    } while (false)

#define test_run(test)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr, "Running " #test "...\n");                                                                     \
        char* message = test();                                                                                        \
        ++(*test_count);                                                                                               \
        if (message != NULL)                                                                                           \
        {                                                                                                              \
            return message;                                                                                            \
        }                                                                                                              \
    } while (false)

typedef enum
{
    TEST_VALUE_INT,
    TEST_VALUE_INT64,
    TEST_VALUE_STR,
} TestValueKind;

typedef struct
{
    TestValueKind kind;
    union {
        int i;
        int64_t i64;
        const char* s;
    } u;
} TestValue;

#define TEST_VALUE_NEW_INT(v) ((TestValue){.kind = TEST_VALUE_INT, .u = {.i = v}})
#define TEST_VALUE_NEW_INT64(v) ((TestValue){.kind = TEST_VALUE_INT64, .u = {.i64 = v}})
#define TEST_VALUE_NEW_STR(v) ((TestValue){.kind = TEST_VALUE_STR, .u = {.s = v}})
