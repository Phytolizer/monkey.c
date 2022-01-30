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

static String StringFromSpan(StringSpan span)
{
    String s = {
        .data = calloc(1, span.length + 1),
        .capacity = span.length,
        .length = span.length,
    };
    memcpy(s.data, span.data, span.length);
    return s;
}

static void StringAlloc(String* s)
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

static void StringAppendC(String* s, const char* c)
{
    size_t n = strlen(c);
    while (s->length + n + 1 > s->capacity)
    {
        StringAlloc(s);
    }

    memcpy(s->data + s->length, c, n);
    s->length += n;
}

static void StringFree(String* s)
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

static Arguments ArgumentsInit(size_t argc, char** argv)
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

static void ArgumentsComplete(Arguments* args)
{
    free(args->dataAlloc);
    args->data = NULL;
    args->dataAlloc = NULL;
    args->count = 0;
}

static StringSpan NextArgument(Arguments* args)
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

    String headerOutputName = StringFromSpan(outputBaseName);
    StringAppendC(&headerOutputName, ".h");
    FILE* headerOutput = fopen(headerOutputName.data, "w");

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
    StringFree(&sourceFile);
    fprintf(sourceOutput, "#include \"%s\"\nconst char %s[] = \"", headerOutputName.data, loser);
    StringFree(&headerOutputName);
    char buffer[1024] = {0};
    while (fgets(buffer, sizeof buffer, input))
    {
        for (size_t i = 0; i < sizeof buffer && buffer[i] != '\0'; ++i)
        {
            switch (buffer[i])
            {
                case '\n':
                    fwrite("\\n", 1, 2, sourceOutput);
                    break;
                case '\r':
                    fwrite("\\r", 1, 2, sourceOutput);
                    break;
                case '\t':
                    fwrite("\\t", 1, 2, sourceOutput);
                    break;
                case '"':
                    fwrite("\\\"", 1, 2, sourceOutput);
                    break;
                default:
                    fputc(buffer[i], sourceOutput);
                    break;
            }
        }
    }
    fprintf(sourceOutput, "\";\n");
    fclose(sourceOutput);

    fclose(input);
    ArgumentsComplete(&args);
    return 0;
}
