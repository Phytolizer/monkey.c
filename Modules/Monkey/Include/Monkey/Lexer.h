#pragma once

#include "Monkey/Token.h"
#include <stddef.h>

typedef struct
{
    const char* input;
    const int inputLen;
    int position;
    int readPosition;
    char ch;
} Lexer;

Lexer LexerInit(const char* input, int inputLen);
Token LexerNextToken(Lexer* l);
