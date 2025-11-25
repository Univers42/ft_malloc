/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bcoalesce_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:22:30 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 12:16:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "get_page_size.h"

void ensure_chain(t_glob *g, t_mhead *mp1, t_mhead *mp2, int nbuck)
{
	if (mp2 != mp1 && chain(mp2) != mp1)
	{
		g->busy[nbuck] = 0;
		set_state_mem((t_addr)0);
		xbotch(0, "bcoalesce: chain(mp2) != mp1", NULL, 0);
	}
}

#ifdef MALLOC_DEBUG

int bcoalesce_check_adjacent(t_mhead *mp1, t_glob *g, int nbuck,
								unsigned long siz)
{
	if (chain(mp1) != (t_mhead *)((char *)mp1 + siz))
	{
		g->busy[nbuck] = 0;
		return (0);
	}
	return (1);
}
#else

int bcoalesce_check_adjacent(t_mhead *mp1, t_glob *g, int nbuck,
							 unsigned long siz)
{
	(void)mp1;
	(void)g;
	(void)nbuck;
	(void)siz;
	return (1);
}
#endif

#ifdef MALLOC_STATS

void bcoalesce_stats_inc(int nbuck)
{
	_mstats.tbcoalesce++;
	_mstats.ncoalesce[nbuck]++;
}
#else

void bcoalesce_stats_inc(int nbuck)
{
	(void)nbuck;
}

#endif
