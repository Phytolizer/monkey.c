#ifndef MONKEY_AST_H_
#define MONKEY_AST_H_

#include "monkey/token.h"
#include "string/string.h"
#include "vec/vec.h"
#define MK_AST_NODES_ \
  X(Program)          \
  X(Statement)        \
  X(Expression)

typedef enum {
#define X(x) kMkAstNode##x,
  MK_AST_NODES_
#undef X
} MkAstNodeType;

typedef struct {
  MkAstNodeType type;
} MkAstNode;

#define MK_AST_STATEMENTS_ X(Let)

typedef enum {
#define X(x) kMkAstStatement##x,
  MK_AST_STATEMENTS_
#undef X
} MkAstStatementType;

typedef struct {
  MkAstNode base;
  MkAstStatementType type;
} MkAstStatement;

#define MK_AST_EXPRESSIONS_ X(Identifier)

typedef enum {
#define X(x) kMkAstExpression##x,
  MK_AST_EXPRESSIONS_
#undef X
} MkAstExpressionType;

typedef struct {
  MkAstNode base;
  MkAstExpressionType type;
} MkAstExpression;

typedef struct {
  MkAstNode base;
  VEC_TYPE(MkAstStatement*) statements;
} MkAstProgram;

typedef struct {
  MkAstExpression base;
  MkToken token;
  String value;
} MkAstIdentifier;

typedef struct {
  MkAstStatement base;
  MkToken token;
  MkAstIdentifier name;
  MkAstExpression* value;
} MkAstLetStatement;

String MkAstNodeTokenLiteral(MkAstNode* node);
void MkAstNodeFree(MkAstNode* node);

#endif  // MONKEY_AST_H_
