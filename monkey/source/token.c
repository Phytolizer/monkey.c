#include "monkey/token.h"

#include <stdlib.h>
#include <string/string.h>

String mk_token_illegal;
String mk_token_eof;

String mk_token_identifier;
String mk_token_int;

String mk_token_assign;
String mk_token_plus;

String mk_token_comma;
String mk_token_semicolon;

String mk_token_lparen;
String mk_token_rparen;
String mk_token_lbrace;
String mk_token_rbrace;

String mk_token_function;
String mk_token_let;

void MkTokenTypesManage(MkTokenTypesAction action) {
  switch (action) {
    case kTokenTypesInit:
      mk_token_illegal = StringFromC("ILLEGAL");
      mk_token_eof = StringFromC("EOF");

      mk_token_identifier = StringFromC("IDENT");
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
      break;
    case kTokenTypesFree:
      VEC_FREE(&mk_token_illegal);
      VEC_FREE(&mk_token_eof);
      VEC_FREE(&mk_token_identifier);
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
      break;
  }
}

void MkTokenFree(MkToken tok) {
  VEC_FREE(&tok.literal);
}
