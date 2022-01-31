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
        case 'f':
            if (StringSpansEqual(ident, STRING_SPAN("fn")))
            {
                return TokenTypeFunction;
            }
            break;
        case 'l':
            if (StringSpansEqual(ident, STRING_SPAN("let")))
            {
                return TokenTypeLet;
            }
            break;
    }
    return TokenTypeIdent;
}
