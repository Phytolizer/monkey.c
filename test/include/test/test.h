#ifndef TEST_TEST_H_
#define TEST_TEST_H_

#include <nonstd/asprintf.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define test_assert(Test, Cleanup, ...)       \
  do {                                        \
    if (!(Test)) {                            \
      char* message;                          \
      nonstd_asprintf(&message, __VA_ARGS__); \
      Cleanup;                                \
      return message;                         \
    }                                         \
  } while (false)

#define test_run_suite(Suite, Count)           \
  do {                                         \
    char* message = test_suite_##Suite(Count); \
    if (message != NULL) {                     \
      fprintf(stderr, "[FAIL] %s\n", message); \
      exit(EXIT_FAILURE);                      \
    }                                          \
  } while (false)

#define test_run(Test)                         \
  do {                                         \
    fprintf(stderr, "Running " #Test "...\n"); \
    char* message = test_##Test();             \
    ++(*test_count);                           \
    if (message != NULL) {                     \
      return message;                          \
    }                                          \
  } while (false)

#define test_func(Test) char* test_##Test(void)
#define test_suite_func(Suite) char* test_suite_##Suite(uint64_t* test_count)
#define test_pass() return NULL
#define test_suite_pass() return NULL

#endif  // TEST_TEST_H_
