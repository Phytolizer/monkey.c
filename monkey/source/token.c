#include "monkey/token.h"

#include <stdlib.h>
#include <string/string.h>

String kTokenIllegal;
String kTokenEof;

String kTokenIdentifier;
String kTokenInt;

String kTokenAssign;
String kTokenPlus;

String kTokenComma;
String kTokenSemicolon;

String kTokenLParen;
String kTokenRParen;
String kTokenLBrace;
String kTokenRBrace;

String kTokenFunction;
String kTokenLet;

void MkTokenTypesManage(MkTokenTypesAction action) {
  switch (action) {
    case kTokenTypesInit:
      kTokenIllegal = StringFromC("ILLEGAL");
      kTokenEof = StringFromC("EOF");

      kTokenIdentifier = StringFromC("IDENT");
      kTokenInt = StringFromC("INT");

      kTokenAssign = StringFromC("=");
      kTokenPlus = StringFromC("+");

      kTokenComma = StringFromC(",");
      kTokenSemicolon = StringFromC(";");

      kTokenLParen = StringFromC("(");
      kTokenRParen = StringFromC(")");
      kTokenLBrace = StringFromC("{");
      kTokenRBrace = StringFromC("}");

      kTokenFunction = StringFromC("FUNCTION");
      kTokenLet = StringFromC("LET");
      break;
    case kTokenTypesFree:
      VEC_FREE(&kTokenIllegal);
      VEC_FREE(&kTokenEof);
      VEC_FREE(&kTokenIdentifier);
      VEC_FREE(&kTokenInt);
      VEC_FREE(&kTokenAssign);
      VEC_FREE(&kTokenPlus);
      VEC_FREE(&kTokenComma);
      VEC_FREE(&kTokenSemicolon);
      VEC_FREE(&kTokenLParen);
      VEC_FREE(&kTokenRParen);
      VEC_FREE(&kTokenLBrace);
      VEC_FREE(&kTokenRBrace);
      VEC_FREE(&kTokenFunction);
      VEC_FREE(&kTokenLet);
      break;
  }
}

void MkTokenFree(MkToken tok) {
  VEC_FREE(&tok.literal);
}
