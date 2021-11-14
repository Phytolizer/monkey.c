#include "monkey/token.h"

void Token_deinit(Token* t)
{
    sdsfree(t->literal);
}
