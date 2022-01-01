#include <monkey/lexer.h>
#include <monkey/token.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char kPrompt[] = ">> ";

void ReplStart(FILE* in, FILE* out) {
  char* line = NULL;
  size_t line_len = 0;
  while (true) {
    fprintf(out, "%s", kPrompt);
    if (getline(&line, &line_len, in) == -1) {
      free(line);
      break;
    }
    MkLexer lexer;
    MkLexerInit(&lexer, (StringView){.begin = line, .end = line + line_len});
    for (MkToken tok = MkLexerNextToken(&lexer);
         !StringEqual(tok.type, mk_token_eof); tok = MkLexerNextToken(&lexer)) {
      MkTokenPrint(out, tok);
      fprintf(out, "\n");
      MkTokenFree(tok);
    }
  }
}

int main(void) {
  char name[32] = {0};
  if (getlogin_r(name, 32) != 0) {
    return 1;
  }

  printf("Hello, %s! This is the Monkey programming language!\n", name);
  printf("Feel free to type in commands\n");

  MkTokenTypesManage(kTokenTypesInit);
  ReplStart(stdin, stdout);
  MkTokenTypesManage(kTokenTypesFree);
  return 0;
}
