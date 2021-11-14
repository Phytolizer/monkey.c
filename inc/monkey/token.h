#pragma once

#include "sds.h"

typedef const char *TokenType;

typedef struct {
  TokenType type;
  sds literal;
} Token;
