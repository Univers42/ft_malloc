/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:28:55 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 21:29:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_STATS

void	morecore_stat_nmorecore(int nu, t_glob *g)
{
	if (g)
		g->mstats.nmorecore[nu]++;
}

void	morecore_stat_nsbrk(long s, t_glob *g)
{
	if (g)
	{
		g->mstats.nsbrk++;
		g->mstats.tsbrk += s;
	}
}
#else

void	morecore_stat_nmorecore(int nu, t_glob *g)
{
	(void)nu;
	(void)g;
}

void	morecore_stat_nsbrk(long s, t_glob *g)
{
	(void)s;
	(void)g;
}
#endif
