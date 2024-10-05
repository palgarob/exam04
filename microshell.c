#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#include <stdio.h>

typedef struct s_pipex
{
	int				pipe_end[2];
	struct s_pipex	*next;
}	t_pipex;

void	create_pipe(t_pipex *pipex)
{
	while (pipex->next != NULL) pipex = pipex->next;
	if (pipe(pipex->pipe_end < 0))
	{
		write(STDERR_FILENO, "error: fatal\n", 13);
		exit(1);
	}
}

void	redirect(t_pipex *pipex)
{
	while (pipex->next != NULL) pipex = pipex->next;

}

void	exec_fork(char **argv, t_pipex *pipex, char **envp)
{
	pid_t	pid;
	char	*arguments;

	pid = fork();
	if (pid < 0)
	{
		write(STDERR_FILENO, "error: fatal\n", 13);
		exit(1);
	}
	if (!pid)
	{
		redirect(pipex);
		arguments = argv + init;
		arguments[n + 1] = NULL;
		if (execve(arguments[init], arguments, envp))
		{
			write(2, "error: cannot execute ", 22);
			write(2, *arguments, 9);
			write(2, "\n", 1);
			exit(1);
		}
	}
}

void	append_pipe_struct(t_pipex *pipex)
{
	while (pipex->next != NULL) pipex = pipex->next;
	pipex->next = (t_pipex *)malloc(sizeof(t_pipex));
	pipex->next->next = NULL;
}

void	free_arguments(char **arguments)
{
	for (int i = 0; arguments[i]; i++)
		free(arguments[i]);
}

size_t	ft_strlen(char *str)
{

}

void	free_pipex(t_pipex  pipex)
{

}

char	**otro_rollo(char **argv, char **envp)
{
	int i;
	t_pipex *pipex;
	char	*buf;

	pipex = NULL;
	for (i = 0; *argv[i] != ';' && argv[i]; i++)
	{
		if (*argv[i] == '|')
		{
			pipex = (t_pipex *)malloc(sizeof(pipex));
			pipex->next = NULL;
			buf = argv[i];
			argv[i] = NULL;
			exec_fork()
		}
	}
}

char	**take_command(char **argv, char **envp)
{
	int		i;

	for (i = 0; *argv[i] != ';' && argv[i]; i++)
	{
		if (*argv[i] == '|')
			return (otro_rollo(argv, envp));
	}
	if (execve(*argv, argv, envp))
	{
		write(2, "error: cannot execute ", 22);
		write(2, *argv, ft_strlen(*argv));
		write(2, "\n", 1);
		exit(1);
	}
	argv[i] = buf;
	return (argv + i);
}

int	main(int argc, char **argv, char **envp)
{
	argv++;
	while (*argv)
		argv = take_command(argv, envp); // Esto devuelve lo que haya justo después de ';' o un puntero nulo si ya ha dejado de leer
	return (0);
}
