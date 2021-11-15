#include "hash/hash.h"
#include "hash/fnv.h"
#include <stdlib.h>
#include <string.h>

#define HASH_INITIAL_CAPACITY 8
#define HASH_MAX_LOAD 0.75

HashBucket* find_bucket(HashBucket* buckets, size_t capacity, void* key, size_t key_len);

void Hash_init(Hash* h)
{
    h->buckets = NULL;
    h->count = 0;
    h->capacity = 0;
}

void Hash_deinit(Hash* h)
{
    for (size_t i = 0; i < h->capacity; ++i)
    {
        if (h->buckets[i].key)
        {
            free(h->buckets[i].key);
            free(h->buckets[i].value);
        }
    }
    free(h->buckets);
}

void Hash_insert(Hash* h, void* key, size_t key_len, void* value, size_t value_len)
{
    if (h->capacity == 0 || (double)h->count / h->capacity >= HASH_MAX_LOAD)
    {
        size_t old_capacity = h->capacity;
        h->capacity = (h->capacity == 0 ? HASH_INITIAL_CAPACITY : h->capacity * 2);
        HashBucket* new_buckets = malloc(h->capacity * sizeof(HashBucket));
        for (size_t i = 0; i < h->capacity; ++i)
        {
            new_buckets[i].key = NULL;
        }
        for (size_t i = 0; i < old_capacity; ++i)
        {
            if (h->buckets[i].key != NULL)
            {
                HashBucket* new_bucket =
                    find_bucket(new_buckets, h->capacity, h->buckets[i].key, h->buckets[i].key_len);
                new_bucket->key = h->buckets[i].key;
                new_bucket->value = h->buckets[i].value;
            }
        }
        free(h->buckets);
        h->buckets = new_buckets;
    }

    HashBucket* bucket = find_bucket(h->buckets, h->capacity, key, key_len);
    bucket->key = malloc(key_len);
    bucket->key_len = key_len;
    memcpy(bucket->key, key_len, key, key_len);
    bucket->value = malloc(value_len);
    bucket->value_len = value_len;
    memcpy(bucket->value, value_len, value, value_len);
    ++h->count;
}

void** Hash_lookup(Hash* h, void* key, size_t key_len)
{
    if (h->count == 0)
    {
        return NULL;
    }
    HashBucket* bucket = find_bucket(h->buckets, h->capacity, key, key_len);
    return &bucket->value;
}

HashBucket* find_bucket(HashBucket* buckets, size_t capacity, void* key, size_t key_len)
{
    if (capacity == 0)
    {
        return NULL;
    }
    uint64_t hash = hash_fnv(key, key_len);
    hash %= capacity;
    while (buckets[hash].key != NULL && memcmp(buckets[hash].key, key, key_len) != 0)
    {
        hash = (hash + 1) % capacity;
    }
    return &buckets[hash];
}
