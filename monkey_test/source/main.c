#include <inttypes.h>
#include <monkey/token.h>
#include <test/test.h>

#include "monkey_test/test_lexer.h"

test_suite_func(lexer_tests) {
  test_run(lexer_next_token);
  test_suite_pass();
}

int main(void) {
  uint64_t test_count = 0;
  MkTokenTypesManage(kTokenTypesInit);
  test_run_suite(lexer_tests, &test_count);
  MkTokenTypesManage(kTokenTypesFree);
  printf("[PASS] %" PRIu64 " tests\n", test_count);
  return 0;
}
