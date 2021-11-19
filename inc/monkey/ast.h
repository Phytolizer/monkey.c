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

void Node_deinit(Node* node);
sds Node_token_literal(Node* node);
sds Node_string(Node* node);

#define STATEMENT_TYPES_                                                                                               \
    X(LET)                                                                                                             \
    X(RETURN)                                                                                                          \
    X(EXPRESSION)

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
sds Statement_token_literal(Statement* s);
sds Statement_string(Statement* s);

typedef vec_t(Statement*) StatementVec;

#define EXPRESSION_TYPES_                                                                                              \
    X(IDENTIFIER)                                                                                                      \
    X(INTEGER)

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
const char* Expression_type_name(ExpressionType type);
sds Expression_token_literal(Expression* e);
sds Expression_string(Expression* e);

typedef struct
{
    Node base;
    StatementVec statements;
} Program;

void Program_init(Program* p);
void Program_deinit(Program* p);
sds Program_token_literal(Program* p);
sds Program_string(Program* p);

typedef struct
{
    Expression base;
    Token token;
    sds value;
} Identifier;

void Identifier_init(Identifier* i);
void Identifier_deinit(Identifier* i);
sds Identifier_token_literal(Identifier* i);
sds Identifier_string(Identifier* i);

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
sds LetStatement_string(LetStatement* l);

typedef struct
{
    Statement base;
    Token token;
    Expression* return_value;
} ReturnStatement;

void ReturnStatement_init(ReturnStatement* s);
void ReturnStatement_deinit(ReturnStatement* s);
sds ReturnStatement_token_literal(ReturnStatement* r);
sds ReturnStatement_string(ReturnStatement* r);

typedef struct
{
    Statement base;
    Token token;
    Expression* expression;
} ExpressionStatement;

void ExpressionStatement_init(ExpressionStatement* s);
void ExpressionStatement_deinit(ExpressionStatement* s);
sds ExpressionStatement_token_literal(ExpressionStatement* s);
sds ExpressionStatement_string(ExpressionStatement* s);

typedef struct
{
    Expression base;
    Token token;
    int64_t value;
} IntegerLiteral;

void IntegerLiteral_init(IntegerLiteral* i);
void IntegerLiteral_deinit(IntegerLiteral* i);
sds IntegerLiteral_token_literal(IntegerLiteral* i);
sds IntegerLiteral_string(IntegerLiteral* i);
