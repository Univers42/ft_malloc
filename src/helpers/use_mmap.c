/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   use_mmap.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:04:46 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:04:55 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef USE_MMAP
static int	should_use_munmap(int nunits, int threshold)
{
	return (nunits > threshold);
}

static void	handle_munmap(t_mhead *p, int nunits, t_glob *g)
{
	int o;

	o = errno;
	munmap(p, binsize(nunits));
	compute_stats_core(g, nunits, STAT_LESS_CORE, 0);
	errno = o;
}

static int try_munmap_free(t_mhead *p, int nunits, t_glob *g)
{
	int freed;

	freed = 0;
	if (should_use_munmap(nunits, MMAP_THRESHOLD))
	{
		handle_munmap(p, nunits, g);
		freed = 1;
	}
	return (freed);
}
#else
static int try_munmap_free(t_mhead *p, int nunits, t_glob *g)
{
	(void)p;
	(void)nunits;
	(void)g;
	return (0);
}
#endif

