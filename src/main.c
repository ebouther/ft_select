#include "ft_select.h"

#include <stdio.h>
t_termcap	*tcap(void)
{
    static t_termcap	tcap;
    return (&tcap);
}

t_list	**lst(void)
{
    static t_list	*lst = NULL;
    return (&lst);
}

int	ft_putc(int c)
{
	int	ret;
	int	ttyfd;

	ttyfd = open("/dev/tty", O_RDWR);
	ret	= write(ttyfd, &c, 1);
	close (ttyfd);
	return (ret);
}

void	putstr(char *str, int (*putchar)(int c))
{
	int	i;

	i = 0;
	while (str[i])
		putchar(str[i++]);
}

void	restore_term(void)
{
	if (tcsetattr(0, 0, &tcap()->term) == -1)
	   exit(-1);
	putstr("\033[?1049l", ft_putc);
}

void	handle_sigtstp(void)
{
	{int fd = open("fifo", O_RDWR);
	dprintf(fd, "CAUGHT SIGNAL\n");
	close (fd);}
	restore_term();
	ft_putstr("SIGTSTP");
	signal(SIGTSTP, SIG_DFL);
	ioctl(0, TIOCSTI, (char [2]){tcap()->new_term.c_cc[VSUSP], 0});
}

void	handle_sigcont (void)
{
	putstr("\033[?1049h\033[H", ft_putc);
	if (tcsetattr(0, 0, &tcap()->new_term) == -1)
	   exit(-1);
	clr_screen();
	disp_menu(*lst());
	signal(SIGTSTP, sig_handler);
}

void	sig_handler(int signo)
{
	if (signo == SIGCONT)
		handle_sigcont();
	else if (signo == SIGTSTP)
		handle_sigtstp();
    else if (signo == SIGWINCH)
		ft_putstr ("RESIZE");
    else if (signo == SIGINT || signo == SIGTERM)
	{
		restore_term();
		exit (0);
	}
}

void	clr_screen(void)
{
	tputs(tcap()->cl_string, 1, ft_putc);
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
	close (ttyfd);
}

int		del_elem (t_list **begin, t_list **end, t_list **cur_elem)
{
	t_list	*tmp;

	tmp = *cur_elem;
	if ((*cur_elem)->next == NULL
			&& ((t_elem *)(*cur_elem)->content)->previous == NULL)
		return (-1);
	else if ((*cur_elem)->next == NULL)
	{
		*end = ((t_elem *)(*cur_elem)->content)->previous;
		((t_elem *)(*cur_elem)->content)->previous->next = NULL;
		tmp = *begin;
	}
	else if (((t_elem *)(*cur_elem)->content)->previous == NULL)
	{
		((t_elem *)(*cur_elem)->next->content)->previous = NULL;
		tmp = tmp->next;
		*begin = tmp;
	}
	else
	{
		((t_elem *)(*cur_elem)->content)->previous->next = (*cur_elem)->next;
		((t_elem *)(*cur_elem)->next->content)->previous = ((t_elem *)(*cur_elem)->content)->previous;
		tmp = tmp->next;
	}
	free ((void *)(*cur_elem)->content);
	free ((void *)(*cur_elem));
	*cur_elem = tmp;
	if (((t_elem *)(*cur_elem)->content)->mode == SELECT)
		((t_elem *)(*cur_elem)->content)->mode = SELECT_HOVER;
	else
		((t_elem *)(*cur_elem)->content)->mode = HOVER;
	return (0);
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

void	set_tcap(void)
{
	char	*tmp;

	tcap()->height = tgetnum("li");
	tcap()->width = tgetnum("co");
	tcap()->cl_string = tgetstr ("cl", 0);
	tcap()->cm_string = tgetstr ("cm", 0);
	tcap()->auto_wrap = tgetflag ("am");
	tmp = tgetstr ("pc", 0);
	tcap()->PC = tmp ? *tmp : 0;
	tcap()->BC = tgetstr("le", 0);
	tcap()->UP = tgetstr("up", 0);
}

t_list	*fill_list(char **argv)
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
		ft_lstadd(lst(), tmp = ft_lstnew((void *)&e, sizeof(t_elem)));
		if (sw == 0)
		{
			end = tmp;
			sw = 1;
		}
		if (previous != NULL)
			((t_elem *)(previous->content))->previous = tmp;
		previous = *(lst());
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

void		disp_menu(t_list *lst)
{
	//{int fd = open("fifo", O_RDWR);
	//dprintf(fd, "DISP\n");
	//close (fd);}
	while (lst)
	{
		ft_print(((t_elem *)(lst->content))->text,
			((t_elem *)(lst->content))->mode);
		lst = lst->next;
	}
	//{int fd = open("fifo", O_RDWR);
	//dprintf(fd, "END DISP\n");
	//close (fd);}
}

static void	ft_move_right(t_list *begin, t_list **cur_elem)
{
	//dprintf(open("fifo", O_RDWR), "MOVE RIGHT\n");
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
	clr_screen();
	disp_menu(begin);
}

static void	ft_move_left(t_list *end, t_list *begin, t_list **cur_elem)
{
	//dprintf(open("fifo", O_RDWR), "MOVE LEFT\n");
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
	clr_screen();
	disp_menu(begin);
}

static int	ft_get_user_input(t_list *begin, t_list *end)
{
	char	buf[3];
	t_list	*cur_elem;

	cur_elem = begin;
	ft_bzero(buf, sizeof(buf));
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGWINCH, sig_handler);
	signal(SIGTSTP, sig_handler);
	signal(SIGCONT, sig_handler);
	while (read(0, buf, sizeof(buf)))
	{
		if ((unsigned int)(buf[0]) == 27)
		{
			if (((unsigned int)buf[2]) == RIGHT_KEY)
				ft_move_right(begin, &cur_elem);
			else if (((unsigned int)buf[2]) == LEFT_KEY)
				ft_move_left(end, begin, &cur_elem);
			else if (buf[2] == 51)
			{
				if (del_elem (&begin, &end, &cur_elem) == -1)
					break ;
				clr_screen();
				disp_menu(begin);
			}
			else if (buf[2] == 0)
				break ;
		}
		else if ((unsigned int)(buf[0]) == RET_KEY)
		{
			//dprintf(open("fifo", O_RDWR), "SPC KEY\n");
			clr_screen();
			restore_term();
			put_selected (begin);
			return (1);
		}
		else if ((unsigned int)(buf[0]) == SPC_KEY)
		{
			//dprintf(open("fifo", O_RDWR), "SPC KEY\n");
			((t_elem *)(cur_elem->content))->mode = 
				(((t_elem *)(cur_elem->content))->mode == SELECT_HOVER) ? HOVER : SELECT_HOVER;
			ft_move_right(begin, &cur_elem);
			clr_screen();
			disp_menu(begin);
		}
		else if ((unsigned int)(buf[0]) == BKSPC_KEY)
		{
			//dprintf(open("fifo", O_RDWR), "BKSPC KEY\n");
			if (del_elem (&begin, &end, &cur_elem) == -1)
				break ;
			clr_screen();
			disp_menu(begin);
		}
		//dprintf(open("/dev/tty", O_RDWR), "0 : '%d'\n", (unsigned int)(buf[0]));
		//dprintf(open("/dev/tty", O_RDWR), "0 : '%d'\n", (unsigned int)(buf[1]));
		//dprintf(open("/dev/tty", O_RDWR), "0 : '%d'\n", (unsigned int)(buf[2]));
		ft_bzero(buf, sizeof(buf));
	}
	return (0);
}

/*
** vi: cursor visibility off
** ks: keypad start
*/
void	init_menu(char **argv)
{
	t_list	*end;

	end = fill_list(argv);
	clr_screen();
	((t_elem *)(*lst())->content)->mode = HOVER;
	disp_menu(*lst());
	tputs(tgetstr("ks", NULL), 1, ft_putc);
	tputs(tgetstr("vi", NULL), 1, ft_putc);
	if (ft_get_user_input(*lst(), end) != 1)
		restore_term();
	ft_quit_menu(*lst());
}

void	init_term(void)
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
	if (tcgetattr(0, &tcap()->new_term) == -1)
		exit(-1);
	if (tcgetattr(0, &tcap()->term) == -1)
		exit(-1);
	tcap()->new_term.c_lflag &= ~(ICANON);
	tcap()->new_term.c_lflag &= ~(ECHO);
	tcap()->new_term.c_cc[VMIN] = 1;
	tcap()->new_term.c_cc[VTIME] = 0;
	putstr("\033[?1049h\033[H", ft_putc);
	if (tcsetattr(0, 0, &tcap()->new_term) == -1)
	   exit(-1);
	set_tcap();
}

int		main(int argc, char **argv)
{
	int				ttyfd;

	if (argc > 1)
	{
		ttyfd = open("/dev/tty", O_RDWR);
		if (isatty(ttyfd))
		{
			init_term();
			init_menu(argv);
		}
		else
			ft_putstr("Not a valid terminal type device.\n");
		close (ttyfd);
	}
	else
		ft_putstr("Usage: ./ft_select element1 element2 ...\n");
	return (0);
}
