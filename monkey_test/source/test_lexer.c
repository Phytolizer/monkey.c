#include "monkey_test/test_lexer.h"

#include <inttypes.h>
#include <monkey/lexer.h>
#include <monkey/token.h>
#include <test/test.h>

#include "monkey_test/input/next_token_test.h"

test_func(lexer_next_token) {
  struct {
    MkTokenType expected_type;
    const char* expected_literal;
  } tests[] = {
      {mk_token_let, "let"},     {mk_token_ident, "five"},
      {mk_token_assign, "="},    {mk_token_int, "5"},
      {mk_token_semicolon, ";"}, {mk_token_let, "let"},
      {mk_token_ident, "ten"},   {mk_token_assign, "="},
      {mk_token_int, "10"},      {mk_token_semicolon, ";"},
      {mk_token_let, "let"},     {mk_token_ident, "add"},
      {mk_token_assign, "="},    {mk_token_function, "fn"},
      {mk_token_lparen, "("},    {mk_token_ident, "x"},
      {mk_token_comma, ","},     {mk_token_ident, "y"},
      {mk_token_rparen, ")"},    {mk_token_lbrace, "{"},
      {mk_token_ident, "x"},     {mk_token_plus, "+"},
      {mk_token_ident, "y"},     {mk_token_semicolon, ";"},
      {mk_token_rbrace, "}"},    {mk_token_semicolon, ";"},
      {mk_token_let, "let"},     {mk_token_ident, "result"},
      {mk_token_assign, "="},    {mk_token_ident, "add"},
      {mk_token_lparen, "("},    {mk_token_ident, "five"},
      {mk_token_comma, ","},     {mk_token_ident, "ten"},
      {mk_token_rparen, ")"},    {mk_token_semicolon, ";"},
      {mk_token_bang, "!"},      {mk_token_minus, "-"},
      {mk_token_slash, "/"},     {mk_token_asterisk, "*"},
      {mk_token_int, "5"},       {mk_token_semicolon, ";"},
      {mk_token_int, "5"},       {mk_token_lt, "<"},
      {mk_token_int, "10"},      {mk_token_gt, ">"},
      {mk_token_int, "5"},       {mk_token_semicolon, ";"},
      {mk_token_if, "if"},       {mk_token_lparen, "("},
      {mk_token_int, "5"},       {mk_token_lt, "<"},
      {mk_token_int, "10"},      {mk_token_rparen, ")"},
      {mk_token_lbrace, "{"},    {mk_token_return, "return"},
      {mk_token_true, "true"},   {mk_token_semicolon, ";"},
      {mk_token_rbrace, "}"},    {mk_token_else, "else"},
      {mk_token_lbrace, "{"},    {mk_token_return, "return"},
      {mk_token_false, "false"}, {mk_token_semicolon, ";"},
      {mk_token_rbrace, "}"},    {mk_token_int, "10"},
      {mk_token_eq, "=="},       {mk_token_int, "10"},
      {mk_token_semicolon, ";"}, {mk_token_int, "10"},
      {mk_token_not_eq, "!="},   {mk_token_int, "9"},
      {mk_token_semicolon, ";"}, {mk_token_eof, ""},
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
    MkTokenFree(t);
  }
  test_pass();
}