#pragma once

#include "monkey/ast.h"
#include "monkey/lexer.h"
#include "vec.h"

typedef vec_t(sds) SdsVec;

typedef struct
{
    Lexer l;

    Token cur_token;
    Token peek_token;

    SdsVec errors;
} Parser;

void Parser_init(Parser* p, const char* input);
void Parser_deinit(Parser* p);
Program Parser_parse_program(Parser* p);
