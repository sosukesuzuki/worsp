/*
<program> ::= <expression>*
<expression> ::= <literal> | <symbol> | <function-call> | <binary-operation> | <conditional> | <list> | <assignment>
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
<special-character> ::= "+" | "-" | "*" | "/" | "=" | "<" | ">" | "&" | "|" | "!" | "?" | "$" | "%" | "^" | "~"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    struct ExpressionNode *expression;
};

enum ExpressionType {
    EXP_LITERAL,
    EXP_FUNCTION_CALL,
    EXP_SYMBOL,
    EXP_BINARY_OPERATION,
    EXP_CONDITIONAL,
    EXP_ASSIGNMENT
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

enum LiteralType {
    LIT_INTERGER,
    LIT_STRING,
    LIT_BOOLEAN
};

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
    struct ExpressionNode *argument;
};

struct SymbolNode {
    char *symbol_name;
};

enum BinaryOperatorType {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
};

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

int match(struct ParseState *state, TokenKind kind) {
    return state->token->kind != kind ? 1 : 0;
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
        while(isalnum(source[state->pos])) {
            state->pos++;
        }
        int length = state->pos - start;

        char *str = malloc(length + 1);
        strncpy(str, &source[start], length);
        str[length] = '\0';

        if (strcmp(str, "if") == 0) {
            new->kind = TK_IF;
            free(str);
        } else if (strcmp(str, "set") == 0) {
            new->kind = TK_SET;
            free(str);
        } else if (strcmp(str, "true") == 0) {
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
        while(source[state->pos] != '"' && source[state->pos] != '\0') {
            state->pos++;
        }
        int length = state->pos - start;
        new->kind = TK_STRING;
        new->str = malloc(length + 1);
        strncpy(new->str, &source[start], length);
        new->str[length] = '\0';
        if (source[state->pos] == '"') {
            state->pos++;  // クオートをスキップ
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

void parseAssignmentExpression(char *source, struct ParseState *state, struct ParseResult *result) {
}

void parseExpression(char *source, struct ParseState *state, struct ParseResult *result) {
}

void parseProgram(char *source, struct ParseState *state, struct ParseResult *result) {
    struct ProgramNode *program = malloc(sizeof(struct ProgramNode));
    result->program = program;
    while (state->token->kind != TK_EOF) {
        parseExpression(source, state, result);
    }
}

void parse(char *source, struct ParseState *state, struct ParseResult *result) {
    // Set first token
    next(source, state);
    parseProgram(source, state, result);
}

int main() {
    char *source = "aa";

    struct ParseState state = (struct ParseState){NULL, 0};
    struct ParseResult result = (struct ParseResult){NULL};

    parse(source, &state, &result);

    return 0;
}
