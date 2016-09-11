#include "ft_select.h"

int	ft_putc(int c)
{
	int	ret;
	int	ttyfd;

	ttyfd = open("/dev/tty", O_RDWR);
	ret	= write(ttyfd, &c, 1);
	close (ttyfd);
	return (ret);
}

void	clr_screen(t_termcap *tcap)
{
	tputs(tcap->cl_string, 1, ft_putc);
}

void	ft_print(char *str, enum e_mode mode)
{
	int	ttyfd;

	ttyfd = open("/dev/tty", O_RDWR);
	if (mode == HOVER)
	{
		tputs(tgetstr("us", NULL), 1, ft_putc);
		ft_putstr_fd(str, ttyfd);
		tputs(tgetstr("ue", NULL), 1, ft_putc);
	}
	else if (mode == SELECT)
	{
		tputs(tgetstr("mr", NULL), 1, ft_putc);
		ft_putstr_fd(str, ttyfd);
		tputs(tgetstr("me", NULL), 1, ft_putc);
	}
	else if (mode == SELECT_HOVER)
	{
		tputs(tgetstr("mr", NULL), 1, ft_putc);
		tputs(tgetstr("us", NULL), 1, ft_putc);
		ft_putstr_fd(str, ttyfd);
		tputs(tgetstr("me", NULL), 1, ft_putc);
		tputs(tgetstr("ue", NULL), 1, ft_putc);
		tputs(tgetstr("me", NULL), 1, ft_putc);
	}
	else
		ft_putstr_fd(str, ttyfd);
	ft_putchar_fd('\n', ttyfd);
}

void	put_selected (t_list *begin)
{
	int		ttyfd;
	char	is_first;

	is_first = 1;
	ttyfd = open("/dev/tty", O_RDWR);
	while (begin)
	{
		if (((t_elem *)begin->content)->mode == SELECT
				|| ((t_elem *)begin->content)->mode == SELECT_HOVER)
		{
			if (!is_first)
				write (ttyfd, " ", 1);
			write (ttyfd, ((t_elem *)begin->content)->text,
					ft_strlen (((t_elem *)begin->content)->text));
			is_first = 0;
		}
		begin = begin->next;
	}
	close (ttyfd);
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

t_list	*fill_list(t_list **lst, char **argv)
{
	int		i;
	char	sw;
	t_elem	e;
	t_list	*previous;
	t_list	*end;
	t_list	*tmp;

	previous = NULL;
	end = NULL;
	tmp = NULL;
	e.mode = NORMAL;
	i = 0;
	sw = 0;
	while (argv[i])
		i++;
	while (--i > 0)
	{
		e.text = argv[i];
		e.previous = NULL;
		if (tmp != NULL)
			previous = tmp;
		ft_lstadd(lst, tmp = ft_lstnew((void *)&e, sizeof(t_elem)));
		if (sw == 0)
		{
			end = tmp;
			sw = 1;
		}
		if (previous != NULL)
			((t_elem *)(previous->content))->previous = tmp;
		previous = *lst;
	}
	return (end);
}

/*
** ve : return cursor to normal
*/
static void	ft_quit_menu(t_list *lst)
{
	t_list	*tmp;

	while (lst)
	{
		tmp = lst;
		lst = lst->next;
		free(tmp->content);
		tmp->content = NULL;
		free(tmp);
		tmp = NULL;
	}
	tputs(tgetstr("ve", NULL), 1, ft_putc);
}

static void	disp_menu(t_list *lst)
{
	while (lst)
	{
		ft_print(((t_elem *)(lst->content))->text,
			((t_elem *)(lst->content))->mode);
		lst = lst->next;
	}
}

static void	ft_move_right(t_list *begin, t_list **cur_elem, t_termcap *tcap)
{
	if (((t_elem *)((*cur_elem)->content))->mode == HOVER)
		((t_elem *)((*cur_elem)->content))->mode = NORMAL;
	else if (((t_elem *)((*cur_elem)->content))->mode == SELECT_HOVER)
		((t_elem *)((*cur_elem)->content))->mode = SELECT;
	if ((*cur_elem)->next != NULL)
		(*cur_elem) = (*cur_elem)->next;
	else
		(*cur_elem) = begin;
	if (((t_elem *)((*cur_elem)->content))->mode != SELECT)
		((t_elem *)((*cur_elem)->content))->mode = HOVER;
	else
		((t_elem *)((*cur_elem)->content))->mode = SELECT_HOVER;
	clr_screen(tcap);
	disp_menu(begin);
}

static void	ft_move_left(t_list *end, t_list *begin, t_list **cur_elem,
						t_termcap *tcap)
{
	if (((t_elem *)((*cur_elem)->content))->mode == HOVER)
		((t_elem *)((*cur_elem)->content))->mode = NORMAL;
	else if (((t_elem *)((*cur_elem)->content))->mode == SELECT_HOVER)
		((t_elem *)((*cur_elem)->content))->mode = SELECT;
	if (((t_elem *)((*cur_elem)->content))->previous != NULL)
		(*cur_elem) = ((t_elem *)((*cur_elem)->content))->previous;
	else
		(*cur_elem) = end;
	if (((t_elem *)((*cur_elem)->content))->mode != SELECT)
		((t_elem *)((*cur_elem)->content))->mode = HOVER;
	else
		((t_elem *)((*cur_elem)->content))->mode = SELECT_HOVER;
	clr_screen(tcap);
	disp_menu(begin);
}

static void	ft_get_user_input(t_list *begin, t_list *end, t_termcap *tcap)
{
	char	buf[3];
	t_list	*cur_elem;

	cur_elem = begin;
	while (1)
	{
		tputs(tgetstr("rs", NULL), 1, ft_putc);
		ft_bzero(buf, sizeof(buf));
		read(0, buf, sizeof(buf));
		if ((unsigned int)(buf[0]) == 27)
		{
			if (((unsigned int)buf[2]) == RIGHT_KEY)
				ft_move_right(begin, &cur_elem, tcap);
			else if (((unsigned int)buf[2]) == LEFT_KEY)
				ft_move_left(end, begin, &cur_elem, tcap);
			else if (buf[2] == 0)
			{
				ft_quit_menu(begin);
				break ;
			}
		}
		else if ((unsigned int)(buf[0]) == RET_KEY)
		{
			clr_screen(tcap);
			put_selected (begin);
			ft_quit_menu(begin);
			break ;
		}
		else if ((unsigned int)(buf[0]) == SPC_KEY)
		{
			((t_elem *)(cur_elem->content))->mode = 
				(((t_elem *)(cur_elem->content))->mode == SELECT_HOVER) ? HOVER : SELECT_HOVER;
			ft_move_right(begin, &cur_elem, tcap);
			clr_screen(tcap);
			disp_menu(begin);
		}
	}
}

/*
** vi: cursor visibility off
** ks: keypad start
*/
void	init_menu(t_termcap *tcap, char **argv, struct termios *term)
{
	t_list	*lst;
	t_list	*end;

	lst = NULL;
	end = fill_list(&lst, argv);
	clr_screen(tcap);
	((t_elem *)lst->content)->mode = HOVER;
	disp_menu(lst);
	(void)tcap;
	tputs(tgetstr("ks", NULL), 1, ft_putc);
	tputs(tgetstr("vi", NULL), 1, ft_putc);
	ft_get_user_input(lst, end, tcap);

	//Restore term
	if (tcgetattr(0, term) == -1)
	   exit(-1);
	term->c_lflag = (ICANON | ECHO);
	if (tcsetattr(0, 0, term) == -1)
	   exit(-1);
}

void	init_term(t_termcap *tcap, struct termios *term)
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
	if (tcgetattr(0, term) == -1)
		exit(-1);
	term->c_lflag &= ~(ICANON);
	term->c_lflag &= ~(ECHO);
	term->c_cc[VMIN] = 1;
	term->c_cc[VTIME] = 0;
	if (tcsetattr(0, 0, term) == -1)
	   exit(-1);
	set_tcap(tcap);
}

int		main(int argc, char **argv)
{
	t_termcap		tcap;
	struct termios	term;
	int				ttyfd;

	if (argc > 1)
	{
		ttyfd = open("/dev/tty", O_RDWR);
		if (isatty(ttyfd))
		{
			init_term(&tcap, &term);
			init_menu(&tcap, argv, &term);
		}
		else
			ft_putstr("Not a valid terminal type device.\n");
		close (ttyfd);
	}
	else
		ft_putstr("Usage: ./ft_select element1 element2 ...\n");
	return (0);
}
