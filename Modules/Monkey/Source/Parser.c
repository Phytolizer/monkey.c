#include "Monkey/Parser.h"

#include "Monkey/Token.h"

#include <stdlib.h>
#include <string.h>

static void ParserNextToken(Parser* p);
static Statement* ParseStatement(Parser* p);
static Statement* ParseLetStatement(Parser* p);
static bool CurTokenIs(Parser* p, TokenType type);
static bool PeekTokenIs(Parser* p, TokenType type);
static bool ExpectPeek(Parser* p, TokenType type);
static void PeekError(Parser* p, TokenType type);

Parser ParserInit(Lexer* l)
{
    Parser p = {.lexer = l};

    ParserNextToken(&p);
    ParserNextToken(&p);

    return p;
}

void ParserDeinit(Parser* p)
{
    for (int i = 0; i < p->errors.length; i += 1)
    {
        free(p->errors.data[i].data);
    }
    free(p->errors.data);
    free(p->curToken.literal.data);
    free(p->peekToken.literal.data);
}

Program* ParseProgram(Parser* p)
{
    int statementsCapacity = 1;
    Statement** statements = calloc(statementsCapacity, sizeof(Statement*));
    int statementsLength = 0;

    while (!CurTokenIs(p, TokenTypeEof))
    {
        Statement* stmt = ParseStatement(p);
        if (stmt != NULL)
        {
            if (statementsLength + 1 > statementsCapacity)
            {
                statementsCapacity *= 2;
                Statement** newStatements = calloc(sizeof(Statement*), statementsCapacity);
                memcpy(newStatements, statements, statementsLength * sizeof(Statement*));
                free(statements);
                statements = newStatements;
            }
            statements[statementsLength] = stmt;
            statementsLength += 1;
        }
        ParserNextToken(p);
    }

    return ProgramInit(statements, statementsLength);
}

void ParserNextToken(Parser* p)
{
    free(p->curToken.literal.data);
    p->curToken = p->peekToken;
    p->peekToken = LexerNextToken(p->lexer);
}

Statement* ParseStatement(Parser* p)
{
    if (p->curToken.type.data == TokenTypeLet.data)
    {
        return ParseLetStatement(p);
    }

    return NULL;
}

Statement* ParseLetStatement(Parser* p)
{
    Token token = p->curToken;
    p->curToken.literal.data = NULL;

    if (!ExpectPeek(p, TokenTypeIdent))
    {
        return NULL;
    }

    Identifier* name = calloc(sizeof(Identifier), 1);
    name->token = p->curToken;
    name->value = StringDuplicate(STRING_AS_SPAN(p->curToken.literal));
    // prevent double free
    p->curToken.literal.data = NULL;

    if (!ExpectPeek(p, TokenTypeAssign))
    {
        free(name->token.literal.data);
        free(name->value.data);
        free(name);
        return NULL;
    }

    // TODO: Skip the expressions until we encounter a semicolon.
    while (!CurTokenIs(p, TokenTypeSemicolon))
    {
        ParserNextToken(p);
    }

    Statement* result = calloc(sizeof(Statement), 1);
    result->type = StatementTypeLet;
    result->letStatement.token = token;
    result->letStatement.name = name;
    result->letStatement.value = NULL;
    return result;
}

bool CurTokenIs(Parser* p, TokenType type)
{
    return p->curToken.type.data == type.data;
}

bool PeekTokenIs(Parser* p, TokenType type)
{
    return p->peekToken.type.data == type.data;
}

bool ExpectPeek(Parser* p, TokenType type)
{
    if (PeekTokenIs(p, type))
    {
        ParserNextToken(p);
        return true;
    }

    PeekError(p, type);
    return false;
}

void PeekError(Parser* p, TokenType type)
{
    String message = StringPrintf("expected next token to be %.*s, got %.*s instead",
                                  type.length,
                                  type.data,
                                  p->peekToken.type.length,
                                  p->peekToken.type.data);
    if (p->errors.length + 1 > p->errorsCapacity)
    {
        p->errorsCapacity *= 2;
        String* newErrors = calloc(p->errorsCapacity, sizeof(String));
        memcpy(newErrors, p->errors.data, p->errors.length * sizeof(String));
        free(p->errors.data);
        p->errors.data = newErrors;
    }
    p->errors.data[p->errors.length] = message;
    p->errors.length += 1;
}
