/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mremap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 01:48:13 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 12:13:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>

/* round up to pages */
static size_t
pages_round_up(size_t bytes)
{
	size_t p;

	p = (size_t)getpagesize();
	return (bytes + p - 1) & ~(p - 1);
}

#if defined(MREMAP_AVAILABLE)
/* Implementation that prefers kernel mremap. */
static void *
ft_mremap_impl(void *oldptr, size_t oldlen, size_t newlen)
{
	void *newptr;
	size_t old_pages;
	size_t new_pages;

	old_pages = pages_round_up(oldlen);
	new_pages = pages_round_up(newlen);

	if (oldptr == NULL)
	{
		newptr = mmap(NULL, new_pages, PROT_READ | PROT_WRITE,
					  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		return (newptr == MAP_FAILED) ? MAP_FAILED : newptr;
	}

	/* Try kernel mremap (may move). */
	newptr = mremap(oldptr, old_pages, new_pages, MREMAP_MAYMOVE);
	if (newptr != MAP_FAILED)
		return newptr;

	/* Fall back to safe behaviour below. */
	if (new_pages == old_pages)
		return oldptr;

	if (new_pages < old_pages)
	{
		void *addr = (void *)((char *)oldptr + new_pages);

		if (munmap(addr, old_pages - new_pages) != 0)
			return MAP_FAILED;
		return oldptr;
	}

	newptr = mmap(NULL, new_pages, PROT_READ | PROT_WRITE,
				  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (newptr == MAP_FAILED)
		return MAP_FAILED;

	{
		size_t tocopy = oldlen < newlen ? oldlen : newlen;

		if (tocopy)
			memcpy(newptr, oldptr, tocopy);
	}

	if (munmap(oldptr, old_pages) != 0)
	{
		munmap(newptr, new_pages);
		return MAP_FAILED;
	}

	return newptr;
}
#else
/* Full portable fallback: no kernel mremap used. */
static void *ft_mremap_impl(void *oldptr, size_t oldlen, size_t newlen)
{
	void *newptr;
	size_t old_pages;
	size_t new_pages;

	old_pages = pages_round_up(oldlen);
	new_pages = pages_round_up(newlen);

	if (oldptr == NULL)
	{
		newptr = mmap(NULL, new_pages, PROT_READ | PROT_WRITE,
					  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		return (newptr == MAP_FAILED) ? MAP_FAILED : newptr;
	}

	if (new_pages == old_pages)
		return oldptr;

	if (new_pages < old_pages)
	{
		void *addr = (void *)((char *)oldptr + new_pages);

		if (munmap(addr, old_pages - new_pages) != 0)
			return MAP_FAILED;
		return oldptr;
	}

	newptr = mmap(NULL, new_pages, PROT_READ | PROT_WRITE,
				  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (newptr == MAP_FAILED)
		return MAP_FAILED;

	{
		size_t tocopy = oldlen < newlen ? oldlen : newlen;

		if (tocopy)
			memcpy(newptr, oldptr, tocopy);
	}

	if (munmap(oldptr, old_pages) != 0)
	{
		munmap(newptr, new_pages);
		return MAP_FAILED;
	}

	return newptr;
}
#endif

/* public wrapper (no inner compile-time branching) */
void *ft_mremap(void *oldptr, size_t oldlen, size_t newlen)
{
	return ft_mremap_impl(oldptr, oldlen, newlen);
}

/* Provide mremap_implement symbol used by internal_realloc.
 * It prefers ft_mremap when available; otherwise falls back to
 * allocate-copy-free (internal_malloc + fastcopy + internal_free).
 */
t_addr
mremap_implement(t_addr mem, size_t n, int newunits, uint32_t tocopy,
				 int flags, const char *file, int line, t_glob *g, int nunits)
{
	t_mhead *p;
	void *old_payload;
	void *remap_ret;
	size_t old_alloc_bytes;
	size_t new_alloc_bytes;
	t_mhead *np;

	(void)flags;
	(void)nunits;
	if (!mem || !g)
		return ((t_addr)0);

	p = (t_mhead *)mem - 1;
	old_payload = (void *)(p + 1);
	old_alloc_bytes = allocated_bytes((size_t)p->s_minfo.mi_nbytes);
	new_alloc_bytes = allocated_bytes((size_t)n);

	/* Try ft_mremap (may be a wrapper around kernel mremap or malloc remap) */
	remap_ret = ft_mremap(old_payload, old_alloc_bytes, new_alloc_bytes);
	if (remap_ret == MAP_FAILED)
	{
		/* fallback: allocate-copy-free */
		t_addr newaddr;

		newaddr = internal_malloc(n, file, line,
								  MALLOC_INTERNAL | MALLOC_NOTRACE | MALLOC_NOREG);
		if (newaddr == (t_addr)0)
			return ((t_addr)0);
		fastcopy((void *)mem, (void *)newaddr, tocopy);
		internal_free(mem, file, line, MALLOC_INTERNAL);
		compute_stats_realloc_copy(g);
		return (newaddr);
	}

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