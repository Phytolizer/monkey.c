#include <inttypes.h>
#include <monkey/lexer.h>
#include <monkey/token.h>
#include <next_token_test.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string/string.h>
#include <test/test.h>

test_func(lexer_next_token) {
  struct {
    MkTokenType expected_type;
    const char* expected_literal;
  } tests[] = {
      {kTokenAssign, "="}, {kTokenPlus, "+"},      {kTokenLParen, "("},
      {kTokenRParen, ")"}, {kTokenLBrace, "{"},    {kTokenRBrace, "}"},
      {kTokenComma, ","},  {kTokenSemicolon, ";"}, {kTokenEof, ""},
  };

  MkLexer l = {0};
  MkLexerInit(&l, (StringView){
                      .begin = next_token_test,
                      .end = next_token_test + next_token_test_size,
                  });
  for (uint64_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    MkToken t = MkLexerNextToken(&l);
    test_assert(
        StringEqual(t.type, tests[i].expected_type),
        do {
          MkTokenFree(t);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        "tests[%" PRIu64 "].expected_type: %" STRING_FMT
        ", t.type: %" STRING_FMT,
        i, STRING_PRINT(tests[i].expected_type), STRING_PRINT(t.type));
    test_assert(
        StringEqualView(t.literal, StringViewFromC(tests[i].expected_literal)),
        do {
          MkTokenFree(t);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        "tests[%" PRIu64 "].expected_literal: %s, t.literal: %" STRING_FMT, i,
        tests[i].expected_literal, STRING_PRINT(t.literal));
  }
  test_pass();
}

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
