#pragma once

#include "monkey/token.h"
#include "sds.h"
#include "vec.h"

// enable introspection via use of X-macros
#define NODE_TYPES_                                                                                                    \
    X(STATEMENT)                                                                                                       \
    X(EXPRESSION)                                                                                                      \
    X(PROGRAM)

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

#define STATEMENT_TYPES_ X(LET)

typedef enum
{
#define X(x) STATEMENT_TYPE_##x,
    STATEMENT_TYPES_
#undef X
} StatementType;

typedef struct
{
    Node base;
    StatementType type;
} Statement;

void Statement_init(Statement* s);
void Statement_deinit(Statement* s);
const char* Statement_type_name(StatementType type);

typedef vec_t(Statement*) StatementVec;

#define EXPRESSION_TYPES_ X(IDENTIFIER)

typedef enum
{
#define X(x) EXPRESSION_TYPE_##x,
    EXPRESSION_TYPES_
#undef X
} ExpressionType;

typedef struct
{
    Node base;
    ExpressionType type;
} Expression;

void Expression_init(Expression* e);
void Expression_deinit(Expression* e);

sds Node_token_literal(Node* n);
sds Statement_token_literal(Statement* s);
sds Expression_token_literal(Expression* e);

typedef struct
{
    Node base;
    StatementVec statements;
} Program;

void Program_init(Program* p);
void Program_deinit(Program* p);
sds Program_token_literal(Program* p);

typedef struct
{
    Expression base;
    Token token;
    sds value;
} Identifier;

void Identifier_init(Identifier* i);
void Identifier_deinit(Identifier* i);
sds Identifier_token_literal(Identifier* i);

typedef struct
{
    Statement base;
    Token token;
    Identifier name;
    Expression* value;
} LetStatement;

void LetStatement_init(LetStatement* s);
void LetStatement_deinit(LetStatement* s);
sds LetStatement_token_literal(LetStatement* l);
