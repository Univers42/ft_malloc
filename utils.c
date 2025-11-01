/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 19:10:54 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 18:37:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "get_page_size.h"
#include <stdio.h>
#include <unistd.h>

/* Use size_t for bitwise operations to ensure an integral type,
	   then cast the result back to int for the function return. */
size_t allocated_bytes (size_t n)
{	
	return (((size_t)n + (size_t)mover_head + (size_t)MSLOP + (size_t)MALIGN_MASK) & ~(size_t)MALIGN_MASK);
}

/**
 * read teh pointer stored at ((char *)a + sizeof(char))
 */
t_mhead *chain(t_mhead *a)
{
	return (*(t_mhead **)((char *)a + sizeof(char *)));
}

/**
 * if we need to reassing thruoght the same location (i.e)
 * we would need to use a lvalue, so I use chain_ptr(a) to obtain
 * t_mhead** and write through it:
 * `*chain_ptr(a) = some_ptr;`
 */
t_mhead **chain_ptr(t_mhead *a)
{
	return (t_mhead **)((char *a) + sizeof(char *));
}

#ifdef MALLOC_STATS
void malloc_stats_sbrk_update(long sbrk_needed)
{
    _mstats.nsbrk++;
    _mstats.tsbrk += sbrk_needed;
}
#else
void malloc_stats_sbrk_update(long sbrk_needed)
{
    curbrk = sbrk(sbrk_needed);
}
#endif

int pagealign(void)
{
	int     nunits;
	t_mhead *mp;
	long    sbrk_needed;
	char    *curbrk;
	t_glob  g;

	g.pagesz = get_page_size();
	if (g.pagesz < MALLOC_PAGESIZE_MIN)
		g.pagesz = MALLOC_PAGESIZE_MIN;
	g.memtop = sbrk(0);
	curbrk = sbrk(0);
	sbrk_needed = g.pagesz - ((long)curbrk & (g.pagesz - 1));
	if (sbrk_needed < 0)
		sbrk_needed += g.pagesz;
	if (sbrk_needed)
	{
		malloc_stats_sbrk_update(sbrk_needed);
		curbrk = sbrk(sbrk_needed);
		if ((long)curbrk == -1)
			return (-1);
		g.memtop += sbrk_needed;
		curbrk += sbrk_needed & (PREPOP_SIZE - 1);
		sbrk_needed -= sbrk_needed & (PREPOP_SIZE - 1);
		nunits = sbrk_needed / PREPOP_SIZE;
		
	}
	nunits = 7;
	while (++nunits < NBUCKETS)
		if (g.pagesz <= binsize(nunits))
			break ;
		g.pagebucket = nunits;
	return (0);
}

#ifdef MALLOC_REGISTER
void register_malloc_impl(t_addr mem, int e, const char *s, int line)
{
	if (mem != (t_addr)0 && malloc_register)
		mregister_describe_mem(mem, stderr);
}
#else
void register_malloc(t_addr mem, int e, const char *s, const char *file, int line)
{
	(void)mem;
	(void)e;
	(void)s;
	(void)file;
	(void)line;
	/* No-op whne malloc register is not defined. */
}
#endif

/**
 * low-lvl assserrtion handler. it repors  a failed assertion msg
 * and then terminated using the function abrt. it is the final
 * fail-stop used by the allocator when an internal invariant is violated
 */
void	botch(const char *s, const char *file, int line)
{
	dprintf(STDERR_FILENO, "malloc: failed assertion: %s\n", s);
	(void)fflush(stderr);
	abort();
}

/**
 * xbotch is an extended version asssertion handler. it prints  a more detailed
 * message that includes  file and line, optionally describe the offending allocation
 * (via the MALLOC_REGISTER helper), flushes stderr, tehn delegates to botch
 *	which termate. So we use xbotch when we want extra context about a memory error
 * botch does the actual shutdown/reporting
 */
void	xbotch(t_addr mem, int e, const char *s, const char *file, int line)
{
	dprintf(STDERR_FILENO, _("\r\nmalloc: %s:%d: assertion botched\r\n"),
			file ? file : _("unknown"), line);
	register_malloc_impl(mem, e, s, file, line);
	(void)fflush(stderr);
	botch(s, file, line);
}
#ifdef MALLOC_DEBUG

#else
#endif


/**
 * Split a  block at index > NU (but less than SPLIT_MAX) into a set of
 * blocks of the correct size and attach them to enxtf[NU]. nextf[NU]
 * is assumed to be empty. Must be called with signals blocked by morecore
 * BUSY[NU] must be set to 1
 */

