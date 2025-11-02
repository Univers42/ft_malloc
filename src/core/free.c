/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:38:07 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:32:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef USE_MMAP
static int should_use_munmap(int nunits, int threshold)
{
	return (nunits > threshold);
}

static void handle_munmap(t_mhead *p, int nunits, t_glob *g)
{
	int o;

	o = errno;
	munmap(p, binsize(nunits));
	compute_stats_core(g, nunits, STAT_LESS_CORE, 0);
	errno = o;
}

static int try_munmap_free(t_mhead *p, int nunits, t_glob *g)
{
	int freed;

	freed = 0;
	if (should_use_munmap(nunits, MMAP_THRESHOLD))
	{
		handle_munmap(p, nunits, g);
		freed = 1;
	}
	return (freed);
}
#else
static int try_munmap_free(t_mhead *p, int nunits, t_glob *g)
{
	(void)p;
	(void)nunits;
	(void)g;
	return (0);
}
#endif

#ifdef USE_LESSCORE
static int should_lesscore(int nunits, char *p, char *memtop)
{
	if (nunits < LESSCORE_MIN)
		return (0);
	if ((char *)p + binsize(nunits) != memtop)
		return (0);
	return (1);
}

static int can_lesscore(int nunits, t_glob *g)
{
	if (nunits >= LESSCORE_FRC)
		return (1);
	if (g->busy[nunits] || g->nextf[nunits] != 0)
		return (1);
	return (0);
}

static void handle_lesscore_internal(int nunits, t_glob *g)
{
	int o;

	o = errno;
	lesscore(nunits);
	errno = o;
}

static int try_lesscore_free(t_mhead *p, int nunits, t_glob *g)
{
	int freed;

	freed = 0;
	if (should_lesscore(nunits, (char *)p, g->memtop))
	{
		if (can_lesscore(nunits, g))
		{
			handle_lesscore_internal(nunits, g);
			freed = 1;
		}
	}
	return (freed);
}
#else
static int try_lesscore_free(t_mhead *p, int nunits, t_glob *g)
{
	(void)p;
	(void)nunits;
	(void)g;
	return (0);
}
#endif

#ifdef MEMSCRAMBLE
static void scramble_memory(void *mem, uint32_t nbytes)
{
	if (nbytes)
		malloc_memset(mem, 0xcf, nbytes);
}

static void scramble_memory_if_enabled(void *mem, uint32_t nbytes)
{
	scramble_memory(mem, nbytes);
}
#else
static void scramble_memory_if_enabled(void *mem, uint32_t nbytes)
{
	(void)mem;
	(void)nbytes;
}
#endif

#if defined(MALLOC_TRACE) || defined(MALLOC_REGISTER) || defined(MALLOC_WATCH)
static int should_trace_or_register_or_watch(void)
{
	return (1);
}
#else
static int should_trace_or_register_or_watch(void)
{
	return (0);
}
#endif

static void trace_and_watch_setup(t_mhead *p, uint32_t *ubytes)
{
	if (should_trace_or_register_or_watch())
		*ubytes = p->s_minfo.mi_nbytes;
}

static void handle_memalign(t_addr *mem, char **ap, t_mhead **p)
{
	(void)mem;
	if ((*p)->s_minfo.mi_alloc == ISMEMALILGN)
	{
		*ap -= (*p)->s_minfo.mi_nbytes;
		*p = (t_mhead *)*ap - 1;
	}
}

static void validate_alloc_status(t_addr mem, t_mhead *p, const char *file, int line)
{
	if (p->s_minfo.mi_alloc != ISALLOC)
	{
		if (p->s_minfo.mi_alloc == ISFREE)
			xbotch(mem, ERR_DUPFREE, "free: called with already freed block argument", file, line);
		else
			xbotch(mem, ERR_UNALLOC, "free: called with unallocated block argument", file, line);
	}
	assert_or_abort(p->s_minfo.mi_magic2 == MAGIC2, "p->s_minfo.mi_magic2 == MAGIC2", file, line);
}

static void validate_magic8(t_mhead *p, t_addr mem, const char *file, int line)
{
	int i;
	uint8_t *z;

	i = 0;
	z = (uint8_t *)p->s_minfo.mi_magic8;
	while (i < MAGIC8_NUMBYTES)
	{
		if (*z++ != MAGIC1)
			xbotch(mem, ERR_UNDERFLOW, "free: underflow detected; magic8 corrupted", file, line);
		i++;
	}
}

static void validate_end_guard(char *ap, t_mhead *p, t_addr mem, const char *file, int line)
{
	t_mguard mg;
	char *z;
	char *ap_end;
	int i;

	ap_end = ap + p->s_minfo.mi_nbytes;
	z = (char *)mg.s;
	i = 0;
	while (i < 4)
	{
		*z++ = *ap_end++;
		i++;
	}
	if (mg.i != p->s_minfo.mi_nbytes)
		xbotch(mem, ERR_ASSERT_FAILED, "free: start and end chunk sizes differ", file, line);
}

static int handle_special_free_cases(t_mhead *p, int nunits, t_glob *g)
{
	int freed;

	freed = try_munmap_free(p, nunits, g);
	if (!freed)
		freed = try_lesscore_free(p, nunits, g);
	return (freed);
}

static void add_to_free_list(t_mhead *p, int nunits, t_glob *g)
{
	p->s_minfo.mi_alloc = ISFREE;
	g->busy[nunits] = 1;
	*chain_ptr(p) = g->nextf[nunits];
	g->nextf[nunits] = p;
	g->busy[nunits] = 0;
}

static void handle_busy_bucket(t_mhead *p, int nunits, t_glob *g)
{
	if (g->busy[nunits] == 1)
		xsplit(p, nunits);
}

#ifdef MALLOC_STATS
static void update_malloc_stats(int nunits, t_glob *g)
{
	g->mstats.nmalloc[nunits]--;
	g->mstats.nfre++;
}
#else
static void update_malloc_stats(int nunits, t_glob *g)
{
	(void)nunits;
	(void)g;
}
#endif

#ifdef MALLOC_TRACE
static void handle_malloc_trace(t_addr mem, uint32_t ubytes, int nunits, const char *file, int line, int flags, t_glob *g)
{
	if (g->malloc_trace && (flags & MALLOC_NOTRACE) == 0)
		mtrace_free(mem, ubytes, file, line);
	else if (g->malloc_trace_buckets[nunits])
		mtrace_free(mem, ubytes, file, line);
}
#else
static void handle_malloc_trace(t_addr mem, uint32_t ubytes, int nunits, const char *file, int line, int flags, t_glob *g)
{
	(void)mem;
	(void)ubytes;
	(void)nunits;
	(void)file;
	(void)line;
	(void)flags;
	(void)g;
}
#endif

#ifdef MALLOC_REGISTER
static void handle_malloc_register(t_addr mem, uint32_t ubytes, const char *file, int line, int flags, t_glob *g)
{
	if (g->malloc_register && (flags & MALLOC_NOREG) == 0)
		mregister_free(mem, ubytes, file, line);
}
#else
static void handle_malloc_register(t_addr mem, uint32_t ubytes, const char *file, int line, int flags, t_glob *g)
{
	(void)mem;
	(void)ubytes;
	(void)file;
	(void)line;
	(void)flags;
	(void)g;
}
#endif

#ifdef MALLOC_WATCH
static void handle_malloc_watch(t_addr mem, uint32_t ubytes, const char *file, int line, t_glob *g)
{
	if (g->_malloc_nwatch > 0)
		_malloc_ckwatch(mem, file, line, W_FREE, ubytes);
}
#else
static void handle_malloc_watch(t_addr mem, uint32_t ubytes, const char *file, int line, t_glob *g)
{
	(void)mem;
	(void)ubytes;
	(void)file;
	(void)line;
	(void)g;
}
#endif

static void finalize_free(t_addr mem, uint32_t ubytes, int nunits, const char *file, int line, int flags, t_glob *g)
{
	update_malloc_stats(nunits, g);
	handle_malloc_trace(mem, ubytes, nunits, file, line, flags, g);
	handle_malloc_register(mem, ubytes, file, line, flags, g);
	handle_malloc_watch(mem, ubytes, file, line, g);
}

t_addr internal_free(t_addr mem, const char *file, int line, int flags)
{
	t_mhead *p;
	char *ap;
	int nunits;
	uint32_t nbytes;
	uint32_t ubytes;
	int freed;
	t_glob *g;

	if (mem == 0)
		return (NULL);
	g = get_glob(GLOB_NONE, NULL);
	ap = (char *)mem;
	p = (t_mhead *)ap - 1;
	handle_memalign(&mem, &ap, &p);
	trace_and_watch_setup(p, &ubytes);
	validate_alloc_status(mem, p, file, line);
	nunits = p->s_minfo.mi_index;
	nbytes = allocated_bytes(p->s_minfo.mi_nbytes);
	if (in_bucket(nbytes, nunits) == 0)
		xbotch(mem, ERR_UNDERFLOW, "free: underflow detected; mh_nbytes out of range", file, line);
	validate_magic8(p, mem, file, line);
	validate_end_guard(ap, p, mem, file, line);
	freed = handle_special_free_cases(p, nunits, g);
	if (!freed)
	{
		scramble_memory_if_enabled(mem, p->s_minfo.mi_nbytes);
		assert_or_abort(nunits < NBUCKETS, "nunits < NBUCKETS", file, line);
		handle_busy_bucket(p, nunits, g);
		if (g->busy[nunits] != 1)
			add_to_free_list(p, nunits, g);
	}
	finalize_free(mem, ubytes, nunits, file, line, flags, g);
	return (NULL);
}