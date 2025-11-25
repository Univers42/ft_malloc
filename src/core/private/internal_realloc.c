/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal_realloc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:30:34 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 14:47:52 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

/* round up to pages */
#if USE_MREMAP == 1
static size_t
pages_round_up(size_t bytes)
{
    size_t p;

    p = (size_t)getpagesize();
    return (bytes + p - 1) & ~(p - 1);
}
#endif

/*
 * Removed local mremap_implement definitions (old multi-arg variants)
 * They conflicted with the new single-arg prototype in private.h.
 * Use the centralized implementation instead.
 */

/* external single-argument declaration (defined in utils/ft_mremap.c) */
extern t_addr mremap_implement(t_mremap_params *params);

/* simple assert replacement */
void assert_or_abort(int cond, const char *expr, const char *file, int line)
{
    if (!cond)
    {
        set_state_mem((t_addr)0);
        xbotch(ERR_ASSERT_FAILED, (char *)expr, file, line);
    }
}

/* compact internal_realloc that computes nunits/newunits correctly
 * and delegates to the shared mremap implementation.
 */
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
    t_addr mresult;

    g = get_glob(GLOB_NONE, NULL);
    if (!g)
        return ((t_addr)0);

    /* handle simple cases */
    if (n == 0)
    {
        internal_free(mem, file, line, MALLOC_INTERNAL);
        return ((t_addr)0);
    }
    if (mem == (t_addr)0)
        return (internal_malloc(n, file, line, MALLOC_INTERNAL));

    /* derive current block info from header */
    /* support memalign: input 'mem' may be an aligned pointer whose header
     * stores ISMEMALILGN and an offset to the real (original) payload.
     * In that case rewind 'ap' by the stored offset and use the canonical
     * header/payload for all subsequent operations.
     */
    {
        char *ap = (char *)mem;
        p = (t_mhead *)ap - 1;
        if (p->s_minfo.mi_alloc == ISMEMALILGN)
        {
            /* p->s_minfo.mi_nbytes holds the distance from aligned->original */
            ap -= p->s_minfo.mi_nbytes;
            p = (t_mhead *)ap - 1;
            mem = (t_addr)(p + 1); /* canonical payload */
        }
    }
    tocopy = p->s_minfo.mi_nbytes;
    if (n < (size_t)tocopy)
        tocopy = (uint32_t)n;
    old_alloc_bytes = allocated_bytes((size_t)p->s_minfo.mi_nbytes);
    new_alloc_bytes = allocated_bytes((size_t)n);
    nunits = (int)p->s_minfo.mi_index;
    assert_or_abort(nunits < NBUCKETS, "nunits < NBUCKETS", file, line);

    /* compute target bucket (newunits) */
    if (new_alloc_bytes > old_alloc_bytes)
        newunits = nunits;
    else
    {
        size_t half_page = (size_t)(g->pagesz >> 1);
        newunits = (new_alloc_bytes <= half_page) ? STARTBUCK : g->pagebucket;
    }
    while (newunits < (int)NBUCKETS && new_alloc_bytes > g->binsizes[(size_t)newunits])
        newunits++;

    /* delegate to shared implementation */
    {
        t_mremap_params params;

        params.mem = mem;
        params.n = n;
        params.newunits = newunits;
        params.tocopy = tocopy;
        params.flags = flags;
        params.file = file;
        params.line = line;
        params.g = g;
        params.nunits = nunits;
        mresult = mremap_implement(&params);
    }
    return (mresult);
}
