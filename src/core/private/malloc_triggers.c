/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_triggers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:57:37 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 12:13:44 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

#ifdef MEMSCRAMBLE
void scramble_allocated_memory(void *mem, size_t n)
{
    if (n)
        malloc_memset((char *)mem, 0xdf, n);
}
#else
void scramble_allocated_memory(void *mem, size_t n)
{
    (void)mem;
    (void)n;
}
#endif

#ifdef MALLOC_STATS
void update_recurse_stats(t_glob *g, int nunits)
{
    if (g->busy[nunits])
        g->mstats.nrecurse++;
}

void update_alloc_stats(t_glob *g, int nunits, size_t n)
{
    g->mstats.nmalloc[nunits]++;
    g->mstats.tmalloc[nunits]++;
    g->mstats.nmal++;
    g->mstats.bytesreq += n;
}
#else
void update_recurse_stats(t_glob *g, int nunits)
{
    (void)g;
    (void)nunits;
}

void update_alloc_stats(t_glob *g, int nunits, size_t n)
{
    (void)g;
    (void)nunits;
    (void)n;
}
#endif

#ifdef MALLOC_TRACE
void handle_malloc_trace_alloc(t_addr ptr, size_t n, int nunits, const char *file, int line, int flags, t_glob *g)
{
    if (g->malloc_trace && (flags & MALLOC_NOTRACE) == 0)
        mtrace_alloc("malloc", ptr, n, file, line);
    else if (g->malloc_trace_buckets[nunits])
        mtrace_alloc("malloc", ptr, n, file, line);
}
#else
void handle_malloc_trace_alloc(t_addr ptr, size_t n, int nunits, const char *file, int line, int flags, t_glob *g)
{
    (void)ptr;
    (void)n;
    (void)nunits;
    (void)file;
    (void)line;
    (void)flags;
    (void)g;
}
#endif

#ifdef MALLOC_REGISTER
void handle_malloc_register_alloc(t_addr ptr, size_t n, const char *file, int line, int flags, t_glob *g)
{
    if (g->malloc_register && (flags & MALLOC_NOREG) == 0)
        mregister_alloc("malloc", ptr, n, file, line);
}
#else
void handle_malloc_register_alloc(t_addr ptr, size_t n, const char *file, int line, int flags, t_glob *g)
{
    (void)ptr;
    (void)n;
    (void)file;
    (void)line;
    (void)flags;
    (void)g;
}
#endif

#ifdef MALLOC_WATCH
void handle_malloc_watch_alloc(t_addr ptr, size_t n, const char *file, int line, t_glob *g)
{
    if (g->_malloc_nwatch > 0)
        _malloc_ckwatch(ptr, file, line, W_ALLOC, n);
}
#else
void handle_malloc_watch_alloc(t_addr ptr, size_t n, const char *file, int line, t_glob *g)
{
    (void)ptr;
    (void)n;
    (void)file;
    (void)line;
    (void)g;
}
#endif

#if defined(MALLOC_DEBUG)
void check_alignment(t_addr ptr, t_mhead *p, const char *file, int line)
{
    char *z;

    z = (char *)ptr;
    if ((unsigned long)z & MALIGN_MASK)
        fprintf(stderr, "malloc: %s:%d: warning: request for %d bytes not aligned on %d byte boundary\r\n",
                file ? file : "unknown", line, p->s_minfo.mi_nbytes, MALIGN_MASK + 1);
}
#else
void check_alignment(t_addr ptr, t_mhead *p, const char *file, int line)
{
    (void)ptr;
    (void)p;
    (void)file;
    (void)line;
}
#endif

#if SIZEOF_SIZE_T == 8
int check_max_alloc_size(size_t n)
{
    if (allocated_bytes(n) > MAXALLOC_SIZE)
        return (0);
    return (1);
}
#else
int check_max_alloc_size(size_t n)
{
    (void)n;
    return (1);
}
#endif

/* helper used by free path to decide whether to sample trace/register/watch */
int should_trace_or_register_or_watch(void)
{
#if defined(MALLOC_TRACE) || defined(MALLOC_REGISTER) || defined(MALLOC_WATCH)
    return (1);
#else
    return (0);
#endif
}