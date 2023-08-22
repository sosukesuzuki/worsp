#include "worsp.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #expr, __FILE__, __LINE__); \
            exit(1); \
        } \
    } while (0)

#define RUN_TEST(test_func) \
    do { \
        printf("Running test: %s\n", #test_func); \
        test_func(); \
        printf("Test passed.\n\n"); \
    } while (0)

void runTest1() {
  char *source = "a";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "a") == 0);
}

void runTest2() {
  char *source = "aaaa";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "aaaa") == 0);
}

int main() {
  RUN_TEST(runTest1);
  RUN_TEST(runTest2);

  return 0;
}
