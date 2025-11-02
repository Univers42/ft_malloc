/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:38:30 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 13:08:30 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

/* round up to pages */
static size_t
pages_round_up(size_t bytes)
{
    size_t p;

    p = (size_t)getpagesize();
    return (bytes + p - 1) & ~(p - 1);
}

/*
 * mremap_implement - three compile-time variants
 *
 * Signature:
 *   mremap_implement(mem, n, newunits, tocopy, flags, file, line, g, nunits)
 *
 * Returns new payload address (t_addr) on success, 0 / (t_addr)0 on failure.
 *
 * Note: compile-time selection is done by the preprocessor so there is no
 * runtime branching inside the function body.
 */

#if USE_MREMAP == 1 /* use kernel mremap */

static t_addr
mremap_implement(t_addr mem, size_t n, int newunits, uint32_t tocopy,
                 int flags, const char *file, int line, t_glob *g, int nunits)
{
    t_mhead		*p;
    void		*old_payload;
    void		*remap_ret;
    size_t		old_alloc_bytes;
    size_t		new_alloc_bytes;
    size_t		old_pages;
    size_t		new_pages;
    t_mhead		*np;

    if (!mem || !g)
        return ((t_addr)0);

    p = (t_mhead *)mem - 1;
    old_payload = (void *)(p + 1);
    old_alloc_bytes = ALLOCATED_BYTES((size_t)p->s_minfo.mi_nbytes);
    new_alloc_bytes = ALLOCATED_BYTES(n);

    old_pages = pages_round_up(old_alloc_bytes);
    new_pages = pages_round_up(new_alloc_bytes);

    remap_ret = mremap(old_payload, old_pages, new_pages, MREMAP_MAYMOVE);
    if (remap_ret == MAP_FAILED)
        return ((t_addr)0);

    np = (t_mhead *)remap_ret - 1;
    np->s_minfo.mi_alloc = ISALLOC;
    np->s_minfo.mi_index = (char)newunits;
    np->s_minfo.mi_nbytes = (uint32_t)n;
    np->s_minfo.mi_magic2 = MAGIC2;
    malloc_memset((char *)np->s_minfo.mi_magic8, MAGIC1, MAGIC8_NUMBYTES);

    /* stats */
    compute_stats_core(g, newunits, STAT_MMAP, new_alloc_bytes);

    return ((t_addr)(np + 1));
}

#elif USE_MREMAP == 2 /* use custom ft_mremap implementation (provided elsewhere) */

extern void *ft_mremap(void *oldptr, size_t oldlen, size_t newlen);

static t_addr
mremap_implement(t_addr mem, size_t n, int newunits, uint32_t tocopy,
                 int flags, const char *file, int line, t_glob *g, int nunits)
{
    t_mhead		*p;
    void		*old_payload;
    void		*remap_ret;
    size_t		old_alloc_bytes;
    size_t		new_alloc_bytes;
    t_mhead		*np;

    if (!mem || !g)
        return ((t_addr)0);

    p = (t_mhead *)mem - 1;
    old_payload = (void *)(p + 1);
    old_alloc_bytes = ALLOCATED_BYTES((size_t)p->s_minfo.mi_nbytes);
    new_alloc_bytes = ALLOCATED_BYTES(n);

    remap_ret = ft_mremap(old_payload, old_alloc_bytes, new_alloc_bytes);
    if (remap_ret == MAP_FAILED)
        return ((t_addr)0);

    np = (t_mhead *)remap_ret - 1;
    np->s_minfo.mi_alloc = ISALLOC;
    np->s_minfo.mi_index = (char)newunits;
    np->s_minfo.mi_nbytes = (uint32_t)n;
    np->s_minfo.mi_magic2 = MAGIC2;
    malloc_memset((char *)np->s_minfo.mi_magic8, MAGIC1, MAGIC8_NUMBYTES);

    /* stats */
    compute_stats_core(g, newunits, STAT_MMAP, new_alloc_bytes);

    return ((t_addr)(np + 1));
}

#else /* fallback: allocate-copy-free (original slow path) */

static t_addr
mremap_implement(t_addr mem, size_t n, int newunits, uint32_t tocopy,
                 int flags, const char *file, int line, t_glob *g, int nunits)
{
    t_addr		newaddr;

    (void)newunits;
    (void)flags;
    (void)file;
    (void)line;
    (void)g;
    (void)nunits;

    /* always fall back to allocate-copy-free */
    newaddr = internal_malloc(n, file, line, MALLOC_INTERNAL | MALLOC_NOTRACE | MALLOC_NOREG);
    if (newaddr == (t_addr)0)
        return ((t_addr)0);
    FASTCOPY((void *)mem, (void *)newaddr, tocopy);
    internal_free(mem, file, line, MALLOC_INTERNAL);
#ifdef MALLOC_STATS
    /* increment copy counter if stats are enabled */
    /* get_glob may be expensive here; caller often has g, but keep it simple */
    t_glob *lg = get_glob(GLOB_NONE, NULL);
    if (lg)
        lg->mstats.nrcopy++;
#endif
    return (newaddr);
}

#endif /* USE_MREMAP */

/* simple assert replacement */
void
assert_or_abort(int cond, const char *expr, const char *file, int line)
{
    if (!cond)
        xbotch((t_addr)0, ERR_ASSERT_FAILED, (char *)expr, file, line);
}

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
    t_mhead		*p;
    uint32_t	tocopy;
    size_t		old_alloc_bytes;
    size_t		new_alloc_bytes;
    int			nunits;
    int			newunits;
    t_glob		*g;
    int			i;
    t_mguard	mg;
    char		*z;
    char		*m;
    t_addr		mresult;

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
        xbotch(mem, ERR_UNALLOC, "realloc: called with unallocated block argument", file, line);
    assert_or_abort(p->s_minfo.mi_magic2 == MAGIC2, "p->s_minfo.mi_magic2 == MAGIC2", file, line);

    /* underflow / magic8 check */
    i = 0;
    z = p->s_minfo.mi_magic8;
    while (i < MAGIC8_NUMBYTES)
    {
        if (*z++ != MAGIC1)
            xbotch(mem, ERR_UNDERFLOW, "realloc: underflow detected; magic8 corrupted", file, line);
        i++;
    }

    /* bytes to copy */
    tocopy = p->s_minfo.mi_nbytes;
    if (n < (size_t)tocopy)
        tocopy = (uint32_t)n;

    /* guard snapshot */
    m = (char *)mem + tocopy;
    z = mg.s;
    *z++ = *m++; *z++ = *m++; *z++ = *m++; *z++ = *m++;
    if (mg.i != p->s_minfo.mi_nbytes)
        xbotch(mem, ERR_ASSERT_FAILED, _("realloc: start and end chunk sizes differ"), file, line);

    /* no change */
    if (n == p->s_minfo.mi_nbytes)
        return (mem);

    old_alloc_bytes = ALLOCATED_BYTES((size_t)p->s_minfo.mi_nbytes);
    new_alloc_bytes = ALLOCATED_BYTES((size_t)n);

    nunits = (int)p->s_minfo.mi_index;
    assert_or_abort(nunits < NBUCKETS, "nunits < NBUCKETS", file, line);

    /* compute newunits (while-only) */
    if (new_alloc_bytes > old_alloc_bytes)
        newunits = nunits;
    else
        newunits = (new_alloc_bytes <= (g->pagesz >> 1)) ? STARTBUCK : g->pagebucket;
    while (newunits < (int)NBUCKETS && new_alloc_bytes > g->binsizes[(size_t)newunits])
        newunits++;

    /* quick in-place adjust when same/adjacent bucket */
    if (right_bucket(new_alloc_bytes, nunits) ||
        (nunits >= 1 && right_bucket(new_alloc_bytes, nunits) - 1))
    {
        char *end;

        end = (char *)mem + p->s_minfo.mi_nbytes;
        end -= 4;
        end[0] = 0; end[1] = 0; end[2] = 0; end[3] = 0;
        p->s_minfo.mi_nbytes = (uint32_t)n;
        mg.i = n;
        z = mg.s;
        end = (char *)mem + p->s_minfo.mi_nbytes;
        *end++ = *z++; *end++ = *z++; *end++ = *z++; *end++ = *z++;
        return (mem);
    }

    /* delegate to compile-time selected mremap_implement */
    mresult = mremap_implement(mem, n, newunits, tocopy, flags, file, line, g, nunits);
    return (mresult);
}