/*

<program> ::= <expression>
           | <expression> <program>

<expression> ::= <atom>
             | <list>

<atom> ::= <symbol>
        | <number>
        | <string>

<symbol> ::= <letter> <symbol_chars>

<number> ::= <integer>
         | <float>

<integer> ::= <digit>+
<float> ::= <digit>+ "." <digit>+

<list> ::= "(" <expression>* ")"

<letter> ::= "A" | "B" | ... | "Z" | "a" | "b" | ... | "z"
<digit> ::= "0" | "1" | ... | "9"
<symbol_chars> ::= <letter> | <digit> | "+" | "-" | "*" | "/" | "=" | ...
<string> ::= '"' <string_characters> '"'
<string_characters> ::= <string_character>*
<string_character> ::= <any_character_except_double_quote>
                   | '\"'  ; ダブルクォートのエスケープ
                   | '\\'  ; バックスラッシュのエスケープ

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

struct ParseResult {};

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
        new->kind = TK_SYMBOL;
        new->str = malloc(length + 1);
        strncpy(new->str, &source[start], length);
        new->str[length] = '\0';
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

void parse(char *source, struct ParseState *state, struct ParseResult *result) {
    next(source, state);
    printf("%s\n", state->token->str);
    next(source, state);
    printf("%d\n", state->token->val);
    next(source, state);
    printf("%s\n", state->token->str);
    next(source, state);
    printf("%s\n", state->token->str);
    next(source, state);
    printf("%s\n", state->token->str);
    next(source, state);
    printf("%s\n", state->token->str);
    next(source, state);
    printf("%s\n", state->token->str);
    next(source, state);
    printf("%s\n", state->token->str);
    next(source, state);
    printf("%s\n", state->token->str);
}

int main() {
    char *source = "(   33 i(adsfl) i \"foo\"  )";

    struct ParseState state = (struct ParseState){NULL, 0};
    struct ParseResult result;

    parse(source, &state, &result);

    return 0;
}
