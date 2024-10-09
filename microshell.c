#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>

static int return_value;

int	find_pipe(char **argv)
{
	int i;

	for (i = 0; argv[i] && strcmp(";", argv[i]); i++)
		if (!strcmp("|", argv[i])) return (i);
	return (-i);
}

void	print_err(char *str)
{
	size_t i = 0;

	while (str[i]) i++;
	write(STDERR_FILENO, str, i);
}

char	**exec_last(int *last_pipe, char **argv, char **envp)
{
	pid_t	pid; pid = fork(); if (pid < 0) {print_err("error: fatal\n"); exit(1);}
	if (!pid)
	{
		if (last_pipe)
		{
			if (dup2(last_pipe[0], STDIN_FILENO) < 0) {print_err("erxror: fatal\n"); exit(1);}
			close(last_pipe[1]);
			close(last_pipe[0]);
		}
		argv[-find_pipe(argv)] = NULL;
		if (execve(*argv, argv, envp)) {print_err("error: cannot execute "); print_err(*argv); print_err("\n"); exit(1);}
	}
	if (last_pipe) {close(last_pipe[1]); close(last_pipe[0]);}
	waitpid(pid, &return_value, 0);
	return (&argv[-find_pipe(argv)]);
}

char	**exec_pipe(int *last_pipe, char **argv, char **envp)
{
	int	pipex[2]; if (pipe(pipex) < 0) {print_err("error: fatal\n"); exit(1);}

	pid_t	pid; pid = fork(); if (pid < 0) {print_err("error: fatal\n"); exit(1);}
	if (!pid)
	{
		if (last_pipe)
		{
			if (dup2(last_pipe[0], STDIN_FILENO) < 0) {print_err("error: fatal\n"); exit(1);}
			close(last_pipe[0]);
			close(last_pipe[1]);
		}
		if (dup2(pipex[1], STDOUT_FILENO) < 0) {print_err("error: fatal\n"); exit(1);}
		close(pipex[1]); close(pipex[0]);
		argv[find_pipe(argv)] = NULL;
		if (execve(*argv, argv, envp)) {print_err("error: cannot execute "), print_err(*argv); print_err("\n"); exit(1);}
	}
	if (last_pipe) {close(last_pipe[1]); close(last_pipe[0]);}
	if (find_pipe(&argv[find_pipe(argv) + 1]) > 0)
		return (exec_pipe(pipex, &argv[find_pipe(argv) + 1], envp));
	else
		return (exec_last(pipex, &argv[find_pipe(argv) + 1], envp));
	
}

char	**exec_builtin(char **argv)
{
	if (find_pipe(argv) != -2) {print_err("error: cd: bad arguments\n"); return (&argv[-find_pipe(argv)]);}
	if (chdir(argv[1])) {print_err("error: cd: cannot change directory to "); print_err(argv[1]); print_err("\n");}
	return (&argv[-find_pipe(argv)]);
}

char	**take_command(char **argv, char **envp)
{
	if (!strcmp("cd", argv[0])) return (exec_builtin(argv));
	if (find_pipe(argv) > 0) return (exec_pipe(NULL, argv, envp));
	else return (exec_last(NULL, argv, envp));
}

int	main(__attribute__((unused)) int argc, char **argv, char **envp)
{ 
	argv++;
	while (*argv)
	{
		while (!strcmp(";", *argv)) {
			argv++;
			if (!*argv) break;
		}
		if (!*argv) break;
		argv = take_command(argv, envp);
	}
	return (return_value);
}
