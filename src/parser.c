#include "parser.h"
#include <stdlib.h>
#include <string.h>

#define MAX_ARG_LENGTH 2048
#define MAX_NUM_ARGS 64

char** parse_command(char* command, size_t size)
{
    // allocate with fixed sizes for now
    char** args = malloc(MAX_NUM_ARGS * sizeof(char*));
    args[0] = malloc(MAX_ARG_LENGTH);
    size_t char_index = 0, arg_index = 0, char_in_arg_index = 0;
    while (char_index < size)
    {
        if (command[char_index] == ' ') {
            // terminate previous string
            args[arg_index][char_in_arg_index] = 0;
            // re-allocate the just-finished argument to its actual size
            size_t arg_size = char_in_arg_index + 1;
            // +1: counting terminating null char we just added
            args[arg_index] = realloc(args[arg_index], arg_size);
            // go to next arg index
            arg_index++;
            // allocate next arg
            args[arg_index] = malloc(MAX_ARG_LENGTH);
            // reset index of arg-char
            char_in_arg_index = 0;
        } else {
            args[arg_index][char_in_arg_index++] = command[char_index];
        }
        char_index++;
    }
    // add null char to last word
    args[arg_index][char_in_arg_index] = 0;
    // only one word
    if (arg_index == 0) {
        args[0] = realloc(args[0], size);
    }

    args[arg_index + 1] = NULL;
    args = realloc(args, (arg_index + 2) * sizeof(char*));

    return args;
}
