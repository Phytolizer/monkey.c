#include "Monkey/Lexer.h"

#include <stdlib.h>
#include <string.h>

static void LexerReadChar(Lexer* l);
static Token NewToken(TokenType type, char ch);

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
            tok.literal = StringCopy("", 0);
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

Token NewToken(TokenType type, char ch)
{
    return (Token){
        .type = type,
        .literal = StringCopy(&ch, 1),
    };
}
