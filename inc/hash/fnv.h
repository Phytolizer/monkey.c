#pragma once

#include <stddef.h>
#include <stdint.h>

uint64_t hash_fnv(const void* key, size_t key_len);
