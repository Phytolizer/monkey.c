#include "monkey/token.h"

Token Token_dup(Token* t)
{
    Token dup = {
        .type = t->type,
        .literal = sdsdup(t->literal),
    };
    return dup;
}

void Token_deinit(Token* t)
{
    sdsfree(t->literal);
}
