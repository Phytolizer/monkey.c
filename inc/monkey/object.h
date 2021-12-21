#pragma once

#include "sds.h"
#include <stdbool.h>
#include <stdint.h>
#define OBJECT_TYPES_                                                                                                  \
    X(INTEGER)                                                                                                         \
    X(BOOLEAN)                                                                                                         \
    X(NIL)

typedef enum
{
#define X(x) OBJECT_TYPE_##x,
    OBJECT_TYPES_
#undef X
} ObjectType;

typedef struct
{
    ObjectType type;
} Object;

sds Object_inspect(Object* object);
const char* Object_type_name(ObjectType type);

typedef struct
{
    Object base;
    int64_t value;
} IntegerObj;

void IntegerObj_init(IntegerObj* object);
sds IntegerObj_inspect(IntegerObj* i);

typedef struct
{
    Object base;
    bool value;
} BooleanObj;

void BooleanObj_init(BooleanObj* object);
sds BooleanObj_inspect(BooleanObj* b);

typedef struct
{
    Object base;
} NilObj;

void NilObj_init(NilObj* object);
sds NilObj_inspect(NilObj* n);
