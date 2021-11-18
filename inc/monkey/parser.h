#pragma once

#include "hash/hash.h"
#include "monkey/ast.h"
#include "monkey/lexer.h"
#include "vec.h"

typedef vec_t(sds) SdsVec;

struct Parser;

typedef Expression* (*PrefixParseFn)(struct Parser*);
typedef Expression* (*InfixParseFn)(struct Parser*, Expression*);

typedef struct Parser
{
    Lexer l;

    Token cur_token;
    Token peek_token;

    SdsVec errors;

    ViewHash prefix_parse_fns;
    ViewHash infix_parse_fns;
} Parser;

void Parser_init(Parser* p, const char* input);
void Parser_deinit(Parser* p);
Program Parser_parse_program(Parser* p);
