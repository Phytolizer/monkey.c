#include <inttypes.h>
#include <monkey/token.h>
#include <test/test.h>

#include "monkey_test/test_lexer.h"
#include "monkey_test/test_parser.h"

TEST_SUITE_FUNC(LexerTests) {
  TEST_RUN(LexerNextToken);
  TEST_SUITE_PASS();
}

TEST_SUITE_FUNC(ParserTests) {
  TEST_RUN(ParserLetStatements);
  TEST_SUITE_PASS();
}

int main(void) {
  uint64_t test_count = 0;
  MkTokenTypesManage(kTokenTypesInit);
  TEST_RUN_SUITE(LexerTests, &test_count);
  TEST_RUN_SUITE(ParserTests, &test_count);
  MkTokenTypesManage(kTokenTypesFree);
  printf("[PASS] %" PRIu64 " tests\n", test_count);
  return 0;
}
