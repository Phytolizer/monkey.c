#include "hash/fnv.h"

#define FNV_PRIME 0x00000100000001B3
#define FNV_OFFSET_BASIS 0xcbf29ce484222325

uint64_t hash_fnv(void* key, size_t key_len)
{
    uint64_t h = FNV_OFFSET_BASIS;
    char* k = key;
    for (size_t i = 0; i < key_len; ++i)
    {
        h ^= k[i];
        h *= FNV_PRIME;
    }
    return h;
}
