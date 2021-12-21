#include "monkey/evaluator.h"
#include "monkey/ast.h"

Object* eval_statements(StatementVec statements);
Object* native_bool_to_boolean_object(bool value, bool cleanup);
Object* nil_object(bool cleanup);
Object* eval_prefix_expression(sds op, Object* right);
Object* eval_bang_operator_expression(Object* right);

static BooleanObj* TRUE = NULL;
static BooleanObj* FALSE = NULL;
static NilObj* NIL = NULL;

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
        case EXPRESSION_TYPE_PREFIX: {
            Object* right = eval(&((PrefixExpression*)expr)->right->base);
            return eval_prefix_expression(((PrefixExpression*)expr)->operator, right);
        }
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

Object* eval_prefix_expression(sds op, Object* right)
{
    if (strcmp(op, "!") == 0)
    {
        return eval_bang_operator_expression(right);
    }
    return NULL;
}

Object* eval_bang_operator_expression(Object* right)
{
    if (right == (Object*)TRUE)
    {
        return (Object*)FALSE;
    }
    if (right == (Object*)FALSE)
    {
        return (Object*)TRUE;
    }
    if (right == (Object*)NIL)
    {
        return (Object*)TRUE;
    }
    return (Object*)FALSE;
}
