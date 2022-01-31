#include "Monkey/Lexer.h"

#include "Monkey/Token.h"

#include <stdlib.h>
#include <string.h>

static void LexerReadChar(Lexer* l);
static String LexerReadIdentifier(Lexer* l);
static String LexerReadNumber(Lexer* l);
static void LexerSkipWhitespace(Lexer* l);
static Token NewToken(TokenType type, char ch);
static bool IsLetter(char ch);
static bool IsDigit(char ch);

Lexer LexerInit(StringSpan input)
{
    Lexer l = {
        .input = input,
        .position = 0,
        .readPosition = 0,
        .ch = '\0',
    };

    LexerReadChar(&l);

    return l;
}

Token LexerNextToken(Lexer* l)
{
    Token tok;

    LexerSkipWhitespace(l);

    switch (l->ch)
    {
        case '=':
            tok = NewToken(TokenTypeAssign, l->ch);
            break;
        case '+':
            tok = NewToken(TokenTypePlus, l->ch);
            break;
        case '!':
            tok = NewToken(TokenTypeBang, l->ch);
            break;
        case '-':
            tok = NewToken(TokenTypeMinus, l->ch);
            break;
        case '/':
            tok = NewToken(TokenTypeSlash, l->ch);
            break;
        case '*':
            tok = NewToken(TokenTypeAsterisk, l->ch);
            break;
        case '<':
            tok = NewToken(TokenTypeLt, l->ch);
            break;
        case '>':
            tok = NewToken(TokenTypeGt, l->ch);
            break;
        case '(':
            tok = NewToken(TokenTypeLparen, l->ch);
            break;
        case ')':
            tok = NewToken(TokenTypeRparen, l->ch);
            break;
        case '{':
            tok = NewToken(TokenTypeLbrace, l->ch);
            break;
        case '}':
            tok = NewToken(TokenTypeRbrace, l->ch);
            break;
        case ',':
            tok = NewToken(TokenTypeComma, l->ch);
            break;
        case ';':
            tok = NewToken(TokenTypeSemicolon, l->ch);
            break;
        case '\0':
            tok.type = TokenTypeEof;
            tok.literal = StringCopy("", 0);
            break;
        default:
            if (IsLetter(l->ch))
            {
                tok.literal = LexerReadIdentifier(l);
                tok.type = LookupIdent(STRING_AS_SPAN(tok.literal));
                return tok;
            }
            if (IsDigit(l->ch))
            {
                tok.type = TokenTypeInt;
                tok.literal = LexerReadNumber(l);
                return tok;
            }

            tok = NewToken(TokenTypeIllegal, l->ch);
            break;
    }

    LexerReadChar(l);

    return tok;
}

void LexerReadChar(Lexer* l)
{
    if (l->readPosition >= l->input.length)
    {
        l->ch = '\0';
    }
    else
    {
        l->ch = l->input.data[l->readPosition];
    }
    l->position = l->readPosition;
    l->readPosition += 1;
}

String LexerReadIdentifier(Lexer* l)
{
    int position = l->position;
    while (IsLetter(l->ch))
    {
        LexerReadChar(l);
    }
    return StringSubstring(l->input, position, l->position);
}

String LexerReadNumber(Lexer* l)
{
    int position = l->position;
    while (IsDigit(l->ch))
    {
        LexerReadChar(l);
    }
    return StringSubstring(l->input, position, l->position);
}

void LexerSkipWhitespace(Lexer* l)
{
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\r' || l->ch == '\n')
    {
        LexerReadChar(l);
    }
}

Token NewToken(TokenType type, char ch)
{
    return (Token){
        .type = type,
        .literal = StringCopy(&ch, 1),
    };
}

bool IsLetter(char ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

bool IsDigit(char ch)
{
    return ch >= '0' && ch <= '9';
}
