#include "monkey/lexer.h"
#include <stdbool.h>

void Lexer_read_char(Lexer* l);
sds Lexer_read_identifier(Lexer* l);
Token Token_new(TokenType type, char ch);
bool is_letter(char ch);
bool is_digit(char ch);
TokenType Lexer_lookup_ident(Lexer* l, const sds ident);
void Lexer_skip_whitespace(Lexer* l);
sds Lexer_read_number(Lexer* l);

void Lexer_init(Lexer* l, const char* input)
{
    l->input = sdsnew(input);
    l->position = 0;
    l->read_position = 0;
    l->ch = '\0';
    Lexer_read_char(l);
    Hash_init(&l->keywords);
    Hash_insert(&l->keywords, "fn", sizeof("fn"), T_FUNCTION, sizeof(T_FUNCTION));
    Hash_insert(&l->keywords, "let", sizeof("let"), T_LET, sizeof(T_LET));
}

void Lexer_deinit(Lexer* l)
{
    sdsfree(l->input);
    Hash_deinit(&l->keywords);
}

Token Lexer_next_token(Lexer* l)
{
    Token tok;

    Lexer_skip_whitespace(l);

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
        if (is_letter(l->ch))
        {
            tok.literal = Lexer_read_identifier(l);
            tok.type = Lexer_lookup_ident(l, tok.literal);
            return tok;
        }
        else if (is_digit(l->ch))
        {
            tok.literal = Lexer_read_number(l);
            tok.type = T_INT;
            return tok;
        }
        else
        {
            tok = Token_new(T_ILLEGAL, l->ch);
        }
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

sds Lexer_read_identifier(Lexer* l)
{
    size_t pos = l->position;
    while (is_letter(l->ch))
    {
        Lexer_read_char(l);
    }
    return sdsnewlen(&l->input[pos], l->position - pos);
}

Token Token_new(TokenType type, char ch)
{
    return (Token){
        .type = type,
        .literal = sdsnewlen(&ch, 1),
    };
}

bool is_letter(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

bool is_digit(char ch)
{
    return ch >= '0' && ch <= '9';
}

TokenType Lexer_lookup_ident(Lexer* l, const sds ident)
{
    TokenType* type = (TokenType*)Hash_lookup(&l->keywords, ident, sdslen(ident) + 1);
    if (type == NULL)
    {
        return T_IDENT;
    }
    return *type;
}

void Lexer_skip_whitespace(Lexer* l)
{
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r')
    {
        Lexer_read_char(l);
    }
}

sds Lexer_read_number(Lexer* l)
{
    size_t pos = l->position;
    while (is_digit(l->ch))
    {
        Lexer_read_char(l);
    }
    return sdsnewlen(&l->input[pos], l->position - pos);
}
