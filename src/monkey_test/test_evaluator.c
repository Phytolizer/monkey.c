#include "test_evaluator.h"
#include "monkey/ast.h"
#include "monkey/evaluator.h"
#include "monkey/object.h"
#include "monkey/parser.h"
#include "test.h"
#include <stdint.h>

char* check_integer_object(Object* obj, int64_t expected)
{
    test_assert(obj != NULL, (void)0, "Object should not be NULL");
    test_assert(obj->type == OBJECT_TYPE_INTEGER, (void)0, "object is not INTEGER. got=%s",
                Object_type_name(obj->type));
    IntegerObj* result = (IntegerObj*)obj;
    test_assert(result->value == expected, (void)0, "integer has wrong value. got=%ld, want=%ld", result->value,
                expected);
    return NULL;
}

Object* test_eval(const char* input)
{
    Parser p;
    Parser_init(&p, input);
    Program program = Parser_parse_program(&p);
    Object* result = eval(&program.base);
    Program_deinit(&program);
    Parser_deinit(&p);
    return result;
}

char* test_eval_integer_expression(void)
{
    struct
    {
        const char* input;
        int64_t expected;
    } tests[] = {
        {"5", 5},
        {"10", 10},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i)
    {
        Object* evaluated = test_eval(tests[i].input);
        char* message = check_integer_object(evaluated, tests[i].expected);
        if (message != NULL)
        {
            return message;
        }
    }

    return NULL;
}

char* evaluator_tests(size_t* test_count)
{
    test_run(test_eval_integer_expression);
    return NULL;
}
