/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal_malloc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:29:32 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 12:00:30 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

int calculate_nunits(uint32_t nbytes, t_glob *g)
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

int handle_reentrant(int nunits, t_glob *g)
{
	update_recurse_stats(g, nunits);
	while (g->busy[nunits])
		nunits++;
	g->busy[nunits] = 1;
	if (nunits > g->maxbuck)
		g->maxbuck = nunits;
	return (nunits);
}

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
