/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dyn_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 21:32:29 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 00:10:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <unistd.h>
#include <stdint.h>

#if defined(USE_MMAP)
#  include <sys/mman.h>
#endif

/* try_mmap_for_morecore: file-scope helper that is compiled in/out */
#if defined(USE_MMAP)
static int try_mmap_for_morecore(int nu, long sbrk_amt, t_mhead **out_mp, t_glob *g)
{
    t_mhead *mp;

    if (!g || nu <= g->malloc_mmap_threshold)
        return 0;

    mp = (t_mhead *)mmap(NULL, (size_t)sbrk_amt, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if ((void *)mp == MAP_FAILED)
        return 0;

    g->nextf[nu] = mp;
    mp->s_minfo.mi_alloc = ISFREE;
    mp->s_minfo.mi_index = (char)nu;
    *chain_ptr(mp) = NULL;

    compute_stats_core(g, nu, STAT_MMAP, (size_t)sbrk_amt);
    if (out_mp)
        *out_mp = mp;
    return 1;
}
#else
static int try_mmap_for_morecore(int nu, long sbrk_amt, t_mhead **out_mp, t_glob *g)
{
    (void)nu; (void)sbrk_amt; (void)out_mp; (void)g;
    return 0;
}
#endif

/* stats helpers (no-op when MALLOC_STATS not defined) */
#ifdef MALLOC_STATS
static void morecore_stat_nmorecore(int nu, t_glob *g) { if (g) g->mstats.nmorecore[nu]++; }
static void morecore_stat_nsbrk(long s, t_glob *g) { if (g) { g->mstats.nsbrk++; g->mstats.tsbrk += s; } }
#else
static void morecore_stat_nmorecore(int nu, t_glob *g) { (void)nu; (void)g; }
static void morecore_stat_nsbrk(long s, t_glob *g) { (void)s; (void)g; }
#endif

void lesscore(int nu)
{
    t_glob *g = get_glob(GLOB_NONE, NULL);
    long siz;

    if (!g || (size_t)nu >= NBUCKETS)
        return;

    siz = (long)g->binsizes[(size_t)nu];
    if (siz <= 0)
        return;

    /* attempt to shrink program break; ignore failure */
    if (sbrk(-siz) == (void *)-1)
        return;

    g->memtop -= siz;
    compute_stats_core(g, nu, STAT_LESS_CORE, (size_t)siz);
}

size_t combine_max(size_t page_bucket)
{
    return page_bucket - 1;
}

void morecore(int nu)
{
    t_mhead	*mp = NULL;
    int		nblks = 0;
    size_t	siz = 0;
    long	sbrk_amt = 0;
    sigset_t set, oset;
    int		blocked_sigs = 0;
    t_glob	*g = get_glob(GLOB_NONE, NULL);

    if (!g)
        return;

    /* block signals unconditionally in this implementation */
    malloc_block_signals(&set, &oset);
    blocked_sigs = 1;

    while (1)
    {
        /* fetch bucket size */
        siz = (size_t)g->binsizes[(size_t)nu];
        if (siz == 0)
            break;

        morecore_stat_nmorecore(nu, g);

        /* try splitting larger block */
        if (nu >= SPLIT_MIN && nu <= g->malloc_mmap_threshold)
        {
            bsplit(nu);
            if (g->nextf[nu] != NULL)
                break;
        }

        /* try coalescing two smaller blocks */
        if (nu >= COMBINE_MIN && nu < combine_max(g->pagebucket) &&
            nu <= g->malloc_mmap_threshold && g->busy[nu - 1] == 0 && g->nextf[nu - 1])
        {
            bcoalesce(nu);
            if (g->nextf[nu] != NULL)
                break;
        }

        /* decide sbrk amount and number of blocks */
        if (siz <= (size_t)g->pagesz)
        {
            sbrk_amt = g->pagesz;
            /* guard division: siz != 0 above */
            nblks = (int)(sbrk_amt / (long)siz);
        }
        else
        {
            long rem = (long)siz & (g->pagesz - 1);
            if (rem == 0)
                sbrk_amt = (long)siz;
            else
                sbrk_amt = (long)siz + g->pagesz - rem;
            nblks = 1;
        }

        /* try mmap (helper is no-op when mmap disabled) */
        if (try_mmap_for_morecore(nu, sbrk_amt, &mp, g))
            break;

        /* account sbrk stats */
        morecore_stat_nsbrk(sbrk_amt, g);

        /* request memory */
        {
            void *sret = sbrk(sbrk_amt);
            if (sret == (void *)-1)
                break;
            mp = (t_mhead *)sret;
        }

        g->memtop += sbrk_amt;

        /* alignment */
        if (((uintptr_t)mp & (uintptr_t)MALIGN_MASK) != 0)
        {
            mp = (t_mhead *)((((uintptr_t)mp + (uintptr_t)MALIGN_MASK) & ~((uintptr_t)MALIGN_MASK)));
            nblks--;
        }

        /* build free list for these blocks */
        g->nextf[nu] = mp;
        while (1)
        {
            mp->s_minfo.mi_alloc = ISFREE;
            mp->s_minfo.mi_index = (char)nu;
            if (--nblks <= 0)
                break;
            *chain_ptr(mp) = (t_mhead *)((char *)mp + siz);
            mp = (t_mhead *)((char *)mp + siz);
        }
        *chain_ptr(mp) = NULL;
        break;
    }

    if (blocked_sigs)
        malloc_unblock_signals(&set, &oset);
}