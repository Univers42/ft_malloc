/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pagealign.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 20:11:52 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 20:15:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "get_page_size.h"
#include <unistd.h>

#ifdef MALLOC_STATS
void malloc_stats_sbrk_update(long sbrk_needed)
{
    _mstats.nsbrk++;
    _mstats.tsbrk += sbrk_needed;
}
#else
void malloc_stats_sbrk_update(long sbrk_needed)
{
    curbrk = sbrk(sbrk_needed);
}
#endif

int pagealign(void)
{
	int     nunits;
	t_mhead *mp;
	long    sbrk_needed;
	char    *curbrk;
	t_glob  g;

	g.pagesz = get_page_size();
	if (g.pagesz < MALLOC_PAGESIZE_MIN)
		g.pagesz = MALLOC_PAGESIZE_MIN;
	g.memtop = sbrk(0);
	curbrk = sbrk(0);
	sbrk_needed = g.pagesz - ((long)curbrk & (g.pagesz - 1));
	if (sbrk_needed < 0)
		sbrk_needed += g.pagesz;
	if (sbrk_needed)
	{
		malloc_stats_sbrk_update(sbrk_needed);
		curbrk = sbrk(sbrk_needed);
		if ((long)curbrk == -1)
			return (-1);
		g.memtop += sbrk_needed;
		curbrk += sbrk_needed & (PREPOP_SIZE - 1);
		sbrk_needed -= sbrk_needed & (PREPOP_SIZE - 1);
		nunits = sbrk_needed / PREPOP_SIZE;
		
	}
	nunits = 7;
	while (++nunits < NBUCKETS)
		if (g.pagesz <= binsize(nunits))
			break ;
		g.pagebucket = nunits;
	return (0);
}
