#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#include <stdio.h>

int	there_are_pipes(char **argv)
{
	int i;

	for (i = 0; argv[i] && *argv[i] != ';'; i++)
		if (*argv[i] == '|') return (i);
	return (-i);
}

char	**exec_with_pipe(int *r_pipe, char **argv, char **envp)
{
	int	pipex[2]; if (pipe(pipex) < 0) {write(STDERR_FILENO, "error: fatal\n", 13); exit(1);}
	
	pid_t	pid; pid = fork(); if (pid < 0) {write(STDERR_FILENO, "error: fatal\n", 13); exit(1);}
	if (!pid)
	{
		if (r_pipe > 0)
		{
			if (dup2(r_pipe, STDIN_FILENO) < 0) {write(STDERR_FILENO, "error: fatal\n", 13); exit(1);}
			close(r_pipe);
		}
		if (dup2(pipex[1], STDOUT_FILENO) < 0) {write(STDERR_FILENO, "error: fatal\n", 13); exit(1);}
		close (pipex[1]);
		close (pipex[0]);
		argv[there_are_pipes(argv)] = NULL;
		if (execve(*argv, argv, envp)) {write(STDERR_FILENO, "error: cannot execute ", 22), write(STDERR_FILENO, *argv, ft_strlen(*argv)); write(STDERR_FILENO, "\n", 1);}
	}
	if (there_are_pipes(argv[there_are_pipes(argv) + 1]) > 0)
		return (exec_with_pipe(pipex[0], &argv[there_are_pipes(argv) + 1], envp));
	
}

char	**take_command(char **argv, char **envp)
{
	if (there_are_pipes(argv) > 0) return ((exec_with_pipe)-1, argv, envp);
	else return (exec_wo_pipe(-1, argv, envp));
}

int	main(int argc, char **argv, char **envp)
{
	argv++;
	while (*argv)
		argv = take_command(argv, envp); // Esto devuelve lo que haya justo después de ';' o un puntero nulo si ya ha dejado de leer
	return (0);
}
