#include "ft_select.h"

void	init_term()
{
	char	*termtype;
	int		success;

	termtype = getenv ("TERM");
	if (termtype == 0)
		ft_error_exit("Specify a terminal type with `setenv TERM <yourtype>'.\n");
	success = tgetent(NULL, termtype);
	if (success < 0)
		ft_error_exit("Could not access the termcap data base.\n");
	else if (success == 0)
	{
		ft_printf("Terminal type `%s' is not defined.\n", termtype);
		exit(-1);
	}
}

int		main(int argc, char **argv)
{
	(void)argc;	
	(void)argv;

	if (isatty(1))
		init_term();
	else
		ft_error_exit("Not a valid terminal type device.");
	return (0);
}
