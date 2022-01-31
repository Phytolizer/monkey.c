#pragma once

#include "Monkey/Token.h"

#include <SimpleString/String.h>

// Debugging is more convenient with a union representation of the AST.
// However, struct inheritance uses less memory.
// To solve this, use a compile-time switch to determine the memory layout of the AST.
//
// NOTE: This means that both Debug and Release profiles should be tested!
#ifdef DEBUG
#define AST_NODE_REPR_UNION 1
#else
#define AST_NODE_REPR_UNION 0
#endif

#if AST_NODE_REPR_UNION

typedef struct
{
    Token token;
    String value;
} Identifier;

typedef enum
{
    ExpressionTypeIdentifier,
} ExpressionType;

typedef struct
{
    ExpressionType type;
    union {
        Identifier identifier;
    };
} Expression;

typedef struct
{
    Token token;
    Identifier* name;
    Expression* value;
} LetStatement;

#define STATEMENT_TYPES_X X(Let)

typedef enum
{
#define X(x) StatementType##x,
    STATEMENT_TYPES_X
#undef X
} StatementType;

StringSpan StatementTypeName(StatementType type);

typedef struct
{
    StatementType type;
    union {
        LetStatement letStatement;
    };
} Statement;

typedef struct
{
    Statement** statements;
    int statementsLength;
} Program;

typedef enum
{
    NodeTypeProgram,
    NodeTypeStatement,
    NodeTypeExpression,
} NodeType;

typedef struct
{
    NodeType type;
    union {
        Program program;
        Statement statement;
        Expression expression;
    };
} Node;

Program* ProgramInit(Statement** statements, int statementsLength);
Node* LetStatementInit(Token token, Identifier* name, Expression* value);
Node* IdentifierInit(Token token, String value);

String NodeTokenLiteral(Node* n);
String StatementTokenLiteral(Statement* s);
String ExpressionTokenLiteral(Expression* e);

#else
#endif
