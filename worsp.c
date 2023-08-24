/*
<program> ::= <expression>*
<expression> ::=
    | <literal>
    | <symbol>
    | <function-call>
    | <binary-operation>
    | <conditional>
    | <list>
    | <assignment>
<list> ::= "(" <expression>* ")"
<assignment> ::= "(" "set" <symbol> <expression> ")"
<literal> ::= <number> | <string> | <boolean>
<symbol> ::= <identifier>
<function-call> ::= "(" <expression> <expression>* ")"
<binary-operation> ::= "(" <operator> <expression> <expression> ")"
<operator> ::= "+" | "-" | "*" | "/"
<conditional> ::= "(" "if" <expression> <expression> <expression> ")"
<number> ::= <digit>+
<string> ::= '"' <character>* '"'
<boolean> ::= "true" | "false"
<identifier> ::= <letter> <identifier-character>*
<letter> ::= "a" | "b" | ... | "z" | "A" | "B" | ... | "Z"
<digit> ::= "0" | "1" | ... | "9"
<character> ::= <letter> | <digit> | <special-character>
<identifier-character> ::= <letter> | <digit> | "_"
<special-character> ::= "+" | "-" | "*" | "/" | "=" | "<" | ">" | "&" | "|" |
"!" | "?" | "$" | "%" | "^" | "~"
*/

#include "worsp.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  } else if (source[state->pos] == '\0') {
    new->kind = TK_EOF;
    new->str = "\0";
  } else if (isalpha(source[state->pos])) {
    // tokenize symbol
    int start = state->pos;
    while (isalnum(source[state->pos])) {
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
      state->pos++; // クオートをスキップ
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

void parseSymbolExpression(char *source, struct ParseState *state,
                           struct ParseResult *result,
                           struct ExpressionNode *expression) {
  expression->type = EXP_SYMBOL;
  expression->data.symbol = malloc(sizeof(struct SymbolNode));
  expression->data.symbol->symbol_name = state->token->str;
  next(source, state);
}

void parseLiteralExpression(char *source, struct ParseState *state,
                            struct ParseResult *result,
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
                     struct ParseResult *result,
                     struct ExpressionNode *expression) {
  if (match(state, TK_LPAREN)) {
  } else {
    // expression does not start with '('
    if (match(state, TK_SYMBOL)) {
      parseSymbolExpression(source, state, result, expression);
    } else if (match(state, TK_DIGIT) || match(state, TK_STRING) ||
               match(state, TK_TRUE) || match(state, TK_FALSE)) {
      parseLiteralExpression(source, state, result, expression);
    } else {
      printf("Unexpected token: %s\n", state->token->str);
      exit(1);
    }
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
  struct ProgramNode *program = malloc(sizeof(struct ProgramNode));
  program->expressions = NULL;

  result->program = program;

  while (!match(state, TK_EOF)) {
    struct ExpressionNode *expression = malloc(sizeof(struct ExpressionNode));
    parseExpression(source, state, result, expression);
    appendExpressionToProgram(program, expression);
  }
}

void parse(char *source, struct ParseState *state, struct ParseResult *result) {
  // Set first token
  next(source, state);
  parseProgram(source, state, result);
}
