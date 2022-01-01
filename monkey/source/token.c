#include "monkey/token.h"

#include <hash/hash.h>
#include <stdlib.h>
#include <string.h>
#include <string/string.h>

MkTokenType mk_token_illegal;
MkTokenType mk_token_eof;

MkTokenType mk_token_ident;
MkTokenType mk_token_int;

MkTokenType mk_token_assign;
MkTokenType mk_token_plus;

MkTokenType mk_token_comma;
MkTokenType mk_token_semicolon;

MkTokenType mk_token_lparen;
MkTokenType mk_token_rparen;
MkTokenType mk_token_lbrace;
MkTokenType mk_token_rbrace;

MkTokenType mk_token_function;
MkTokenType mk_token_let;

static HASH_TYPE(MkTokenType) token_types = {0};

static HashKeySpan CreateKey(const char* k);
static HashKeySpan ConvertKey(StringView s);

void MkTokenTypesManage(MkTokenTypesAction action) {
  switch (action) {
    case kTokenTypesInit:
      mk_token_illegal = StringFromC("ILLEGAL");
      mk_token_eof = StringFromC("EOF");

      mk_token_ident = StringFromC("IDENT");
      mk_token_int = StringFromC("INT");

      mk_token_assign = StringFromC("=");
      mk_token_plus = StringFromC("+");

      mk_token_comma = StringFromC(",");
      mk_token_semicolon = StringFromC(";");

      mk_token_lparen = StringFromC("(");
      mk_token_rparen = StringFromC(")");
      mk_token_lbrace = StringFromC("{");
      mk_token_rbrace = StringFromC("}");

      mk_token_function = StringFromC("FUNCTION");
      mk_token_let = StringFromC("LET");

      HASH_ADD(&token_types, CreateKey("fn"), mk_token_function);
      HASH_ADD(&token_types, CreateKey("let"), mk_token_let);
      break;
    case kTokenTypesFree:
      VEC_FREE(&mk_token_illegal);
      VEC_FREE(&mk_token_eof);
      VEC_FREE(&mk_token_ident);
      VEC_FREE(&mk_token_int);
      VEC_FREE(&mk_token_assign);
      VEC_FREE(&mk_token_plus);
      VEC_FREE(&mk_token_comma);
      VEC_FREE(&mk_token_semicolon);
      VEC_FREE(&mk_token_lparen);
      VEC_FREE(&mk_token_rparen);
      VEC_FREE(&mk_token_lbrace);
      VEC_FREE(&mk_token_rbrace);
      VEC_FREE(&mk_token_function);
      VEC_FREE(&mk_token_let);
      HASH_FREE(&token_types);
      break;
  }
}

void MkTokenFree(MkToken tok) {
  VEC_FREE(&tok.literal);
}

MkTokenType MkLookupIdent(StringView ident) {
  MkTokenType type;
  if (!HASH_GET(&token_types, ConvertKey(ident), &type)) {
    type = mk_token_ident;
  }
  return type;
}

HashKeySpan CreateKey(const char* k) {
  return (HashKeySpan){
      .begin = (const uint8_t*)k,
      .end = (const uint8_t*)k + strlen(k),
  };
}

HashKeySpan ConvertKey(StringView s) {
  return (HashKeySpan){
      .begin = (const uint8_t*)s.begin,
      .end = (const uint8_t*)s.end,
  };
}
