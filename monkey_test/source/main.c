#include <inttypes.h>
#include <monkey/token.h>
#include <test/test.h>

#include "monkey_test/test_lexer.h"
#include "monkey_test/test_parser.h"

test_suite_func(lexer_tests) {
  test_run(lexer_next_token);
  test_suite_pass();
}

test_suite_func(parser_tests) {
  test_run(parser_let_statements);
  test_suite_pass();
}

int main(void) {
  uint64_t test_count = 0;
  MkTokenTypesManage(kTokenTypesInit);
  test_run_suite(lexer_tests, &test_count);
  test_run_suite(parser_tests, &test_count);
  MkTokenTypesManage(kTokenTypesFree);
  printf("[PASS] %" PRIu64 " tests\n", test_count);
  return 0;
}
