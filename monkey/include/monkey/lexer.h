#ifndef MONKEY_LEXER_H_
#define MONKEY_LEXER_H_

#include <stdint.h>
#include <string/string.h>
#include "monkey/token.h"

typedef struct {
  StringView source;
  uint64_t position;
  uint64_t read_position;
  uint8_t ch;
} MkLexer;

void MkLexerInit(MkLexer* lexer, StringView source);
MkToken MkLexerNextToken(MkLexer* lexer);

#endif  // MONKEY_LEXER_H_
