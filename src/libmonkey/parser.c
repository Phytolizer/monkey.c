#include "monkey/parser.h"
#include <stdbool.h>

typedef enum
{
    PREC_LOWEST,
    PREC_EQUALS,
    PREC_LESSGREATER,
    PREC_SUM,
    PREC_PRODUCT,
    PREC_PREFIX,
    PREC_CALL,
} Precedence;

void Parser_next_token(Parser* p);
Statement* Parser_parse_statement(Parser* p);
LetStatement* Parser_parse_let_statement(Parser* p);
ReturnStatement* Parser_parse_return_statement(Parser* p);
ExpressionStatement* Parser_parse_expression_statement(Parser* p);
Expression* Parser_parse_expression(Parser* p, Precedence precedence);
Expression* Parser_parse_identifier(Parser* p);
Expression* Parser_parse_integer_literal(Parser* p);
Expression* Parser_parse_prefix_expression(Parser* p);
bool Parser_cur_token_is(Parser* p, TokenType type);
bool Parser_peek_token_is(Parser* p, TokenType type);
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

    // TODO: parse expression
    while (!Parser_cur_token_is(p, T_SEMICOLON))
    {
        Parser_next_token(p);
    }

    LetStatement* stmt = malloc(sizeof(LetStatement));
    LetStatement_init(stmt);
    stmt->token = tok;
    stmt->name = name;
    stmt->value = NULL;
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

bool Parser_cur_token_is(Parser* p, TokenType type)
{
    return strcmp(p->cur_token.type, type) == 0;
}

bool Parser_peek_token_is(Parser* p, TokenType type)
{
    return strcmp(p->peek_token.type, type) == 0;
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
