#include "monkey/evaluator.h"
#include "monkey/ast.h"

Object* eval_statements(StatementVec statements);
Object* native_bool_to_boolean_object(bool value, bool cleanup);
Object* nil_object(bool cleanup);

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
        case EXPRESSION_TYPE_BOOLEAN:
            return native_bool_to_boolean_object(((Boolean*)expr)->value, false);
        }
    }
    }

    return NULL;
}

void evaluator_exit(void)
{
    native_bool_to_boolean_object(false, true);
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

Object* native_bool_to_boolean_object(bool value, bool cleanup)
{
    static BooleanObj* TRUE = NULL;
    static BooleanObj* FALSE = NULL;

    if (cleanup)
    {
        free(TRUE);
        TRUE = NULL;
        free(FALSE);
        FALSE = NULL;
        return NULL;
    }

    if (!TRUE)
    {
        TRUE = malloc(sizeof(BooleanObj));
        BooleanObj_init(TRUE);
        TRUE->value = true;
        FALSE = malloc(sizeof(BooleanObj));
        BooleanObj_init(FALSE);
        FALSE->value = false;
    }

    return (Object*)(value ? TRUE : FALSE);
}

Object* nil_object(bool cleanup)
{
    static NilObj* NIL = NULL;
    if (!NIL)
    {
        NIL = malloc(sizeof(NilObj));
        NilObj_init(NIL);
    }

    if (cleanup)
    {
        free(NIL);
        NIL = NULL;
    }

    return (Object*)NIL;
}
