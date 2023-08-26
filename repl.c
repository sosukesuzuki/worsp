#include "worsp.h"
#include <stdio.h>
#include <stdlib.h>

void handleCtrlC() {
  printf("\nCtrl-C is pressed.\n");
  exit(0);
}

int main() {
  signal(SIGINT, handleCtrlC);

  while (1) {
    char input[1024];
    printf("> ");

    if (scanf("%[^\n]%*c", input) == EOF) {
      printf("\nAbort .\n");
      break;
    }

    struct ParseState state = (struct ParseState){NULL, 0};
    struct ParseResult result = (struct ParseResult){NULL};
    struct Object evaluated = (struct Object){};
    parse(input, &state, &result);
    struct Env env = (struct Env){};
    initEnv(&env);
    evaluateExpression(result.program->expressions->expression, &evaluated,
                       &env);

    char *stringified = stringifyObject(&evaluated);

    printf("%s\n", stringified);

    free(stringified);
  }

  return 0;
}
