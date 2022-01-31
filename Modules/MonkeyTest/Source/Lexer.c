#include "MonkeyTest/Lexer.h"

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
        {TokenTypeLet, STRING_SPAN("let")},     {TokenTypeIdent, STRING_SPAN("five")},
        {TokenTypeAssign, STRING_SPAN("=")},    {TokenTypeInt, STRING_SPAN("5")},
        {TokenTypeSemicolon, STRING_SPAN(";")}, {TokenTypeLet, STRING_SPAN("let")},
        {TokenTypeIdent, STRING_SPAN("ten")},   {TokenTypeAssign, STRING_SPAN("=")},
        {TokenTypeInt, STRING_SPAN("10")},      {TokenTypeSemicolon, STRING_SPAN(";")},
        {TokenTypeLet, STRING_SPAN("let")},     {TokenTypeIdent, STRING_SPAN("add")},
        {TokenTypeAssign, STRING_SPAN("=")},    {TokenTypeFunction, STRING_SPAN("fn")},
        {TokenTypeLparen, STRING_SPAN("(")},    {TokenTypeIdent, STRING_SPAN("x")},
        {TokenTypeComma, STRING_SPAN(",")},     {TokenTypeIdent, STRING_SPAN("y")},
        {TokenTypeRparen, STRING_SPAN(")")},    {TokenTypeLbrace, STRING_SPAN("{")},
        {TokenTypeIdent, STRING_SPAN("x")},     {TokenTypePlus, STRING_SPAN("+")},
        {TokenTypeIdent, STRING_SPAN("y")},     {TokenTypeSemicolon, STRING_SPAN(";")},
        {TokenTypeRbrace, STRING_SPAN("}")},    {TokenTypeSemicolon, STRING_SPAN(";")},
        {TokenTypeLet, STRING_SPAN("let")},     {TokenTypeIdent, STRING_SPAN("result")},
        {TokenTypeAssign, STRING_SPAN("=")},    {TokenTypeIdent, STRING_SPAN("add")},
        {TokenTypeLparen, STRING_SPAN("(")},    {TokenTypeIdent, STRING_SPAN("five")},
        {TokenTypeComma, STRING_SPAN(",")},     {TokenTypeIdent, STRING_SPAN("ten")},
        {TokenTypeRparen, STRING_SPAN(")")},    {TokenTypeSemicolon, STRING_SPAN(";")},
        {TokenTypeBang, STRING_SPAN("!")},      {TokenTypeMinus, STRING_SPAN("-")},
        {TokenTypeSlash, STRING_SPAN("/")},     {TokenTypeAsterisk, STRING_SPAN("*")},
        {TokenTypeInt, STRING_SPAN("5")},       {TokenTypeSemicolon, STRING_SPAN(";")},
        {TokenTypeInt, STRING_SPAN("5")},       {TokenTypeLt, STRING_SPAN("<")},
        {TokenTypeInt, STRING_SPAN("10")},      {TokenTypeGt, STRING_SPAN(">")},
        {TokenTypeInt, STRING_SPAN("5")},       {TokenTypeSemicolon, STRING_SPAN(";")},
        {TokenTypeIf, STRING_SPAN("if")},       {TokenTypeLparen, STRING_SPAN("(")},
        {TokenTypeInt, STRING_SPAN("5")},       {TokenTypeLt, STRING_SPAN("<")},
        {TokenTypeInt, STRING_SPAN("10")},      {TokenTypeRparen, STRING_SPAN(")")},
        {TokenTypeLbrace, STRING_SPAN("{")},    {TokenTypeReturn, STRING_SPAN("return")},
        {TokenTypeTrue, STRING_SPAN("true")},   {TokenTypeSemicolon, STRING_SPAN(";")},
        {TokenTypeRbrace, STRING_SPAN("}")},    {TokenTypeElse, STRING_SPAN("else")},
        {TokenTypeLbrace, STRING_SPAN("{")},    {TokenTypeReturn, STRING_SPAN("return")},
        {TokenTypeFalse, STRING_SPAN("false")}, {TokenTypeSemicolon, STRING_SPAN(";")},
        {TokenTypeRbrace, STRING_SPAN("}")},    {TokenTypeInt, STRING_SPAN("10")},
        {TokenTypeEq, STRING_SPAN("==")},       {TokenTypeInt, STRING_SPAN("10")},
        {TokenTypeSemicolon, STRING_SPAN(";")}, {TokenTypeInt, STRING_SPAN("10")},
        {TokenTypeNotEq, STRING_SPAN("!=")},    {TokenTypeInt, STRING_SPAN("9")},
        {TokenTypeSemicolon, STRING_SPAN(";")}, {TokenTypeEof, STRING_SPAN("")},
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
