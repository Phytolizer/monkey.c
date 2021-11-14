#pragma once

#include "monkey/token.h"
#include "sds.h"

typedef struct
{
    sds input;
    size_t position;
    size_t read_position;
    char ch;
} Lexer;

void Lexer_init(Lexer* l, const char* input);
void Lexer_deinit(Lexer* l);
Token Lexer_next_token(Lexer* l);
