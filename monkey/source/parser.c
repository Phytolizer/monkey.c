#include "monkey/parser.h"

#include <stdlib.h>

#include "monkey/ast.h"
#include "monkey/token.h"
#include "string/string.h"

static void ParserNextToken(MkParser* parser);
static MkAstStatement* ParseStatement(MkParser* parser);
static MkAstStatement* ParseLetStatement(MkParser* parser);
static MkAstExpression* ParseExpression(MkParser* parser);

void MkParserInit(MkParser* parser, MkLexer lexer) {
  parser->lexer = lexer;
  ParserNextToken(parser);
  ParserNextToken(parser);
}

MkAstProgram* MkParserParseProgram(MkParser* parser) {
  MkAstProgram* program = calloc(sizeof(MkAstProgram), 1);
  while (!StringEqual(parser->current_token.type, mk_token_eof)) {
    MkAstStatement* stmt = ParseStatement(parser);
    if (stmt != NULL) {
      VEC_PUSH(&program->statements, stmt);
    }
    ParserNextToken(parser);
  }
  return program;
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

MkAstStatement* ParseStatement(MkParser* parser) {
  if (StringEqual(parser->current_token.type, mk_token_let)) {
    return ParseLetStatement(parser);
  }
  return NULL;
}

MkAstStatement* ParseLetStatement(MkParser* parser) {
  MkAstLetStatement* let_statement = calloc(sizeof(MkAstLetStatement), 1);
  let_statement->base.base.type = kMkAstNodeStatement;
  let_statement->base.type = kMkAstStatementLet;
  let_statement->token = MkTokenDuplicate(parser->current_token);
  ParserNextToken(parser);
  if (!StringEqual(parser->current_token.type, mk_token_ident)) {
    free(let_statement);
    return NULL;
  }
  let_statement->name = (MkAstIdentifier){
      .base = {.base = {.type = kMkAstNodeExpression},
               .type = kMkAstExpressionIdentifier},
      .token = MkTokenDuplicate(parser->current_token),
      .value = StringDuplicate(parser->current_token.literal),
  };
  ParserNextToken(parser);
  if (!StringEqual(parser->current_token.type, mk_token_assign)) {
    MkAstNodeFree(&let_statement->name.base.base);
    free(let_statement);
    return NULL;
  }
  ParserNextToken(parser);
  // TODO
  while (!StringEqual(parser->current_token.type, mk_token_semicolon)) {
    ParserNextToken(parser);
  }
  return (MkAstStatement*)let_statement;
}
