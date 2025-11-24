/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mremap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 01:48:13 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:25:03 by dlesieur         ###   ########.fr       */
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
	void	*newptr;
	size_t	old_pages;
	size_t	new_pages;

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
	void	*newptr;
	size_t	old_pages;
	size_t	new_pages;

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
void *
ft_mremap(void *oldptr, size_t oldlen, size_t newlen)
{
	return ft_mremap_impl(oldptr, oldlen, newlen);
}