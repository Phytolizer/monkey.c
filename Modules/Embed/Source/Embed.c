#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char* data;
    size_t capacity;
    size_t length;
} String;

typedef struct
{
    const char* data;
    size_t length;
} StringSpan;

#define SS_FMT "%.*s"
#define SS_ARG(span) (int)span.length, span.data

String StringFromSpan(StringSpan span)
{
    String s = {
        .data = calloc(1, span.length + 1),
        .capacity = span.length,
        .length = span.length,
    };
    memcpy(s.data, span.data, span.length);
    return s;
}

String StringFromC(const char* c)
{
    size_t n = strlen(c);
    String s = {
        .data = calloc(1, n + 1),
        .capacity = n,
        .length = n,
    };
    memcpy(s.data, c, n);
    return s;
}

String StringInitN(size_t n)
{
    String s = {
        .data = calloc(1, n + 1),
        .capacity = n + 1,
        .length = 0,
    };
    return s;
}

void StringAlloc(String* s)
{
    size_t oldCapacity = s->capacity;
    if (s->capacity == 0)
    {
        s->capacity = 8;
    }
    s->capacity *= 2;
    s->data = realloc(s->data, s->capacity);
    memset(s->data + oldCapacity, 0, s->capacity - oldCapacity);
}

void StringAppendC(String* s, const char* c)
{
    size_t n = strlen(c);
    while (s->length + n + 1 > s->capacity)
    {
        StringAlloc(s);
    }

    memcpy(s->data + s->length, c, n);
    s->length += n;
}

void StringFree(String* s)
{
    free(s->data);
    s->data = NULL;
    s->capacity = 0;
    s->length = 0;
}

typedef struct
{
    size_t count;
    StringSpan* dataAlloc;
    StringSpan* data;
} Arguments;

Arguments ArgumentsInit(size_t argc, char** argv)
{
    StringSpan* data = calloc(sizeof(StringSpan), argc);
    Arguments args = {
        .count = argc,
        .data = data,
        .dataAlloc = data,
    };
    for (size_t i = 0; i < argc; ++i)
    {
        args.data[i] = (StringSpan){
            .data = argv[i],
            .length = strlen(argv[i]),
        };
    }
    return args;
}

void ArgumentsComplete(Arguments* args)
{
    free(args->dataAlloc);
    args->data = NULL;
    args->dataAlloc = NULL;
    args->count = 0;
}

bool ArgumentsEmpty(Arguments args)
{
    return args.count == 0;
}

StringSpan NextArgument(Arguments* args)
{
    StringSpan arg = args->data[0];
    args->data++;
    args->count--;
    return arg;
}

int main(int argc, char** argv)
{
    Arguments args = ArgumentsInit(argc, argv);

    StringSpan programName = NextArgument(&args);
    if (args.count < 2)
    {
        fprintf(stderr, "Usage: " SS_FMT " <file> <outputBaseName>\n", SS_ARG(programName));
        ArgumentsComplete(&args);
        return 1;
    }

    StringSpan fileName = NextArgument(&args);
    StringSpan outputBaseName = NextArgument(&args);

    String fileNameStr = StringFromSpan(fileName);
    FILE* input = fopen(fileName.data, "r");
    if (input == NULL)
    {
        fprintf(stderr, "Failed to open %s", fileNameStr.data);
        StringFree(&fileNameStr);
        return 1;
    }
    StringFree(&fileNameStr);

    fseek(input, 0, SEEK_END);
    size_t inputLen = ftell(input);
    fseek(input, 0, SEEK_SET);
    String inputText = StringInitN(inputLen);
    fread(inputText.data, 1, inputLen, input);
    inputText.length = inputLen;
    fclose(input);

    String headerOutputName = StringFromSpan(outputBaseName);
    StringAppendC(&headerOutputName, ".h");
    FILE* headerOutput = fopen(headerOutputName.data, "w");

    char buffer[1024] = {0};
    size_t cursor = 0;
    const char* loser = strrchr(outputBaseName.data, '\\');
    if (loser == NULL)
    {
        loser = strrchr(outputBaseName.data, '/');
    }
    if (loser == NULL)
    {
        loser = outputBaseName.data;
    }
    else
    {
        loser++;
    }
    fprintf(headerOutput, "#pragma once\nextern const char %s[];\n", loser);
    fclose(headerOutput);

    String sourceFile = StringFromSpan(outputBaseName);
    StringAppendC(&sourceFile, ".c");
    FILE* sourceOutput = fopen(sourceFile.data, "w");
    fprintf(sourceOutput, "#include \"%s\"\nconst char %s[] = \"", headerOutputName.data, loser);
    StringFree(&headerOutputName);
    for (size_t i = 0; i < inputText.length; ++i)
    {
        if (cursor > sizeof(buffer) - 2)
        {
            fwrite(buffer, 1, cursor, sourceOutput);
            cursor = 0;
        }
        switch (inputText.data[i])
        {
        case '\n':
            buffer[cursor++] = '\\';
            buffer[cursor++] = 'n';
            break;
        case '\r':
            buffer[cursor++] = '\\';
            buffer[cursor++] = 'r';
            break;
        case '\t':
            buffer[cursor++] = '\\';
            buffer[cursor++] = 't';
            break;
        case '"':
            buffer[cursor++] = '\\';
            buffer[cursor++] = '"';
            break;
        default:
            buffer[cursor++] = inputText.data[i];
            break;
        }
    }
    fwrite(buffer, 1, cursor, sourceOutput);
    fprintf(sourceOutput, "\";\n");
    fclose(sourceOutput);

    StringFree(&inputText);
    ArgumentsComplete(&args);
    return 0;
}
