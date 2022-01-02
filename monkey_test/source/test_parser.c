#include "monkey_test/test_parser.h"

#include <inttypes.h>
#include <monkey/ast.h>
#include <monkey/lexer.h>
#include <monkey/parser.h>
#include <monkey/token.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string/string.h>

TEST_SUBTEST_FUNC(TestLetStatement,
                  MkAstLetStatement* statement,
                  const char* expected_name);
TEST_SUBTEST_FUNC(CheckParserErrors, MkParser parser);

TEST_FUNC(ParserLetStatements) {
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
    TEST_ASSERT(
        program != NULL,
        do {
          MkParserFree(parser);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        "tests[%" PRIu64 "]: program is null", i);
    TEST_RUN_SUBTEST(
        CheckParserErrors,
        do {
          MkAstNodeFree(&program->base);
          free(program);
          MkParserFree(parser);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        parser);
    TEST_ASSERT(
        program->statements.size == 1,
        do {
          MkAstNodeFree(&program->base);
          free(program);
          MkParserFree(parser);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        "tests[%" PRIu64 "]: program->statements.size != 1", i);
    MkAstStatement* statement = program->statements.data[0];
    TEST_ASSERT(
        statement->type == kMkAstStatementLet,
        do {
          MkAstNodeFree(&program->base);
          free(program);
          MkParserFree(parser);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        "tests[%" PRIu64 "]: statement->type != Let", i);
    TEST_RUN_SUBTEST(
        TestLetStatement,
        do {
          MkAstNodeFree(&program->base);
          free(program);
          MkParserFree(parser);
          MkTokenTypesManage(kTokenTypesFree);
        } while (false),
        (MkAstLetStatement*)statement, tests[i].expected_identifier);
    MkAstNodeFree(&program->base);
    free(program);
    MkParserFree(parser);
  }
  TEST_PASS();
}

TEST_SUBTEST_FUNC(TestLetStatement,
                  MkAstLetStatement* statement,
                  const char* expected_name) {
  String toklit = MkAstNodeTokenLiteral(&statement->base.base);
  TEST_ASSERT(StringEqualView(toklit, StringViewFromC("let")),
              VEC_FREE(&toklit), "statement TokenLiteral != 'let'");
  VEC_FREE(&toklit);
  TEST_ASSERT(
      StringEqualView(statement->name.value, StringViewFromC(expected_name)),
      (void)0, "statement name.value != '%s'", expected_name);
  toklit = MkAstNodeTokenLiteral(&statement->name.base.base);
  TEST_ASSERT(StringEqualView(toklit, StringViewFromC(expected_name)),
              VEC_FREE(&toklit), "statement name TokenLiteral != '%s'",
              expected_name);
  VEC_FREE(&toklit);
  TEST_PASS();
}

TEST_SUBTEST_FUNC(CheckParserErrors, MkParser parser) {
  if (parser.errors.size == 0) {
    TEST_PASS();
  }

  for (uint64_t i = 0; i < parser.errors.size; ++i) {
    String error = parser.errors.data[i];
    fprintf(stderr, "[PARSER ERROR] %" STRING_FMT "\n", STRING_PRINT(error));
  }
  TEST_FAIL("parser has %" PRIu64 " errors", parser.errors.size);
}
