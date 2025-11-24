/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat_utils4.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:41:15 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 21:45:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_STATS

void	handle_less_core(t_glob *g, int nu, size_t value)
{
	if ((size_t)nu >= NBUCKETS)
		return ;
	g->mstats.nsbrk++;
	g->mstats.tsbrk -= (long)value;
	g->mstats.nlesscore[nu]++;
	g->mstats.nfre++;
}

void	handle_mmap(t_glob *g, size_t value)
{
	g->mstats.nmmap++;
	g->mstats.tmmap += (long)value;
}

void	handle_brk(t_glob *g, size_t value)
{
	g->mstats.nsbrk++;
	g->mstats.tsbrk += (long)value;
}

void	handle_realloc(t_glob *g)
{
	(void)g;
	g->mstats.nrealloc++;
}

void	handle_more_core(t_glob *g, int nu, size_t value)
{
	if ((size_t)nu >= NBUCKETS)
		return ;
	g->mstats.nsbrk++;
	g->mstats.tsbrk += (long)value;
	g->mstats.nmorecore[nu]++;
}
#endif