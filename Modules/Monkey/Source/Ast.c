#include "Monkey/Ast.h"

#include <stdlib.h>

#if AST_NODE_REPR_UNION

static String ProgramTokenLiteral(Program* p);

StringSpan StatementTypeName(StatementType type)
{
    switch (type)
    {
#define X(x)                                                                                                           \
    case StatementType##x:                                                                                             \
        return STRING_SPAN("StatementType" #x);
        STATEMENT_TYPES_X
#undef X
    }

    return STRING_SPAN("<invalid>");
}

Program* ProgramInit(Statement** statements, int statementsLength)
{
    Program* result = calloc(sizeof(Program), 1);
    result->statements = statements;
    result->statementsLength = statementsLength;

    return result;
}

Node* LetStatementInit(Token token, Identifier* name, Expression* value)
{
    Node* result = calloc(sizeof(Node), 1);
    result->type = NodeTypeStatement;
    result->statement.type = StatementTypeLet;
    result->statement.letStatement.token = token;
    result->statement.letStatement.name = name;
    result->statement.letStatement.value = value;

    return result;
}

Node* IdentifierInit(Token token, String value)
{
    Node* result = calloc(sizeof(Node), 1);
    result->type = NodeTypeExpression;
    result->expression.type = ExpressionTypeIdentifier;
    result->expression.identifier.token = token;
    result->expression.identifier.value = value;

    return result;
}

String NodeTokenLiteral(Node* n)
{
    switch (n->type)
    {
        case NodeTypeProgram:
            return ProgramTokenLiteral(&n->program);
        case NodeTypeStatement:
            return StatementTokenLiteral(&n->statement);
        case NodeTypeExpression:
            return ExpressionTokenLiteral(&n->expression);
    }

    return (String){0};
}

String StatementTokenLiteral(Statement* s)
{
    switch (s->type)
    {
        case StatementTypeLet:
            return StringDuplicate(STRING_AS_SPAN(s->letStatement.token.literal));
    }

    return (String){0};
}

String ExpressionTokenLiteral(Expression* e)
{
    switch (e->type)
    {
        case ExpressionTypeIdentifier:
            return StringDuplicate(STRING_AS_SPAN(e->identifier.token.literal));
    }

    return (String){0};
}

String ProgramTokenLiteral(Program* p)
{
    if (p->statementsLength > 0)
    {
        return StatementTokenLiteral(p->statements[0]);
    }
    return (String){0};
}

#else
#endif
