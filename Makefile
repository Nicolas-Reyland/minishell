CFLAGS = -Wall -Wextra -g -fsanitize=address

minishell: strtools.o command.o parser.o prompt.o tokenize.o

clean:
	rm *.o minishell -f
