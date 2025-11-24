/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 18:56:10 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:30:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

static void finalize_alloc(t_addr ptr, t_mhead *p, size_t n, int nunits, const char *file, int line, int flags, t_glob *g)
{
	scramble_allocated_memory(ptr, n);
	update_alloc_stats(g, nunits, n);
	handle_malloc_trace_alloc(ptr, n, nunits, file, line, flags, g);
	handle_malloc_register_alloc(ptr, n, file, line, flags, g);
	handle_malloc_watch_alloc(ptr, n, file, line, g);
	check_alignment(ptr, p, file, line);
	track_allocation(ptr, n);
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

t_addr ft_malloc(size_t size)
{
	ensure_init();
	return (internal_malloc(size, (char *)NULL, 0, 0));
}