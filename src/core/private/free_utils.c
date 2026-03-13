/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:02:33 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 14:45:33 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

static void	read_guard(t_mguard *mg, char *ap_end)
{
	char	*z;
	int		i;

	z = (char *)mg->s;
	i = 0;
	while (i < 4)
	{
		*z++ = *ap_end++;
		i++;
	}
}

void	validate_end_guard(char *ap, t_mhead *p, t_val_ctx *v)
{
	t_mguard	mg;

	set_state_mem(v->mem);
	read_guard(&mg, ap + p->s_minfo.mi_nbytes);
	if (mg.i != p->s_minfo.mi_nbytes)
		xbotch(ERR_ASSERT_FAILED,
			"free: start and end sizes differ",
			v->file, v->line);
}

int	handle_special_free_cases(t_mhead *p, int nunits, t_glob *g)
{
	int	freed;

	freed = try_munmap_free(p, nunits, g);
	if (!freed)
		freed = try_lesscore_free(p, nunits, g);
	return (freed);
}

void	add_to_free_list(t_mhead *p, int nunits, t_glob *g)
{
	p->s_minfo.mi_alloc = ISFREE;
	g->busy[nunits] = 1;
	*chain_ptr(p) = g->nextf[nunits];
	g->nextf[nunits] = p;
	g->busy[nunits] = 0;
}

void	handle_busy_bucket(t_mhead *p, int nunits, t_glob *g)
{
	if (g->busy[nunits] == 1)
		xsplit(p, nunits);
}
