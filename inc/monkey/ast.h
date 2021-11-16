#pragma once

#include "monkey/token.h"
#include "sds.h"
#include "vec.h"

// enable introspection via use of X-macros
#define NODE_TYPES_                                                                                                    \
    X(STATEMENT)                                                                                                       \
    X(EXPRESSION)

typedef enum
{
#define X(x) NODE_TYPE_##x,
    NODE_TYPES_
#undef X
} NodeType;

typedef struct
{
    NodeType type;
} Node;

typedef struct
{
    Node base;
} Statement;

typedef vec_t(Statement) StatementVec;

typedef struct
{
    Node base;
} Expression;

sds Node_token_literal(Node* n);
sds Statement_token_literal(Statement* s);
sds Expression_token_literal(Expression* e);

typedef struct
{
    Node base;
    StatementVec statements;
} Program;

sds Program_token_literal(Program* p);

typedef struct
{
    Node base;
    Token token;
    sds value;
} Identifier;

sds Identifier_token_literal(Identifier* i);

typedef struct
{
    Node base;
    Token token;
    Identifier name;
    Expression* value;
} LetStatement;

sds LetStatement_token_literal(LetStatement* l);
