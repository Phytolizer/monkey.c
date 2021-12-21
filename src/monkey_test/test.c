#include "test.h"
#include "test_ast.h"
#include "test_evaluator.h"
#include "test_lexer.h"
#include "test_parser.h"

char* all_tests(size_t* test_count)
{
    test_suite(lexer_tests);
    test_suite(parser_tests);
    test_suite(ast_tests);
    test_suite(evaluator_tests);
    return NULL;
}

int main(void)
{
    size_t test_count = 0;
    char* message = all_tests(&test_count);
    if (message != NULL)
    {
        fprintf(stderr, "%s\n", message);
        free(message);
    }
    else
    {
        printf("== ALL PASSED ==\n");
    }
    printf("Tests run: %zu\n", test_count);

    return message != NULL;
}
