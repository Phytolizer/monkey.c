#include "repl.h"
#include "monkey/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* input_line(FILE* in)
{
    char* line = malloc(1);
    size_t len = 1;
    int c;
    while ((c = fgetc(in)) != '\n' && c != EOF)
    {
        line = realloc(line, len + 1);
        line[len - 1] = c;
        len++;
    }
    if (len == 1 && c == EOF)
    {
        free(line);
        return NULL;
    }
    line[len - 1] = '\0';
    return line;
}

void print_parser_errors(FILE* out, SdsVec errors)
{
    for (int i = 0; i < errors.length; ++i)
    {
        fprintf(out, "    %s\n", errors.data[i]);
    }
}

void repl(FILE* in, FILE* out)
{
    char* line;
    while ((line = input_line(in)) != NULL)
    {
        Parser parser;
        Parser_init(&parser, line);
        Program program = Parser_parse_program(&parser);
        if (parser.errors.length > 0)
        {
            print_parser_errors(out, parser.errors);
            continue;
        }
        sds programStr = Program_string(&program);
        fprintf(out, "%s\n", programStr);
        sdsfree(programStr);
        Parser_deinit(&parser);
        free(line);
    }
}
