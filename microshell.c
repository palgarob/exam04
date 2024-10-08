#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

static int return_value;

int	find_pipe(char **argv)
{
	int i;

	for (i = 0; argv[i] && strcmp(";", argv[i]); i++)
		if (!strcmp("|", argv[i])) return (i);
	return (-i);
}

size_t	ft_strlen(char *str)
{
	size_t i = 0;

	while (str[i]) i++;
	return (i);
}

char	**exec_last(int *last_pipe, char **argv, char **envp)
{
	pid_t	pid; pid = fork(); if (pid < 0) {write(STDERR_FILENO, "error: fatal\n", 13); exit(1);}
	if (!pid)
	{
		if (last_pipe)
		{
			if (dup2(last_pipe[0], STDIN_FILENO) < 0) {write(STDERR_FILENO, "error: fatal\n", 13); exit(1);}
			close(last_pipe[1]);
			close(last_pipe[0]);
		}
		argv[-find_pipe(argv)] = NULL;
		if (execve(*argv, argv, envp)) {write(STDERR_FILENO, "error: cannot execute ", 23), write(STDERR_FILENO, *argv, ft_strlen(*argv)); write(STDERR_FILENO, "\n", 1);}
	}
	if (last_pipe) {close(last_pipe[1]); close(last_pipe[0]);}
	waitpid(pid, &return_value, 0);
	return (&argv[-find_pipe(argv)]);
}

char	**exec_pipe(int *last_pipe, char **argv, char **envp)
{
	int	pipex[2]; if (pipe(pipex) < 0) {write(STDERR_FILENO, "error: fatal\n", 13); exit(1);}
	
	pid_t	pid; pid = fork(); if (pid < 0) {write(STDERR_FILENO, "error: fatal\n", 13); exit(1);}
	if (!pid)
	{
		if (last_pipe)
		{
			if (dup2(last_pipe[0], STDIN_FILENO) < 0) {write(STDERR_FILENO, "error: fatal\n", 13); exit(1);}
			close(last_pipe[0]);
			close(last_pipe[1]);
		}
		if (dup2(pipex[1], STDOUT_FILENO) < 0) {write(STDERR_FILENO, "error: fatal\n", 13); exit(1);}
		close(pipex[1]);
		close(pipex[0]);
		argv[find_pipe(argv)] = NULL;
		if (execve(*argv, argv, envp)) {write(STDERR_FILENO, "error: cannot execute ", 22), write(STDERR_FILENO, *argv, ft_strlen(*argv)); write(STDERR_FILENO, "\n", 1);}
	}
	if (last_pipe) {close(last_pipe[1]); close(last_pipe[0]);}
	if (find_pipe(&argv[find_pipe(argv) + 1]) > 0)
		return (exec_pipe(pipex, &argv[find_pipe(argv) + 1], envp));
	else
		return (exec_last(pipex, &argv[find_pipe(argv) + 1], envp));
	
}

char	**exec_builtin(char **argv)
{
	if (find_pipe(argv) != -2) {write(STDERR_FILENO, "error: cd: bad arguments\n", 25); return (&argv[-find_pipe(argv)]);}
	if (chdir(argv[1]))
	{
		write(STDERR_FILENO, "error: cd: cannot change directory to ", 38);
		write(STDERR_FILENO, argv[1], ft_strlen(argv[1]));
		write(STDERR_FILENO, "\n", 1);
	}
	return (&argv[-find_pipe(argv)]);
}

char	**take_command(char **argv, char **envp)
{
	if (!strcmp("cd", argv[0])) return (exec_builtin(argv));
	if (find_pipe(argv) > 0) return (exec_pipe(NULL, argv, envp));
	else return (exec_last(NULL, argv, envp));
}

int	main(int argc, char **argv, char **envp)
{
	argv++;
	while (*argv)
	{
		if (!strcmp(";", *argv)) argv++;
		argv = take_command(argv, envp);
	}
	return (return_value);
}
