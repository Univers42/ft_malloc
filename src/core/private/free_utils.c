/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:02:33 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:24:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

void	validate_end_guard(char *ap, t_mhead *p, t_addr mem, const char *file, int line)
{
	t_mguard mg;
	char *z;
	char *ap_end;
	int i;

	set_state_mem(mem);
	ap_end = ap + p->s_minfo.mi_nbytes;
	z = (char *)mg.s;
	i = 0;
	while (i < 4)
	{
		*z++ = *ap_end++;
		i++;
	}
	if (mg.i != p->s_minfo.mi_nbytes)
		xbotch(ERR_ASSERT_FAILED, "free: start and end chunk sizes differ", file, line);
}

int	handle_special_free_cases(t_mhead *p, int nunits, t_glob *g)
{
	int freed;

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

