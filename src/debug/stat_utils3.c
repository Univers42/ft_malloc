/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat_utils3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:33:30 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 21:35:59 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_STATS

void	compute_stats_mmap(t_glob *g, size_t value)
{
	compute_stats_core(g, 0, STAT_MMAP, value);
}

void	compute_stats_brk(t_glob *g, size_t value)
{
	compute_stats_core(g, 0, STAT_BRK, value);
}

#else

void	compute_stats_brk(t_glob *g, size_t value)
{
	(void)g;
	(void)value;
}

void	compute_stats_mmap(t_glob *g, size_t value)
{
	(void)g;
	(void)value;
}

#endif