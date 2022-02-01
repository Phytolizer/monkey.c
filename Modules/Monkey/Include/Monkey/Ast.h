#pragma once

#include "Monkey/Token.h"

#include <SimpleString/String.h>

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
    } as;
} Expression;

typedef struct
{
    Token token;
    Identifier* name;
    Expression* value;
} LetStatement;

typedef struct
{
    Token token;
    Expression* returnValue;
} ReturnStatement;

#define STATEMENT_TYPES_X                                                                                              \
    X(Let)                                                                                                             \
    X(Return)

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
        ReturnStatement returnStatement;
    } as;
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
    } as;
} Node;

Program* ProgramInit(Statement** statements, int statementsLength);
void ProgramDeinit(Program* p);
void StatementDeinit(Statement* s);
void ExpressionDeinit(Expression* e);
Node* LetStatementInit(Token token, Identifier* name, Expression* value);
Node* IdentifierInit(Token token, String value);

String NodeTokenLiteral(Node* n);
String StatementTokenLiteral(Statement* s);
String ExpressionTokenLiteral(Expression* e);
