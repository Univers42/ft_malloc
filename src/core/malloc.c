/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 18:56:10 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:30:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MEMSCRAMBLE
static void scramble_allocated_memory(void *mem, size_t n)
{
	if (n)
		malloc_memset((char *)mem, 0xdf, n);
}
#else
static void scramble_allocated_memory(void *mem, size_t n)
{
	(void)mem;
	(void)n;
}
#endif

#ifdef MALLOC_STATS
static void update_recurse_stats(t_glob *g, int nunits)
{
	if (g->busy[nunits])
		g->mstats.nrecurse++;
}

static void update_alloc_stats(t_glob *g, int nunits, size_t n)
{
	g->mstats.nmalloc[nunits]++;
	g->mstats.tmalloc[nunits]++;
	g->mstats.nmal++;
	g->mstats.bytesreq += n;
}
#else
static void update_recurse_stats(t_glob *g, int nunits)
{
	(void)g;
	(void)nunits;
}

static void update_alloc_stats(t_glob *g, int nunits, size_t n)
{
	(void)g;
	(void)nunits;
	(void)n;
}
#endif

#ifdef MALLOC_TRACE
static void handle_malloc_trace_alloc(t_addr ptr, size_t n, int nunits, const char *file, int line, int flags, t_glob *g)
{
	if (g->malloc_trace && (flags & MALLOC_NOTRACE) == 0)
		mtrace_alloc("malloc", ptr, n, file, line);
	else if (g->malloc_trace_buckets[nunits])
		mtrace_alloc("malloc", ptr, n, file, line);
}
#else
static void handle_malloc_trace_alloc(t_addr ptr, size_t n, int nunits, const char *file, int line, int flags, t_glob *g)
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
static void handle_malloc_register_alloc(t_addr ptr, size_t n, const char *file, int line, int flags, t_glob *g)
{
	if (g->malloc_register && (flags & MALLOC_NOREG) == 0)
		mregister_alloc("malloc", ptr, n, file, line);
}
#else
static void handle_malloc_register_alloc(t_addr ptr, size_t n, const char *file, int line, int flags, t_glob *g)
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
static void handle_malloc_watch_alloc(t_addr ptr, size_t n, const char *file, int line, t_glob *g)
{
	if (g->_malloc_nwatch > 0)
		_malloc_ckwatch(ptr, file, line, W_ALLOC, n);
}
#else
static void handle_malloc_watch_alloc(t_addr ptr, size_t n, const char *file, int line, t_glob *g)
{
	(void)ptr;
	(void)n;
	(void)file;
	(void)line;
	(void)g;
}
#endif

#if defined(MALLOC_DEBUG)
static void check_alignment(t_addr ptr, t_mhead *p, const char *file, int line)
{
	char *z;

	z = (char *)ptr;
	if ((unsigned long)z & MALIGN_MASK)
		fprintf(stderr, "malloc: %s:%d: warning: request for %d bytes not aligned on %d byte boundary\r\n",
				file ? file : "unknown", line, p->s_minfo.mi_nbytes, MALIGN_MASK + 1);
}
#else
static void check_alignment(t_addr ptr, t_mhead *p, const char *file, int line)
{
	(void)ptr;
	(void)p;
	(void)file;
	(void)line;
}
#endif

#if SIZEOF_SIZE_T == 8
static int check_max_alloc_size(size_t n)
{
	if (allocated_bytes(n) > MAXALLOC_SIZE)
		return (0);
	return (1);
}
#else
static int check_max_alloc_size(size_t n)
{
	(void)n;
	return (1);
}
#endif

static int calculate_nunits(uint32_t nbytes, t_glob *g)
{
	int nunits;
	uint32_t half_page;

	half_page = (uint32_t)(g->pagesz >> 1);
	if (nbytes <= half_page)
		nunits = STARTBUCK;
	else
		nunits = g->pagebucket;
	while (nunits < NBUCKETS)
	{
		if (nbytes <= binsize(nunits))
			break;
		nunits++;
	}
	return (nunits);
}

static int handle_reentrant(int nunits, t_glob *g)
{
	update_recurse_stats(g, nunits);
	while (g->busy[nunits])
		nunits++;
	g->busy[nunits] = 1;
	if (nunits > g->maxbuck)
		g->maxbuck = nunits;
	return (nunits);
}

static t_mhead *get_block_from_freelist(int nunits, t_glob *g)
{
	t_mhead *p;

	if (g->nextf[nunits] == 0)
		morecore(nunits);
	p = g->nextf[nunits];
	if (p == NULL)
	{
		g->busy[nunits] = 0;
		return (NULL);
	}
	g->nextf[nunits] = *chain_ptr(p);
	g->busy[nunits] = 0;
	return (p);
}

static void validate_free_block(t_mhead *p, int nunits, const char *file, int line)
{
	if (p->s_minfo.mi_alloc != ISFREE || p->s_minfo.mi_index != nunits)
		xbotch((t_addr)(p + 1), 0, "malloc: block on free list clobbered", file, line);
}

static void setup_block_header(t_mhead *p, size_t n)
{
	p->s_minfo.mi_alloc = ISALLOC;
	p->s_minfo.mi_magic2 = MAGIC2;
	p->s_minfo.mi_nbytes = n;
	malloc_memset((char *)p->s_minfo.mi_magic8, MAGIC1, MAGIC8_NUMBYTES);
}

static void setup_end_guard(t_mhead *p, size_t n)
{
	t_mguard mg;
	uint8_t *z;
	char *m;

	mg.i = n;
	z = (uint8_t *)mg.s;
	m = (char *)(p + 1) + n;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
}

static void finalize_alloc(t_addr ptr, t_mhead *p, size_t n, int nunits, const char *file, int line, int flags, t_glob *g)
{
	scramble_allocated_memory(ptr, n);
	update_alloc_stats(g, nunits, n);
	handle_malloc_trace_alloc(ptr, n, nunits, file, line, flags, g);
	handle_malloc_register_alloc(ptr, n, file, line, flags, g);
	handle_malloc_watch_alloc(ptr, n, file, line, g);
	check_alignment(ptr, p, file, line);
}

t_addr internal_malloc(size_t n, const char *file, int line, int flags)
{
	t_mhead *p;
	int nunits;
	uint32_t nbytes;
	t_addr ptr;
	t_glob *g;

	g = get_glob(GLOB_NONE, NULL);
	if (g->pagesz == 0)
		if (pagealign() < 0)
			return ((t_addr)NULL);
	if (check_max_alloc_size(n) == 0)
		return ((t_addr)NULL);
	nbytes = allocated_bytes(n);
	nunits = calculate_nunits(nbytes, g);
	if (nunits >= NBUCKETS)
		return ((t_addr)NULL);
	nunits = handle_reentrant(nunits, g);
	p = get_block_from_freelist(nunits, g);
	if (p == NULL)
		return (NULL);
	validate_free_block(p, nunits, file, line);
	setup_block_header(p, n);
	setup_end_guard(p, n);
	ptr = (t_addr)(p + 1);
	finalize_alloc(ptr, p, n, nunits, file, line, flags, g);
	return (ptr);
}