/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebouther <ebouther@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/05/28 21:02:12 by ebouther          #+#    #+#             */
/*   Updated: 2016/06/09 23:09:12 by ebouther         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SELECT_H
# define FT_SELECT_H

# include <term.h>
# include "libft.h"

# define LEFT_KEY 68
# define RIGHT_KEY 67
# define UP_KEY 172052251
# define DOWN_KEY 172117787
# define ESC_KEY 2587
# define SPACE_KEY 32
# define DEL_KEY 127
# define RET_KEY 10
# define SUP_KEY 2117294875
# define HOME_KEY 4741915
# define END_KEY 4610843
# define PAGE_UP_KEY 2117425947
# define PAGE_DOWN_KEY 2117491483
# define TAB_KEY 9
# define CRTL_A_KEY 1

typedef struct	s_termcap
{
	char	*termtype;
	char	PC;
	char	*BC;
	char	*UP;
	char	*cl_string;
	char	*cm_string;
	int		height;
	int		width;
	int		auto_wrap;
}				t_termcap;

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

#endif
