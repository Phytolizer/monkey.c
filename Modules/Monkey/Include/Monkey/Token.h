#pragma once

#include "Monkey/Prelude.h"
// The above always comes first!

#include <SimpleString/String.h>

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

typedef String TokenType;

#define X(x, y) extern const TokenType TokenType##x;
TOKEN_TYPES_X
#undef X

typedef struct
{
    TokenType type;
    String literal;
} Token;

TokenType LookupIdent(StringSpan ident);
