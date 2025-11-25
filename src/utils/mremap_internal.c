/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mremap_internal.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:54:12 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 14:26:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>

void	*alloc_new_mapping(size_t new_pages);
size_t	pages_round_up(size_t bytes);

/* allocate a new mapping and copy up to 'tocopy' bytes, freeing old mapping */
void	*alloc_and_copy_fallback(void *oldptr, size_t old_pages,
						size_t new_pages, size_t tocopy)
{
	void	*newptr;

	newptr = alloc_new_mapping(new_pages);
	if (newptr == MAP_FAILED)
		return (MAP_FAILED);
	if (tocopy)
		memcpy(newptr, oldptr, tocopy);
	if (munmap(oldptr, old_pages) != 0)
	{
		munmap(newptr, new_pages);
		return (MAP_FAILED);
	}
	return (newptr);
}

/* unmap the tail when shrinking in-place */
int	shrink_unmap_tail(void *oldptr, size_t old_pages, size_t new_pages)
{
	void	*addr;

	addr = (void *)((char *)oldptr + new_pages);
	if (munmap(addr, old_pages - new_pages) != 0)
		return (0);
	return (1);
}

#ifdef MREMAP_AVAILABLE

/* try kernel mremap (may move) */
void	*try_kernel_mremap(void *oldptr, size_t old_pages, size_t new_pages)
{
	void	*ret;

	ret = mremap(oldptr, old_pages, new_pages, MREMAP_MAYMOVE);
	return (ret);
}

/* prefer kernel mremap; fall back to safe allocate-copy-free behavior */
void	*ft_mremap_impl(void *oldptr, size_t oldlen, size_t newlen)
{
	size_t			tocopy;
	const size_t	old_pages = pages_round_up(oldlen);
	const size_t	new_pages = pages_round_up(newlen);

	if (oldptr == NULL)
		return (alloc_new_mapping(new_pagese));
	newptr = try_kernel_mremap(oldptr, old_pages, new_pages);
	if (newptr != MAP_FAILED)
		return (newptr);
	if (new_pages == old_pages)
		return (oldptr);
	if (new_pages < old_pages)
	{
		if (!shrink_unmap_tail(oldptr, old_pages, new_pages))
			return (MAP_FAILED);
		return (oldptr);
	}
	if (oldlen < newlen)
		tocopy = oldlen;
	else
		tocopy = newlen;
	return (alloc_and_copy_fallback(oldptr, old_pages, new_pages, tocopy));
}

#else

/* portable fallback: no kernel mremap used */
void	*ft_mremap_impl(void *oldptr, size_t oldlen, size_t newlen)
{
	const size_t	old_pages = pages_round_up(oldlen);
	const size_t	new_pages = pages_round_up(newlen);
	size_t			tocopy;

	if (oldptr == NULL)
		return (alloc_new_mapping(new_pages));
	if (new_pages == old_pages)
		return (oldptr);
	if (new_pages < old_pages)
	{
		if (!shrink_unmap_tail(oldptr, old_pages, new_pages))
			return (MAP_FAILED);
		return (oldptr);
	}
	if (oldlen < newlen)
		tocopy = oldlen;
	else
		tocopy = newlen;
	return (alloc_and_copy_fallback(oldptr, old_pages, new_pages, tocopy));
}

#endif