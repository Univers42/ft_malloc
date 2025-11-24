/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stats.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:58:21 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 21:44:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_STATS

void	handle_less_core(t_glob *g, int nu, size_t value);
void	handle_mmap(t_glob *g, size_t value);
void	handle_brk(t_glob *g, size_t value);
void	handle_realloc(t_glob *g);
void	handle_more_core(t_glob *g, int nu, size_t value);

void	compute_stats_core(t_glob *g, int nu, int flag, size_t value)
{
	if (!g)
		return ;
	if (flag == STAT_LESS_CORE)
	{
		handle_less_core(g, nu, value);
		return ;
	}
	if (flag == STAT_MMAP)
	{
		handle_mmap(g, value);
		return ;
	}
	if (flag == STAT_BRK)
	{
		handle_brk(g, value);
		return ;
	}
	if (flag == STAT_REALLOC)
	{
		handle_realloc(g);
		return ;
	}
	handle_more_core(g, nu, value);
}

void	compute_stats_realloc_copy(t_glob *g)
{
	if (g)
		g->mstats.nrcopy++;
}

#else

void	compute_stats_core(t_glob *g, int nu, int flag, size_t value)
{
	(void)g;
	(void)nu;
	(void)flag;
	(void)value;
}

void	compute_stats_realloc_copy(t_glob *g)
{
	(void)g;
}

#endif
