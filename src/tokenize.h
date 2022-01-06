#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stddef.h>
#include <stdbool.h>

#define MAX_TOKEN_SIZE 2048
#define MAX_NUM_TOKENS 64

typedef enum token_type token_type;
enum token_type
{
    UNKNOWN,
    DQ_STRING, // ""
    SQ_STRING, // ''
    EVAL_STRING, // ``
    LITERAL, // literally a word, most often a command
    PARENTHESIZED, // (expr)
    BRACKETTED, // [expr]
    OPERATOR, // & && | || > < ; $ * ? # ~ = %
    OPERATOR_RESET,
};

typedef struct Token Token;
struct Token
{
    char *token; // has an implicit trailing null char
    size_t size; // does not include the trailing null char
    token_type type;
    size_t startpos;
    size_t endpos;
};

Token* tokenize(char* expr, size_t expr_len, size_t *num_tokens);

#endif
