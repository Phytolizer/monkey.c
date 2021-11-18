#include "monkey/ast.h"
#include "test.h"
#include "test_ast.h"

char* test_string(void)
{
    Program program;
    Program_init(&program);
    LetStatement* stmt = malloc(sizeof(LetStatement));
    LetStatement_init(stmt);
    stmt->token.type = T_LET;
    stmt->token.literal = sdsnew("let");
    Identifier_init(&stmt->name);
    stmt->name.token.type = T_IDENT;
    stmt->name.token.literal = sdsnew("myVar");
    stmt->name.value = sdsnew("myVar");
    Identifier* value = malloc(sizeof(Identifier));
    Identifier_init(value);
    value->token.type = T_IDENT;
    value->token.literal = sdsnew("anotherVar");
    value->value = sdsnew("anotherVar");
    stmt->value = (Expression*)value;
    vec_push(&program.statements, (Statement*)stmt);

    sds string = Program_string(&program);
    test_assert(
        strcmp(string, "let myVar = anotherVar;") == 0,
        do {
            sdsfree(string);
            Program_deinit(&program);
        } while (false),
        "Program_string(&program) wrong. got='%s'", string);
    sdsfree(string);
    Program_deinit(&program);

    return NULL;
}

char* ast_tests(size_t* test_count)
{
    test_run(test_string);
    return NULL;
}
