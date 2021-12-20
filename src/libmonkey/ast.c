#include "monkey/ast.h"
#include "sds.h"
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
    case STATEMENT_TYPE_BLOCK:
        return BlockStatement_token_literal((BlockStatement*)s);
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
    case EXPRESSION_TYPE_BOOLEAN:
        return sdsdup(((Boolean*)e)->token.literal);
    case EXPRESSION_TYPE_IF:
        return sdsdup(((IfExpression*)e)->token.literal);
    case EXPRESSION_TYPE_FUNCTION:
        return sdsdup(((FunctionLiteral*)e)->token.literal);
    case EXPRESSION_TYPE_CALL:
        return sdsdup(((CallExpression*)e)->token.literal);
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
    case STATEMENT_TYPE_BLOCK:
        BlockStatement_deinit((BlockStatement*)s);
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
    case EXPRESSION_TYPE_BOOLEAN:
        Boolean_deinit((Boolean*)e);
        return;
    case EXPRESSION_TYPE_IF:
        IfExpression_deinit((IfExpression*)e);
        return;
    case EXPRESSION_TYPE_FUNCTION:
        FunctionLiteral_deinit((FunctionLiteral*)e);
        return;
    case EXPRESSION_TYPE_CALL:
        CallExpression_deinit((CallExpression*)e);
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
    case STATEMENT_TYPE_BLOCK:
        return BlockStatement_string((BlockStatement*)s);
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
    case EXPRESSION_TYPE_BOOLEAN:
        return Boolean_string((Boolean*)e);
    case EXPRESSION_TYPE_IF:
        return IfExpression_string((IfExpression*)e);
    case EXPRESSION_TYPE_FUNCTION:
        return FunctionLiteral_string((FunctionLiteral*)e);
    case EXPRESSION_TYPE_CALL:
        return CallExpression_string((CallExpression*)e);
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
    return Expression_string(e->expression);
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
    if (p->right)
    {
        Expression_deinit(p->right);
        free(p->right);
    }
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
    if (i->left)
    {
        Expression_deinit(i->left);
        free(i->left);
    }
    if (i->right)
    {
        Expression_deinit(i->right);
        free(i->right);
    }
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

void Boolean_init(Boolean* b)
{
    Expression_init(&b->base);
    b->base.type = EXPRESSION_TYPE_BOOLEAN;
}

void Boolean_deinit(Boolean* b)
{
    Token_deinit(&b->token);
}

sds Boolean_token_literal(Boolean* b)
{
    return sdsdup(b->token.literal);
}

sds Boolean_string(Boolean* b)
{
    return sdsdup(b->token.literal);
}

void BlockStatement_init(BlockStatement* b)
{
    Statement_init(&b->base);
    b->base.type = STATEMENT_TYPE_BLOCK;
}

void BlockStatement_deinit(BlockStatement* b)
{
    Token_deinit(&b->token);
    for (int i = 0; i < b->statements.length; ++i)
    {
        Statement_deinit(b->statements.data[i]);
        free(b->statements.data[i]);
    }
    vec_deinit(&b->statements);
}

sds BlockStatement_token_literal(BlockStatement* b)
{
    return sdsdup(b->token.literal);
}

sds BlockStatement_string(BlockStatement* b)
{
    sds s = sdsempty();
    for (int i = 0; i < b->statements.length; ++i)
    {
        sds stmt = Statement_string(b->statements.data[i]);
        s = sdscatsds(s, stmt);
        sdsfree(stmt);
    }
    return s;
}

void IfExpression_init(IfExpression* i)
{
    Expression_init(&i->base);
    i->base.type = EXPRESSION_TYPE_IF;
}

void IfExpression_deinit(IfExpression* i)
{
    Token_deinit(&i->token);
    Expression_deinit(i->condition);
    free(i->condition);
    BlockStatement_deinit(&i->consequence);
    if (i->alternative != NULL)
    {
        BlockStatement_deinit(i->alternative);
        free(i->alternative);
    }
}

sds IfExpression_token_literal(IfExpression* i)
{
    return sdsdup(i->token.literal);
}

sds IfExpression_string(IfExpression* i)
{
    sds s = sdsnew("if");
    sds condition = Expression_string(i->condition);
    s = sdscatsds(s, condition);
    sdsfree(condition);
    s = sdscat(s, " ");
    sds consequence = BlockStatement_string(&i->consequence);
    s = sdscatsds(s, consequence);
    sdsfree(consequence);
    if (i->alternative != NULL)
    {
        s = sdscat(s, " else ");
        sds alternative = BlockStatement_string(i->alternative);
        s = sdscatsds(s, alternative);
        sdsfree(alternative);
    }
    return s;
}

void FunctionLiteral_init(FunctionLiteral* f)
{
    Expression_init(&f->base);
    f->base.type = EXPRESSION_TYPE_FUNCTION;
}

void FunctionLiteral_deinit(FunctionLiteral* f)
{
    Token_deinit(&f->token);
    for (int i = 0; i < f->parameters.length; ++i)
    {
        Identifier_deinit(f->parameters.data[i]);
        free(f->parameters.data[i]);
    }
    vec_deinit(&f->parameters);
    BlockStatement_deinit(&f->body);
}

sds FunctionLiteral_token_literal(FunctionLiteral* f)
{
    return sdsdup(f->token.literal);
}

sds FunctionLiteral_string(FunctionLiteral* f)
{
    sds s = sdsempty();
    s = sdscatsds(s, f->token.literal);
    s = sdscat(s, "(");
    for (int i = 0; i < f->parameters.length; ++i)
    {
        sds param = Identifier_token_literal(f->parameters.data[i]);
        s = sdscatsds(s, param);
        sdsfree(param);
        if (i < f->parameters.length - 1)
        {
            s = sdscat(s, ", ");
        }
    }
    s = sdscat(s, ") ");
    sds body = BlockStatement_string(&f->body);
    s = sdscatsds(s, body);
    sdsfree(body);
    return s;
}

void CallExpression_init(CallExpression* c)
{
    Expression_init(&c->base);
    c->base.type = EXPRESSION_TYPE_CALL;
}

void CallExpression_deinit(CallExpression* c)
{
    Token_deinit(&c->token);
    Expression_deinit(c->function);
    free(c->function);
    for (int i = 0; i < c->args.length; ++i)
    {
        Expression_deinit(c->args.data[i]);
        free(c->args.data[i]);
    }
    vec_deinit(&c->args);
}

sds CallExpression_token_literal(CallExpression* c)
{
    return sdsdup(c->token.literal);
}

sds CallExpression_string(CallExpression* c)
{
    sds s = sdsempty();
    sds function = Expression_string(c->function);
    s = sdscatsds(s, function);
    sdsfree(function);
    s = sdscat(s, "(");
    for (int i = 0; i < c->args.length; ++i)
    {
        sds arg = Expression_string(c->args.data[i]);
        s = sdscatsds(s, arg);
        sdsfree(arg);
        if (i < c->args.length - 1)
        {
            s = sdscat(s, ", ");
        }
    }
    s = sdscat(s, ")");
    return s;
}
