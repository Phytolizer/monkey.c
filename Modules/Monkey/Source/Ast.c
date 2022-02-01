#include "Monkey/Ast.h"

#include <stdlib.h>

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

void ProgramDeinit(Program* p)
{
    for (int i = 0; i < p->statementsLength; i++)
    {
        StatementDeinit(p->statements[i]);
        free(p->statements[i]);
    }
    free(p->statements);
}

void StatementDeinit(Statement* s)
{
    switch (s->type)
    {
        case StatementTypeLet:
            free(s->as.letStatement.token.literal.data);
            free(s->as.letStatement.name->token.literal.data);
            free(s->as.letStatement.name->value.data);
            free(s->as.letStatement.name);
            if (s->as.letStatement.value != NULL)
            {
                ExpressionDeinit(s->as.letStatement.value);
                free(s->as.letStatement.value);
            }
            break;
        case StatementTypeReturn:
            free(s->as.returnStatement.token.literal.data);
            if (s->as.returnStatement.returnValue != NULL)
            {
                ExpressionDeinit(s->as.returnStatement.returnValue);
                free(s->as.returnStatement.returnValue);
            }
            break;
    }
}

void ExpressionDeinit(Expression* e)
{
    switch (e->type)
    {
        case ExpressionTypeIdentifier:
            free(e->as.identifier.token.literal.data);
            free(e->as.identifier.value.data);
            break;
    }
}

Node* LetStatementInit(Token token, Identifier* name, Expression* value)
{
    Node* result = calloc(sizeof(Node), 1);
    result->type = NodeTypeStatement;
    result->as.statement.type = StatementTypeLet;
    result->as.statement.as.letStatement.token = token;
    result->as.statement.as.letStatement.name = name;
    result->as.statement.as.letStatement.value = value;

    return result;
}

Node* IdentifierInit(Token token, String value)
{
    Node* result = calloc(sizeof(Node), 1);
    result->type = NodeTypeExpression;
    result->as.expression.type = ExpressionTypeIdentifier;
    result->as.expression.as.identifier.token = token;
    result->as.expression.as.identifier.value = value;

    return result;
}

String NodeTokenLiteral(Node* n)
{
    switch (n->type)
    {
        case NodeTypeProgram:
            return ProgramTokenLiteral(&n->as.program);
        case NodeTypeStatement:
            return StatementTokenLiteral(&n->as.statement);
        case NodeTypeExpression:
            return ExpressionTokenLiteral(&n->as.expression);
    }

    return (String){0};
}

String StatementTokenLiteral(Statement* s)
{
    switch (s->type)
    {
        case StatementTypeLet:
            return StringDuplicate(STRING_AS_SPAN(s->as.letStatement.token.literal));
        case StatementTypeReturn:
            return StringDuplicate(STRING_AS_SPAN(s->as.returnStatement.token.literal));
    }

    return (String){0};
}

String ExpressionTokenLiteral(Expression* e)
{
    switch (e->type)
    {
        case ExpressionTypeIdentifier:
            return StringDuplicate(STRING_AS_SPAN(e->as.identifier.token.literal));
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
