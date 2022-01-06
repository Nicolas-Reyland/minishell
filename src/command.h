#ifndef COMMAND_H
#define COMMAND_H

#include <stddef.h>
#include <stdbool.h>
#include "tokenize.h"

typedef struct Command Command;
struct Command
{
    Token* tokens;
    size_t num_tokens;
    int stdin_fd;
    int stdout_fd;
    bool background;
};

int run_command(Command command);

#endif
