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
    case NODE_TYPE_PROGRAM:
        return Program_token_literal((Program*)n);
    }

    assert(false && "corrupt node type");
}

sds Statement_token_literal(Statement* s)
{
    switch (s->type)
    {
    case STATEMENT_TYPE_LET:
        return LetStatement_token_literal((LetStatement*)s);
    case STATEMENT_TYPE_RETURN:
        return ReturnStatement_token_literal((ReturnStatement*)s);
    case STATEMENT_TYPE_EXPRESSION:
        return ExpressionStatement_token_literal((ExpressionStatement*)s);
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
    if (p->statements.length > 0)
    {
        return Statement_token_literal(p->statements.data[0]);
    }
    return sdsempty();
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
    return sdsdup(l->token.literal);
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
    case STATEMENT_TYPE_RETURN:
        ReturnStatement_deinit((ReturnStatement*)s);
        break;
    case STATEMENT_TYPE_EXPRESSION:
        ExpressionStatement_deinit((ExpressionStatement*)s);
        break;
    }

    assert(false && "corrupt statement type");
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
        free(s->value);
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

void ReturnStatement_init(ReturnStatement* s)
{
    Statement_init(&s->base);
    s->base.type = STATEMENT_TYPE_RETURN;
}

void ReturnStatement_deinit(ReturnStatement* s)
{
    Token_deinit(&s->token);
    if (s->return_value != NULL)
    {
        Expression_deinit(s->return_value);
        free(s->return_value);
    }
}

sds ReturnStatement_token_literal(ReturnStatement* r)
{
    return sdsdup(r->token.literal);
}

void ExpressionStatement_init(ExpressionStatement* s)
{
    Statement_init(&s->base);
    s->base.type = STATEMENT_TYPE_EXPRESSION;
}

void ExpressionStatement_deinit(ExpressionStatement* s)
{
    Expression_deinit(s->expression);
    free(s->expression);
    Token_deinit(&s->token);
}

sds ExpressionStatement_token_literal(ExpressionStatement* s)
{
    return sdsdup(s->token.literal);
}

void Node_deinit(Node* node)
{
    switch (node->type)
    {
    case NODE_TYPE_PROGRAM:
        Program_deinit((Program*)node);
        break;
    case NODE_TYPE_STATEMENT:
        Statement_deinit((Statement*)node);
        break;
    case NODE_TYPE_EXPRESSION:
        Expression_deinit((Expression*)node);
        break;
    }

    assert(false && "corrupt node type");
}

sds Node_string(Node* node)
{
    switch (node->type)
    {
    case NODE_TYPE_PROGRAM:
        return Program_string((Program*)node);
    case NODE_TYPE_STATEMENT:
        return Statement_string((Statement*)node);
    case NODE_TYPE_EXPRESSION:
        return Expression_string((Expression*)node);
    }

    assert(false && "corrupt node type");
}

sds Statement_string(Statement* s)
{
    switch (s->type)
    {
    case STATEMENT_TYPE_LET:
        return LetStatement_string((LetStatement*)s);
    case STATEMENT_TYPE_RETURN:
        return ReturnStatement_string((ReturnStatement*)s);
    case STATEMENT_TYPE_EXPRESSION:
        return ExpressionStatement_string((ExpressionStatement*)s);
    }

    assert(false && "corrupt statement type");
}

sds Expression_string(Expression* e)
{
    switch (e->type)
    {
    case EXPRESSION_TYPE_IDENTIFIER:
        return Identifier_string((Identifier*)e);
    }

    assert(false && "corrupt expression type");
}

sds Program_string(Program* p)
{
    sds s = sdsempty();
    for (int i = 0; i < p->statements.length; i++)
    {
        s = sdscat(s, Statement_string(p->statements.data[i]));
    }
    return s;
}

sds Identifier_string(Identifier* i)
{
    return sdsdup(i->value);
}

sds LetStatement_string(LetStatement* l)
{
    sds s = sdsempty();
    s = sdscat(s, l->token.literal);
    s = sdscat(s, " ");
    sds name = Identifier_string(&l->name);
    s = sdscatsds(s, name);
    sdsfree(name);
    if (l->value != NULL)
    {
        s = sdscat(s, " = ");
        sds value = Expression_string(l->value);
        s = sdscatsds(s, value);
        sdsfree(value);
    }
    s = sdscat(s, ";");
    return s;
}

sds ReturnStatement_string(ReturnStatement* r)
{
    sds s = sdsempty();
    s = sdscat(s, r->token.literal);
    s = sdscat(s, " ");
    if (r->return_value != NULL)
    {
        sds value = Expression_string(r->return_value);
        s = sdscatsds(s, value);
        sdsfree(value);
    }
    s = sdscat(s, ";");
    return s;
}

sds ExpressionStatement_string(ExpressionStatement* e)
{
    sds s = sdsempty();
    sds expression = Expression_string(e->expression);
    s = sdscatsds(s, expression);
    sdsfree(expression);
    s = sdscat(s, ";");
    return s;
}
