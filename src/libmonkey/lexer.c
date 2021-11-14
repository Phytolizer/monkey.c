#include "monkey/lexer.h"

void Lexer_read_char(Lexer* l);
Token Token_new(TokenType type, char ch);

void Lexer_init(Lexer* l, const char* input)
{
    l->input = sdsnew(input);
    l->position = 0;
    l->read_position = 0;
    l->ch = '\0';
    Lexer_read_char(l);
}

void Lexer_deinit(Lexer* l)
{
    sdsfree(l->input);
}

Token Lexer_next_token(Lexer* l)
{
    Token tok;

    switch (l->ch)
    {
    case '=':
        tok = Token_new(T_ASSIGN, l->ch);
        break;
    case ';':
        tok = Token_new(T_SEMICOLON, l->ch);
        break;
    case '(':
        tok = Token_new(T_LPAREN, l->ch);
        break;
    case ')':
        tok = Token_new(T_RPAREN, l->ch);
        break;
    case ',':
        tok = Token_new(T_COMMA, l->ch);
        break;
    case '+':
        tok = Token_new(T_PLUS, l->ch);
        break;
    case '{':
        tok = Token_new(T_LBRACE, l->ch);
        break;
    case '}':
        tok = Token_new(T_RBRACE, l->ch);
        break;
    case '\0':
        tok.literal = sdsempty();
        tok.type = T_EOF;
        break;
    default:
        break;
    }

    Lexer_read_char(l);
    return tok;
}

void Lexer_read_char(Lexer* l)
{
    if (l->read_position >= sdslen(l->input))
    {
        l->ch = '\0';
    }
    else
    {
        l->ch = l->input[l->read_position];
    }
    l->position = l->read_position;
    ++l->read_position;
}

Token Token_new(TokenType type, char ch)
{
    return (Token){
        .type = type,
        .literal = sdsnewlen(&ch, 1),
    };
}