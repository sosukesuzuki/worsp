#include "worsp.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(expr)                                                      \
  do {                                                                         \
    if (!(expr)) {                                                             \
      fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #expr, __FILE__,       \
              __LINE__);                                                       \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)

#define RUN_TEST(test_func)                                                    \
  do {                                                                         \
    printf("Running test: %s\n", #test_func);                                  \
    test_func();                                                               \
    printf("Test passed.\n\n");                                                \
  } while (0)

void next_singleCharSymbol() {
  char *source = "a";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "a") == 0);
}

void next_multipleCharSymbol() {
  char *source = "aaaa";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "aaaa") == 0);
}

void next_parenAndDigit() {
  char *source = "(1)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_LPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_DIGIT);
  TEST_ASSERT(state.token->val == 1);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_RPAREN);
}

void next_ifAndSet() {
  char *source = "(if (set a 1) (set b 2) (set c 3))";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_LPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_IF);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_LPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SET);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "a") == 0);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_DIGIT);
  TEST_ASSERT(state.token->val == 1);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_RPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_LPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SET);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "b") == 0);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_DIGIT);
  TEST_ASSERT(state.token->val == 2);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_RPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_LPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SET);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "c") == 0);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_DIGIT);
  TEST_ASSERT(state.token->val == 3);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_RPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_RPAREN);
}

void next_string() {
  char *source = "\"hello\" () 1 \"foo\" \"bar\"";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_STRING);
  TEST_ASSERT(strcmp(state.token->str, "hello") == 0);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_LPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_RPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_DIGIT);
  TEST_ASSERT(state.token->val == 1);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_STRING);
  TEST_ASSERT(strcmp(state.token->str, "foo") == 0);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_STRING);
  TEST_ASSERT(strcmp(state.token->str, "bar") == 0);
}

int main() {
  RUN_TEST(next_singleCharSymbol);
  RUN_TEST(next_multipleCharSymbol);
  RUN_TEST(next_parenAndDigit);
  RUN_TEST(next_ifAndSet);
  RUN_TEST(next_string);

  return 0;
}
