#pragma once

#include "sds.h"

typedef const char* TokenType;

typedef struct
{
    TokenType type;
    sds literal;
} Token;

void Token_deinit(Token* t);

#define T_ILLEGAL "ILLEGAL"
#define T_EOF "EOF"

#define T_IDENT "IDENT"
#define T_INT "INT"

#define T_ASSIGN "="
#define T_PLUS "+"
#define T_BANG "!"
#define T_MINUS "-"
#define T_SLASH "/"
#define T_ASTERISK "*"

#define T_LT "<"
#define T_GT ">"

#define T_COMMA ","
#define T_SEMICOLON ";"

#define T_LPAREN "("
#define T_RPAREN ")"
#define T_LBRACE "{"
#define T_RBRACE "}"

#define T_FUNCTION "FUNCTION"
#define T_LET "LET"
