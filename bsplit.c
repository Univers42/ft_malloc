/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsplit.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 18:38:18 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 19:00:19 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "get_page_size.h"

#ifdef MALLOC_STATS
static void	bsplit_stats_inc(int nbuck)
{
    _mstats.tbsplit++;
    _mstats.nsplit[nbuck]++;
}
#else
static void	bsplit_stats_inc(int nbuck)
{
    (void)nbuck;
}
#endif

/*
 * Split a block at index > NU (but less than SPLIT_MAX) into a set of
 * blocks of the correct size, and attach them to nextf[NU].  nextf[NU]
 * is assumed to be empty.  Must be called with signals blocked (e.g.,
 * by morecore()).  BUSY[NU] must be set to 1.
 */
void	bsplit(int nu)
{
    t_mhead		*mp;
    int32_t		nbuck;
    int32_t		nblks;
    int32_t		split_max;
    uint64_t	siz;
    t_glob		*g;

    g = get_glob(GLOB_NONE, NULL);
    if (g->maxbuck > SPLIT_MAX)
        split_max = g->maxbuck;
    else
        split_max = SPLIT_MAX;
    if (nu >= SPLIT_MID)
    {
        nbuck = split_max;
        while (nbuck > nu)
        {
            if (g->busy[nbuck] || g->nextf[nbuck] == NULL)
            {
                nbuck--;
                continue;
            }
            break;
        }
    }
    else
    {
        nbuck = nu + 1;
        while (nbuck <= split_max)
        {
            if (g->busy[nbuck] || g->nextf[nbuck] == NULL)
            {
                nbuck++;
                continue;
            }
            break;
        }
    }
    if (nbuck > split_max || nbuck <= nu)
        return ;
    g->busy[nbuck] = 1;
    mp = g->nextf[nbuck];
    g->nextf[nbuck] = chain(mp);
    g->busy[nbuck] = 0;
    bsplit_stats_inc(nbuck);
    siz = binsize(nu);
    nblks = binsize(nbuck) / siz;
    g->nextf[nu] = mp;
    while (1)
    {
        mp->s_minfo.mi_alloc = ISFREE;
        mp->s_minfo.mi_index = nu;
        if (--nblks <= 0)
            break ;
        *chain_ptr(mp) = (t_mhead *)((char *)mp + siz);
        mp = (t_mhead *)((char *)mp + siz);
    }
    *chain_ptr(mp) = NULL;
}

static void
xsplit(t_mhead *mp, int nu)
{
    t_mhead		*nh;
    int32_t		nbuck;
    int32_t		nblks;
    uint64_t	siz;
    t_glob		*g;

    g = get_glob(GLOB_NONE, NULL);
    nbuck = nu - 1;
    while (nbuck >= SPLIT_MIN && g->busy[nbuck])
        nbuck--;
    if (nbuck < SPLIT_MIN)
        return ;
    bsplit_stats_inc(nu);
    siz = binsize(nu);
    nblks = siz / binsize(nbuck);
    siz = binsize(nbuck);
    nh = mp;
    while (1)
    {
        mp->s_minfo.mi_alloc = ISFREE;
        mp->s_minfo.mi_index = nbuck;
        if (--nblks <= 0)
            break ;
        *chain_ptr(mp) = (t_mhead *)((char *)mp + siz);
        mp = (t_mhead *)((char *)mp + siz);
    }
    g->busy[nbuck] = 1;
    *chain_ptr(mp) = g->nextf[nbuck];
    g->nextf[nbuck] = nh;
    g->busy[nbuck] = 0;
}