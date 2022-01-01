#ifndef MONKEY_TOKEN_H_
#define MONKEY_TOKEN_H_

#include <string/string.h>
#include <vec/vec.h>

typedef String TokenType;

typedef struct {
  TokenType type;
  String literal;
} Token;

extern String kTokenIllegal;
extern String kTokenEof;

extern String kTokenIdentifier;
extern String kTokenInt;

extern String kTokenAssign;
extern String kTokenPlus;

extern String kTokenComma;
extern String kTokenSemicolon;

extern String kTokenLParen;
extern String kTokenRParen;
extern String kTokenLBrace;
extern String kTokenRBrace;

extern String kTokenFunction;
extern String kTokenLet;

typedef enum {
  kTokenTypesInit,
  kTokenTypesFree,
} TokenTypesAction;

void TokenTypesManage(TokenTypesAction action);

#endif  // MONKEY_TOKEN_H_
