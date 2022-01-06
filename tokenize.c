#include "tokenize.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <err.h>

const char token_type_str[][32] = {
    [UNKNOWN] = "UNKNOWN",
    [DQ_STRING] = "DQ_STRING",
    [SQ_STRING] = "SQ_STRING",
    [EVAL_STRING] = "EVAL_STRING",
    [LITERAL] = "LITERAL",
    [PARENTHESIZED] = "PARENTHESIZED",
    [BRACKETTED] = "BRACKETTED",
    [OPERATOR] = "OPERATOR",
    [OPERATOR_RESET] = "OPERATOR_RESET",
};

static const char DELIMITERS[] = {
    ' ', '\t',
};

static const char ENCAPSULATORS[] = {
    '(', ')', '[', ']', '\'', '"', '`',
};

static const char OPERATORS[] = {
    '&', '|', '>', '<', ';', '$', '*', '?', '#', '=', '~', '%',
};

static char
encapsulation_pair(char c)
{
    switch (c) {
        // parentheses
        case '(': return ')';
        case ')': return '(';
        // brackets []
        case '[': return ']';
        case ']': return '[';
        // string-like encapsulators
        case '\'': case '"': case '`':
            return c;
        // unknown char
        default:
            err(1, "Parse error: unknown delimiter pair of '%c'\n", c);
    }
}

static bool
char_in_group(char c, const char group[], size_t group_size)
{
    for (size_t i = 0; i < group_size; i++)
        if (c == group[i])
            return true;
    return false;
}

static size_t
end_of_encapsulated_expr_index(char *str, size_t cur_index, size_t length,
    char delimiter, char token_str_buffer[])
{
    char alter = encapsulation_pair(delimiter);
    short depth = 1;
    token_str_buffer[0] = str[cur_index];
    size_t start_index = cur_index;
    cur_index++;
    bool escape_next = false;
    while (depth != 0 && cur_index < length)
    {
        char c = str[cur_index];
        token_str_buffer[cur_index - start_index] = c;
        // escape next char
        if (escape_next) {
            escape_next = false;
            cur_index++;
            continue;
        }

        if (c == alter) {
            depth--;
        } else if (c == delimiter) {
            depth++;
        }
        cur_index++;
    }
    assert(depth == 0);
    return cur_index;
}

token_type
token_type_from_delim(char c)
{
    switch (c) {
        case  '(': case ')': return PARENTHESIZED;
        case  '[': case ']': return BRACKETTED;
        case '\'': return SQ_STRING;
        case  '"': return DQ_STRING;
        case  '`': return EVAL_STRING;
        default: err(1, "Unknown string-like delimiter '%c'\n", c);
    }
}

static void
assert_opening_encaps(char c)
{
    switch (c) {
        case '(': case '[': case '"': case '\'': case '`': return;
        default: err(1, "Not an opening encapsulator '%c'\n", c);
    }
}

static void
add_token(Token* tokens, size_t *token_index, const char token_str_buffer[],
    size_t *token_str_index, token_type *current_type,
    size_t *startpos, size_t endpos)
{
    // setup next token
    char *token_str = malloc(*token_str_index + 1);
    // copy the stack-allocated buffer into the heap
    strncpy(token_str, token_str_buffer, *token_str_index);
    token_str[*token_str_index] = 0;
    // create new struct
    tokens[(*token_index)++] = (Token) {
        .token = token_str,
        .size = *token_str_index,
        .type = *current_type,
        .startpos = *startpos,
        .endpos = endpos,
    };
    // verbose
    printf("Added token: str = %s, size = %lu, type = %s, start = %lu, end = %lu\n",
        token_str,
        *token_str_index,
        token_type_str[*current_type],
        *startpos,
        endpos
    );
    // setup vars for next token
    *current_type = UNKNOWN;
    *token_str_index = 0;
    *startpos = endpos;
}

Token*
tokenize(char *expr, size_t expr_len, size_t *num_tokens)
{
    size_t char_index = 0; // index of character in str
    bool escape_next = false; // next char should be escaped
    token_type current_type = UNKNOWN;
    char token_str_buffer[MAX_TOKEN_SIZE];
    Token *tokens = malloc(MAX_NUM_TOKENS * sizeof(struct Token));
    size_t token_str_index = 0, // index related to token_str_buffer
           token_index = 0, // index related to tokens
           startpos = 0; // start position of current token
    // Parsing loop
    while (char_index < expr_len)
    {
        char c = expr[char_index];
        // backslash
        if (c == '\\') {
            // incompatible token_types
            if (current_type == OPERATOR) {
                add_token(
                    tokens,
                    &token_index,
                    token_str_buffer,
                    &token_str_index,
                    &current_type,
                    &startpos,
                    char_index
                );
                goto EndOfParseLoop;
            }
            // backslash escaped ?
            if (escape_next) {
                escape_next = false;
                goto AddCurrentChar;
            }
            escape_next = true;
            goto NextTokenChar;
        }
        // whitespace delimiter
        else if (char_in_group(c, DELIMITERS, sizeof(DELIMITERS))) {
            // escaped char
            if (escape_next) {
                escape_next = false;
                goto AddCurrentChar;
            }

            // useless whitespace
            //printf("wtf? %c\n", expr[char_index-1]);
            if (token_str_index == 0) {
                startpos++;
                goto EndOfParseLoop;
            }

            // whitespace between two words
            add_token(
                tokens,
                &token_index,
                token_str_buffer,
                &token_str_index,
                &current_type,
                &startpos,
                char_index
            );
            // we are in a whitespace, so incr startpos
            startpos++;
            goto EndOfParseLoop;
        }
        // encapsulator
        else if (char_in_group(c, ENCAPSULATORS, sizeof(ENCAPSULATORS))) {
            // escaped encapsulator (e.g. "\(")
            if (escape_next) {
                escape_next = false;
                goto AddCurrentChar;
            }

            // incompatible token types
            if (current_type == OPERATOR || current_type == LITERAL) {
                add_token(
                    tokens,
                    &token_index,
                    token_str_buffer,
                    &token_str_index,
                    &current_type,
                    &startpos,
                    char_index
                );
            }

            assert_opening_encaps(c);
            // get index of end of encapsulated expr
            current_type = token_type_from_delim(c);
            size_t old_index = char_index;
            char_index = end_of_encapsulated_expr_index(
                expr,
                char_index,
                expr_len,
                c,
                token_str_buffer
            );
            token_str_index = char_index - old_index;
            add_token(
                tokens,
                &token_index,
                token_str_buffer,
                &token_str_index,
                &current_type,
                &startpos,
                char_index
            );
            // correct char_index value has already been set.
            // should not increment further
            continue;
        }
        // operator
        else if (char_in_group(c, OPERATORS, sizeof(OPERATORS))) {
            // escaped operator
            if (escape_next) {
                escape_next = false;
                goto AddCurrentChar;
            }
            // already in foreign token
            if (current_type != OPERATOR && token_str_index != 0) {
                // adding this token
                add_token(
                    tokens,
                    &token_index,
                    token_str_buffer,
                    &token_str_index,
                    &current_type,
                    &startpos,
                    char_index
                );
                // reset loop to current
                continue;
            }

            // set correct type
            current_type = OPERATOR;
            // empty buffer or double-operator
            if (token_str_index == 0 || (token_str_index == 1 && token_str_buffer[0] == c))
                goto AddCurrentChar;
            // side-by-side operators ?
            if (token_str_buffer[0] != c) {
                add_token(
                    tokens,
                    &token_index,
                    token_str_buffer,
                    &token_str_index,
                    &current_type,
                    &startpos,
                    char_index
                );
                continue; // yes, continue. we reset the loop
            }
            // there are three times the same operator side-by-side (ILLEGAL)
            err(1, "Parsing error. 3 same consecutive operators: '%c':%lu\n", c, char_index + 1);
        }
        // literal
        if (current_type == UNKNOWN)
            current_type = LITERAL;
        else if (current_type == OPERATOR) {
            add_token(
                tokens,
                &token_index,
                token_str_buffer,
                &token_str_index,
                &current_type,
                &startpos,
                char_index
            );
        }

        // add current char
        AddCurrentChar:
        token_str_buffer[token_str_index] = c;

        // increment token_str_buffer index
        NextTokenChar:
        token_str_index++;

        // end of parse-loop. increment char index
        EndOfParseLoop:
        char_index++;
    }
    // end of last token
    if (token_str_index != 0) {
        add_token(
            tokens,
            &token_index,
            token_str_buffer,
            &token_str_index,
            &current_type,
            &startpos,
            char_index
        );
    }

    // de-alloc unused memory through re-allocating the tokens
    tokens = realloc(tokens, token_index * sizeof(struct Token));
    *num_tokens = token_index;
    return tokens;
}
