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
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "a") == 0);
}

void next_multipleCharSymbol() {
  char *source = "aaaa";
  struct ParseState state = (struct ParseState){NULL, 0};
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "aaaa") == 0);
}

void next_parenAndDigit() {
  char *source = "(1)";
  struct ParseState state = (struct ParseState){NULL, 0};
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
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_LPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "if") == 0);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_LPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "set") == 0);
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
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "set") == 0);
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
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "set") == 0);
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

void next_addOp() {
  char *source = "(+ 1 2)";
  struct ParseState state = (struct ParseState){NULL, 0};
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_LPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_SYMBOL);
  TEST_ASSERT(strcmp(state.token->str, "+") == 0);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_DIGIT);
  TEST_ASSERT(state.token->val == 1);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_DIGIT);
  TEST_ASSERT(state.token->val == 2);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_RPAREN);
}

void parse_intLiteral() {
  char *source = "3";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);
  TEST_ASSERT(result.program->expressions->expression->type == EXP_LITERAL);
  TEST_ASSERT(result.program->expressions->expression->data.literal->type ==
              LIT_INTERGER);
  TEST_ASSERT(
      result.program->expressions->expression->data.literal->int_value == 3);
}

void parse_stringLiteral() {
  char *source = "\"foo\"";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);
  TEST_ASSERT(result.program->expressions->expression->type == EXP_LITERAL);
  TEST_ASSERT(result.program->expressions->expression->data.literal->type ==
              LIT_STRING);
  TEST_ASSERT(
      strcmp(
          result.program->expressions->expression->data.literal->string_value,
          "foo") == 0);
}

void parse_multipleLiteralExpressions() {
  char *source = "3 \"foo\"";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);
  TEST_ASSERT(result.program->expressions->expression->type == EXP_LITERAL);
  TEST_ASSERT(result.program->expressions->expression->data.literal->type ==
              LIT_INTERGER);
  TEST_ASSERT(
      result.program->expressions->expression->data.literal->int_value == 3);

  TEST_ASSERT(result.program->expressions->next->expression->type ==
              EXP_LITERAL);
  TEST_ASSERT(
      result.program->expressions->next->expression->data.literal->type ==
      LIT_STRING);
  TEST_ASSERT(strcmp(result.program->expressions->next->expression->data
                         .literal->string_value,
                     "foo") == 0);
}

void parse_integersList() {
  char *source = "(1 2 3)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  TEST_ASSERT(result.program->expressions->expression->type == EXP_LIST);

  struct ExpressionNode *expr1 = result.program->expressions->expression->data
                                     .list->expressions->expression;
  TEST_ASSERT(expr1->type == EXP_LITERAL);
  TEST_ASSERT(expr1->data.literal->type == LIT_INTERGER);
  TEST_ASSERT(expr1->data.literal->int_value == 1);

  struct ExpressionNode *expr2 = result.program->expressions->expression->data
                                     .list->expressions->next->expression;
  TEST_ASSERT(expr2->type == EXP_LITERAL);
  TEST_ASSERT(expr2->data.literal->type == LIT_INTERGER);
  TEST_ASSERT(expr2->data.literal->int_value == 2);

  struct ExpressionNode *expr3 = result.program->expressions->expression->data
                                     .list->expressions->next->next->expression;
  TEST_ASSERT(expr3->type == EXP_LITERAL);
  TEST_ASSERT(expr3->data.literal->type == LIT_INTERGER);
  TEST_ASSERT(expr3->data.literal->int_value == 3);

  TEST_ASSERT(match(&state, TK_EOF));
}

int main() {
  RUN_TEST(next_singleCharSymbol);
  RUN_TEST(next_multipleCharSymbol);
  RUN_TEST(next_parenAndDigit);
  RUN_TEST(next_ifAndSet);
  RUN_TEST(next_string);
  RUN_TEST(next_addOp);

  RUN_TEST(parse_intLiteral);
  RUN_TEST(parse_stringLiteral);
  RUN_TEST(parse_multipleLiteralExpressions);
  RUN_TEST(parse_integersList);

  return 0;
}
