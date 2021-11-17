#include "monkey/parser.h"
#include <stdbool.h>

void Parser_next_token(Parser* p);
Statement* Parser_parse_statement(Parser* p);
LetStatement* Parser_parse_let_statement(Parser* p);
bool Parser_cur_token_is(Parser* p, TokenType type);
bool Parser_peek_token_is(Parser* p, TokenType type);
bool Parser_expect_peek(Parser* p, TokenType type);

void Parser_init(Parser* p, const char* input)
{
    Lexer_init(&p->l, input);
    p->cur_token.literal = NULL;
    p->peek_token.literal = NULL;
    Parser_next_token(p);
    Parser_next_token(p);
}

void Parser_deinit(Parser* p)
{
    Lexer_deinit(&p->l);
    Token_deinit(&p->cur_token);
    Token_deinit(&p->peek_token);
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
    else
    {
        return NULL;
    }
}

LetStatement* Parser_parse_let_statement(Parser* p)
{
    Token tok = p->cur_token;
    if (!Parser_expect_peek(p, T_IDENT))
    {
        return NULL;
    }

    Identifier name = {
        .token = p->cur_token,
        .value = p->cur_token.literal,
    };

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
        return false;
    }
}
