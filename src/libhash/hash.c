#include "hash/fnv.h"
#include "hash/hash.h"
#include <stdlib.h>
#include <string.h>

#define HASH_INITIAL_CAPACITY 8
#define HASH_MAX_LOAD 0.75

HashBucket* find_bucket(HashBucket* buckets, size_t capacity, const void* key, size_t key_len);
ViewHashBucket* find_view_bucket(ViewHashBucket* buckets, size_t capacity, const void* key, size_t key_len);

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

void Hash_insert(Hash* h, const void* key, size_t key_len, const void* value, size_t value_len)
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
                new_bucket->key_len = h->buckets[i].key_len;
                new_bucket->value = h->buckets[i].value;
                new_bucket->value_len = h->buckets[i].value_len;
            }
        }
        free(h->buckets);
        h->buckets = new_buckets;
    }

    HashBucket* bucket = find_bucket(h->buckets, h->capacity, key, key_len);
    bucket->key = malloc(key_len);
    bucket->key_len = key_len;
    memcpy(bucket->key, key, key_len);
    bucket->value = malloc(value_len);
    bucket->value_len = value_len;
    memcpy(bucket->value, value, value_len);
    ++h->count;
}

void** Hash_lookup(Hash* h, const void* key, size_t key_len)
{
    if (h->count == 0)
    {
        return NULL;
    }
    HashBucket* bucket = find_bucket(h->buckets, h->capacity, key, key_len);
    if (bucket->key != NULL)
    {
        return &bucket->value;
    }
    return NULL;
}

HashBucket* find_bucket(HashBucket* buckets, size_t capacity, const void* key, size_t key_len)
{
    if (capacity == 0)
    {
        return NULL;
    }
    uint64_t hash = hash_fnv(key, key_len);
    hash %= capacity;
    while (buckets[hash].key != NULL &&
           (buckets[hash].key_len != key_len || memcmp(buckets[hash].key, key, key_len) != 0))
    {
        hash = (hash + 1) % capacity;
    }
    return &buckets[hash];
}

ViewHashBucket* find_view_bucket(ViewHashBucket* buckets, size_t capacity, const void* key, size_t key_len)
{
    if (capacity == 0)
    {
        return NULL;
    }
    uint64_t hash = hash_fnv(key, key_len);
    hash %= capacity;
    while (buckets[hash].key != NULL &&
           (buckets[hash].key_len != key_len || memcmp(buckets[hash].key, key, key_len) != 0))
    {
        hash = (hash + 1) % capacity;
    }
    return &buckets[hash];
}

void ViewHash_init(ViewHash* h)
{
    h->buckets = NULL;
    h->count = 0;
    h->capacity = 0;
}

void ViewHash_deinit(ViewHash* h)
{
    free(h->buckets);
}

void ViewHash_insert(ViewHash* h, const void* key, size_t key_len, const void* value, size_t value_len)
{
    if (h->capacity == 0 || (double)h->count / h->capacity >= HASH_MAX_LOAD)
    {
        size_t old_capacity = h->capacity;
        h->capacity = (h->capacity == 0 ? HASH_INITIAL_CAPACITY : h->capacity * 2);
        ViewHashBucket* new_buckets = malloc(h->capacity * sizeof(HashBucket));
        for (size_t i = 0; i < h->capacity; ++i)
        {
            new_buckets[i].key = NULL;
        }
        for (size_t i = 0; i < old_capacity; ++i)
        {
            if (h->buckets[i].key != NULL)
            {
                ViewHashBucket* new_bucket =
                    find_view_bucket(new_buckets, h->capacity, h->buckets[i].key, h->buckets[i].key_len);
                new_bucket->key = h->buckets[i].key;
                new_bucket->key_len = h->buckets[i].key_len;
                new_bucket->value = h->buckets[i].value;
                new_bucket->value_len = h->buckets[i].value_len;
            }
        }
        free(h->buckets);
        h->buckets = new_buckets;
    }

    ViewHashBucket* bucket = find_view_bucket(h->buckets, h->capacity, key, key_len);
    bucket->key = key;
    bucket->key_len = key_len;
    bucket->value = value;
    bucket->value_len = value_len;
    ++h->count;
}

const void** ViewHash_lookup(ViewHash* h, const void* key, size_t key_len)
{
    if (h->count == 0)
    {
        return NULL;
    }
    ViewHashBucket* bucket = find_view_bucket(h->buckets, h->capacity, key, key_len);
    if (bucket->key != NULL)
    {
        return &bucket->value;
    }
    return NULL;
}
