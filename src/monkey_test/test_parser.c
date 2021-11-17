#include "monkey/parser.h"
#include "test.h"
#include "test_parser.h"

char* check_let_statement(Statement* s, const char* name);

char* test_let_statements(void)
{
    const char* input = "let x = 5;\n"
                        "let y = 10;\n"
                        "let foobar = 838383;\n";
    Parser p;
    Parser_init(&p, input);

    Program program = Parser_parse_program(&p);
    test_assert(program.statements.length == 3, Parser_deinit(&p), "Program should have 3 statements.");
    struct
    {
        const char* expected_identifier;
    } tests[] = {
        {"x"},
        {"y"},
        {"foobar"},
    };
    size_t ntests = sizeof(tests) / sizeof(tests[0]);

    for (size_t i = 0; i < ntests; i++)
    {
        char* message = check_let_statement(&program.statements.data[i], tests[i].expected_identifier);
        if (message != NULL)
        {
            Parser_deinit(&p);
            return message;
        }
    }

    Parser_deinit(&p);
    return NULL;
}

char* parser_tests(size_t* test_count)
{
    test_run(test_let_statements);
    return NULL;
}

char* check_let_statement(Statement* s, const char* name)
{
    sds toklit = Statement_token_literal(s);
    test_assert(strcmp(toklit, T_LET) == 0, sdsfree(toklit), "Statement_token_literal(s) should be 'let', not '%s'.",
                toklit);
    sdsfree(toklit);

    test_assert(s->type == STATEMENT_TYPE_LET, (void)0, "s->type should be LET, not %s.", Statement_type_name(s->type));
    LetStatement* let_stmt = (LetStatement*)s;
    test_assert(strcmp(let_stmt->name.value, name) == 0, (void)0, "let_stmt->name.value should be '%s', not '%s'.",
                name, let_stmt->name.value);

    toklit = Identifier_token_literal(&let_stmt->name);
    test_assert(strcmp(toklit, name) == 0, sdsfree(toklit),
                "Identifier_token_literal(&let_stmt->name) should be '%s', not '%s'.", name, toklit);
    sdsfree(toklit);
    return NULL;
}
