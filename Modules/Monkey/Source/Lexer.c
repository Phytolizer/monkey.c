#include "Monkey/Lexer.h"

#include <stdlib.h>
#include <string.h>

static void LexerReadChar(Lexer* l);
static Token NewToken(TokenType type, char ch);
static char* CopyString(const char* str, int len);

Lexer LexerInit(const char* input, const int inputLen)
{
    Lexer l = {
        .input = input,
        .inputLen = inputLen,
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

    switch (l->ch)
    {
        case '=':
            tok = NewToken(TokenTypeAssign, l->ch);
            break;
        case '+':
            tok = NewToken(TokenTypePlus, l->ch);
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
            tok.literal = CopyString("", 0);
            tok.literalLen = 0;
            break;
    }

    LexerReadChar(l);

    return tok;
}

void LexerReadChar(Lexer* l)
{
    if (l->readPosition >= l->inputLen)
    {
        l->ch = '\0';
    }
    else
    {
        l->ch = l->input[l->readPosition];
    }
    l->position = l->readPosition;
    l->readPosition += 1;
}

Token NewToken(TokenType type, char ch)
{
    return (Token){
        .type = type,
        .literal = CopyString(&ch, 1),
        .literalLen = 1,
    };
}

char* CopyString(const char* str, int len)
{
    char* result = calloc(len + 1, 1);
    memcpy(result, str, len);
    return result;
}
