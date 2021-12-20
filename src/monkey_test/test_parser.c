#include "test_parser.h"
#include "monkey/ast.h"
#include "monkey/parser.h"
#include "nonstd/strdup.h"
#include "nonstd/strtok.h"
#include "test.h"
#include <assert.h>

char* check_let_statement(Statement* s, const char* name, TestValue value);
char* check_integer_literal(Expression* e, int64_t value);
char* check_identifier(Expression* e, const char* value);
char* check_boolean(Expression* e, bool value);
char* check_literal_expression(Expression* e, TestValue value);
char* check_infix_expression(Expression* e, TestValue left, const char* operator, TestValue right);
char* check_parser_errors(Parser* p);

char* test_let_statements(void)
{
    struct
    {
        const char* input;
        const char* expectedIdentifier;
        TestValue expectedValue;
    } tests[] = {
        {"let x = 5;", "x", TEST_VALUE_NEW_INT64(5)},
        {"let y = true;", "y", TEST_VALUE_NEW_BOOL(true)},
        {"let foobar = y;", "foobar", TEST_VALUE_NEW_STR("y")},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i)
    {
        Parser p;
        Parser_init(&p, tests[i].input);

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

        message = check_let_statement(program.statements.data[0], tests[i].expectedIdentifier, tests[i].expectedValue);
        if (message != NULL)
        {
            Program_deinit(&program);
            Parser_deinit(&p);
            return message;
        }

        Program_deinit(&program);
        Parser_deinit(&p);
    }
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
    check_identifier(expr, "foobar");

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
        TestValue value;
    } prefix_tests[] = {
        {"!5;", "!", TEST_VALUE_NEW_INT64(5)},
        {"-15;", "-", TEST_VALUE_NEW_INT64(15)},
        {"!true;", "!", TEST_VALUE_NEW_BOOL(true)},
        {"!false;", "!", TEST_VALUE_NEW_BOOL(false)},
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
        message = check_literal_expression(prefix->right, prefix_tests[i].value);
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
        TestValue left;
        const char* operator;
        TestValue right;
    } infix_tests[] = {
        {"5 + 5;", TEST_VALUE_NEW_INT64(5), "+", TEST_VALUE_NEW_INT64(5)},
        {"5 - 5;", TEST_VALUE_NEW_INT64(5), "-", TEST_VALUE_NEW_INT64(5)},
        {"5 * 5;", TEST_VALUE_NEW_INT64(5), "*", TEST_VALUE_NEW_INT64(5)},
        {"5 / 5;", TEST_VALUE_NEW_INT64(5), "/", TEST_VALUE_NEW_INT64(5)},
        {"5 > 5;", TEST_VALUE_NEW_INT64(5), ">", TEST_VALUE_NEW_INT64(5)},
        {"5 < 5;", TEST_VALUE_NEW_INT64(5), "<", TEST_VALUE_NEW_INT64(5)},
        {"5 == 5;", TEST_VALUE_NEW_INT64(5), "==", TEST_VALUE_NEW_INT64(5)},
        {"5 != 5;", TEST_VALUE_NEW_INT64(5), "!=", TEST_VALUE_NEW_INT64(5)},
        {"true == true", TEST_VALUE_NEW_BOOL(true), "==", TEST_VALUE_NEW_BOOL(true)},
        {"true != false", TEST_VALUE_NEW_BOOL(true), "!=", TEST_VALUE_NEW_BOOL(false)},
        {"false == false", TEST_VALUE_NEW_BOOL(false), "==", TEST_VALUE_NEW_BOOL(false)},
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
        message = check_infix_expression(expr, infix_tests[i].left, infix_tests[i].operator, infix_tests[i].right);
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
        {"true", "true"},
        {"false", "false"},
        {"3 > 5 == false", "((3 > 5) == false)"},
        {"3 < 5 == true", "((3 < 5) == true)"},
        {"1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"},
        {"(5 + 5) * 2", "((5 + 5) * 2)"},
        {"2 / (5 + 5)", "(2 / (5 + 5))"},
        {"-(5 + 5)", "(-(5 + 5))"},
        {"!(true == true)", "(!(true == true))"},
        {"a + add(b * c) + d", "((a + add((b * c))) + d)"},
        {"add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))", "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))"},
        {"add(a + b + c * d / f + g)", "add((((a + b) + ((c * d) / f)) + g))"},
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

char* test_boolean_expression(void)
{
    const char* input = "true;";
    Parser p;
    Parser_init(&p, input);
    Program program = Parser_parse_program(&p);

    test_assert(
        program.statements.length == 1,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "program.statements.length not 1, got %d", program.statements.length);
    Statement* stmt = program.statements.data[0];
    test_assert(
        stmt->type == STATEMENT_TYPE_EXPRESSION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "stmt->type not EXPRESSION, got %s", Statement_type_name(stmt->type));
    Expression* expr = ((ExpressionStatement*)stmt)->expression;
    char* message = check_literal_expression(expr, TEST_VALUE_NEW_BOOL(true));
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }

    Program_deinit(&program);
    Parser_deinit(&p);
    return NULL;
}

char* test_if_expression(void)
{
    const char* input = "if (x < y) { x }";
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
        "program.statements.length not 1. got=%d", program.statements.length);
    Statement* stmt = program.statements.data[0];
    test_assert(
        stmt->type == STATEMENT_TYPE_EXPRESSION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "stmt->type not EXPRESSION. got=%s", Statement_type_name(stmt->type));
    Expression* expr = ((ExpressionStatement*)stmt)->expression;
    test_assert(
        expr->type == EXPRESSION_TYPE_IF,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "expr->type not IF. got=%s", Expression_type_name(expr->type));
    IfExpression* ifExp = (IfExpression*)expr;
    test_assert(
        ifExp->consequence.statements.length == 1,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "ifExp->consequence.statements.length not 1. got=%d", ifExp->consequence.statements.length);
    Statement* consequence = ifExp->consequence.statements.data[0];
    test_assert(
        consequence->type == STATEMENT_TYPE_EXPRESSION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "consequence->type not EXPRESSION. got=%s", Statement_type_name(consequence->type));
    Expression* consequenceExp = ((ExpressionStatement*)consequence)->expression;
    message = check_identifier(consequenceExp, "x");
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }

    test_assert(
        ifExp->alternative == NULL,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "ifExp->alternative not NULL. got=%s", BlockStatement_string(ifExp->alternative));

    Program_deinit(&program);
    Parser_deinit(&p);
    return NULL;
}

char* test_if_else_expression(void)
{
    const char* input = "if (x < y) { x } else { y }";
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
        "program.statements.length not 1. got=%d", program.statements.length);
    Statement* stmt = program.statements.data[0];
    test_assert(
        stmt->type == STATEMENT_TYPE_EXPRESSION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "stmt->type not EXPRESSION. got=%s", Statement_type_name(stmt->type));
    Expression* expr = ((ExpressionStatement*)stmt)->expression;
    test_assert(
        expr->type == EXPRESSION_TYPE_IF,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "expr->type not IF. got=%s", Expression_type_name(expr->type));
    IfExpression* ifExp = (IfExpression*)expr;
    test_assert(
        ifExp->consequence.statements.length == 1,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "ifExp->consequence.statements.length not 1. got=%d", ifExp->consequence.statements.length);
    Statement* consequence = ifExp->consequence.statements.data[0];
    test_assert(
        consequence->type == STATEMENT_TYPE_EXPRESSION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "consequence->type not EXPRESSION. got=%s", Statement_type_name(consequence->type));
    Expression* consequenceExp = ((ExpressionStatement*)consequence)->expression;
    message = check_identifier(consequenceExp, "x");
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }

    test_assert(
        ifExp->alternative != NULL,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "ifExp->alternative was NULL.");
    test_assert(
        ifExp->alternative->statements.length == 1,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "ifExp->alternative->statements.length not 1. got=%d", ifExp->alternative->statements.length);
    Statement* alternative = ifExp->alternative->statements.data[0];
    test_assert(
        alternative->type == STATEMENT_TYPE_EXPRESSION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "alternative->type not EXPRESSION. got=%s", Statement_type_name(alternative->type));
    Expression* alt = ((ExpressionStatement*)alternative)->expression;
    message = check_identifier(alt, "y");
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }

    Program_deinit(&program);
    Parser_deinit(&p);
    return NULL;
}

char* test_function_literal_parsing(void)
{
    const char* input = "fn(x, y) { x + y; }";
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
        "program.statements.length not 1. got=%d", program.statements.length);
    test_assert(
        program.statements.data[0]->type == STATEMENT_TYPE_EXPRESSION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "stmt->type not EXPRESSION. got=%s", Statement_type_name(program.statements.data[0]->type));
    ExpressionStatement* stmt = (ExpressionStatement*)program.statements.data[0];
    test_assert(
        stmt->expression->type == EXPRESSION_TYPE_FUNCTION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "stmt->expression->type not FUNCTION. got=%s", Expression_type_name(stmt->expression->type));
    FunctionLiteral* function = (FunctionLiteral*)stmt->expression;
    test_assert(
        function->parameters.length == 2,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "function->parameters.length not 2. got=%d", function->parameters.length);
    message = check_literal_expression((Expression*)function->parameters.data[0], TEST_VALUE_NEW_STR("x"));
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }
    message = check_literal_expression((Expression*)function->parameters.data[1], TEST_VALUE_NEW_STR("y"));
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }
    test_assert(
        function->body.statements.length == 1,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "function->body.statements.length not 1. got=%d", function->body.statements.length);
    test_assert(
        function->body.statements.data[0]->type == STATEMENT_TYPE_EXPRESSION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "function->body.statements.data[0]->type != EXPRESSION. got=%s",
        Statement_type_name(function->body.statements.data[0]->type));
    ExpressionStatement* bodyStmt = (ExpressionStatement*)function->body.statements.data[0];
    message = check_infix_expression(bodyStmt->expression, TEST_VALUE_NEW_STR("x"), "+", TEST_VALUE_NEW_STR("y"));
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }

    Program_deinit(&program);
    Parser_deinit(&p);
    return NULL;
}

char* test_function_parameter_parsing(void)
{
    struct
    {
        const char* input;
        const char* expectedParams;
    } tests[] = {
        {"fn() {};", ""},
        {"fn(x) {};", "x"},
        {"fn(x, y, z) {};", "x,y,z"},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i)
    {
        Parser p;
        Parser_init(&p, tests[i].input);
        Program program = Parser_parse_program(&p);
        char* message = check_parser_errors(&p);
        if (message != NULL)
        {
            Program_deinit(&program);
            Parser_deinit(&p);
            return message;
        }
        ExpressionStatement* stmt = (ExpressionStatement*)program.statements.data[0];
        FunctionLiteral* function = (FunctionLiteral*)stmt->expression;
        char* expectedParams = nonstd_strdup(tests[i].expectedParams);
        char* saveptr = NULL;
        int i = 0;
        for (char* param = nonstd_strtok_r(expectedParams, ",", &saveptr); param != NULL;
             param = nonstd_strtok_r(NULL, ",", &saveptr))
        {
            test_assert(
                i < function->parameters.length,
                do {
                    free(expectedParams);
                    Program_deinit(&program);
                    Parser_deinit(&p);
                } while (false),
                "function has too few parameters. got=%d", function->parameters.length);
            test_assert(
                strcmp(param, function->parameters.data[i]->value) == 0,
                do {
                    free(expectedParams);
                    Program_deinit(&program);
                    Parser_deinit(&p);
                } while (false),
                "function has wrong parameters. got=%s", function->parameters.data[i]->value);
            ++i;
        }
        free(expectedParams);
        Program_deinit(&program);
        Parser_deinit(&p);
    }

    return NULL;
}

char* test_call_expression_parsing(void)
{
    const char* input = "add(1, 2 * 3, 4 + 5);";
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
        "program.statements.length not 1. got=%d", program.statements.length);
    test_assert(
        program.statements.data[0]->type == STATEMENT_TYPE_EXPRESSION,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "program.statements.data[0]->type not EXPRESSION. got=%s",
        Statement_type_name(program.statements.data[0]->type));
    ExpressionStatement* stmt = (ExpressionStatement*)program.statements.data[0];
    test_assert(
        stmt->expression->type == EXPRESSION_TYPE_CALL,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "stmt->expression->type not CALL. got=%s", Expression_type_name(stmt->expression->type));
    CallExpression* exp = (CallExpression*)stmt->expression;
    message = check_identifier(exp->function, "add");
    if (message != NULL)
    {
        Program_deinit(&program);
        Parser_deinit(&p);
        return message;
    }

    test_assert(
        exp->args.length == 3,
        do {
            Program_deinit(&program);
            Parser_deinit(&p);
        } while (false),
        "exp->args.length not 3. got=%d", exp->args.length);
#define CHECK_MESSAGE(call)                                                                                            \
    message = call;                                                                                                    \
    if (message != NULL)                                                                                               \
    {                                                                                                                  \
        Program_deinit(&program);                                                                                      \
        Parser_deinit(&p);                                                                                             \
        return message;                                                                                                \
    }
    CHECK_MESSAGE(check_literal_expression(exp->args.data[0], TEST_VALUE_NEW_INT64(1)))
    CHECK_MESSAGE(check_infix_expression(exp->args.data[1], TEST_VALUE_NEW_INT64(2), "*", TEST_VALUE_NEW_INT64(3)))
    CHECK_MESSAGE(check_infix_expression(exp->args.data[2], TEST_VALUE_NEW_INT64(4), "+", TEST_VALUE_NEW_INT64(5)))
#undef CHECK_MESSAGE

    Program_deinit(&program);
    Parser_deinit(&p);
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
    test_run(test_boolean_expression);
    test_run(test_if_expression);
    test_run(test_if_else_expression);
    test_run(test_function_literal_parsing);
    test_run(test_function_parameter_parsing);
    test_run(test_call_expression_parsing);
    return NULL;
}

char* check_let_statement(Statement* s, const char* name, TestValue value)
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

    char* message = check_literal_expression(let_stmt->value, value);
    if (message != NULL)
    {
        return message;
    }

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

char* check_identifier(Expression* e, const char* value)
{
    test_assert(e->type == EXPRESSION_TYPE_IDENTIFIER, (void)0, "e->type should be IDENTIFIER, not %s.",
                Expression_type_name(e->type));
    Identifier* ident = (Identifier*)e;
    test_assert(strcmp(ident->value, value) == 0, (void)0, "ident->value not %s. got=%s", value, ident->value);
    sds toklit = Identifier_token_literal(ident);
    test_assert(strcmp(toklit, value) == 0, sdsfree(toklit), "Identifier_token_literal(ident) not %s. got=%s", value,
                toklit);
    sdsfree(toklit);
    return NULL;
}

char* check_boolean(Expression* e, bool value)
{
    test_assert(e->type == EXPRESSION_TYPE_BOOLEAN, (void)0, "e->type should be BOOLEAN, not %s.",
                Expression_type_name(e->type));
    Boolean* b = (Boolean*)e;
    test_assert(b->value == value, (void)0, "b->value not %s. got=%s", value ? "true" : "false",
                b->value ? "true" : "false");
    const char* expected = value ? "true" : "false";
    sds toklit = Boolean_token_literal(b);
    test_assert(strcmp(toklit, expected) == 0, sdsfree(toklit), "Boolean_token_literal(b) not %s. got=%s", expected,
                toklit);
    sdsfree(toklit);
    return NULL;
}

char* check_literal_expression(Expression* e, TestValue value)
{
    test_assert(e != NULL, (void)0, "Expression should not be NULL.");
    switch (value.kind)
    {
    case TEST_VALUE_INT:
        return check_integer_literal(e, value.u.i);
    case TEST_VALUE_INT64:
        return check_integer_literal(e, value.u.i64);
    case TEST_VALUE_STR:
        return check_identifier(e, value.u.s);
    case TEST_VALUE_BOOL:
        return check_boolean(e, value.u.b);
    }

    assert(false && "corrupt test value");
}

char* check_infix_expression(Expression* e, TestValue left, const char* operator, TestValue right)
{
    test_assert(e->type == EXPRESSION_TYPE_INFIX, (void)0, "e->type is not INFIX. got=%s",
                Expression_type_name(e->type));
    InfixExpression* infix = (InfixExpression*)e;
    char* message = check_literal_expression(infix->left, left);
    if (message != NULL)
    {
        return message;
    }
    test_assert(strcmp(infix->operator, operator) == 0, (void)0, "infix->operator is not '%s'. got='%s'", operator,
                infix->operator);
    return check_literal_expression(infix->right, right);
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
    // make compatible with libc free()
    char* result = malloc(sdslen(message) + 1);
    strncpy(result, message, sdslen(message));
    sdsfree(message);
    return result;
}
