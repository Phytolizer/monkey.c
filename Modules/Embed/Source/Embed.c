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
    if (args.count < 3)
    {
        fprintf(stderr, "Usage: " SS_FMT " <file> <outputBaseName>\n", SS_ARG(programName));
        ArgumentsComplete(&args);
        return 1;
    }

    StringSpan fileName = NextArgument(&args);
    StringSpan outputBaseName = NextArgument(&args);

    FILE* input = fopen(fileName.data, "r");
    String headerOutput = StringFromSpan(outputBaseName);
    StringAppendC(&headerOutput, ".h");

    ArgumentsComplete(&args);
    return 0;
}
