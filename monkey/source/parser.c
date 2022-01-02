#include "monkey/parser.h"

#include "monkey/token.h"

static void ParserNextToken(MkParser* parser);

void MkParserInit(MkParser* parser, MkLexer lexer) {
  parser->lexer = lexer;
  ParserNextToken(parser);
  ParserNextToken(parser);
}

MkAstProgram* MkParserParseProgram(MkParser* parser) {
  return NULL;
}

void MkParserFree(MkParser parser) {
  MkTokenFree(parser.current_token);
  MkTokenFree(parser.peek_token);
}

void ParserNextToken(MkParser* parser) {
  MkTokenFree(parser->current_token);
  parser->current_token = parser->peek_token;
  parser->peek_token = MkLexerNextToken(&parser->lexer);
}
