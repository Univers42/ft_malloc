/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_stats.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:01:58 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:02:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_STATS
static void update_malloc_stats(int nunits, t_glob *g)
{
	g->mstats.nmalloc[nunits]--;
	g->mstats.nfre++;
}
#else
static void update_malloc_stats(int nunits, t_glob *g)
{
	(void)nunits;
	(void)g;
}
#endif


