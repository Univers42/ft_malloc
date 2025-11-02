/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stats.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:58:21 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:04:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_STATS
void compute_stats_core(t_glob *g, int nu, int flag, size_t value)
{
    if (!g)
        return;

    if (flag == STAT_LESS_CORE)
    {
        if ((size_t)nu >= NBUCKETS)
            return;
        g->mstats.nsbrk++;
        g->mstats.tsbrk -= (long)value;
        g->mstats.nlesscore[nu]++;
        g->mstats.nfre++;
    }
    else if (flag == STAT_MMAP)
    {
        g->mstats.nmmap++;
        g->mstats.tmmap += (long)value;
    }
    else if (flag == STAT_BRK)
    {
        g->mstats.nsbrk++;
        g->mstats.tsbrk += (long)value;
    }
    else if (flag == STAT_REALLOC)
        g->mstats.nrealloc++;
    else /* STAT_MORE_CORE */
    {
        if ((size_t)nu >= NBUCKETS)
            return;
        g->mstats.nsbrk++;
        g->mstats.tsbrk += (long)value;
        g->mstats.nmorecore[nu]++;
    }
}

int malloc_free_blocks(int size, t_glob *g)
{
    int nfree;
    t_mhead *p;

    nfree = 0;
    p = g->nextf[size];
    while (p)
    {
        g->mstats.nfree++;
        p = chain(p);
    }
    return (g->mstats.nfree);
}
#else
void compute_stats_core(t_glob *g, int nu, int flag, size_t value)
{
    (void)g;
    (void)nu;
    (void)flag;
    (void)value;
}
int malloc_free_blocks(int size, t_glob *g)
{
    (void)size;
    (void)g;
    return (0);
}
#endif

void compute_stats_brk(t_glob *g, size_t value)
{
    compute_stats_core(g, 0, STAT_BRK, value);
}

void compute_stats_mmap(t_glob *g, size_t value)
{
    compute_stats_core(g, 0, STAT_MMAP, value);
}
