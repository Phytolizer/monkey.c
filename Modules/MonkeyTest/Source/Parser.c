#include "MonkeyTest/Parser.h"

#include "Monkey/Ast.h"
#include "SimpleString/String.h"

#include <Monkey/Lexer.h>
#include <Monkey/Parser.h>
#include <TestFramework/Test.h>

static TEST_FUNC(LetStatements);
static TEST_SUBTEST_FUNC(TestLetStatement, Statement* s, StringSpan name);

TEST_SUITE_FUNC(Parser)
{
    TEST_RUN(LetStatements);
}

static TEST_FUNC(LetStatements)
{
    const char input[] = "let x = 5;\nlet y = 10;\nlet foobar = 838383;";
    Lexer l = LexerInit(STRING_SPAN(input));
    Parser p = ParserInit(&l);

    Program* program = ParseProgram(&p);
    TEST_ASSERT(program != NULL, ParserDeinit(&p), "ParseProgram returned NULL");
    TEST_ASSERT(program->statementsLength == 3,
                ParserDeinit(&p),
                "program->statementsLength != 3. got=%d",
                program->statementsLength);

    struct
    {
        StringSpan expectedIdentifier;
    } tests[] = {
        {STRING_SPAN("x")},
        {STRING_SPAN("y")},
        {STRING_SPAN("foobar")},
    };

    for (size_t i = 0; i < sizeof tests / sizeof tests[0]; i += 1)
    {
        Statement* stmt = program->statements[i];
        TEST_RUN_SUBTEST(
            TestLetStatement,
            {
                ParserDeinit(&p);
                ProgramDeinit(program);
                free(program);
            },
            stmt,
            tests[i].expectedIdentifier);
    }

    ParserDeinit(&p);
    ProgramDeinit(program);
    free(program);
    TEST_PASS();
}

static TEST_SUBTEST_FUNC(TestLetStatement, Statement* s, StringSpan name)
{
    String tokenLiteral = StatementTokenLiteral(s);
    TEST_ASSERT(StringSpansEqual(STRING_AS_SPAN(tokenLiteral), STRING_SPAN("let")),
                free(tokenLiteral.data),
                "tokenLiteral not 'let'. got='%.*s'",
                tokenLiteral.length,
                tokenLiteral.data);
    free(tokenLiteral.data);

    StringSpan actualType = StatementTypeName(s->type);
    TEST_ASSERT(s->type == StatementTypeLet,
                TEST_CLEANUP_NONE,
                "s not LetStatement. got=%.*s",
                actualType.length,
                actualType.data);

    TEST_ASSERT(StringSpansEqual(STRING_AS_SPAN(s->letStatement.name->value), name),
                TEST_CLEANUP_NONE,
                "letStatement.name.value not '%.*s'. got='%.*s'",
                name.length,
                name.data,
                s->letStatement.name->value.length,
                s->letStatement.name->value.data);

    tokenLiteral = s->letStatement.name->token.literal;
    TEST_ASSERT(StringSpansEqual(STRING_AS_SPAN(tokenLiteral), name),
                TEST_CLEANUP_NONE,
                "letStatement.name not '%.*s'. got='%.*s'",
                name.length,
                name.data,
                tokenLiteral.length,
                tokenLiteral.data);

    TEST_PASS();
}
