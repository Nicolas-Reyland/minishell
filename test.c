#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "tokenize.h"

int main(int argc, char** argv)
{
    assert(argc == 2);
    char *expr = argv[1];
    size_t expr_len = strlen(argv[1]),
           num_tokens = 0;
    Token* tokens = tokenize(expr, expr_len, &num_tokens);

    printf("Num tokens: %lu\n", num_tokens);
    for (size_t i = 0; i < num_tokens; i++) {
        printf("Token %lu: |%s|\n", i, tokens[i].token);
    }

    return 0;
}