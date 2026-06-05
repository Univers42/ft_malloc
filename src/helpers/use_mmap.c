/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   use_mmap.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:04:46 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:43:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef USE_MMAP

int	should_use_munmap(int nunits, int threshold)
{
	return (nunits > threshold);
}

void	handle_munmap(t_mhead *p, int nunits, t_glob *g)
{
	int	o;

	o = errno;
	munmap(p, binsize(nunits));
	arena_remove(p);
	compute_stats_core(g, nunits, STAT_LESS_CORE, 0);
	errno = o;
}

/*
 * Large (mmap-backed) free. Instead of munmap-ing every large block, keep it on
 * its freelist for reuse while total cached large bytes stay under
 * LARGE_CACHE_CAP (return 0 so the caller chains it via add_to_free_list). Past
 * the cap, return the block to the OS as before so memory stays bounded. The
 * matching decrement happens on a large cache-hit in get_block_from_freelist.
 */
int	try_munmap_free(t_mhead *p, int nunits, t_glob *g)
{
	size_t	bsz;

	if (!should_use_munmap(nunits, MMAP_THRESHOLD))
		return (0);
	bsz = binsize(nunits);
	if (g->large_cached_bytes + bsz <= LARGE_CACHE_CAP)
	{
		g->large_cached_bytes += bsz;
		return (0);
	}
	handle_munmap(p, nunits, g);
	return (1);
}
#else

int	try_munmap_free(t_mhead *p, int nunits, t_glob *g)
{
	(void)p;
	(void)nunits;
	(void)g;
	return (0);
}
#endif
