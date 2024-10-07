#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#include <stdio.h>

int	there_are_pipes(char **argv)
{
	int i = 0;

	for (i = 0; argv[i] && *argv[i] != ';'; i++)
		if (*argv[i] == '|') return (i);
	return (-i);
}

size_t	ft_strlen(char *str)
{
	size_t i = 0;

	while (str[i]) i++;
	return (i);
}

char	**exec_wo_pipe(int *last_pipe, char **argv, char **envp)
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
		argv[-there_are_pipes(argv)] = NULL;
		if (execve(*argv, argv, envp)) {write(STDERR_FILENO, "error: cannot execute ", 23), write(STDERR_FILENO, *argv, ft_strlen(*argv)); write(STDERR_FILENO, "\n", 1);}
	}
	if (last_pipe) {close(last_pipe[1]); close(last_pipe[0]);}
	waitpid(pid, NULL, 0);
	if (argv[-there_are_pipes(argv)])
		return (&argv[-there_are_pipes(argv) + 1]);
	else
		return (&argv[-there_are_pipes(argv)]);
}

char	**exec_with_pipe(int *last_pipe, char **argv, char **envp)
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
		argv[there_are_pipes(argv)] = NULL;
		if (execve(*argv, argv, envp)) {write(STDERR_FILENO, "error: cannot execute ", 22), write(STDERR_FILENO, *argv, ft_strlen(*argv)); write(STDERR_FILENO, "\n", 1);}
	}
	if (last_pipe) {close(last_pipe[1]); close(last_pipe[0]);}
	if (there_are_pipes(&argv[there_are_pipes(argv) + 1]) > 0)
		return (exec_with_pipe(pipex, &argv[there_are_pipes(argv) + 1], envp));
	else
		return (exec_wo_pipe(pipex, &argv[there_are_pipes(argv) + 1], envp));
	
}

char	**exec_builtin(char **argv)
{
	if (there_are_pipes(argv) != -2) write(STDERR_FILENO, "error: cd: bad arguments\n", 25);
	if (chdir(argv[1]))
	{
		write(STDERR_FILENO, "error: cd: cannot change directory to ", 38);
		write(STDERR_FILENO, argv[1], ft_strlen(argv[1]));
		write(STDERR_FILENO, "\n", 1);
	}
	if (argv[-there_are_pipes(argv)])
		return (&argv[-there_are_pipes(argv) + 1]);
	else
		return (&argv[-there_are_pipes(argv)]);
}

char	**take_command(char **argv, char **envp)
{
	if (!strcmp("cd", argv[0])) return (exec_builtin(argv));
	if (there_are_pipes(argv) > 0) return (exec_with_pipe(NULL, argv, envp));
	else return (exec_wo_pipe(NULL, argv, envp));
}

int	main(int argc, char **argv, char **envp)
{
	argv++;
	while (*argv)
		argv = take_command(argv, envp); // Esto devuelve lo que haya justo después de ';' o un puntero nulo si ya ha dejado de leer
	return (0);
}
