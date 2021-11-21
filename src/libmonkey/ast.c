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
    case EXPRESSION_TYPE_INTEGER:
        return sdsdup(((IntegerLiteral*)e)->token.literal);
    case EXPRESSION_TYPE_PREFIX:
        return sdsdup(((PrefixExpression*)e)->token.literal);
    case EXPRESSION_TYPE_INFIX:
        return sdsdup(((InfixExpression*)e)->token.literal);
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
    i->base.type = EXPRESSION_TYPE_IDENTIFIER;
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

const char* Expression_type_name(ExpressionType type)
{
    static const char* EXPRESSION_TYPE_NAMES[] = {
#define X(x) #x,
        EXPRESSION_TYPES_
#undef X
    };
    return EXPRESSION_TYPE_NAMES[type];
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
        return;
    case STATEMENT_TYPE_RETURN:
        ReturnStatement_deinit((ReturnStatement*)s);
        return;
    case STATEMENT_TYPE_EXPRESSION:
        ExpressionStatement_deinit((ExpressionStatement*)s);
        return;
    }

    assert(false && "corrupt statement type");
}

void Expression_deinit(Expression* e)
{
    switch (e->type)
    {
    case EXPRESSION_TYPE_IDENTIFIER:
        Identifier_deinit((Identifier*)e);
        return;
    case EXPRESSION_TYPE_INTEGER:
        IntegerLiteral_deinit((IntegerLiteral*)e);
        return;
    case EXPRESSION_TYPE_PREFIX:
        PrefixExpression_deinit((PrefixExpression*)e);
        return;
    case EXPRESSION_TYPE_INFIX:
        InfixExpression_deinit((InfixExpression*)e);
        return;
    }

    assert(false && "corrupt expression type");
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
    if (s->expression)
    {
        Expression_deinit(s->expression);
    }
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
        return;
    case NODE_TYPE_STATEMENT:
        Statement_deinit((Statement*)node);
        return;
    case NODE_TYPE_EXPRESSION:
        Expression_deinit((Expression*)node);
        return;
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
    case EXPRESSION_TYPE_INTEGER:
        return IntegerLiteral_string((IntegerLiteral*)e);
    case EXPRESSION_TYPE_PREFIX:
        return PrefixExpression_string((PrefixExpression*)e);
    case EXPRESSION_TYPE_INFIX:
        return InfixExpression_string((InfixExpression*)e);
    }

    assert(false && "corrupt expression type");
}

sds Program_string(Program* p)
{
    sds s = sdsempty();
    for (int i = 0; i < p->statements.length; i++)
    {
        sds stmt = Statement_string(p->statements.data[i]);
        s = sdscat(s, stmt);
        sdsfree(stmt);
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
    return s;
}

void IntegerLiteral_init(IntegerLiteral* i)
{
    Expression_init(&i->base);
    i->base.type = EXPRESSION_TYPE_INTEGER;
}

void IntegerLiteral_deinit(IntegerLiteral* i)
{
    Token_deinit(&i->token);
}

sds IntegerLiteral_token_literal(IntegerLiteral* i)
{
    return sdsdup(i->token.literal);
}

sds IntegerLiteral_string(IntegerLiteral* i)
{
    return sdsdup(i->token.literal);
}

void PrefixExpression_init(PrefixExpression* p)
{
    Expression_init(&p->base);
    p->base.type = EXPRESSION_TYPE_PREFIX;
}

void PrefixExpression_deinit(PrefixExpression* p)
{
    Token_deinit(&p->token);
    sdsfree(p->operator);
    Expression_deinit(p->right);
    free(p->right);
}

sds PrefixExpression_token_literal(PrefixExpression* p)
{
    return sdsdup(p->token.literal);
}

sds PrefixExpression_string(PrefixExpression* p)
{
    sds s = sdsempty();
    s = sdscat(s, "(");
    s = sdscat(s, p->operator);
    sds right = Expression_string(p->right);
    s = sdscatsds(s, right);
    sdsfree(right);
    s = sdscat(s, ")");
    return s;
}

void InfixExpression_init(InfixExpression* i)
{
    Expression_init(&i->base);
    i->base.type = EXPRESSION_TYPE_INFIX;
}

void InfixExpression_deinit(InfixExpression* i)
{
    Token_deinit(&i->token);
    sdsfree(i->operator);
    Expression_deinit(i->left);
    free(i->left);
    Expression_deinit(i->right);
    free(i->right);
}

sds InfixExpression_token_literal(InfixExpression* i)
{
    return sdsdup(i->token.literal);
}

sds InfixExpression_string(InfixExpression* i)
{
    sds s = sdsempty();
    s = sdscat(s, "(");
    sds left = Expression_string(i->left);
    s = sdscatsds(s, left);
    sdsfree(left);
    s = sdscat(s, " ");
    s = sdscat(s, i->operator);
    s = sdscat(s, " ");
    sds right = Expression_string(i->right);
    s = sdscatsds(s, right);
    sdsfree(right);
    s = sdscat(s, ")");
    return s;
}
