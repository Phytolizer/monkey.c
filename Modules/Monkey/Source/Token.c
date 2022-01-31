#include "Monkey/Token.h"

#define X(x, y) const TokenType TokenType##x = {.data = y, .length = sizeof y - 1};
TOKEN_TYPES_X
#undef X
