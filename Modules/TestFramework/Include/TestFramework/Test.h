#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/// TestState simply tracks some testing statistics.
/// They can be dumped to the console with `TEST_SUMMARY`.
typedef struct
{
    size_t numTests;
    size_t numFailures;
    size_t numAssertions;
} TestState;

/// TestResult is used internally and has no public interface.
typedef struct
{
    bool success;
    char* message;
    int messageLength;
} TestResult;

/// Declare or define a test.
#define TEST_FUNC(Name) TestResult TestFunc##Name(TestState* testState)
/// Declare or define a test suite.
#define TEST_SUITE_FUNC(Name) void TestSuiteFunc##Name(TestState* testState)
/// Declare or define a subtest.
/// Subtests are useful for avoiding repetition in your test code, and may accept arbitrary arguments.
#define TEST_SUBTEST_FUNC(Name, ...) TestResult TestSubtestFunc##Name(TestState* testState, __VA_ARGS__)

/// Run a test suite. (Self-explanatory.)
#define TEST_RUN_SUITE(Name, State)                                                                                    \
    do                                                                                                                 \
    {                                                                                                                  \
        fputs("[SUITE] " #Name "\n", stderr);                                                                          \
        TestSuiteFunc##Name(State);                                                                                    \
    } while (0)

/// Run a test.
/// This should only be called from within a `TEST_SUITE_FUNC`.
#define TEST_RUN(Name)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        fputs("[TEST] " #Name "\n", stderr);                                                                           \
        TestResult result = TestFunc##Name(testState);                                                                 \
        if (!result.success)                                                                                           \
        {                                                                                                              \
            testState->numFailures += 1;                                                                               \
            fprintf(stderr, "[FAIL] " #Name ": %.*s\n", result.messageLength, result.message);                         \
            free(result.message);                                                                                      \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            fputs("[PASS] " #Name "\n", stderr);                                                                       \
        }                                                                                                              \
        testState->numTests += 1;                                                                                      \
    } while (0)

/// Run a sub-test.
/// A sub-test is a function that may call `TEST_ASSERT`, and also accept arbitrary arguments.
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

/// Pass the test.
/// This is typically called at the end of every `TEST_FUNC`.
#define TEST_PASS()                                                                                                    \
    return (TestResult)                                                                                                \
    {                                                                                                                  \
        .success = true, .message = NULL, .messageLength = 0                                                           \
    }

/// Perform an assertion.
/// The `Condition` will be checked for truthiness.
/// `CleanupOnFailure` will only be executed if the assertion fails.
/// It is only executed after formatting the failure message, so you may include allocated resources in that message.
/// The `...` arguments are passed directly to a `printf`-style function.
/// Be aware that they are evaluated multiple times!
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

/// Dump a test summary to stderr.
/// This can be called after running all suites for a user-friendly summary.
#define TEST_SUMMARY(State)                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr,                                                                                                \
                "[SUMMARY] %zu tests, %zu failures, %zu assertions\n",                                                 \
                (State)->numTests,                                                                                     \
                (State)->numFailures,                                                                                  \
                (State)->numAssertions);                                                                               \
    } while (0)
