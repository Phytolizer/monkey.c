#include "MonkeyRepl/WhoAmI.h"

#include <Monkey/Lexer.h>
#include <Monkey/Token.h>
#include <SimpleString/String.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const StringSpan Prompt = STRING_SPAN_INIT(">> ");

static String ReadLine(FILE* in)
{
    int capacity = 16;
    String result = {calloc(capacity, 1), 0};
    while (true)
    {
        int c = fgetc(in);
        if (c == EOF)
        {
            free(result.data);
            return (String){NULL, 0};
        }
        if (c == '\n')
        {
            break;
        }
        if (result.length + 1 > capacity)
        {
            capacity *= 2;
            char* newData = calloc(capacity, 1);
            if (newData == NULL)
            {
                free(result.data);
                return (String){NULL, 0};
            }
            memcpy(newData, result.data, result.length);
            result.data = newData;
        }
        result.data[result.length] = (char)c;
        result.length += 1;
    }

    return result;
}

void Start(FILE* in, FILE* out)
{
    while (true)
    {
        fprintf(out, "%.*s", Prompt.length, Prompt.data);
        fflush(out);
        String line = ReadLine(in);
        if (line.data == NULL)
        {
            break;
        }

        Lexer l = LexerInit(STRING_AS_SPAN(line));
        while (true)
        {
            Token tok = LexerNextToken(&l);
            if (tok.type.data == TokenTypeEof.data)
            {
                free(tok.literal.data);
                break;
            }

            fprintf(out,
                    "{Type:%.*s Literal:%.*s}\n",
                    tok.type.length,
                    tok.type.data,
                    tok.literal.length,
                    tok.literal.data);
            free(tok.literal.data);
        }
        fflush(out);

        free(line.data);
    }
}

int main(void)
{
    String user = WhoAmI();
    if (user.length == 0)
    {
        fprintf(stderr, "WhoAmI() failed\n");
        return 1;
    }
    printf("Hello %.*s! This is the Monkey programming language!\n", user.length, user.data);
    puts("Feel free to type in commands");
    Start(stdin, stdout);
    return 0;
}
