#ifndef MONKEY_TOKEN_H_
#define MONKEY_TOKEN_H_

#include <string/string.h>
#include <vec/vec.h>

typedef String MkTokenType;

typedef struct {
  MkTokenType type;
  String literal;
} MkToken;

extern String mk_token_illegal;
extern String mk_token_eof;

extern String mk_token_identifier;
extern String mk_token_int;

extern String mk_token_assign;
extern String mk_token_plus;

extern String mk_token_comma;
extern String mk_token_semicolon;

extern String mk_token_lparen;
extern String mk_token_rparen;
extern String mk_token_lbrace;
extern String mk_token_rbrace;

extern String mk_token_function;
extern String mk_token_let;

typedef enum {
  kTokenTypesInit,
  kTokenTypesFree,
} MkTokenTypesAction;

void MkTokenTypesManage(MkTokenTypesAction action);

void MkTokenFree(MkToken tok);

#endif  // MONKEY_TOKEN_H_
