#ifndef MONKEY_PARSER_H_
#define MONKEY_PARSER_H_

#include "monkey/ast.h"
#include "monkey/lexer.h"
typedef struct {
  MkLexer lexer;

  MkToken current_token;
  MkToken peek_token;
} MkParser;

void MkParserInit(MkParser* parser, MkLexer lexer);
MkAstProgram* MkParserParseProgram(MkParser* parser);
void MkParserFree(MkParser parser);

#endif  // MONKEY_PARSER_H_
