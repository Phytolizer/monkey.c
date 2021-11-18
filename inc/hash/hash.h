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
    const void* key;
    size_t key_len;
    const void* value;
    size_t value_len;
} ViewHashBucket;

typedef struct
{
    HashBucket* buckets;
    size_t count;
    size_t capacity;
} Hash;

typedef struct
{
    ViewHashBucket* buckets;
    size_t count;
    size_t capacity;
} ViewHash;

void Hash_init(Hash* h);
void Hash_deinit(Hash* h);
void Hash_insert(Hash* h, const void* key, size_t key_len, const void* value, size_t value_len);
void** Hash_lookup(Hash* h, const void* key, size_t key_len);

void ViewHash_init(ViewHash* h);
void ViewHash_deinit(ViewHash* h);
void ViewHash_insert(ViewHash* h, const void* key, size_t key_len, const void* value, size_t value_len);
const void** ViewHash_lookup(ViewHash* h, const void* key, size_t key_len);
