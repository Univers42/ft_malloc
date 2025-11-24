/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:38:30 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:31:06 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

/**
 * internal_realloc: main routine (calls compile-time chosen mremap_implement
 * *Compile time remap selection
 * *API / symbol names
 * *Stats / watch register behavior
 * *Guard and magic checks
 * *Remap return values
 * *Busy/buckets and concurrency
 * *page / allocation rounding
 * *fastcopy, internal malloc/internal_free signatures
 * *error_handling /errno preservation
 * *norminette / style
 * */
t_addr
internal_realloc(t_addr mem, size_t n, const char *file, int line, int flags)
{
    t_mhead *p;
    uint32_t tocopy;
    size_t old_alloc_bytes;
    size_t new_alloc_bytes;
    int nunits;
    int newunits;
    t_glob *g;
    int i;
    t_mguard mg;
    uint8_t *z;
    char *m;
    t_addr mresult;

    g = get_glob(GLOB_NONE, NULL);
    compute_stats_core(g, 0, STAT_REALLOC, 0);

    if (n == 0)
    {
        internal_free(mem, file, line, MALLOC_INTERNAL);
        return ((t_addr)0);
    }
    if (mem == (t_addr)0)
        return (internal_malloc(n, file, line, MALLOC_INTERNAL));

    p = (t_mhead *)mem - 1;

    /* basic validation */
    if (p->s_minfo.mi_alloc != ISALLOC)
    {
        set_state_mem(mem);
        xbotch(ERR_UNALLOC, "realloc: called with unallocated block argument", file, line);
    }
    assert_or_abort(p->s_minfo.mi_magic2 == MAGIC2, "p->s_minfo.mi_magic2 == MAGIC2", file, line);

    /* underflow / magic8 check */
    i = 0;
    z = (uint8_t *)p->s_minfo.mi_magic8;
    while (i < MAGIC8_NUMBYTES)
    {
        if (*z++ != MAGIC1)
        {
            set_state_mem(mem);
            xbotch(ERR_UNDERFLOW, "realloc: underflow detected; magic8 corrupted", file, line);
        }
        i++;
    }

    /* bytes to copy */
    tocopy = p->s_minfo.mi_nbytes;
    if (n < (size_t)tocopy)
        tocopy = (uint32_t)n;

    /* guard snapshot */
    m = (char *)mem + tocopy;
    z = (uint8_t *)mg.s;
    *z++ = (uint8_t)*m++;
    *z++ = (uint8_t)*m++;
    *z++ = (uint8_t)*m++;
    *z++ = (uint8_t)*m++;
    if (mg.i != p->s_minfo.mi_nbytes)
    {
        set_state_mem(mem);
        xbotch(ERR_ASSERT_FAILED, "realloc: start and end chunk sizes differ", file, line);
    }

    /* no change */
    if (n == p->s_minfo.mi_nbytes)
        return (mem);

    old_alloc_bytes = allocated_bytes((size_t)p->s_minfo.mi_nbytes);
    new_alloc_bytes = allocated_bytes((size_t)n);

    nunits = (int)p->s_minfo.mi_index;
    assert_or_abort(nunits < NBUCKETS, "nunits < NBUCKETS", file, line);

    /* compute newunits (while-only) */
    if (new_alloc_bytes > old_alloc_bytes)
        newunits = nunits;
    else
    {
        size_t half_page = (size_t)(g->pagesz >> 1);
        newunits = (new_alloc_bytes <= half_page) ? STARTBUCK : g->pagebucket;
    }
    while (newunits < (int)NBUCKETS && new_alloc_bytes > g->binsizes[(size_t)newunits])
        newunits++;

    /* quick in-place adjust when same/adjacent bucket */
    if (right_bucket(new_alloc_bytes, nunits) ||
        (nunits >= 1 && right_bucket(new_alloc_bytes, nunits - 1)))
    {
        char *end;
        uint8_t *zz;

        end = (char *)mem + p->s_minfo.mi_nbytes;
        end -= 4;
        end[0] = 0;
        end[1] = 0;
        end[2] = 0;
        end[3] = 0;
        p->s_minfo.mi_nbytes = (uint32_t)n;
        mg.i = n;
        zz = (uint8_t *)mg.s;
        end = (char *)mem + p->s_minfo.mi_nbytes;
        *end++ = (char)*zz++;
        *end++ = (char)*zz++;
        *end++ = (char)*zz++;
        *end++ = (char)*zz++;
        return (mem);
    }

    /* delegate to compile-time selected mremap_implement */
    mresult = mremap_implement(mem, n, newunits, tocopy, flags, file, line, g, nunits);
    return (mresult);
}

t_addr ft_realloc(t_addr mem, size_t nbytes)
{
	ensure_init();
	return (internal_realloc(mem, nbytes, (char *)NULL, 0, 0));
}
