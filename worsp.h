#ifndef WORST_H
#define WORST_H

typedef enum {
  TK_RESERVED,
  TK_DIGIT,
  TK_LETTER,
  TK_LPAREN, // (
  TK_RPAREN, // )
  TK_SYMBOL,
  TK_STRING,
  TK_IF,
  TK_SET,
  TK_TRUE,
  TK_FALSE,
  TK_EOF
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
  EXP_FUNCTION_CALL,
  EXP_SYMBOL,
  EXP_BINARY_OPERATION,
  EXP_CONDITIONAL,
  EXP_ASSIGNMENT
};

struct ExpressionList {
  struct ExpressionNode *expression;
  struct ExpressionList *next;
};

struct ExpressionNode {
  enum ExpressionType type;
  union {
    struct LiteralNode *literal;
    struct FunctionCallNode *function_call;
    struct SymbolNode *symbol;
    struct BinaryOperationNode *binary_operation;
    struct ConditionalNode *conditional;
    struct AssignmentNode *assignment;
  } data;
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

struct FunctionCallNode {
  struct ExpressionNode *function;
  struct ExpressionList *argument;
};

struct SymbolNode {
  char *symbol_name;
};

enum BinaryOperatorType { OP_ADD, OP_SUB, OP_MUL, OP_DIV };

struct BinaryOperationNode {
  enum BinaryOperatorType type;
  struct ExpressionNode *left;
  struct ExpressionNode *right;
};

struct ConditionalNode {
  struct ExpressionNode *condition;
  struct ExpressionNode *then_expression;
  struct ExpressionNode *else_expression;
};

struct AssignmentNode {
  struct SymbolNode *symbol;
  struct ExpressionNode *expression;
};

struct ParseResult {
  struct ProgramNode *program;
};

int match(struct ParseState *state, TokenKind kind);
void next(char *source, struct ParseState *state);
void parse(char *source, struct ParseState *state, struct ParseResult *result);

#endif
