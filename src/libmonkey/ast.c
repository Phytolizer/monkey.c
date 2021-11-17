#include "monkey/ast.h"
#include <assert.h>
#include <stdbool.h>

void Expression_init(Expression* e)
{
    e->base.type = NODE_TYPE_EXPRESSION;
}

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
    switch (s->type)
    {
    case STATEMENT_TYPE_LET:
        return sdsdup(((LetStatement*)s)->token.literal);
    }

    assert(false && "corrupt statement type");
}

sds Expression_token_literal(Expression* e)
{
    switch (e->type)
    {
    case EXPRESSION_TYPE_IDENTIFIER:
        return sdsdup(((Identifier*)e)->token.literal);
    }

    assert(false && "corrupt expression type");
}

sds Program_token_literal(Program* p)
{
    sds result = sdsempty();
    for (int i = 0; i < p->statements.length; i++)
    {
        Statement* s = p->statements.data[i];
        result = sdscat(result, Statement_token_literal(s));
    }
    return result;
}

void Identifier_init(Identifier* i)
{
    Expression_init(&i->base);
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

void Statement_init(Statement* s)
{
    s->base.type = NODE_TYPE_STATEMENT;
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

void LetStatement_init(LetStatement* s)
{
    Statement_init(&s->base);
    s->base.type = STATEMENT_TYPE_LET;
}

void Statement_deinit(Statement* s)
{
    switch (s->type)
    {
    case STATEMENT_TYPE_LET:
        LetStatement_deinit((LetStatement*)s);
        break;
    }
}

void Expression_deinit(Expression* e)
{
    switch (e->type)
    {
    case EXPRESSION_TYPE_IDENTIFIER:
        Identifier_deinit((Identifier*)e);
        break;
    }
}

void LetStatement_deinit(LetStatement* s)
{
    Token_deinit(&s->token);
    Identifier_deinit(&s->name);
    if (s->value != NULL)
    {
        Expression_deinit(s->value);
    }
}

void Identifier_deinit(Identifier* i)
{
    Token_deinit(&i->token);
    sdsfree(i->value);
}

void Program_init(Program* p)
{
    p->base.type = NODE_TYPE_PROGRAM;
    vec_init(&p->statements);
}

void Program_deinit(Program* p)
{
    for (int i = 0; i < p->statements.length; i++)
    {
        Statement* s = p->statements.data[i];
        Statement_deinit(s);
        free(s);
    }
    vec_deinit(&p->statements);
}
