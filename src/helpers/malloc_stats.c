/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_stats.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:01:58 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:40:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_STATS

void	update_malloc_stats(int nunits, t_glob *g)
{
	g->mstats.nmalloc[nunits]--;
	g->mstats.nfre++;
}
#else

void	update_malloc_stats(int nunits, t_glob *g)
{
	(void)nunits;
	(void)g;
}

#endif
