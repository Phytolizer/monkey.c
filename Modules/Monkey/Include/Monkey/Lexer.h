#pragma once

#include "Monkey/Token.h"

#include <SimpleString/String.h>
#include <stddef.h>

typedef struct
{
    StringSpan input;
    int position;
    int readPosition;
    char ch;
} Lexer;

Lexer LexerInit(StringSpan input);
Token LexerNextToken(Lexer* l);
