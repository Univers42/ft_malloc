/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/05 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Registry of the contiguous runs morecore hands out. Recorded under the
 * central lock (morecore holds it). show_alloc_mem / malloc_live_* walk these
 * to enumerate live blocks, so the alloc/free hot path needs no per-op tracker.
 */

#include "alloc.h"

t_arena	*arena_table(void)
{
	static t_arena	arenas[MAX_ARENAS];

	return (arenas);
}

static int	*arena_count_ref(void)
{
	static int	count;

	return (&count);
}

int	arena_count(void)
{
	return (*arena_count_ref());
}

void	arena_record(void *start, size_t extent)
{
	int	*cp;

	if (!start || extent == 0)
		return ;
	cp = arena_count_ref();
	if (*cp >= MAX_ARENAS)
		return ;
	arena_table()[*cp].start = (char *)start;
	arena_table()[*cp].extent = extent;
	(*cp)++;
}

/* Drop a region's entry when it is munmap-ed, so the walk never touches
 * unmapped memory (swap-remove; munmap is rare). */
void	arena_remove(void *start)
{
	t_arena	*a;
	int		*cp;
	int		i;

	a = arena_table();
	cp = arena_count_ref();
	i = -1;
	while (++i < *cp)
	{
		if (a[i].start == (char *)start)
		{
			a[i] = a[*cp - 1];
			(*cp)--;
			return ;
		}
	}
}
