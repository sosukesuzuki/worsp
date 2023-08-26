#include "worsp.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =================================================
//   tokenizer
// =================================================

int isop(int ch) {
  return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' ||
         ch == '|' || ch == '&' || ch == '=';
}

int match(struct ParseState *state, TokenKind kind) {
  return state->token->kind == kind ? 1 : 0;
}

void next(char *source, struct ParseState *state) {
  // Skip whitespaces
  while (isspace(source[state->pos])) {
    state->pos++;
  }

  struct Token *current = state->token;
  struct Token *new = malloc(sizeof(struct Token));

  if (source[state->pos] == '(') {
    new->kind = TK_LPAREN;
    new->str = "(";
    state->pos++;
  } else if (source[state->pos] == ')') {
    new->kind = TK_RPAREN;
    new->str = ")";
    state->pos++;
  } else if (source[state->pos] == '\'') {
    new->kind = TK_QUOTE;
    new->str = "'";
    state->pos++;
  } else if (source[state->pos] == '\0') {
    new->kind = TK_EOF;
    new->str = "\0";
  } else if (isalpha(source[state->pos]) || isop(source[state->pos])) {
    // tokenize symbol
    int start = state->pos;
    while (isalnum(source[state->pos]) || isop(source[state->pos])) {
      state->pos++;
    }
    int length = state->pos - start;

    char *str = malloc(length + 1);
    strncpy(str, &source[start], length);
    str[length] = '\0';

    if (strcmp(str, "true") == 0) {
      new->kind = TK_TRUE;
      free(str);
    } else if (strcmp(str, "false") == 0) {
      new->kind = TK_FALSE;
      free(str);
    } else {
      new->kind = TK_SYMBOL;
      new->str = str;
    }
  } else if (isdigit(source[state->pos])) {
    // tokenize digit
    int start = state->pos;
    while (isdigit(source[state->pos])) {
      state->pos++;
    }
    int length = state->pos - start;

    char *str = malloc(length + 1);
    strncpy(str, &source[start], length);
    str[length] = '\0';
    int val = atoi(str);
    free(str);

    new->kind = TK_DIGIT;
    new->val = val;
  } else if (source[state->pos] == '"') {
    // tokenize string
    state->pos++; // skip '"'
    int start = state->pos;
    while (source[state->pos] != '"' && source[state->pos] != '\0') {
      state->pos++;
    }
    int length = state->pos - start;
    new->kind = TK_STRING;
    new->str = malloc(length + 1);
    strncpy(new->str, &source[start], length);
    new->str[length] = '\0';
    if (source[state->pos] == '"') {
      state->pos++; // Skip quote
    }
  } else {
    printf("Unexpected token: %c\n", source[state->pos]);
    exit(1);
  }

  if (current == NULL) {
    state->token = new;
    return;
  }
  state->token = current->next = new;
}

// =================================================
//   parser
//     <program>          ::= <expression>*
//     <expression>       ::=
//                        | <s_expression>
//                        | <list>
//                        | <symbol>
//                        | <literal>
//     <s_expression>     ::= "(" <expression>* ")"
//     <list>             ::= "'(" <expression>* ")"
//     <symbol>           ::= <symbol_name>
//     <literal>          ::=
//                         | <interger_literal>
//                         | <string_literal>
//                         | <boolean_literal>
//     <interger_literal> ::= '1' | '2' | '3' | ...
//     <string_literal>   ::= '"' <letter>* '"'
//     <letter>           ::= 'a' | 'b' | 'c' | ...
//     <boolean_literal>  ::= 'true' | 'false'
// =================================================

void appendExpressionToListExpression(struct ListNode *listNode,
                                      struct ExpressionNode *expression) {
  struct ExpressionList *expressions = malloc(sizeof(struct ExpressionList));
  expressions->expression = expression;
  expressions->next = NULL;
  if (listNode->expressions == NULL) {
    listNode->expressions = expressions;
    return;
  }
  struct ExpressionList *current = listNode->expressions;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = expressions;
}

void appendExpressionToSymbolicExpression(
    struct SymbolicExpNode *symbolicExpNode,
    struct ExpressionNode *expression) {
  struct ExpressionList *expressions = malloc(sizeof(struct ExpressionList));
  expressions->expression = expression;
  expressions->next = NULL;
  if (symbolicExpNode->expressions == NULL) {
    symbolicExpNode->expressions = expressions;
    return;
  }
  struct ExpressionList *current = symbolicExpNode->expressions;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = expressions;
}

void parseExpression(char *source, struct ParseState *state,
                     struct ExpressionNode *expression);

void parseSymbolicExpression(char *source, struct ParseState *state,
                             struct ExpressionNode *expression) {
  struct SymbolicExpNode *symbolicExp = malloc(sizeof(struct SymbolicExpNode));

  expression->type = EXP_SYMBOLIC_EXP;
  expression->data.symbolic_exp = symbolicExp;
  expression->data.symbolic_exp->expressions = NULL;
  next(source, state); // eat '('
  while (!match(state, TK_RPAREN)) {
    struct ExpressionNode *expressionItem =
        malloc(sizeof(struct ExpressionNode));
    parseExpression(source, state, expressionItem);
    appendExpressionToSymbolicExpression(symbolicExp, expressionItem);
  }
  next(source, state); // eat ')'
}

void parseListExpression(char *source, struct ParseState *state,
                         struct ExpressionNode *expression) {
  struct ListNode *list = malloc(sizeof(struct ListNode));

  expression->type = EXP_LIST;
  expression->data.list = list;
  expression->data.list->expressions = NULL;
  next(source, state); // eat quote
  next(source, state); // eat '('
  while (!match(state, TK_RPAREN)) {
    struct ExpressionNode *expressionItem =
        malloc(sizeof(struct ExpressionNode));
    parseExpression(source, state, expressionItem);
    appendExpressionToListExpression(list, expressionItem);
  }
  next(source, state); // eat ')'
}

void parseSymbolExpression(char *source, struct ParseState *state,
                           struct ExpressionNode *expression) {
  expression->type = EXP_SYMBOL;
  expression->data.symbol = malloc(sizeof(struct SymbolNode));
  expression->data.symbol->symbol_name = state->token->str;
  next(source, state);
}

void parseLiteralExpression(char *source, struct ParseState *state,
                            struct ExpressionNode *expression) {
  if (match(state, TK_DIGIT)) {
    expression->type = EXP_LITERAL;
    expression->data.literal = malloc(sizeof(struct LiteralNode));
    expression->data.literal->type = LIT_INTERGER;
    expression->data.literal->int_value = state->token->val;
    next(source, state);
  } else if (match(state, TK_STRING)) {
    expression->type = EXP_LITERAL;
    expression->data.literal = malloc(sizeof(struct LiteralNode));
    expression->data.literal->type = LIT_STRING;
    expression->data.literal->string_value = state->token->str;
    next(source, state);
  } else if (match(state, TK_TRUE)) {
    expression->type = EXP_LITERAL;
    expression->data.literal = malloc(sizeof(struct LiteralNode));
    expression->data.literal->type = LIT_BOOLEAN;
    expression->data.literal->boolean_value = 1;
    next(source, state);
  } else if (match(state, TK_FALSE)) {
    expression->type = EXP_LITERAL;
    expression->data.literal = malloc(sizeof(struct LiteralNode));
    expression->data.literal->type = LIT_BOOLEAN;
    expression->data.literal->boolean_value = 0;
    next(source, state);
  } else {
    printf("Unexpected token: %s\n", state->token->str);
    exit(1);
  }
}

void parseExpression(char *source, struct ParseState *state,
                     struct ExpressionNode *expression) {
  if (match(state, TK_LPAREN)) {
    parseSymbolicExpression(source, state, expression);
  } else if (match(state, TK_QUOTE)) {
    parseListExpression(source, state, expression);
  } else if (match(state, TK_SYMBOL)) {
    parseSymbolExpression(source, state, expression);
  } else if (match(state, TK_DIGIT) || match(state, TK_STRING) ||
             match(state, TK_TRUE) || match(state, TK_FALSE)) {
    parseLiteralExpression(source, state, expression);
  } else {
    printf("Unexpected token: %s\n", state->token->str);
    exit(1);
  }
}

void appendExpressionToProgram(struct ProgramNode *program,
                               struct ExpressionNode *expression) {
  struct ExpressionList *expressions = malloc(sizeof(struct ExpressionList));
  expressions->expression = expression;
  expressions->next = NULL;
  if (program->expressions == NULL) {
    program->expressions = expressions;
    return;
  }
  struct ExpressionList *current = program->expressions;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = expressions;
}

void parseProgram(char *source, struct ParseState *state,
                  struct ParseResult *result) {
  // Set first token
  next(source, state);
  struct ProgramNode *program = malloc(sizeof(struct ProgramNode));
  program->expressions = NULL;

  result->program = program;

  while (!match(state, TK_EOF)) {
    struct ExpressionNode *expression = malloc(sizeof(struct ExpressionNode));
    parseExpression(source, state, expression);
    appendExpressionToProgram(program, expression);
  }
}

void parse(char *source, struct ParseState *state, struct ParseResult *result) {
  parseProgram(source, state, result);
}

// =================================================
//   defined functions
// =================================================

int boolVal(struct Object *obj) {
  if (obj->type == OBJ_BOOL) {
    return obj->bool_value;
  } else if (obj->type == OBJ_NIL) {
    return 0;
  } else {
    return 1;
  }
}

void definedFunctionAdd(struct Object *op1, struct Object *op2,
                        struct Object *evaluated) {
  if (op1->type == OBJ_INTEGER && op2->type == OBJ_INTEGER) {
    evaluated->type = OBJ_INTEGER;
    evaluated->int_value = op1->int_value + op2->int_value;
  } else {
    printf("Type error: operands for + must be integers.\n");
    exit(1);
  }
}

void definedFunctionSub(struct Object *op1, struct Object *op2,
                        struct Object *evaluated) {
  if (op1->type == OBJ_INTEGER && op2->type == OBJ_INTEGER) {
    evaluated->type = OBJ_INTEGER;
    evaluated->int_value = op1->int_value - op2->int_value;
  } else {
    printf("Type error: operands for - must be integers.\n");
    exit(1);
  }
}

void definedFunctionMul(struct Object *op1, struct Object *op2,
                        struct Object *evaluated) {
  if (op1->type == OBJ_INTEGER && op2->type == OBJ_INTEGER) {
    evaluated->type = OBJ_INTEGER;
    evaluated->int_value = op1->int_value * op2->int_value;
  } else {
    printf("Type error: operands for * must be integers.\n");
    exit(1);
  }
}

void definedFunctionDiv(struct Object *op1, struct Object *op2,
                        struct Object *evaluated) {
  if (op1->type == OBJ_INTEGER && op2->type == OBJ_INTEGER) {
    evaluated->type = OBJ_INTEGER;
    evaluated->int_value = op1->int_value / op2->int_value;
  } else {
    printf("Type error: operands for / must be integers.\n");
    exit(1);
  }
}

void definedFunctionMod(struct Object *op1, struct Object *op2,
                        struct Object *evaluated) {
  if (op1->type == OBJ_INTEGER && op2->type == OBJ_INTEGER) {
    evaluated->type = OBJ_INTEGER;
    evaluated->int_value = op1->int_value % op2->int_value;
  } else {
    printf("Type error: operands for % must be integers.\n");
    exit(1);
  }
}

void definedFunctionOr(struct Object *op1, struct Object *op2,
                       struct Object *evaluated) {
  if (boolVal(op1) || boolVal(op2)) {
    evaluated->type = OBJ_BOOL;
    evaluated->bool_value = 1;
  } else {
    evaluated->type = OBJ_BOOL;
    evaluated->bool_value = 0;
  }
}

// =================================================
//   evaluator
// =================================================

void evalateListExpression(struct ExpressionNode *expression,
                           struct Object *evaluated) {
  struct ExpressionList *expressions = expression->data.list->expressions;

  // empty data list is evaluated as nil
  if (expressions == NULL) {
    evaluated->type = OBJ_NIL;
    return;
  }

  evaluated->type = OBJ_LIST;

  struct ConsCell *car_conscell = NULL;
  struct ConsCell *prev_conscell = NULL;

  while (expressions != NULL) {
    struct ConsCell *new_conscell = malloc(sizeof(struct ConsCell));
    if (car_conscell == NULL) {
      car_conscell = new_conscell;
    }

    struct Object *evaluatedItem = malloc(sizeof(struct Object));
    new_conscell->car = evaluatedItem;
    evaluateExpression(expressions->expression, evaluatedItem);

    expressions = expressions->next;

    if (prev_conscell != NULL) {
      prev_conscell->cdr.cdr_cell = new_conscell;
    }
    prev_conscell = new_conscell;

    if (expressions == NULL) {
      struct Object *nilObj = malloc(sizeof(struct Object));
      nilObj->type = OBJ_NIL;
      prev_conscell->cdr.cdr_nil = nilObj;
    }
  }

  evaluated->list_value = car_conscell;
}

void evaluateSymbolicExpression(struct ExpressionNode *expression,
                                struct Object *evaluated) {
  struct ExpressionList *expressions = expression->data.list->expressions;
  if (expressions != NULL) {
    struct ExpressionNode *expr = expressions->expression;
    if (expr != NULL && expr->type == EXP_SYMBOL) {
      if (strcmp(expr->data.symbol->symbol_name, "if") == 0) {
        // if
      } else if (strcmp(expr->data.symbol->symbol_name, "while") == 0) {
        // while
      } else if (strcmp(expr->data.symbol->symbol_name, "=") == 0) {
        // assignment
      } else if ((strcmp(expr->data.symbol->symbol_name, "defun") == 0)) {
        // define function
      } else {
        // function call
        // note: default defined functions:
        if (strcmp(expr->data.symbol->symbol_name, "+") == 0) {
          // +
          struct Object *operand1 = malloc(sizeof(struct Object));
          struct Object *operand2 = malloc(sizeof(struct Object));
          evaluateExpression(expressions->next->expression, operand1);
          evaluateExpression(expressions->next->next->expression, operand2);
          definedFunctionAdd(operand1, operand2, evaluated);
          free(operand1);
          free(operand2);
        } else if (strcmp(expr->data.symbol->symbol_name, "-") == 0) {
          // -
          struct Object *operand1 = malloc(sizeof(struct Object));
          struct Object *operand2 = malloc(sizeof(struct Object));
          evaluateExpression(expressions->next->expression, operand1);
          evaluateExpression(expressions->next->next->expression, operand2);
          definedFunctionSub(operand1, operand2, evaluated);
          free(operand1);
          free(operand2);
        } else if (strcmp(expr->data.symbol->symbol_name, "*") == 0) {
          // *
          struct Object *operand1 = malloc(sizeof(struct Object));
          struct Object *operand2 = malloc(sizeof(struct Object));
          evaluateExpression(expressions->next->expression, operand1);
          evaluateExpression(expressions->next->next->expression, operand2);
          definedFunctionMul(operand1, operand2, evaluated);
          free(operand1);
          free(operand2);
        } else if (strcmp(expr->data.symbol->symbol_name, "/") == 0) {
          // /
          struct Object *operand1 = malloc(sizeof(struct Object));
          struct Object *operand2 = malloc(sizeof(struct Object));
          evaluateExpression(expressions->next->expression, operand1);
          evaluateExpression(expressions->next->next->expression, operand2);
          definedFunctionDiv(operand1, operand2, evaluated);
          free(operand1);
          free(operand2);
        } else if (strcmp(expr->data.symbol->symbol_name, "%") == 0) {
          // %
          struct Object *operand1 = malloc(sizeof(struct Object));
          struct Object *operand2 = malloc(sizeof(struct Object));
          evaluateExpression(expressions->next->expression, operand1);
          evaluateExpression(expressions->next->next->expression, operand2);
          definedFunctionMod(operand1, operand2, evaluated);
          free(operand1);
          free(operand2);
        } else if (strcmp(expr->data.symbol->symbol_name, "||") == 0) {
          // ||
          struct Object *operand1 = malloc(sizeof(struct Object));
          struct Object *operand2 = malloc(sizeof(struct Object));
          evaluateExpression(expressions->next->expression, operand1);
          evaluateExpression(expressions->next->next->expression, operand2);
          definedFunctionOr(operand1, operand2, evaluated);
          free(operand1);
          free(operand2);
        } else if (strcmp(expr->data.symbol->symbol_name, "&&") == 0) {
          // &&
        } else if (strcmp(expr->data.symbol->symbol_name, "==") == 0) {
          // ==
        } else if (strcmp(expr->data.symbol->symbol_name, "print") == 0) {
          // print
        } else if (strcmp(expr->data.symbol->symbol_name, "println") == 0) {
          // println
        } else if (strcmp(expr->data.symbol->symbol_name, "car") == 0) {
          // car
        } else if (strcmp(expr->data.symbol->symbol_name, "cdr") == 0) {
          // cdr
        } else if (strcmp(expr->data.symbol->symbol_name, "cons") == 0) {
          // cons
        } else {
          printf("Undefined function: %s\n", expr->data.symbol->symbol_name);
          exit(1);
        }
      }
    } else {
      printf("S-exp must be started with symbol.\n");
      exit(1);
    }
  } else {
    evaluated->type = OBJ_NIL;
  }
}

void evaluateLiteralExpression(struct ExpressionNode *expression,
                               struct Object *evaluated) {
  if (expression->data.literal->type == LIT_INTERGER) {
    evaluated->type = OBJ_INTEGER;
    evaluated->int_value = expression->data.literal->int_value;
  } else if (expression->data.literal->type == LIT_STRING) {
    evaluated->type = OBJ_STRING;
    evaluated->string_value = expression->data.literal->string_value;
  } else if (expression->data.literal->type == LIT_BOOLEAN) {
    evaluated->type = OBJ_BOOL;
    evaluated->bool_value = expression->data.literal->boolean_value;
  }
}

void evaluateSymbolExpression(struct ExpressionNode *expression,
                              struct Object *evaluated) {
  if (strcmp(expression->data.symbol->symbol_name, "nil") == 0) {
    evaluated->type = OBJ_NIL;
  } else {
    // get value from symbol table
    printf("variable is not implemented yet.\n");
    exit(1);
  }
}

void evaluateExpression(struct ExpressionNode *expression,
                        struct Object *evaluated) {
  if (expression->type == EXP_LIST) {
    evalateListExpression(expression, evaluated);
  } else if (expression->type == EXP_SYMBOLIC_EXP) {
    evaluateSymbolicExpression(expression, evaluated);
  } else if (expression->type == EXP_LITERAL) {
    evaluateLiteralExpression(expression, evaluated);
  } else if (expression->type == EXP_SYMBOL) {
    evaluateSymbolExpression(expression, evaluated);
  }
}

void evaluateProgram(struct ProgramNode *program) {
  struct ExpressionList *expressions = program->expressions;
  while (expressions != NULL) {
    struct Object *evaluated = malloc(sizeof(struct Object));
    evaluateExpression(expressions->expression, evaluated);
    expressions = expressions->next;
    free(evaluated);
  }
}

void evalate(struct ParseResult *result) {}
