/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mremap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 01:48:13 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 14:47:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "../core/private/private.h"
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

/* forward declaration of implementation provided in mremap_internal.c */
void *ft_mremap_impl(void *oldptr, size_t oldlen, size_t newlen);

/* public wrapper for ft_mremap_impl (keeps previous API) */
void *ft_mremap(void *oldptr, size_t oldlen, size_t newlen)
{
	return ft_mremap_impl(oldptr, oldlen, newlen);
}

/* helper: compute allocated bytes for a stored header size */
static size_t
alloc_bytes_from_header(uint32_t stored_nbytes)
{
	return allocated_bytes((size_t)stored_nbytes);
}

/* helper: finalize header fields after successful remap */
static void
finalize_header_after_remap(t_mhead *np, size_t n, int newunits)
{
	np->s_minfo.mi_alloc = ISALLOC;
	np->s_minfo.mi_index = (char)newunits;
	np->s_minfo.mi_nbytes = (uint32_t)n;
	np->s_minfo.mi_magic2 = MAGIC2;
	malloc_memset((char *)np->s_minfo.mi_magic8, MAGIC1, MAGIC8_NUMBYTES);
}

/* helper: fallback path allocate-copy-free */
static t_addr
fallback_alloc_copy_free(t_mremap_params *pr)
{
	t_addr newaddr;

	newaddr = internal_malloc(pr->n, pr->file, pr->line,
							  MALLOC_INTERNAL | MALLOC_NOTRACE | MALLOC_NOREG);
	if (newaddr == (t_addr)0)
		return ((t_addr)0);
	fastcopy((void *)pr->mem, (void *)newaddr, pr->tocopy);
	internal_free(pr->mem, pr->file, pr->line, MALLOC_INTERNAL);
	compute_stats_realloc_copy(pr->g);
	return (newaddr);
}

/* single-arg mremap implementation (delegates to ft_mremap and fallbacks) */
t_addr
mremap_implement(t_mremap_params *pr)
{
	void *old_payload;
	void *remap_ret;
	size_t old_alloc_bytes;
	size_t new_alloc_bytes;
	t_mhead *np;

	if (!pr || !pr->g || pr->mem == (t_addr)0)
		return ((t_addr)0);

	/* derive payload and sizes */
	{
		t_mhead *p = (t_mhead *)pr->mem - 1;
		old_payload = (void *)(p + 1);
		old_alloc_bytes = alloc_bytes_from_header(p->s_minfo.mi_nbytes);
		new_alloc_bytes = allocated_bytes((size_t)pr->n);
	}

	/* try ft_mremap (may use kernel mremap or portable wrapper) */
	remap_ret = ft_mremap(old_payload, old_alloc_bytes, new_alloc_bytes);
	if (remap_ret == MAP_FAILED)
		return fallback_alloc_copy_free(pr);

	/* update header in-place for the remapped region */
	np = (t_mhead *)remap_ret - 1;
	finalize_header_after_remap(np, pr->n, pr->newunits);
	setup_end_guard(np, pr->n); /* <-- add this: set the end guard on the new block */
	compute_stats_core(pr->g, pr->newunits, STAT_MMAP, new_alloc_bytes);
	return ((t_addr)(np + 1));
}
