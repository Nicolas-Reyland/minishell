#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "src/strtools.h"
#include "src/command.h"
#include "src/prompt.h"
#include "src/tokenize.h"

#define LINE_BUFFER_SIZE 4096

void init_signal_handlers();

int main()
{
    init_signal_handlers();
    int last_cmd_status = 0;
    greetings();
    while (1)
    {
        write_prompt(last_cmd_status);
        // read line
        char *line = NULL;
        size_t line_size = readline_stdin(&line, LINE_BUFFER_SIZE);
        // tokenize, parse and run
        size_t num_tokens = 0;
        Token* tokens = tokenize(line, line_size, &num_tokens);
        if (num_tokens == 0) {
            free(line);
            continue;
        }
        Command command = {
            .tokens = tokens,
            .num_tokens = num_tokens,
            .stdin_fd = STDIN_FILENO,
            .stdout_fd = STDOUT_FILENO,
            .background = false,
        };
        last_cmd_status = run_command(command);
        // free tokens, etc.
        for (size_t i = 0; i < num_tokens; i++)
            free(tokens[i].token);
        free(tokens);
        free(line);
    }
    return 1;
}

void minishell_exit(int exit_status)
{
    // TODO: add exit_status write to stdout
    write(STDOUT_FILENO, "\nKilled\n", 8);
    fsync(STDOUT_FILENO);
    _exit(exit_status);
}

void minishell_normal_exit(int exit_status)
{
    _exit(exit_status);
}

void init_signal_handlers()
{
    signal(SIGHUP, minishell_exit);
    signal(SIGINT, minishell_exit);
    signal(SIGQUIT, minishell_exit);
    signal(SIGABRT, minishell_exit);
    signal(SIGTERM, minishell_normal_exit);
}
