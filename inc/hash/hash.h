#pragma once

#include <stddef.h>

typedef struct
{
    void* key;
    size_t key_len;
    void* value;
    size_t value_len;
} HashBucket;

typedef struct
{
    HashBucket* buckets;
    size_t count;
    size_t capacity;
} Hash;

void Hash_init(Hash* h);
void Hash_deinit(Hash* h);
void Hash_insert(Hash* h, const void* key, size_t key_len, const void* value, size_t value_len);
void** Hash_lookup(Hash* h, const void* key, size_t key_len);
