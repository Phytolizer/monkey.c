#include "Monkey/Token.h"

#include "SimpleString/String.h"

#define X(x, y) const TokenType TokenType##x = {.data = y, .length = sizeof y - 1};
TOKEN_TYPES_X
#undef X

TokenType LookupIdent(StringSpan ident)
{
    if (ident.length == 0)
    {
        return TokenTypeIdent;
    }
    switch (ident.data[0])
    {
        case 'e':
            if (StringSpansEqual(ident, STRING_SPAN("else")))
            {
                return TokenTypeElse;
            }
            break;
        case 'f':
            if (StringSpansEqual(ident, STRING_SPAN("false")))
            {
                return TokenTypeFalse;
            }
            if (StringSpansEqual(ident, STRING_SPAN("fn")))
            {
                return TokenTypeFunction;
            }
            break;
        case 'i':
            if (StringSpansEqual(ident, STRING_SPAN("if")))
            {
                return TokenTypeIf;
            }
            break;
        case 'l':
            if (StringSpansEqual(ident, STRING_SPAN("let")))
            {
                return TokenTypeLet;
            }
            break;
        case 'r':
            if (StringSpansEqual(ident, STRING_SPAN("return")))
            {
                return TokenTypeReturn;
            }
            break;
        case 't':
            if (StringSpansEqual(ident, STRING_SPAN("true")))
            {
                return TokenTypeTrue;
            }
            break;
    }
    return TokenTypeIdent;
}
