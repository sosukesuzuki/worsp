#ifndef WORST_H
#define WORST_H

// =================================================
//   tokenizer & parser
// =================================================

typedef enum {
  TK_DIGIT,
  TK_LPAREN, // (
  TK_RPAREN, // )
  TK_SYMBOL,
  TK_STRING,
  TK_TRUE,
  TK_FALSE,
  TK_EOF,
  TK_QUOTE, // '
} TokenKind;

struct Token {
  TokenKind kind;
  struct Token *next;
  int val;
  char *str;
};

struct ParseState {
  struct Token *token;
  int pos;
};

struct ProgramNode {
  struct ExpressionList *expressions;
};

enum ExpressionType {
  EXP_LITERAL,
  EXP_SYMBOL,
  EXP_LIST,
  EXP_SYMBOLIC_EXP,
};

struct ExpressionList {
  struct ExpressionNode *expression;
  struct ExpressionList *next;
};

struct ExpressionNode {
  enum ExpressionType type;
  union {
    struct SymbolicExpNode *symbolic_exp;
    struct ListNode *list;
    struct LiteralNode *literal;
    struct SymbolNode *symbol;
  } data;
};

struct SymbolicExpNode {
  struct ExpressionList *expressions;
};

struct ListNode {
  struct ExpressionList *expressions;
};

enum LiteralType { LIT_INTERGER, LIT_STRING, LIT_BOOLEAN };

struct LiteralNode {
  enum LiteralType type;
  union {
    int int_value;
    int boolean_value;
    char *string_value;
  };
};

struct SymbolNode {
  char *symbol_name;
};

struct ParseResult {
  struct ProgramNode *program;
};

int match(struct ParseState *state, TokenKind kind);
void next(char *source, struct ParseState *state);
void parse(char *source, struct ParseState *state, struct ParseResult *result);

#endif
