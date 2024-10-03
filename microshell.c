#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#include <stdio.h>

void	execute(char **args)
{
	while (*args)
	{
		write(1, *args, strlen(*args));
		write(1, " - ", 3);
		args++;
	}
}

char	***parse(char **argv)
{
	char	***parsed_args;

	int		sc_n = 1;
	int		i = 1;
	while (argv[i])
	{
		if (*argv[i] == ';' && argv[i + 1] != 0)
			sc_n++;
		i++;
	}
	parsed_args = (char ***)malloc(sizeof(char **) * (sc_n + 1));

	int j = 0;
	i = 1;
	while (argv[i])
	{
		while (*argv[i] != ';')
		{
			parsed_args[j] = &argv[i];
		}
		i++;
	}
	printf("%p\n", argv[i]);
	parsed_args[j] = 0;
	return (parsed_args);
}

int	main(int argc, char **argv)
{
	char	***parsed_args;
	int		i = 1;

	if (argc > 1)
	{
		parsed_args = parse(argv); // Esto debería separar según ;
		while (parsed_args[i])
		{
			execute(parsed_args[i]);
			write(1, "\n", 1);
			i++;
		}
	}
	return(0);
}
