#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#define BANNER " --- Welcome to the Minishell !! ---\n"

void greetings(void)
{
    write(STDOUT_FILENO, BANNER, 37);
}

void write_prompt(int last_cmd_status)
{
    if (last_cmd_status != 0)
        write(STDOUT_FILENO, "X ", 2);
    char* pwd_str = getenv("PWD");
    size_t pwd_str_len = strlen(pwd_str);
    write(STDOUT_FILENO, pwd_str, pwd_str_len);
    char user_symbol;
    if (getuid() == 0)
        user_symbol = '#';
    else
        user_symbol = '$';
    write(STDOUT_FILENO, " ", 1);
    write(STDOUT_FILENO, &user_symbol, 1);
    write(STDOUT_FILENO, " ", 1);
}

