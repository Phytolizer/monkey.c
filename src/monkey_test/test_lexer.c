#define _GNU_SOURCE
#include "monkey/lexer.h"
#include "monkey/token.h"
#include "sds.h"
#include "test.h"
#include <stdio.h>
#include <string.h>

char* test_next_token(void)
{
    const char* input = "=+(){},;";

    struct
    {
        TokenType expected_type;
        const char* expected_literal;
    } tests[] = {
        {T_ASSIGN, "="}, {T_PLUS, "+"},  {T_LPAREN, "("},    {T_RPAREN, ")"}, {T_LBRACE, "{"},
        {T_RBRACE, "}"}, {T_COMMA, ","}, {T_SEMICOLON, ";"}, {T_EOF, ""},
    };

    Lexer l;
    Lexer_init(&l, input);

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i)
    {
        Token tok = Lexer_next_token(&l);

        test_assert(strcmp(tok.type, tests[i].expected_type) == 0, "tokentype wrong");
        test_assert(strcmp(tok.literal, tests[i].expected_literal) == 0, "literal wrong");

        Token_deinit(&tok);
    }

    Lexer_deinit(&l);
    return NULL;
}

char* all_tests(size_t* test_count)
{
    test_run(test_next_token);
    return NULL;
}

int main(void)
{
    size_t test_count = 0;
    char* message = all_tests(&test_count);
    if (message != NULL)
    {
        printf("%s\n", message);
    }
    else
    {
        printf("== ALL PASSED ==\n");
    }
    printf("Tests run: %zu\n", test_count);

    return message != NULL;
}
