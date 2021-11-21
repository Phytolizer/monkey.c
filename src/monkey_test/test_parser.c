#include "test_parser.h"
#include "monkey/parser.h"
#include "test.h"

char* check_let_statement(Statement* s, const char* name);
char* check_integer_literal(Expression* e, int64_t value);
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

char* test_integer_literal_expression(void)
{
    const char* input = "5;\n";
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
        "Program should have 1 statement, not %d.", program.statements.length);
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
        expr->type == EXPRESSION_TYPE_INTEGER,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "expr->type should be INTEGER, not %s.", Expression_type_name(expr->type));
    IntegerLiteral* integer = (IntegerLiteral*)expr;
    test_assert(
        integer->value == 5,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "integer->value should be 5, not %ld.", integer->value);
    sds value = IntegerLiteral_token_literal(integer);
    test_assert(
        strcmp(value, "5") == 0,
        do {
            sdsfree(value);
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "IntegerLiteral_token_literal(integer) not '5', got '%s'", value);
    sdsfree(value);
    Program_deinit(&program);
    Parser_deinit(&p);
    return NULL;
}

char* test_parsing_prefix_expressions(void)
{
    struct
    {
        const char* input;
        const char* operator;
        int64_t value;
    } prefix_tests[] = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
    };

    for (size_t i = 0; i < sizeof(prefix_tests) / sizeof(prefix_tests[0]); i++)
    {
        Parser parser;
        Parser_init(&parser, prefix_tests[i].input);
        Program program = Parser_parse_program(&parser);
        char* message = check_parser_errors(&parser);
        if (message != NULL)
        {
            Program_deinit(&program);
            Parser_deinit(&parser);
            return message;
        }
        test_assert(
            program.statements.length == 1,
            do {
                Program_deinit(&program);
                Parser_deinit(&parser);
            } while (false),
            "Program should have 1 statement, not %d.", program.statements.length);
        Statement* stmt = program.statements.data[0];
        test_assert(
            stmt->type == STATEMENT_TYPE_EXPRESSION,
            do {
                Program_deinit(&program);
                Parser_deinit(&parser);
            } while (false),
            "stmt->type should be EXPRESSION, not %s.", Statement_type_name(stmt->type));
        Expression* expr = ((ExpressionStatement*)stmt)->expression;
        test_assert(
            expr->type == EXPRESSION_TYPE_PREFIX,
            do {
                Program_deinit(&program);
                Parser_deinit(&parser);
            } while (false),
            "expr->type should be PREFIX, not %s.", Expression_type_name(expr->type));
        PrefixExpression* prefix = (PrefixExpression*)expr;
        test_assert(
            strcmp(prefix->operator, prefix_tests[i].operator) == 0,
            do {
                Program_deinit(&program);
                Parser_deinit(&parser);
            } while (false),
            "prefix->operator should be '%s', not '%s'.", prefix_tests[i].operator, prefix->operator);
        message = check_integer_literal(prefix->right, prefix_tests[i].value);
        if (message != NULL)
        {
            Program_deinit(&program);
            Parser_deinit(&parser);
            return message;
        }
        Program_deinit(&program);
        Parser_deinit(&parser);
    }

    return NULL;
}

char* test_parsing_infix_expressions(void)
{
    struct
    {
        const char* input;
        const char* operator;
        int64_t value;
    } infix_tests[] = {
        {"5 + 5;", "+", 5}, {"5 - 5;", "-", 5}, {"5 * 5;", "*", 5},   {"5 / 5;", "/", 5},
        {"5 > 5;", ">", 5}, {"5 < 5;", "<", 5}, {"5 == 5;", "==", 5}, {"5 != 5;", "!=", 5},
    };

    for (size_t i = 0; i < sizeof(infix_tests) / sizeof(infix_tests[0]); i++)
    {
        Parser parser;
        Parser_init(&parser, infix_tests[i].input);
        Program program = Parser_parse_program(&parser);
        char* message = check_parser_errors(&parser);
        if (message != NULL)
        {
            Program_deinit(&program);
            Parser_deinit(&parser);
            return message;
        }
        test_assert(
            program.statements.length == 1,
            do {
                Program_deinit(&program);
                Parser_deinit(&parser);
            } while (false),
            "Program should have 1 statement, not %d.", program.statements.length);
        Statement* stmt = program.statements.data[0];
        test_assert(
            stmt->type == STATEMENT_TYPE_EXPRESSION,
            do {
                Program_deinit(&program);
                Parser_deinit(&parser);
            } while (false),
            "stmt->type should be EXPRESSION, not %s.", Statement_type_name(stmt->type));
        Expression* expr = ((ExpressionStatement*)stmt)->expression;
        test_assert(
            expr->type == EXPRESSION_TYPE_INFIX,
            do {
                Program_deinit(&program);
                Parser_deinit(&parser);
            } while (false),
            "expr->type should be INFIX, not %s.", Expression_type_name(expr->type));
        InfixExpression* infix = (InfixExpression*)expr;
        message = check_integer_literal(infix->left, infix_tests[i].value);
        if (message != NULL)
        {
            Program_deinit(&program);
            Parser_deinit(&parser);
            return message;
        }
        test_assert(
            strcmp(infix->operator, infix_tests[i].operator) == 0,
            do {
                Program_deinit(&program);
                Parser_deinit(&parser);
            } while (false),
            "infix->operator should be '%s', not '%s'.", infix_tests[i].operator, infix->operator);
        message = check_integer_literal(infix->right, infix_tests[i].value);
        if (message != NULL)
        {
            Program_deinit(&program);
            Parser_deinit(&parser);
            return message;
        }
        Program_deinit(&program);
        Parser_deinit(&parser);
    }

    return NULL;
}

char* test_operator_precedence_parsing(void)
{
    struct
    {
        const char* input;
        const char* expected;
    } tests[] = {
        {"-a * b", "((-a) * b)"},
        {"!-a", "(!(-a))"},
        {"a + b + c", "((a + b) + c)"},
        {"a + b - c", "((a + b) - c)"},
        {"a * b * c", "((a * b) * c)"},
        {"a * b / c", "((a * b) / c)"},
        {"a + b / c", "(a + (b / c))"},
        {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
        {"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
        {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
        {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        Parser parser;
        Parser_init(&parser, tests[i].input);
        Program program = Parser_parse_program(&parser);
        char* message = check_parser_errors(&parser);
        if (message != NULL)
        {
            Program_deinit(&program);
            Parser_deinit(&parser);
            return message;
        }
        sds actual = Program_string(&program);
        test_assert(
            strcmp(actual, tests[i].expected) == 0,
            do {
                sdsfree(actual);
                Program_deinit(&program);
                Parser_deinit(&parser);
            } while (false),
            "Expected: %s, got: %s.", tests[i].expected, actual);
        sdsfree(actual);
        Program_deinit(&program);
        Parser_deinit(&parser);
    }

    return NULL;
}

char* parser_tests(size_t* test_count)
{
    test_run(test_let_statements);
    test_run(test_return_statements);
    test_run(test_identifier_expression);
    test_run(test_integer_literal_expression);
    test_run(test_parsing_prefix_expressions);
    test_run(test_parsing_infix_expressions);
    test_run(test_operator_precedence_parsing);
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

char* check_integer_literal(Expression* e, int64_t value)
{
    test_assert(e->type == EXPRESSION_TYPE_INTEGER, (void)0, "e->type should be INTEGER, not %s.",
                Expression_type_name(e->type));
    IntegerLiteral* integer = (IntegerLiteral*)e;
    test_assert(integer->value == value, (void)0, "integer->value should be %ld, not %ld.", value, integer->value);
    sds toklit = IntegerLiteral_token_literal(integer);
    char strval[10];
    snprintf(strval, 10, "%ld", value);
    test_assert(strcmp(toklit, strval) == 0, sdsfree(toklit),
                "IntegerLiteral_token_literal(integer) not '%s', got '%s'", strval, toklit);
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
