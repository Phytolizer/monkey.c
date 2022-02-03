#include "Monkey/Parser.h"
#include "Monkey/Token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void ParserNextToken(Parser* p);
static Statement* ParseStatement(Parser* p);
static Statement* ParseLetStatement(Parser* p);
static Statement* ParseReturnStatement(Parser* p);
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
    // This is not uninitialized.
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 6001)
#endif
    for (int i = 0; i < p->errors.length; i += 1)
    {
        free(p->errors.data[i].data);
    }
#ifdef _MSC_VER
#pragma warning(pop)
#endif
    free(p->errors.data);
    free(p->curToken.literal.data);
    free(p->peekToken.literal.data);
}

Program* ParseProgram(Parser* p)
{
    int statementsCapacity = 1;
    Statement** statements = calloc(statementsCapacity, sizeof(Statement*));
    if (statements == NULL)
    {
        return NULL;
    }
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
                if (newStatements == NULL)
                {
                    free(statements);
                    return NULL;
                }
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
    if (CurTokenIs(p, TokenTypeLet))
    {
        return ParseLetStatement(p);
    }
    if (CurTokenIs(p, TokenTypeReturn))
    {
        return ParseReturnStatement(p);
    }

    return NULL;
}

Statement* ParseLetStatement(Parser* p)
{
    Token token = p->curToken;
    p->curToken.literal.data = NULL;

    if (!ExpectPeek(p, TokenTypeIdent))
    {
        free(token.literal.data);
        return NULL;
    }

    Identifier* name = calloc(sizeof(Identifier), 1);
    if (name == NULL)
    {
        free(token.literal.data);
        return NULL;
    }
    name->token = p->curToken;
    name->value = StringDuplicate(STRING_AS_SPAN(p->curToken.literal));
    // prevent double free
    p->curToken.literal.data = NULL;

    if (!ExpectPeek(p, TokenTypeAssign))
    {
        free(token.literal.data);
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
    if (result == NULL)
    {
        return NULL;
    }
    result->type = StatementTypeLet;
    result->as.letStatement.token = token;
    result->as.letStatement.name = name;
    result->as.letStatement.value = NULL;
    return result;
}

Statement* ParseReturnStatement(Parser* p)
{
    Token token = p->curToken;
    p->curToken.literal.data = NULL;

    ParserNextToken(p);

    // TODO: Skip the expressions until we encounter a semicolon.
    while (!CurTokenIs(p, TokenTypeSemicolon))
    {
        ParserNextToken(p);
    }

    Statement* result = calloc(sizeof(Statement), 1);
    if (result == NULL)
    {
        free(token.literal.data);
        return NULL;
    }
    result->type = StatementTypeReturn;
    result->as.returnStatement.token = token;
    result->as.returnStatement.returnValue = NULL;
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
        p->errorsCapacity = p->errorsCapacity * 2 + 1;
        String* newErrors = calloc(p->errorsCapacity, sizeof(String));
        if (newErrors == NULL)
        {
            free(p->errors.data);
            free(message.data);
            fprintf(stderr, "[FATAL] Out of memory\n");
            exit(1);
        }
        if (p->errors.length > 0)
        {
            memcpy(newErrors, p->errors.data, p->errors.length * sizeof(String));
        }
        free(p->errors.data);
        p->errors.data = newErrors;
    }
    p->errors.data[p->errors.length] = message;
    p->errors.length += 1;
}
