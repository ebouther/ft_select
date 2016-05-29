#include "ft_select.h"

int	ft_putc(int c)
{
	return (write(1, &c, 1));
}

inline void	clr_screen(t_termcap *tcap)
{
	tputs(tcap->cl_string, 1, ft_putc);
}

void	set_tcap(t_termcap *tcap)
{
	char	*tmp;

	tcap->height = tgetnum("li");
	tcap->width = tgetnum("co");
	tcap->cl_string = tgetstr ("cl", 0);
	tcap->cm_string = tgetstr ("cm", 0);
	tcap->auto_wrap = tgetflag ("am");
	tmp = tgetstr ("pc", 0);
	tcap->PC = tmp ? *tmp : 0;
	tcap->BC = tgetstr("le", 0);
	tcap->UP = tgetstr("up", 0);
}

void	fill_list(t_list **lst, char **argv)
{
	int		i;
	t_elem	e;
	
	e.mode = NORMAL;
	i = 0;
	while (argv[i])
		i++;
	while (--i > 0)
	{
		e.text = argv[i];
		ft_lstadd(lst, ft_lstnew((void *)&e, sizeof(t_elem)));
	}
}

void	disp_menu(t_termcap *tcap, char **argv)
{
	t_list	*lst;

	lst = NULL;
	fill_list(&lst, argv);
	while (lst)
	{
		
		lst = lst->next;
	}
	(void)tcap;
}

void	init_term(t_termcap *tcap)
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
	set_tcap(tcap);
}

int		main(int argc, char **argv)
{
	t_termcap	tcap;

	if (argc > 1)
	{
		if (isatty(1))
		{
			init_term(&tcap);
			disp_menu(&tcap, argv);
		}
		else
			ft_putstr("Not a valid terminal type device.\n");
	}
	else
		ft_putstr("Usage: ./ft_select element1 element2 ...\n");
	return (0);
}
