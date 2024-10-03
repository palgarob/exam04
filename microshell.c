#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

#include <stdio.h>

char	**take_command(char **argv)
{

	int	i;
	for (i = 1; *argv[i] != ';'; i++)
	{
		if (*argv[i] != '|')
		{
			char	**split = ft_split(argv[i]);
		}
		else

	}
}

int	main(int argc, char **argv)
{
	argv++;
	while (argv)
		argv = take_command(argv); // Esto devuelve lo que haya justo después de ';' o un puntero nulo si ya ha dejado de leer
	return(0);
}
