/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mremap_internal.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:54:12 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:56:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

/* round up to pages */
static size_t	pages_round_up(size_t bytes)
{
	size_t	p;

	p = (size_t)getpagesize();
	return (bytes + p - 1) & ~(p - 1);
}

#ifdef MREMAP_AVAILABLE

void	*ft_mremap_impl(void *oldptr, size_t oldlen, size_t newlen)
{
	void	*newptr;
	size_t	old_pages;
	size_t	new_pages;
	void	*addr;
	size_t	tocopy;

	old_pages = pages_round_up(oldlen);
	new_pages = pages_round_up(newlen);
	if (oldptr == NULL)
	{
		newptr = mmap(NULL, new_pages, PROT_READ | PROT_WRITE,
					MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		return ((newptr == MAP_FAILED) ? MAP_FAILED : newptr);
	}
	newptr = mremap(oldptr, old_pages, new_pages, MREMAP_MAYMOVE);
	if (newptr != MAP_FAILED)
		return (newptr);
	if (new_pages == old_pages)
		return (oldptr);
	if (new_pages < old_pages)
	{
		addr = (void *)((char *)oldptr + new_pages);
		if (munmap(addr, old_pages - new_pages) != 0)
			return (MAP_FAILED);
		return (oldptr);
	}
	newptr = mmap(NULL, new_pages, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (newptr == MAP_FAILED)
		return (MAP_FAILED);
	{
		tocopy = oldlen < newlen ? oldlen : newlen;
		if (tocopy)
			memcpy(newptr, oldptr, tocopy);
	}
	if (munmap(oldptr, old_pages) != 0)
	{
		munmap(newptr, new_pages);
		return (MAP_FAILED);
	}
	return (newptr);
}

#else

void	*ft_mremap_impl(void *oldptr, size_t oldlen, size_t newlen)
{
	void			*newptr;
	void			*addr;
	size_t			tocopy;
	const size_t	old_pages = pages_round_up(oldlen);
	const size_t	new_pages = pages_round_up(newlen);

	if (oldptr == NULL)
	{
		newptr = mmap(NULL, new_pages, PROT_READ | PROT_WRITE,
					MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		return (newptr == MAP_FAILED) ? MAP_FAILED : newptr;
	}
	if (new_pages == old_pages)
		return (oldptr);
	if (new_pages < old_pages)
	{
		addr = (void *)((char *)oldptr + new_pages);
		if (munmap(addr, old_pages - new_pages) != 0)
			return (MAP_FAILED);
		return (oldptr);
	}
	newptr = mmap(NULL, new_pages, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (newptr == MAP_FAILED)
		return (MAP_FAILED);
	{
		tocopy = oldlen < newlen ? oldlen : newlen;
		if (tocopy)
			memcpy(newptr, oldptr, tocopy);
	}
	if (munmap(oldptr, old_pages) != 0)
		return (munmap(newptr, new_pages), MAP_FAILED);
	return (newptr);
}

#endif