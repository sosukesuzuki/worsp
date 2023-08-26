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

void next_listExpr() {
  char *source = "'(1 2 3)";
  struct ParseState state = (struct ParseState){NULL, 0};
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_QUOTE);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_LPAREN);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_DIGIT);
  TEST_ASSERT(state.token->val == 1);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_DIGIT);
  TEST_ASSERT(state.token->val == 2);
  next(source, &state);
  TEST_ASSERT(state.token->kind == TK_DIGIT);
  TEST_ASSERT(state.token->val == 3);
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

void parse_integersSymbolicExpr() {
  char *source = "(1 2 3)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  TEST_ASSERT(result.program->expressions->expression->type ==
              EXP_SYMBOLIC_EXP);

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

void parse_integersList() {
  char *source = "'(1 2 3)";
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

void evaluate_literalExpressionInt() {
  char *source = "3";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->data.literal->int_value == 3);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.int_value == 3);
}

void evaluate_literalExpressionString() {
  char *source = "\"foo\"";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(strcmp(expr->data.literal->string_value, "foo") == 0);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_STRING);
  TEST_ASSERT(strcmp(evaluated.string_value, "foo") == 0);
}

void evaluate_nil() {
  char *source = "nil";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(strcmp(expr->data.symbol->symbol_name, "nil") == 0);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_NIL);
}

void evaluate_listWithInt() {
  char *source = "'(133)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_LIST);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_LIST);
  TEST_ASSERT(evaluated.list_value->car->type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.list_value->car->int_value == 133);
  TEST_ASSERT(evaluated.list_value->cdr.cdr_nil->type == OBJ_NIL);
}

void evaluate_listWithMultipleInt() {
  char *source = "'(133 234 345)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_LIST);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_LIST);
  TEST_ASSERT(evaluated.list_value->car->type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.list_value->car->int_value == 133);
  TEST_ASSERT(evaluated.list_value->cdr.cdr_cell->car->type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.list_value->cdr.cdr_cell->car->int_value == 234);
  TEST_ASSERT(evaluated.list_value->cdr.cdr_cell->cdr.cdr_cell->car->type ==
              OBJ_INTEGER);
  TEST_ASSERT(
      evaluated.list_value->cdr.cdr_cell->cdr.cdr_cell->car->int_value == 345);
  TEST_ASSERT(
      evaluated.list_value->cdr.cdr_cell->cdr.cdr_cell->cdr.cdr_nil->type ==
      OBJ_NIL);
}

void evaluate_emptyList() {
  char *source = "'()";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_LIST);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_NIL);
}

void evaluate_emptySymbolicExp() {
  char *source = "()";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_NIL);
}

void evaluate_addOp() {
  char *source = "(+ 1222 21)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.int_value == 1243);
}

void evaluate_subOp() {
  char *source = "(- 1222 21)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.int_value == 1201);
}

void evaluate_mulOp() {
  char *source = "(* 1222 21)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.int_value == 25662);
}

void evaluate_divOp() {
  char *source = "(/ 1222 21)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.int_value == 58);
}

void evaluate_modOp() {
  char *source = "(% 1222 21)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.int_value == 4);
}

void evaluate_orOpTrue() {
  char *source = "(|| true false)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 1);
}

void evaluate_orOpFalse() {
  char *source = "(|| false false)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 0);
}

void evaluate_orOpTrueInt() {
  char *source = "(|| 1 false)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 1);
}

void evaluate_orOpTrueNil() {
  char *source = "(|| 1 nil)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 1);
}

void evaluted_nestedOps() {
  char *source = "(+ 1 (- 2 (* 3 (/ 4 2))))";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.int_value == -3);
}

void evaluate_andOpFalse() {
  char *source = "(&& false false)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 0);
}

void evaluate_andOpFalseInt() {
  char *source = "(&& 1 false)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 0);
}

void evaluate_andOpFalseNil() {
  char *source = "(&& 1 nil)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 0);
}

void evaluate_andOpTrue() {
  char *source = "(&& true true)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 1);
}

void evaluate_andOpTrueInt() {
  char *source = "(&& 1 2)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 1);
}

void evaluate_eqTrue() {
  char *source = "(eq 1 1)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 1);
}

void evaluate_eqFalse() {
  char *source = "(eq 2 1)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 0);
}

void evaluate_eqTrueNil() {
  char *source = "(eq nil nil)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 1);
}

void evaluate_eqTrueNilList() {
  char *source = "(eq '() nil)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 1);
}

void evaluate_eqTrueNilSExp() {
  char *source = "(eq () nil)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  parse(source, &state, &result);

  struct ExpressionNode *expr = result.program->expressions->expression;
  TEST_ASSERT(expr->type == EXP_SYMBOLIC_EXP);

  struct Object evaluated = (struct Object){};
  evaluateExpression(expr, &evaluated);

  TEST_ASSERT(evaluated.type == OBJ_BOOL);
  TEST_ASSERT(evaluated.bool_value == 1);
}

void evaluate_printString() {
  char *source = "(print \"hello\")";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  struct Object evaluated = (struct Object){};
  parse(source, &state, &result);
  evaluateExpression(result.program->expressions->expression, &evaluated);
  TEST_ASSERT(evaluated.type == OBJ_NIL);
}

void evaluate_printInt() {
  char *source = "(print 3)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  struct Object evaluated = (struct Object){};
  parse(source, &state, &result);
  evaluateExpression(result.program->expressions->expression, &evaluated);
  TEST_ASSERT(evaluated.type == OBJ_NIL);
}

void evaluate_printBooleanT() {
  char *source = "(print true)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  struct Object evaluated = (struct Object){};
  parse(source, &state, &result);
  evaluateExpression(result.program->expressions->expression, &evaluated);
  TEST_ASSERT(evaluated.type == OBJ_NIL);
}

void evaluate_printBooleanF() {
  char *source = "(print false)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  struct Object evaluated = (struct Object){};
  parse(source, &state, &result);
  evaluateExpression(result.program->expressions->expression, &evaluated);
  TEST_ASSERT(evaluated.type == OBJ_NIL);
}

void evaluate_printList() {
  char *source = "(print '(1 2 3))";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  struct Object evaluated = (struct Object){};
  parse(source, &state, &result);
  evaluateExpression(result.program->expressions->expression, &evaluated);
  TEST_ASSERT(evaluated.type == OBJ_NIL);
}

void evaluate_ifThen() {
  char *source = "(if true 1)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  struct Object evaluated = (struct Object){};
  parse(source, &state, &result);
  evaluateExpression(result.program->expressions->expression, &evaluated);
  TEST_ASSERT(evaluated.type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.int_value == 1);
}

void evaluate_ifThenElse() {
  char *source = "(if false 1 2)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  struct Object evaluated = (struct Object){};
  parse(source, &state, &result);
  evaluateExpression(result.program->expressions->expression, &evaluated);
  TEST_ASSERT(evaluated.type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.int_value == 2);
}

void evaluate_complexIf() {
  char *source = "(if (|| (eq 1 1) false) (if true 1 2) 2)";
  struct ParseState state = (struct ParseState){NULL, 0};
  struct ParseResult result = (struct ParseResult){NULL};
  struct Object evaluated = (struct Object){};
  parse(source, &state, &result);
  evaluateExpression(result.program->expressions->expression, &evaluated);
  TEST_ASSERT(evaluated.type == OBJ_INTEGER);
  TEST_ASSERT(evaluated.int_value == 1);
}

int main() {
  RUN_TEST(next_singleCharSymbol);
  RUN_TEST(next_multipleCharSymbol);
  RUN_TEST(next_parenAndDigit);
  RUN_TEST(next_ifAndSet);
  RUN_TEST(next_string);
  RUN_TEST(next_addOp);
  RUN_TEST(next_listExpr);

  RUN_TEST(parse_intLiteral);
  RUN_TEST(parse_stringLiteral);
  RUN_TEST(parse_multipleLiteralExpressions);
  RUN_TEST(parse_integersSymbolicExpr);
  RUN_TEST(parse_integersList);

  RUN_TEST(evaluate_literalExpressionInt);
  RUN_TEST(evaluate_literalExpressionString);
  RUN_TEST(evaluate_nil);
  RUN_TEST(evaluate_listWithInt);
  RUN_TEST(evaluate_listWithMultipleInt);
  RUN_TEST(evaluate_emptyList);
  RUN_TEST(evaluate_emptySymbolicExp);
  RUN_TEST(evaluate_addOp);
  RUN_TEST(evaluate_subOp);
  RUN_TEST(evaluate_mulOp);
  RUN_TEST(evaluate_divOp);
  RUN_TEST(evaluate_modOp);
  RUN_TEST(evaluate_orOpTrue);
  RUN_TEST(evaluate_orOpFalse);
  RUN_TEST(evaluate_orOpTrueInt);
  RUN_TEST(evaluate_orOpTrueNil);
  RUN_TEST(evaluted_nestedOps);
  RUN_TEST(evaluate_andOpFalse);
  RUN_TEST(evaluate_andOpFalseInt);
  RUN_TEST(evaluate_andOpFalseNil);
  RUN_TEST(evaluate_andOpTrue);
  RUN_TEST(evaluate_andOpTrueInt);
  RUN_TEST(evaluate_eqTrue);
  RUN_TEST(evaluate_eqFalse);
  RUN_TEST(evaluate_eqTrueNil);
  RUN_TEST(evaluate_eqTrueNilList);
  RUN_TEST(evaluate_eqTrueNilSExp);
  RUN_TEST(evaluate_printString);
  RUN_TEST(evaluate_printInt);
  RUN_TEST(evaluate_printBooleanT);
  RUN_TEST(evaluate_printBooleanF);
  RUN_TEST(evaluate_printList);
  RUN_TEST(evaluate_ifThen);
  RUN_TEST(evaluate_ifThenElse);
  RUN_TEST(evaluate_complexIf);

  return 0;
}
