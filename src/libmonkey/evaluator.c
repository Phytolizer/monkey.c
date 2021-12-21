#include "monkey/evaluator.h"
#include "monkey/ast.h"

Object* eval_statements(StatementVec statements);

Object* eval(Node* node)
{
    switch (node->type)
    {
    case NODE_TYPE_PROGRAM:
        return eval_statements(((Program*)node)->statements);
    case NODE_TYPE_STATEMENT: {
        Statement* stmt = (Statement*)node;
        switch (stmt->type)
        {
        case STATEMENT_TYPE_EXPRESSION:
            return eval(&((ExpressionStatement*)stmt)->expression->base);
        }
    }
    case NODE_TYPE_EXPRESSION: {
        Expression* expr = (Expression*)node;
        switch (expr->type)
        {
        case EXPRESSION_TYPE_INTEGER: {
            IntegerObj* result = malloc(sizeof(IntegerObj));
            IntegerObj_init(result);
            result->value = ((IntegerLiteral*)expr)->value;
            return (Object*)result;
        }
        }
    }
    }

    return NULL;
}

Object* eval_statements(StatementVec statements)
{
    Object* result = NULL;
    for (int i = 0; i < statements.length; ++i)
    {
        result = eval(&statements.data[i]->base);
    }
    return result;
}
