#include "monkey/lexer.h"

#include <stdbool.h>
#include <stdint.h>
#include <string/string.h>

#include "monkey/token.h"

static void ReadChar(MkLexer* lexer);
static String ReadIdentifier(MkLexer* lexer);
static MkToken SingleCharToken(MkTokenType type, uint8_t literal);
static bool IsLetter(uint8_t ch);

void MkLexerInit(MkLexer* lexer, StringView source) {
  lexer->source = source;
  lexer->position = 0;
  lexer->read_position = 0;
  ReadChar(lexer);
}

MkToken MkLexerNextToken(MkLexer* lexer) {
  MkToken tok = {0};
  switch (lexer->ch) {
    case '=':
      tok = SingleCharToken(mk_token_assign, lexer->ch);
      break;
    case ';':
      tok = SingleCharToken(mk_token_semicolon, lexer->ch);
      break;
    case '(':
      tok = SingleCharToken(mk_token_lparen, lexer->ch);
      break;
    case ')':
      tok = SingleCharToken(mk_token_rparen, lexer->ch);
      break;
    case ',':
      tok = SingleCharToken(mk_token_comma, lexer->ch);
      break;
    case '+':
      tok = SingleCharToken(mk_token_plus, lexer->ch);
      break;
    case '{':
      tok = SingleCharToken(mk_token_lbrace, lexer->ch);
      break;
    case '}':
      tok = SingleCharToken(mk_token_rbrace, lexer->ch);
      break;
    case '\0':
      tok.type = mk_token_eof;
      break;
    default:
      if (IsLetter(lexer->ch)) {
        tok.literal = ReadIdentifier(lexer);
        tok.type = MkLookupIdent((StringView){
            .begin = tok.literal.data,
            .end = tok.literal.data + tok.literal.size,
        });
        return tok;
      } else {
        tok = SingleCharToken(mk_token_illegal, lexer->ch);
      }
  }
  ReadChar(lexer);
  return tok;
}

void ReadChar(MkLexer* lexer) {
  if (lexer->read_position >= SPAN_SIZE(&lexer->source)) {
    lexer->ch = 0;
  } else {
    lexer->ch = lexer->source.begin[lexer->read_position];
  }
  lexer->position = lexer->read_position;
  lexer->read_position += 1;
}

String ReadIdentifier(MkLexer* lexer) {
  StringView ident = {0};
  ident.begin = &lexer->source.begin[lexer->read_position];
  while (IsLetter(lexer->ch)) {
    ReadChar(lexer);
  }
  ident.end = &lexer->source.begin[lexer->read_position];
  return StringFromSpan(ident);
}

MkToken SingleCharToken(MkTokenType type, uint8_t literal) {
  return (MkToken){
      .type = type,
      .literal = StringFromSpan((StringView){
          .begin = (const char*)&literal,
          .end = (const char*)&literal + 1,
      }),
  };
}

bool IsLetter(uint8_t ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}
