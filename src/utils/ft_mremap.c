/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_mremap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 01:48:13 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:57:46 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>

void	*ft_mremap_impl(void *oldptr, size_t oldlen, size_t newlen);

/* public wrapper (no inner compile-time branching) */
void	*ft_mremap(void *oldptr, size_t oldlen, size_t newlen)
{
	return ft_mremap_impl(oldptr, oldlen, newlen);
}

/* Provide mremap_implement symbol used by internal_realloc.
 * It prefers ft_mremap when available; otherwise falls back to
 * allocate-copy-free (internal_malloc + fastcopy + internal_free).
 */
t_addr	mremap_implement(t_addr mem, size_t n, int newunits, uint32_t tocopy,
				 int flags, const char *file, int line, t_glob *g, int nunits)
{
	t_mhead *p;
	void *old_payload;
	void *remap_ret;
	size_t old_alloc_bytes;
	size_t new_alloc_bytes;
	t_mhead *np;
	t_addr	newaddr;

	(void)flags;
	(void)nunits;
	if (!mem || !g)
		return ((t_addr)0);
	p = (t_mhead *)mem - 1;
	old_payload = (void *)(p + 1);
	old_alloc_bytes = allocated_bytes((size_t)p->s_minfo.mi_nbytes);
	new_alloc_bytes = allocated_bytes((size_t)n);
	remap_ret = ft_mremap(old_payload, old_alloc_bytes, new_alloc_bytes);
	if (remap_ret == MAP_FAILED)
	{
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
	compute_stats_core(g, newunits, STAT_MMAP, new_alloc_bytes);
	return ((t_addr)(np + 1));
}
