#include <Monkey/Lexer.h>
#include <Monkey/Token.h>
#include <MonkeyTest/Input/Lexer/NextToken.h>
#include <TestFramework/Test.h>
#include <string.h>

static TEST_FUNC(NextToken)
{
    struct
    {
        TokenType expectedType;
        const char* expectedLiteral;
    } tests[] = {
        {TokenTypeAssign, "="},
        {TokenTypePlus, "+"},
        {TokenTypeLparen, "("},
        {TokenTypeRparen, ")"},
        {TokenTypeLbrace, "{"},
        {TokenTypeRbrace, "}"},
        {TokenTypeComma, ","},
        {TokenTypeSemicolon, ";"},
        {TokenTypeEof, ""},
    };
    Lexer l = LexerInit(NextToken, NextToken_LENGTH);
    for (size_t i = 0; i < sizeof tests / sizeof tests[0]; i += 1)
    {
        Token t = LexerNextToken(&l);
        TEST_ASSERT(t.type.data == tests[i].expectedType.data,
                    (void)0,
                    "tests[%zu] -- tokentype wrong. expected='%.*s', actual='%.*s'",
                    i,
                    tests[i].expectedType.len,
                    tests[i].expectedType.data,
                    t.type.len,
                    t.type.data);
        TEST_ASSERT(strcmp(t.literal, tests[i].expectedLiteral) == 0,
                    (void)0,
                    "tests[%zu] -- literal wrong. expected='%s', actual='%s'",
                    i,
                    tests[i].expectedLiteral,
                    t.literal);
    }
    TEST_PASS();
}

TEST_SUITE_FUNC(Lexer)
{
    TEST_RUN(NextToken);
}
