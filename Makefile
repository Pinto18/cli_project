ms: minishell.c minishell.h user.h
	g++  -lreadline -DDEBUG -o ms  minishell.c

clean:
	rm -f ms
