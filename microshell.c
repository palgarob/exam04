#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

static int return_value;

int	check_pipe(char **argv)
{
	int	i;
	for (i = 0; argv[i] && strcmp(argv[i], ";"); i++) if (strcmp(*argv, "|")) return (i);
	return (-i);
}

void	p_error(char *str)
{
	size_t	i = 0;
	while (str[i]) i++;
	write(STDERR_FILENO, str, i);
}

char	**exec_builtin(char **argv)
{
	if (check_pipe(argv) != -2) {p_error("error: cd: bad arguments\n"); return (argv + -check_pipe(argv));}
	if (chdir(argv[1])) {p_error("error: cd: cannot change directory to "); p_error(argv[1]); p_error("\n");}
	return (argv + 2);	
}

char	**exec_last(char **argv, char** envp, int *last_pipe)
{
	pid_t	pid = fork(); if (pid < 0) {p_error("error: fatal\n"); exit(1);}

	if (!pid)
	{
		if (last_pipe)
		{
			if (dup2(last_pipe[0], STDIN_FILENO) < 0) {p_error("error: fatal\n"); exit(1);}
			close(last_pipe[0]); close(last_pipe[1]);
		}
		argv[-check_pipe(argv)] = NULL;
		if (execve(*argv, argv, envp)) {p_error("error: cannot execute "); p_error(*argv); p_error("\n"); exit(1);}
	}
	if (last_pipe) {close(last_pipe[0]); close(last_pipe[1]);}
	waitpid(pid, &return_value, 0);
	return (argv + -check_pipe(argv));
}

char	**exec_pipe(char **argv, char **envp, int *last_pipe)
{
	int	pipex[2]; if (pipe(pipex) < 0) {p_error("error: fatal\n"); exit(1);}
	pid_t	pid = fork(); if (pid < 0) {p_error("error: fatal\n"); exit(1);}
	if (!pid)
	{
		if (last_pipe)
		{
			if (dup2(last_pipe[0], STDIN_FILENO) < 0) {p_error("error: fatal\n"); exit(1);}
			close(last_pipe[0]); close(last_pipe[1]);
		}
		if (dup2(pipex[1], STDOUT_FILENO) < 0) {p_error("error: fatal\n"), exit(1);}
		argv[check_pipe(argv)] = NULL;
		close(pipex[0]); close(pipex[1]);
		if (execve(*argv, argv, envp)) {p_error("error: cannot execute "); p_error(*argv); p_error("\n"); exit(1);}
	}
	if (last_pipe) {close(last_pipe[0]); close(last_pipe[1]);}
	if (check_pipe(argv + check_pipe(argv) + 1) > 0) return (exec_pipe(argv + check_pipe(argv) + 1, envp, pipex));
	else return (exec_last(argv + check_pipe(argv) + 1, envp, pipex));
}

char	**enter(char **argv, char ** envp)
{
	if (!strcmp(*argv, "cd")) return (exec_builtin(argv));
	if (check_pipe(argv) > 0) return (exec_pipe(argv, envp, NULL));
	else return (exec_last(argv, envp, NULL));
}

int	main(__attribute__((unused)) int argc, char **argv, char **envp)
{
	argv++;
	while (*argv)
	{
		while (!strcmp(*argv, ";")) {argv++; if (!*argv) break;}
		if (!*argv) break;
		argv = enter(argv, envp);
	}
	return (return_value);
}

