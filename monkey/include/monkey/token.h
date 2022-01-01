#ifndef MONKEY_TOKEN_H_
#define MONKEY_TOKEN_H_

#include <string/string.h>
#include <vec/vec.h>

typedef String MkTokenType;

typedef struct {
  MkTokenType type;
  String literal;
} MkToken;

extern MkTokenType mk_token_illegal;
extern MkTokenType mk_token_eof;

extern MkTokenType mk_token_ident;
extern MkTokenType mk_token_int;

extern MkTokenType mk_token_assign;
extern MkTokenType mk_token_plus;

extern MkTokenType mk_token_comma;
extern MkTokenType mk_token_semicolon;

extern MkTokenType mk_token_lparen;
extern MkTokenType mk_token_rparen;
extern MkTokenType mk_token_lbrace;
extern MkTokenType mk_token_rbrace;

extern MkTokenType mk_token_function;
extern MkTokenType mk_token_let;

typedef enum {
  kTokenTypesInit,
  kTokenTypesFree,
} MkTokenTypesAction;

void MkTokenTypesManage(MkTokenTypesAction action);

void MkTokenFree(MkToken tok);
MkTokenType MkLookupIdent(StringView ident);

#endif  // MONKEY_TOKEN_H_
