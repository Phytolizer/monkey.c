#pragma once

#define TOKEN_TYPES_X                                                                                                  \
    X(Illegal, "ILLEGAL")                                                                                              \
    X(Eof, "EOF")                                                                                                      \
    X(Ident, "IDENT")                                                                                                  \
    X(Int, "INT")                                                                                                      \
    X(Assign, "=")                                                                                                     \
    X(Plus, "+")                                                                                                       \
    X(Comma, ",")                                                                                                      \
    X(Semicolon, ";")                                                                                                  \
    X(Lparen, "(")                                                                                                     \
    X(Rparen, ")")                                                                                                     \
    X(Lbrace, "{")                                                                                                     \
    X(Rbrace, "}")                                                                                                     \
    X(Function, "FUNCTION")                                                                                            \
    X(Let, "LET")

typedef struct
{
    const char* data;
    int len;
} TokenType;

#define X(x, y) extern const TokenType TokenType##x;
TOKEN_TYPES_X
#undef X

typedef struct
{
    TokenType type;
    char* literal;
    int literalLen;
} Token;
