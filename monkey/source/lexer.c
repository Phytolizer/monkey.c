#include "monkey/lexer.h"

#include <stdbool.h>
#include <stdint.h>
#include <string/string.h>

#include "monkey/token.h"

static void ReadChar(MkLexer* lexer);
static char PeekChar(MkLexer* lexer);
static String ReadIdentifier(MkLexer* lexer);
static String ReadNumber(MkLexer* lexer);
static void SkipWhitespace(MkLexer* lexer);
static MkToken SingleCharToken(MkTokenType type, uint8_t literal);
static MkToken TwoCharToken(MkTokenType type, uint8_t c1, uint8_t c2);
static bool IsLetter(uint8_t ch);
static bool IsDigit(uint8_t ch);

void MkLexerInit(MkLexer* lexer, StringView source) {
  lexer->source = source;
  lexer->position = 0;
  lexer->read_position = 0;
  ReadChar(lexer);
}

MkToken MkLexerNextToken(MkLexer* lexer) {
  MkToken tok = {0};
  SkipWhitespace(lexer);
  switch (lexer->ch) {
    case '=':
      if (PeekChar(lexer) == '=') {
        tok = TwoCharToken(mk_token_eq, lexer->ch, PeekChar(lexer));
        ReadChar(lexer);
      } else {
        tok = SingleCharToken(mk_token_assign, lexer->ch);
      }
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
    case '-':
      tok = SingleCharToken(mk_token_minus, lexer->ch);
      break;
    case '!':
      if (PeekChar(lexer) == '=') {
        tok = TwoCharToken(mk_token_not_eq, lexer->ch, PeekChar(lexer));
        ReadChar(lexer);
      } else {
        tok = SingleCharToken(mk_token_bang, lexer->ch);
      }
      break;
    case '*':
      tok = SingleCharToken(mk_token_asterisk, lexer->ch);
      break;
    case '/':
      tok = SingleCharToken(mk_token_slash, lexer->ch);
      break;
    case '<':
      tok = SingleCharToken(mk_token_lt, lexer->ch);
      break;
    case '>':
      tok = SingleCharToken(mk_token_gt, lexer->ch);
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
      } else if (IsDigit(lexer->ch)) {
        tok.literal = ReadNumber(lexer);
        tok.type = mk_token_int;
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

char PeekChar(MkLexer* lexer) {
  if (lexer->read_position >= SPAN_SIZE(&lexer->source)) {
    return '\0';
  } else {
    return lexer->source.begin[lexer->read_position];
  }
}

String ReadIdentifier(MkLexer* lexer) {
  StringView ident = {0};
  ident.begin = &lexer->source.begin[lexer->position];
  while (IsLetter(lexer->ch)) {
    ReadChar(lexer);
  }
  ident.end = &lexer->source.begin[lexer->position];
  return StringFromSpan(ident);
}

String ReadNumber(MkLexer* lexer) {
  StringView number = {0};
  number.begin = &lexer->source.begin[lexer->position];
  while (IsDigit(lexer->ch)) {
    ReadChar(lexer);
  }
  number.end = &lexer->source.begin[lexer->position];
  return StringFromSpan(number);
}

void SkipWhitespace(MkLexer* lexer) {
  while (lexer->ch == ' ' || lexer->ch == '\t' || lexer->ch == '\n' ||
         lexer->ch == '\r') {
    ReadChar(lexer);
  }
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

MkToken TwoCharToken(MkTokenType type, uint8_t c1, uint8_t c2) {
  const char chars[] = {c1, c2};
  return (MkToken){
      .type = type,
      .literal = StringFromSpan((StringView){
          .begin = chars,
          .end = chars + 2,
      }),
  };
}

bool IsLetter(uint8_t ch) {
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool IsDigit(uint8_t ch) {
  return ch >= '0' && ch <= '9';
}
