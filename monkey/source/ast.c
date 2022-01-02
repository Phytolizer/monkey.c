#include "monkey/ast.h"

#include <stdint.h>
#include <stdlib.h>
#include <string/string.h>

#include "monkey/token.h"

static String ProgramTokenLiteral(MkAstProgram* prog);
static String StatementTokenLiteral(MkAstStatement* stmt);
static String ExpressionTokenLiteral(MkAstExpression* expr);
static void ProgramFree(MkAstProgram* prog);
static void StatementFree(MkAstStatement* stmt);
static void ExpressionFree(MkAstExpression* expr);

String MkAstNodeTokenLiteral(MkAstNode* node) {
  switch (node->type) {
    case kMkAstNodeProgram:
      return ProgramTokenLiteral((MkAstProgram*)node);
    case kMkAstNodeStatement:
      return StatementTokenLiteral((MkAstStatement*)node);
    case kMkAstNodeExpression:
      return ExpressionTokenLiteral((MkAstExpression*)node);
  }

  return StringFromC("invalid node");
}

void MkAstNodeFree(MkAstNode* node) {
  if (node == NULL) {
    return;
  }
  switch (node->type) {
    case kMkAstNodeProgram:
      ProgramFree((MkAstProgram*)node);
      break;
    case kMkAstNodeStatement:
      StatementFree((MkAstStatement*)node);
      break;
    case kMkAstNodeExpression:
      ExpressionFree((MkAstExpression*)node);
      break;
  }
}

String ProgramTokenLiteral(MkAstProgram* prog) {
  String result = {0};
  for (size_t i = 0; i < prog->statements.size; i++) {
    MkAstStatement* stmt = prog->statements.data[i];
    String stmt_str = StatementTokenLiteral(stmt);
    VEC_APPEND(&result, stmt_str.data, stmt_str.size);
    VEC_FREE(&stmt_str);
  }
  return result;
}

String StatementTokenLiteral(MkAstStatement* stmt) {
  switch (stmt->type) {
    case kMkAstStatementLet:
      return StringDuplicate(((MkAstLetStatement*)stmt)->token.literal);
  }

  return StringFromC("invalid statement");
}

String ExpressionTokenLiteral(MkAstExpression* expr) {
  switch (expr->type) {
    case kMkAstExpressionIdentifier:
      return StringDuplicate(((MkAstIdentifier*)expr)->token.literal);
  }

  return StringFromC("invalid expression");
}

void ProgramFree(MkAstProgram* prog) {
  if (prog == NULL) {
    return;
  }
  for (uint64_t i = 0; i < prog->statements.size; i++) {
    StatementFree(prog->statements.data[i]);
    free(prog->statements.data[i]);
  }
  VEC_FREE(&prog->statements);
}

void StatementFree(MkAstStatement* stmt) {
  if (stmt == NULL) {
    return;
  }
  switch (stmt->type) {
    case kMkAstStatementLet: {
      MkAstLetStatement* let_stmt = (MkAstLetStatement*)stmt;
      MkTokenFree(let_stmt->token);
      ExpressionFree(&let_stmt->name.base);
      ExpressionFree(let_stmt->value);
      free(let_stmt->value);
    } break;
  }
}

void ExpressionFree(MkAstExpression* expr) {
  if (expr == NULL) {
    return;
  }
  switch (expr->type) {
    case kMkAstExpressionIdentifier: {
      MkAstIdentifier* ident = (MkAstIdentifier*)expr;
      MkTokenFree(ident->token);
      VEC_FREE(&ident->value);
    } break;
  }
}
