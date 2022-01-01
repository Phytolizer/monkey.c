#ifndef MONKEY_TOKEN_H_
#define MONKEY_TOKEN_H_

#include <string/string.h>
#include <vec/vec.h>

typedef String MkTokenType;

typedef struct {
  MkTokenType type;
  String literal;
} MkToken;

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
} MkTokenTypesAction;

void MkTokenTypesManage(MkTokenTypesAction action);

void MkTokenFree(MkToken tok);

#endif  // MONKEY_TOKEN_H_
