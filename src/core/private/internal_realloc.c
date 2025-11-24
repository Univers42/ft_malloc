/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal_realloc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:30:34 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:30:40 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

/* Ensure singleton accessor prototypes are visible in this TU to avoid
 * implicit-function-declaration errors when calling set_state_mem/get_state_mem.
 */
void set_state_mem(t_addr mem);
t_addr get_state_mem(void);

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
    t_mhead *p;
    void *old_payload;
    void *remap_ret;
    size_t old_alloc_bytes;
    size_t new_alloc_bytes;
    size_t old_pages;
    size_t new_pages;
    t_mhead *np;

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
    t_mhead *p;
    void *old_payload;
    void *remap_ret;
    size_t old_alloc_bytes;
    size_t new_alloc_bytes;
    t_mhead *np;

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
    t_addr newaddr;

    (void)newunits;
    (void)flags;
    (void)nunits;
    (void)g;

    /* always fall back to allocate-copy-free */
    newaddr = internal_malloc(n, file, line, MALLOC_INTERNAL | MALLOC_NOTRACE | MALLOC_NOREG);
    if (newaddr == (t_addr)0)
        return ((t_addr)0);
    fastcopy((void *)mem, (void *)newaddr, tocopy);
    internal_free(mem, file, line, MALLOC_INTERNAL);
    compute_stats_realloc_copy(g);
    return (newaddr);
}

#endif /* USE_MREMAP */

/* simple assert replacement */
void assert_or_abort(int cond, const char *expr, const char *file, int line)
{
    if (!cond)
    {
        set_state_mem((t_addr)0);
        xbotch(ERR_ASSERT_FAILED, (char *)expr, file, line);
    }
}
