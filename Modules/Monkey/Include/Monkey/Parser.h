#pragma once

#include "Monkey/Ast.h"
#include "Monkey/Lexer.h"

#include <SimpleString/String.h>

typedef struct
{
    String* data;
    int length;
} ParserErrors;

typedef struct
{
    Lexer* lexer;
    ParserErrors errors;
    int errorsCapacity;
    Token curToken;
    Token peekToken;
} Parser;

Parser ParserInit(Lexer* l);
void ParserDeinit(Parser* p);
Program* ParseProgram(Parser* p);
