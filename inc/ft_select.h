/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebouther <ebouther@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/05/28 21:02:12 by ebouther          #+#    #+#             */
/*   Updated: 2016/10/08 20:04:31 by ebouther         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SELECT_H
# define FT_SELECT_H

# include <term.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
# include "libft.h"

# define LEFT_KEY 68
# define RIGHT_KEY 67
# define ESC_KEY 87
# define SPC_KEY 32
# define RET_KEY 10
# define DEL_KEY 51
# define BKSPC_KEY 127

typedef struct		s_termcap
{
	struct termios	term;
	struct termios	new_term;
	char			*termtype;
	char			PC;
	char			*BC;
	char			*UP;
	char			*cl_string;
	char			*cm_string;
	int				height;
	int				width;
	int				auto_wrap;
}					t_termcap;

enum				e_mode
{
					NORMAL,
					SELECT,
					HOVER,
					SELECT_HOVER
};

typedef struct		s_elem
{
	t_list			*previous;
	char			*text;
	enum e_mode		mode;
}					t_elem;

void	sig_handler(int signo);
void	clr_screen(void);
void	disp_menu(t_list *lst);

#endif
