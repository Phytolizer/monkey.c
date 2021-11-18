#include "monkey/parser.h"
#include "test.h"
#include "test_parser.h"

char* check_let_statement(Statement* s, const char* name);
char* check_parser_errors(Parser* p);

char* test_let_statements(void)
{
    const char* input = "let x = 5;\n"
                        "let y = 10;\n"
                        "let foobar = 838383;\n";
    Parser p;
    Parser_init(&p, input);

    Program program = Parser_parse_program(&p);
    char* message = check_parser_errors(&p);
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }
    test_assert(
        program.statements.length == 3,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "Program should have 3 statements.");
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
        char* message = check_let_statement(program.statements.data[i], tests[i].expected_identifier);
        if (message != NULL)
        {
            Program_deinit(&program);
            Parser_deinit(&p);
            return message;
        }
    }

    Program_deinit(&program);
    Parser_deinit(&p);
    return NULL;
}

char* test_return_statements(void)
{
    const char* input = "return 5;\n"
                        "return 10;\n"
                        "return 993322;\n";
    Parser p;
    Parser_init(&p, input);
    Program program = Parser_parse_program(&p);
    char* message = check_parser_errors(&p);
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }
    test_assert(
        program.statements.length == 3,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "Program should have 3 statements.");

    for (int i = 0; i < program.statements.length; i++)
    {
        Statement* stmt = program.statements.data[i];
        test_assert(
            stmt->type == STATEMENT_TYPE_RETURN,
            do {
                Program_deinit(&program);
                Parser_deinit(&p);
            } while (false),
            "stmt->type should be STATEMENT_TYPE_RETURN.");
        sds toklit = ReturnStatement_token_literal((ReturnStatement*)stmt);
        test_assert(
            strcmp(toklit, "return") == 0,
            do {
                sdsfree(toklit);
                Program_deinit(&program);
                Parser_deinit(&p);
            } while (false),
            "ReturnStatement_token_literal(return_stmt) not 'return', got '%s'", toklit);
        sdsfree(toklit);
    }

    Program_deinit(&program);
    Parser_deinit(&p);
    return NULL;
}

char* test_identifier_expression(void)
{
    const char* input = "foobar;\n";
    Parser p;
    Parser_init(&p, input);
    Program program = Parser_parse_program(&p);
    char* message = check_parser_errors(&p);
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }

    test_assert(
        program.statements.length == 1,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "Program should have 1 statement.");
    Statement* stmt = program.statements.data[0];
    test_assert(
        stmt->type == STATEMENT_TYPE_EXPRESSION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "stmt->type should be EXPRESSION, not %s.", Statement_type_name(stmt->type));
    Expression* expr = ((ExpressionStatement*)stmt)->expression;
    test_assert(
        expr->type == EXPRESSION_TYPE_IDENTIFIER,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "expr->type should be IDENTIFIER, not %s.", Expression_type_name(expr->type));
    Identifier* ident = (Identifier*)expr;
    test_assert(
        strcmp(ident->value, "foobar") == 0,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "ident->name should be 'foobar', not '%s'.", ident->value);
    sds value = Identifier_token_literal(ident);
    test_assert(
        strcmp(value, "foobar") == 0,
        do {
            sdsfree(value);
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "Identifier_token_literal(ident) not 'foobar', got '%s'", value);
    sdsfree(value);

    Program_deinit(&program);
    Parser_deinit(&p);
    return NULL;
}

char* parser_tests(size_t* test_count)
{
    test_run(test_let_statements);
    test_run(test_return_statements);
    test_run(test_identifier_expression);
    return NULL;
}

char* check_let_statement(Statement* s, const char* name)
{
    sds toklit = Statement_token_literal(s);
    test_assert(strcmp(toklit, "let") == 0, sdsfree(toklit), "Statement_token_literal(s) should be 'let', not '%s'.",
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

char* check_parser_errors(Parser* p)
{
    if (p->errors.length == 0)
    {
        return NULL;
    }

    sds message = sdscatprintf(sdsempty(), "parser has %d errors\n", p->errors.length);
    for (int i = 0; i < p->errors.length; i++)
    {
        message = sdscatprintf(message, "%s\n", p->errors.data[i]);
    }
    return message;
}
