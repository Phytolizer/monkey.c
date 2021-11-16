#include "monkey/lexer.h"
#include "repl.h"
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

void repl(FILE* in, FILE* out)
{
    char* line;
    while ((line = input_line(in)) != NULL)
    {
        Lexer lexer;
        Lexer_init(&lexer, line);
        for (Token tok = Lexer_next_token(&lexer); strcmp(tok.type, T_EOF) != 0; tok = Lexer_next_token(&lexer))
        {
            fprintf(out, "{Type:%s Literal:%s}\n", tok.type, tok.literal);
            Token_deinit(&tok);
        }
        Lexer_deinit(&lexer);
        free(line);
    }
}
