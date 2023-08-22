#include "worsp.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void runTest1() {
  char *source = "a";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  next(source, &state);
  assert(state.token->kind == TK_SYMBOL);
}

int main() {
  printf("Run tests\n");

  runTest1();

  printf("Done\n");

  return 0;
}
