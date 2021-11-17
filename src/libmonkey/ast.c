#include "monkey/ast.h"
#include <assert.h>
#include <stdbool.h>

sds Node_token_literal(Node* n)
{
    switch (n->type)
    {
    case NODE_TYPE_STATEMENT:
        return Statement_token_literal((Statement*)n);
    case NODE_TYPE_EXPRESSION:
        return Expression_token_literal((Expression*)n);
    }

    assert(false && "corrupt node type");
}

sds Statement_token_literal(Statement* s)
{
    // TODO
    return sdsempty();
}

sds Expression_token_literal(Expression* e)
{
    // TODO
    return sdsempty();
}

sds Program_token_literal(Program* p)
{
    sds result = sdsempty();
    for (int i = 0; i < p->statements.length; i++)
    {
        Statement* s = &p->statements.data[i];
        result = sdscat(result, Statement_token_literal(s));
    }
    return result;
}

sds Identifier_token_literal(Identifier* i)
{
    return sdsdup(i->value);
}

sds LetStatement_token_literal(LetStatement* l)
{
    sds result = sdsnewlen("let ", 4);
    sds name = Identifier_token_literal(&l->name);
    result = sdscat(result, name);
    sdsfree(name);
    result = sdscat(result, " = ");
    if (l->value != NULL)
    {
        sds value = Expression_token_literal(l->value);
        result = sdscat(result, value);
        sdsfree(value);
    }
    result = sdscat(result, ";");
    return result;
}

const char* Statement_type_name(StatementType type)
{
    static const char* STATEMENT_TYPE_NAMES[] = {
#define X(x) #x,
        STATEMENT_TYPES_
#undef X
    };
    return STATEMENT_TYPE_NAMES[type];
}
