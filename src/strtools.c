#include "strtools.h"
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

ssize_t readline_stdin(char** line, size_t max_chars)
{
    // check for line buffer
    if (line == NULL)
        err(1, "line is NULL\n");
    if (*line == NULL)
        *line = malloc(max_chars);
    //
    char next_char;
    size_t count = 0;
    do
    {
        // read from stdin
        switch(read(STDIN_FILENO, &next_char, 1))
        {
            case 0:
                // control + d pressed
                write(STDOUT_FILENO, "\nExiting\n", 9);
                _exit(0);
            case -1:
                err(1, "read on stdin failed\n");
            default:
                break;
        }
        // check for return char
        if (next_char == '\n') {
            (*line)[count] = 0;
            return count;
        }
        // add char to buffer
        (*line)[count] = next_char;
    } while (count++ < max_chars);
    err(1, "line buffer size exceeded");
    return -1;
}

