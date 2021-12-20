#include "monkey/parser.h"
#include "monkey/ast.h"
#include <stdbool.h>

#define PRECEDENCES_                                                                                                   \
    X(LOWEST)                                                                                                          \
    X(EQUALS)                                                                                                          \
    X(LESSGREATER)                                                                                                     \
    X(SUM)                                                                                                             \
    X(PRODUCT)                                                                                                         \
    X(PREFIX)                                                                                                          \
    X(CALL)

typedef enum
{
#define X(x) PREC_##x,
    PRECEDENCES_
#undef X
} Precedence;

static const Precedence PRECEDENCES[] = {
#define X(x) PREC_##x,
    PRECEDENCES_
#undef X
};

void Parser_next_token(Parser* p);
Statement* Parser_parse_statement(Parser* p);
LetStatement* Parser_parse_let_statement(Parser* p);
ReturnStatement* Parser_parse_return_statement(Parser* p);
ExpressionStatement* Parser_parse_expression_statement(Parser* p);
BlockStatement* Parser_parse_block_statement(Parser* p);
IdentifierVec* Parser_parse_function_parameters(Parser* p);
ExpressionVec* Parser_parse_call_arguments(Parser* p);
Expression* Parser_parse_expression(Parser* p, Precedence precedence);
Expression* Parser_parse_identifier(Parser* p);
Expression* Parser_parse_integer_literal(Parser* p);
Expression* Parser_parse_prefix_expression(Parser* p);
Expression* Parser_parse_infix_expression(Parser* p, Expression* left);
Expression* Parser_parse_call_expression(Parser* p, Expression* function);
Expression* Parser_parse_boolean(Parser* p);
Expression* Parser_parse_grouped_expression(Parser* p);
Expression* Parser_parse_if_expression(Parser* p);
Expression* Parser_parse_function_literal(Parser* p);
bool Parser_cur_token_is(Parser* p, TokenType type);
bool Parser_peek_token_is(Parser* p, TokenType type);
Precedence Parser_peek_precedence(Parser* p);
Precedence Parser_cur_precedence(Parser* p);
bool Parser_expect_peek(Parser* p, TokenType type);
void Parser_peek_error(Parser* p, TokenType type);
void Parser_no_prefix_parse_fn_error(Parser* p, TokenType type);

void Parser_init(Parser* p, const char* input)
{
    Lexer_init(&p->l, input);
    p->cur_token.literal = NULL;
    p->peek_token.literal = NULL;
    vec_init(&p->errors);
    ViewHash_init(&p->prefix_parse_fns);
    ViewHash_init(&p->infix_parse_fns);
    ViewHash_insert(&p->prefix_parse_fns, T_IDENT, sizeof(T_IDENT), (void*)Parser_parse_identifier,
                    sizeof(PrefixParseFn));
    ViewHash_insert(&p->prefix_parse_fns, T_INT, sizeof(T_INT), (void*)Parser_parse_integer_literal,
                    sizeof(PrefixParseFn));
    ViewHash_insert(&p->prefix_parse_fns, T_BANG, sizeof(T_BANG), (void*)Parser_parse_prefix_expression,
                    sizeof(PrefixParseFn));
    ViewHash_insert(&p->prefix_parse_fns, T_MINUS, sizeof(T_MINUS), (void*)Parser_parse_prefix_expression,
                    sizeof(PrefixParseFn));
    ViewHash_insert(&p->prefix_parse_fns, T_IF, sizeof(T_IF), (void*)Parser_parse_if_expression, sizeof(PrefixParseFn));
    ViewHash_insert(&p->prefix_parse_fns, T_FUNCTION, sizeof(T_FUNCTION), (void*)Parser_parse_function_literal,
                    sizeof(PrefixParseFn));
    ViewHash_insert(&p->prefix_parse_fns, T_TRUE, sizeof(T_TRUE), (void*)Parser_parse_boolean, sizeof(InfixParseFn));
    ViewHash_insert(&p->prefix_parse_fns, T_FALSE, sizeof(T_FALSE), (void*)Parser_parse_boolean, sizeof(InfixParseFn));
    ViewHash_insert(&p->prefix_parse_fns, T_LPAREN, sizeof(T_LPAREN), (void*)Parser_parse_grouped_expression,
                    sizeof(InfixParseFn));
    ViewHash_insert(&p->infix_parse_fns, T_PLUS, sizeof(T_PLUS), (void*)Parser_parse_infix_expression,
                    sizeof(InfixParseFn));
    ViewHash_insert(&p->infix_parse_fns, T_MINUS, sizeof(T_MINUS), (void*)Parser_parse_infix_expression,
                    sizeof(InfixParseFn));
    ViewHash_insert(&p->infix_parse_fns, T_SLASH, sizeof(T_SLASH), (void*)Parser_parse_infix_expression,
                    sizeof(InfixParseFn));
    ViewHash_insert(&p->infix_parse_fns, T_ASTERISK, sizeof(T_ASTERISK), (void*)Parser_parse_infix_expression,
                    sizeof(InfixParseFn));
    ViewHash_insert(&p->infix_parse_fns, T_EQ, sizeof(T_EQ), (void*)Parser_parse_infix_expression,
                    sizeof(InfixParseFn));
    ViewHash_insert(&p->infix_parse_fns, T_NOT_EQ, sizeof(T_NOT_EQ), (void*)Parser_parse_infix_expression,
                    sizeof(InfixParseFn));
    ViewHash_insert(&p->infix_parse_fns, T_LT, sizeof(T_LT), (void*)Parser_parse_infix_expression,
                    sizeof(InfixParseFn));
    ViewHash_insert(&p->infix_parse_fns, T_GT, sizeof(T_GT), (void*)Parser_parse_infix_expression,
                    sizeof(InfixParseFn));
    ViewHash_insert(&p->infix_parse_fns, T_LPAREN, sizeof(T_LPAREN), (void*)Parser_parse_call_expression,
                    sizeof(InfixParseFn));
    Hash_init(&p->precedences);
    Hash_insert(&p->precedences, T_EQ, sizeof(T_EQ), &PRECEDENCES[PREC_EQUALS], sizeof(Precedence));
    Hash_insert(&p->precedences, T_NOT_EQ, sizeof(T_NOT_EQ), &PRECEDENCES[PREC_EQUALS], sizeof(Precedence));
    Hash_insert(&p->precedences, T_LT, sizeof(T_LT), &PRECEDENCES[PREC_LESSGREATER], sizeof(Precedence));
    Hash_insert(&p->precedences, T_GT, sizeof(T_GT), &PRECEDENCES[PREC_LESSGREATER], sizeof(Precedence));
    Hash_insert(&p->precedences, T_PLUS, sizeof(T_PLUS), &PRECEDENCES[PREC_SUM], sizeof(Precedence));
    Hash_insert(&p->precedences, T_MINUS, sizeof(T_MINUS), &PRECEDENCES[PREC_SUM], sizeof(Precedence));
    Hash_insert(&p->precedences, T_SLASH, sizeof(T_SLASH), &PRECEDENCES[PREC_PRODUCT], sizeof(Precedence));
    Hash_insert(&p->precedences, T_ASTERISK, sizeof(T_ASTERISK), &PRECEDENCES[PREC_PRODUCT], sizeof(Precedence));
    Hash_insert(&p->precedences, T_LPAREN, sizeof(T_LPAREN), &PRECEDENCES[PREC_CALL], sizeof(Precedence));
    Parser_next_token(p);
    Parser_next_token(p);
}

void Parser_deinit(Parser* p)
{
    Lexer_deinit(&p->l);
    Token_deinit(&p->cur_token);
    Token_deinit(&p->peek_token);
    for (int i = 0; i < p->errors.length; ++i)
    {
        sdsfree(p->errors.data[i]);
    }
    vec_deinit(&p->errors);
    ViewHash_deinit(&p->prefix_parse_fns);
    ViewHash_deinit(&p->infix_parse_fns);
    Hash_deinit(&p->precedences);
}

Program Parser_parse_program(Parser* p)
{
    Program program = {0};
    vec_init(&program.statements);

    while (strcmp(p->cur_token.type, T_EOF) != 0)
    {
        Statement* stmt = Parser_parse_statement(p);
        if (stmt != NULL)
        {
            vec_push(&program.statements, stmt);
        }
        Parser_next_token(p);
    }

    return program;
}

void Parser_next_token(Parser* p)
{
    if (p->cur_token.literal != NULL)
    {
        Token_deinit(&p->cur_token);
    }
    p->cur_token = p->peek_token;
    p->peek_token = Lexer_next_token(&p->l);
}

Statement* Parser_parse_statement(Parser* p)
{
    if (strcmp(p->cur_token.type, T_LET) == 0)
    {
        return (Statement*)Parser_parse_let_statement(p);
    }
    if (strcmp(p->cur_token.type, T_RETURN) == 0)
    {
        return (Statement*)Parser_parse_return_statement(p);
    }
    return (Statement*)Parser_parse_expression_statement(p);
}

LetStatement* Parser_parse_let_statement(Parser* p)
{
    Token tok = Token_dup(&p->cur_token);
    if (!Parser_expect_peek(p, T_IDENT))
    {
        return NULL;
    }

    Identifier name = {
        .token = Token_dup(&p->cur_token),
        .value = sdsdup(p->cur_token.literal),
    };
    Identifier_init(&name);

    if (!Parser_expect_peek(p, T_ASSIGN))
    {
        return NULL;
    }

    Parser_next_token(p);
    Expression* value = Parser_parse_expression(p, PREC_LOWEST);
    if (Parser_peek_token_is(p, T_SEMICOLON))
    {
        Parser_next_token(p);
    }

    LetStatement* stmt = malloc(sizeof(LetStatement));
    LetStatement_init(stmt);
    stmt->token = tok;
    stmt->name = name;
    stmt->value = value;
    return stmt;
}

ReturnStatement* Parser_parse_return_statement(Parser* p)
{
    Token tok = Token_dup(&p->cur_token);

    Parser_next_token(p);

    // TODO: parse expression
    while (!Parser_cur_token_is(p, T_SEMICOLON))
    {
        Parser_next_token(p);
    }

    ReturnStatement* stmt = malloc(sizeof(ReturnStatement));
    ReturnStatement_init(stmt);
    stmt->token = tok;
    stmt->return_value = NULL;
    return stmt;
}

ExpressionStatement* Parser_parse_expression_statement(Parser* p)
{
    Token tok = Token_dup(&p->cur_token);

    Expression* expr = Parser_parse_expression(p, PREC_LOWEST);

    if (Parser_peek_token_is(p, T_SEMICOLON))
    {
        Parser_next_token(p);
    }

    ExpressionStatement* stmt = malloc(sizeof(ExpressionStatement));
    ExpressionStatement_init(stmt);
    stmt->token = tok;
    stmt->expression = expr;
    return stmt;
}

BlockStatement* Parser_parse_block_statement(Parser* p)
{
    Token tok = Token_dup(&p->cur_token);
    StatementVec statements;
    vec_init(&statements);
    Parser_next_token(p);

    while (!Parser_cur_token_is(p, T_RBRACE) && !Parser_cur_token_is(p, T_EOF))
    {
        Statement* stmt = Parser_parse_statement(p);
        if (stmt != NULL)
        {
            vec_push(&statements, stmt);
        }
        Parser_next_token(p);
    }

    BlockStatement* result = malloc(sizeof(BlockStatement));
    BlockStatement_init(result);
    result->token = tok;
    result->statements = statements;
    return result;
}

IdentifierVec* Parser_parse_function_parameters(Parser* p)
{
    IdentifierVec* identifiers = malloc(sizeof(IdentifierVec));
    vec_init(identifiers);

    if (Parser_peek_token_is(p, T_RPAREN))
    {
        Parser_next_token(p);
        return identifiers;
    }

    Parser_next_token(p);

    Identifier* ident = malloc(sizeof(Identifier));
    Identifier_init(ident);
    ident->token = Token_dup(&p->cur_token);
    ident->value = sdsdup(p->cur_token.literal);
    vec_push(identifiers, ident);

    while (Parser_peek_token_is(p, T_COMMA))
    {
        Parser_next_token(p);
        Parser_next_token(p);
        ident = malloc(sizeof(Identifier));
        Identifier_init(ident);
        ident->token = Token_dup(&p->cur_token);
        ident->value = sdsdup(p->cur_token.literal);
        vec_push(identifiers, ident);
    }

    if (!Parser_expect_peek(p, T_RPAREN))
    {
        for (int i = 0; i < identifiers->length; ++i)
        {
            Identifier_deinit(identifiers->data[i]);
            free(identifiers->data[i]);
        }
        vec_deinit(identifiers);
        free(identifiers);
        return NULL;
    }

    return identifiers;
}

ExpressionVec* Parser_parse_call_arguments(Parser* p)
{
    ExpressionVec* args = malloc(sizeof(ExpressionVec));
    vec_init(args);

    if (Parser_peek_token_is(p, T_RPAREN))
    {
        Parser_next_token(p);
        return args;
    }

    Parser_next_token(p);
    vec_push(args, Parser_parse_expression(p, PREC_LOWEST));
    while (Parser_peek_token_is(p, T_COMMA))
    {
        Parser_next_token(p);
        Parser_next_token(p);
        vec_push(args, Parser_parse_expression(p, PREC_LOWEST));
    }

    if (!Parser_expect_peek(p, T_RPAREN))
    {
        for (int i = 0; i < args->length; ++i)
        {
            Expression_deinit(args->data[i]);
            free(args->data[i]);
        }
        vec_deinit(args);
        free(args);
        return NULL;
    }

    return args;
}

Expression* Parser_parse_expression(Parser* p, Precedence precedence)
{
    PrefixParseFn* prefix =
        (PrefixParseFn*)ViewHash_lookup(&p->prefix_parse_fns, p->cur_token.type, strlen(p->cur_token.type) + 1);
    if (prefix == NULL)
    {
        Parser_no_prefix_parse_fn_error(p, p->cur_token.type);
        return NULL;
    }
    Expression* left_expr = (*prefix)(p);

    while (!Parser_peek_token_is(p, T_SEMICOLON) && precedence < Parser_peek_precedence(p))
    {
        InfixParseFn* infix =
            (InfixParseFn*)ViewHash_lookup(&p->infix_parse_fns, p->peek_token.type, strlen(p->peek_token.type) + 1);
        if (infix == NULL)
        {
            return left_expr;
        }
        Parser_next_token(p);
        left_expr = (*infix)(p, left_expr);
    }

    return left_expr;
}

Expression* Parser_parse_identifier(Parser* p)
{
    Token tok = Token_dup(&p->cur_token);
    Identifier* ident = malloc(sizeof(Identifier));
    Identifier_init(ident);
    ident->token = tok;
    ident->value = sdsdup(p->cur_token.literal);
    return (Expression*)ident;
}

Expression* Parser_parse_integer_literal(Parser* p)
{
    Token tok = Token_dup(&p->cur_token);
    IntegerLiteral* lit = malloc(sizeof(IntegerLiteral));
    IntegerLiteral_init(lit);
    lit->token = tok;
    lit->value = strtol(p->cur_token.literal, NULL, 10);
    return (Expression*)lit;
}

Expression* Parser_parse_prefix_expression(Parser* p)
{
    Token tok = Token_dup(&p->cur_token);
    sds operator= sdsdup(p->cur_token.literal);
    Parser_next_token(p);

    Expression* right = Parser_parse_expression(p, PREC_PREFIX);

    PrefixExpression* expr = malloc(sizeof(PrefixExpression));
    PrefixExpression_init(expr);
    expr->token = tok;
    expr->operator= operator;
    expr->right = right;
    return (Expression*)expr;
}

Expression* Parser_parse_infix_expression(Parser* p, Expression* left)
{
    Token tok = Token_dup(&p->cur_token);
    sds operator= sdsdup(p->cur_token.literal);

    Precedence precedence = Parser_cur_precedence(p);
    Parser_next_token(p);
    Expression* right = Parser_parse_expression(p, precedence);

    InfixExpression* expr = malloc(sizeof(InfixExpression));
    InfixExpression_init(expr);
    expr->token = tok;
    expr->operator= operator;
    expr->left = left;
    expr->right = right;
    return (Expression*)expr;
}

Expression* Parser_parse_call_expression(Parser* p, Expression* function)
{
    CallExpression* result = malloc(sizeof(CallExpression));
    CallExpression_init(result);
    result->token = Token_dup(&p->cur_token);
    result->function = function;
    ExpressionVec* args = Parser_parse_call_arguments(p);
    if (args == NULL)
    {
        Expression_deinit(result->function);
        free(result->function);
        Token_deinit(&result->token);
        free(result);
        return NULL;
    }
    result->args = *args;
    return (Expression*)result;
}

Expression* Parser_parse_boolean(Parser* p)
{
    Token tok = Token_dup(&p->cur_token);
    bool value = Parser_cur_token_is(p, T_TRUE);

    Boolean* b = malloc(sizeof(Boolean));
    Boolean_init(b);
    b->token = tok;
    b->value = value;
    return (Expression*)b;
}

Expression* Parser_parse_grouped_expression(Parser* p)
{
    Parser_next_token(p);
    Expression* exp = Parser_parse_expression(p, PREC_LOWEST);

    if (!Parser_expect_peek(p, T_RPAREN))
    {
        free(exp);
        return NULL;
    }

    return exp;
}

Expression* Parser_parse_if_expression(Parser* p)
{
    Token tok = Token_dup(&p->cur_token);

    if (!Parser_expect_peek(p, T_LPAREN))
    {
        Token_deinit(&tok);
        return NULL;
    }

    Parser_next_token(p);
    Expression* condition = Parser_parse_expression(p, PREC_LOWEST);

    if (!Parser_expect_peek(p, T_RPAREN) || !Parser_expect_peek(p, T_LBRACE))
    {
        Expression_deinit(condition);
        free(condition);
        Token_deinit(&tok);
        return NULL;
    }

    BlockStatement* consequence = Parser_parse_block_statement(p);
    BlockStatement* alternative = NULL;

    if (Parser_peek_token_is(p, T_ELSE))
    {
        Parser_next_token(p);
        if (!Parser_expect_peek(p, T_LBRACE))
        {
            BlockStatement_deinit(consequence);
            free(consequence);
            Expression_deinit(condition);
            free(condition);
            Token_deinit(&tok);
            return NULL;
        }

        alternative = Parser_parse_block_statement(p);
    }

    IfExpression* result = malloc(sizeof(IfExpression));
    IfExpression_init(result);
    result->token = tok;
    result->condition = condition;
    result->consequence = *consequence;
    free(consequence);
    result->alternative = alternative;
    return (Expression*)result;
}

Expression* Parser_parse_function_literal(Parser* p)
{
    Token tok = Token_dup(&p->cur_token);

    if (!Parser_expect_peek(p, T_LPAREN))
    {
        Token_deinit(&tok);
        return NULL;
    }

    IdentifierVec* parameters_result = Parser_parse_function_parameters(p);
    if (parameters_result == NULL)
    {
        Token_deinit(&tok);
        return NULL;
    }
    IdentifierVec parameters = *parameters_result;
    free(parameters_result);

    if (!Parser_expect_peek(p, T_LBRACE))
    {
        Token_deinit(&tok);
        for (int i = 0; i < parameters.length; i++)
        {
            Identifier_deinit(parameters.data[i]);
            free(parameters.data[i]);
        }
        vec_deinit(&parameters);
        return NULL;
    }

    BlockStatement* body = Parser_parse_block_statement(p);

    FunctionLiteral* result = malloc(sizeof(FunctionLiteral));
    FunctionLiteral_init(result);
    result->token = tok;
    result->parameters = parameters;
    result->body = *body;
    free(body);
    return (Expression*)result;
}

bool Parser_cur_token_is(Parser* p, TokenType type)
{
    return strcmp(p->cur_token.type, type) == 0;
}

bool Parser_peek_token_is(Parser* p, TokenType type)
{
    return strcmp(p->peek_token.type, type) == 0;
}

Precedence Parser_peek_precedence(Parser* p)
{
    Precedence** prec = (Precedence**)Hash_lookup(&p->precedences, p->peek_token.type, strlen(p->peek_token.type) + 1);
    if (prec == NULL)
    {
        return PREC_LOWEST;
    }
    return **prec;
}

Precedence Parser_cur_precedence(Parser* p)
{
    Precedence** prec = (Precedence**)Hash_lookup(&p->precedences, p->cur_token.type, strlen(p->cur_token.type) + 1);
    if (prec == NULL)
    {
        return PREC_LOWEST;
    }
    return **prec;
}

bool Parser_expect_peek(Parser* p, TokenType type)
{
    if (Parser_peek_token_is(p, type))
    {
        Parser_next_token(p);
        return true;
    }
    else
    {
        Parser_peek_error(p, type);
        return false;
    }
}

void Parser_peek_error(Parser* p, TokenType type)
{
    sds msg = sdscatprintf(sdsempty(), "expected next token to be %s, got %s instead", type, p->peek_token.type);
    vec_push(&p->errors, msg);
}

void Parser_no_prefix_parse_fn_error(Parser* p, TokenType type)
{
    sds msg = sdscatprintf(sdsempty(), "no prefix parse function for %s found", type);
    vec_push(&p->errors, msg);
}
