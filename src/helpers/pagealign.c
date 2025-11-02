/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pagealign.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 20:11:52 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:37:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "get_page_size.h"
#include <unistd.h>
#include <stdint.h>

int pagealign(void)
{
    t_glob *g;
    int nunits;
    t_mhead *mp;
    long sbrk_needed;
    char *curbrk;
    void *sret;

    g = get_glob(GLOB_NONE, NULL);
    if (!g)
        return (-1);

    g->pagesz = get_page_size();
    if (g->pagesz < MALLOC_PAGESIZE_MIN)
        g->pagesz = MALLOC_PAGESIZE_MIN;

    /* initialize memtop and compute how much to advance break to be page aligned */
    g->memtop = sbrk(0);
    curbrk = sbrk(0);
    sbrk_needed = g->pagesz - ((long)curbrk & (g->pagesz - 1));
    if (sbrk_needed < 0)
        sbrk_needed += g->pagesz;

    /* allocate the wasted partial page and populate PREPOP bin with 64-byte chunks */
    if (sbrk_needed)
    {
        compute_stats_brk(g, sbrk_needed);
        sret = sbrk(sbrk_needed);
        if (sret == (t_addr)-1)
            return (-1);
        curbrk = sbrk(sbrk_needed);
        if ((long)curbrk == -1)
            return (-1);
        g->memtop += sbrk_needed;

        /* align curbrk to PREPOP_SIZE, compute number of chunks */
        curbrk += sbrk_needed & (PREPOP_SIZE - 1);
        sbrk_needed -= sbrk_needed & (PREPOP_SIZE - 1);
        nunits = (int)(sbrk_needed / PREPOP_SIZE);

        if (nunits > 0)
        {
            mp = (t_mhead *)curbrk;
            g->nextf[PREPOP_BIN] = mp;

            /* build chain of PREPOP_SIZE blocks using while only */
            while (1)
            {
                mp->s_minfo.mi_alloc = ISFREE;
                mp->s_minfo.mi_index = (char)PREPOP_BIN;
                if (--nunits <= 0)
                    break;
                *chain_ptr(mp) = (t_mhead *)((char *)mp + PREPOP_SIZE);
                mp = (t_mhead *)((char *)mp + PREPOP_SIZE);
            }
            *chain_ptr(mp) = NULL;
        }
    }

    /* compute which bin corresponds to the page size (use while) */
    nunits = 7;
    while (nunits < NBUCKETS)
    {
        if ((uint64_t)g->pagesz <= g->binsizes[(size_t)nunits])
            break;
        nunits++;
    }
    g->pagebucket = nunits;

    return (0);
}