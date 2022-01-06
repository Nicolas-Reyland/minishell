CFLAGS = -Wall -Wextra -g -fsanitize=address

minishell: src/strtools.o src/command.o src/parser.o src/prompt.o src/tokenize.o

clean:
	rm src/*.o minishell -f
