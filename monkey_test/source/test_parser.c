#include "monkey_test/test_parser.h"

#include <inttypes.h>
#include <monkey/ast.h>
#include <monkey/lexer.h>
#include <monkey/parser.h>
#include <monkey/token.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <string/string.h>

test_subtest_func(TestLetStatement,
                  MkAstLetStatement* statement,
                  const char* expected_name);

test_func(parser_let_statements) {
  struct {
    const char* input;
    const char* expected_identifier;
  } tests[] = {
      {"let x = 5;", "x"},
      {"let y = true;", "y"},
      {"let foobar = y;", "foobar"},
  };

  for (uint64_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
    MkLexer lexer = {0};
    MkLexerInit(&lexer, StringViewFromC(tests[i].input));
    MkParser parser = {0};
    MkParserInit(&parser, lexer);
    MkAstProgram* program = MkParserParseProgram(&parser);
    test_assert(
        program != NULL,
        do {
          MkParserFree(parser);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        "tests[%" PRIu64 "]: program is null", i);
    test_assert(
        program->statements.size == 1,
        do {
          MkAstNodeFree(&program->base);
          MkParserFree(parser);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        "tests[%" PRIu64 "]: program->statements.size != 1", i);
    MkAstStatement* statement = program->statements.data[0];
    test_assert(
        statement->type == kMkAstStatementLet,
        do {
          MkAstNodeFree(&program->base);
          MkParserFree(parser);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        "tests[%" PRIu64 "]: statement->type != Let", i);
    test_run_subtest(
        TestLetStatement,
        do {
          MkAstNodeFree(&program->base);
          MkParserFree(parser);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        (MkAstLetStatement*)statement, tests[i].expected_identifier);
  }
  test_pass();
}

test_subtest_func(TestLetStatement,
                  MkAstLetStatement* statement,
                  const char* expected_name) {
  String toklit = MkAstNodeTokenLiteral(&statement->base.base);
  test_assert(StringEqualView(toklit, StringViewFromC("let")),
              VEC_FREE(&toklit), "statement TokenLiteral != 'let'");
  VEC_FREE(&toklit);
  test_assert(
      StringEqualView(statement->name.value, StringViewFromC(expected_name)),
      (void)0, "statement name.value != '%s'", expected_name);
  toklit = MkAstNodeTokenLiteral(&statement->name.base.base);
  test_assert(StringEqualView(toklit, StringViewFromC(expected_name)),
              VEC_FREE(&toklit), "statement name TokenLiteral != '%s'",
              expected_name);
  VEC_FREE(&toklit);
  test_pass();
}
