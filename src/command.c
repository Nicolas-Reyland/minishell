#include "command.h"
#include "parser.h"
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static char**
command_args(Command command)
{
    char **args = malloc(command.num_tokens * sizeof(char*));
    for (size_t i = 0; i < command.num_tokens; i++)
        args[i] = command.tokens[i].token;
    return args;
}

int
run_command(Command command)
{
    // TODO: implement redirects, pipes, etc.
    assert(command.num_tokens != 0);
    // parse command
    char** args = command_args(command);
    // fork & exec flow
    int child_pid = fork();
    switch(child_pid)
    {
        case -1:
            err(1, "Could not create a child process");
        case 0:
            int exec_status = execvp(args[0], args);
            err(1, "Command failed: %d", exec_status);
        default:
            int wstatus;
            waitpid(child_pid, &wstatus, 0);
            return wstatus;
    }
}
