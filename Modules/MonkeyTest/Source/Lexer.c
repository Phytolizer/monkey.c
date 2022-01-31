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
        StringSpan expectedLiteral;
    } tests[] = {
        {TokenTypeAssign, STRING_SPAN("=")},
        {TokenTypePlus, STRING_SPAN("+")},
        {TokenTypeLparen, STRING_SPAN("(")},
        {TokenTypeRparen, STRING_SPAN(")")},
        {TokenTypeLbrace, STRING_SPAN("{")},
        {TokenTypeRbrace, STRING_SPAN("}")},
        {TokenTypeComma, STRING_SPAN(",")},
        {TokenTypeSemicolon, STRING_SPAN(";")},
        {TokenTypeEof, STRING_SPAN("")},
    };
    Lexer l = LexerInit((StringSpan){.data = NextToken, .length = NextToken_LENGTH});
    for (size_t i = 0; i < sizeof tests / sizeof tests[0]; i += 1)
    {
        Token t = LexerNextToken(&l);
        TEST_ASSERT(t.type.data == tests[i].expectedType.data,
                    free(t.literal.data),
                    "tests[%zu] -- tokentype wrong. expected='%.*s', actual='%.*s'",
                    i,
                    tests[i].expectedType.length,
                    tests[i].expectedType.data,
                    t.type.length,
                    t.type.data);
        TEST_ASSERT(StringSpansEqual(STRING_AS_SPAN(t.literal), tests[i].expectedLiteral),
                    free(t.literal.data),
                    "tests[%zu] -- literal wrong. expected='%.*s', actual='%.*s'",
                    i,
                    tests[i].expectedLiteral.length,
                    tests[i].expectedLiteral.data,
                    t.literal.length,
                    t.literal.data);
        free(t.literal.data);
    }
    TEST_PASS();
}

TEST_SUITE_FUNC(Lexer)
{
    TEST_RUN(NextToken);
}
