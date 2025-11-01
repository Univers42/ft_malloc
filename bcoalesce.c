/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bcoalesce.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 18:31:59 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 18:37:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "get_page_size.h"

/**
 * OK, now we have mp1 pointing to the block we want to add to nextf[NU].
 * chain(mp2) must equal mp1. chekc that mp1 and mp are adjacent
*/
#ifdef MALLOC_DEBUG

static int	bcoalesce_check_adjacent(t_mhead *mp1, t_glob *g,
									int nbuck, unsigned long siz)
{
	if (chain(mp1) != (t_mhead *)((char *)mp1 + siz))
	{
		g->busy[nbuck] = 0;
		return (0);
	}
	return (1);
}
#else

static int	bcoalesce_check_adjacent(t_mhead *mp1, t_glob *g,
									int nbuck, unsigned long siz)
{
	(void)mp1;
	(void)g;
	(void)nbuck;
	(void)siz;
	return (1);
}
#endif

#ifdef MALLOC_STATS

static void	bcoalesce_stats_inc(int nbuck)
{
	_mstats.tbcoalesce++;
	_mstats.ncoalesce[nbuck]++;
}
#else

static void	bcoalesce_stats_inc(int nbuck)
{
	(void)nbuck;
}
#endif

/**
 * coalesce two adjaent free blocks off the free list for size NU - 1,
 * as long as we can find two adjacent free blocks. next[NU - 1] is 
 * assumed to not be busy; the caller (morecore()) check for this.
 * BUSY[NU] must be set to 1.
 */
static void	bcoalesce(int nu)
{
    t_mhead         *mp;
    t_mhead         *mp1;
    t_mhead         *mp2;
    int             nbuck;
    unsigned long   siz;
    t_glob          *g;

    g = get_glob(GLOB_NONE, NULL);
    nbuck = nu - 1;
    if (g->nextf[nbuck] == NULL || g->busy[nbuck])
        return ;
    g->busy[nbuck] = 1;
    siz = binsize(nbuck);
    mp2 = mp1 = g->nextf[nbuck];
    mp = chain(mp1);
    while (mp && mp != (t_mhead *)((char *)mp1 + siz))
    {
        mp2 = mp1;
        mp1 = mp;
        mp = chain(mp);
    }
    if (mp == NULL)
    {
        g->busy[nbuck] = 0;
        return ;
    }

    /* ensure chain(mp2) == mp1 */
    if (mp2 != mp1 && chain(mp2) != mp1)
    {
        g->busy[nbuck] = 0;
        xbotch((t_addr)0, 0, "bcoalesce: chain(mp2) != mp1", NULL, 0);
    }

    if (!bcoalesce_check_adjacent(mp1, g, nbuck, siz))
        return ;
    if (mp1 == g->nextf[nbuck])
        g->nextf[nbuck] = chain(mp);
    else
        *chain_ptr(mp2) = chain(mp);
    g->busy[nbuck] = 0;
    bcoalesce_stats_inc(nbuck);
    mp1->s_minfo.mi_alloc = ISFREE;
    mp1->s_minfo.mi_index = nu;
    *chain_ptr(mp1) = g->nextf[nu];
    g->nextf[nu] = mp1;
}