#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    size_t numTests;
    size_t numFailures;
    size_t numAssertions;
} TestState;

typedef struct
{
    bool success;
    char* message;
    int messageLength;
} TestResult;

#define TEST_FUNC(Name) TestResult TestFunc##Name(TestState* testState)
#define TEST_SUITE_FUNC(Name) void TestSuiteFunc##Name(TestState* testState)
#define TEST_SUBTEST_FUNC(Name, ...) TestResult TestSubtestFunc##Name(TestState* testState, __VA_ARGS__)

#define TEST_RUN_SUITE(Name, State)                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr, "[SUITE] %s\n", #Name);                                                                        \
        TestSuiteFunc##Name(State);                                                                                    \
    } while (0)

#define TEST_RUN(Name)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr, "[TEST] %s\n", #Name);                                                                         \
        TestResult result = TestFunc##Name(testState);                                                                 \
        if (!result.success)                                                                                           \
        {                                                                                                              \
            testState->numFailures += 1;                                                                               \
            fprintf(stderr, "[FAIL] %.*s\n", result.messageLength, result.message);                                    \
            free(result.message);                                                                                      \
        }                                                                                                              \
        testState->numTests += 1;                                                                                      \
    } while (0)

#define TEST_RUN_SUBTEST(Name, CleanupOnFailure, ...)                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        TestResult result = TestSubtestFunc##Name(State, __VA_ARGS__);                                                 \
        if (!result.success)                                                                                           \
        {                                                                                                              \
            CleanupOnFailure;                                                                                          \
            return result;                                                                                             \
        }                                                                                                              \
    } while (0)

#define TEST_PASS()                                                                                                    \
    return (TestResult)                                                                                                \
    {                                                                                                                  \
        .success = true, .message = NULL, .messageLength = 0                                                           \
    }

#define TEST_ASSERT(Condition, CleanupOnFailure, ...)                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        testState->numAssertions += 1;                                                                                 \
        if (!(Condition))                                                                                              \
        {                                                                                                              \
            int messageLength = snprintf(NULL, 0, __VA_ARGS__);                                                        \
            char* message = calloc(messageLength + 1, 1);                                                              \
            snprintf(message, messageLength, __VA_ARGS__);                                                             \
            CleanupOnFailure;                                                                                          \
            return (TestResult){.success = false, .message = message, .messageLength = messageLength};                 \
        }                                                                                                              \
    } while (0)