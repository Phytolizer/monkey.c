#include "monkey/object.h"
#include <assert.h>
#include <stdbool.h>

sds Object_inspect(Object* object)
{
    switch (object->type)
    {
    case OBJECT_TYPE_INTEGER:
        return IntegerObj_inspect((IntegerObj*)object);
    case OBJECT_TYPE_BOOLEAN:
        return BooleanObj_inspect((BooleanObj*)object);
    case OBJECT_TYPE_NIL:
        return NilObj_inspect((NilObj*)object);
    }

    assert(false && "Unexpected object type");
}

const char* Object_type_name(ObjectType type)
{
    switch (type)
    {
#define X(x)                                                                                                           \
    case OBJECT_TYPE_##x:                                                                                              \
        return #x;
        OBJECT_TYPES_
#undef X
    }

    assert(false && "Unexpected object type");
}

void IntegerObj_init(IntegerObj* object)
{
    object->base.type = OBJECT_TYPE_INTEGER;
}

sds IntegerObj_inspect(IntegerObj* i)
{
    return sdscatprintf(sdsempty(), "%ld", i->value);
}

void BooleanObj_init(BooleanObj* object)
{
    object->base.type = OBJECT_TYPE_BOOLEAN;
}

sds BooleanObj_inspect(BooleanObj* b)
{
    return sdsnew(b->value ? "true" : "false");
}

void NilObj_init(NilObj* object)
{
    object->base.type = OBJECT_TYPE_NIL;
}

sds NilObj_inspect(NilObj* n)
{
    (void)n;
    return sdsnew("nil");
}
