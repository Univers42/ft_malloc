/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arena_walk.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/05 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Walk the arena registry to enumerate live (ISALLOC) blocks. Each arena is a
 * tiled run of header-bearing blocks; advancing by binsize(mi_index) visits
 * every block (split/coalesce preserve the tiling). Fills t_awalk with the
 * total bytes and count of allocated blocks whose user size is in (min, max].
 */

#include "alloc.h"
#include <stdio.h>

static void	arena_print_block(t_mhead *h)
{
	void	*start;
	void	*end;

	start = (void *)(h + 1);
	end = (void *)((char *)start + h->s_minfo.mi_nbytes);
	printf("%p - %p : %u bytes\n", start, end, h->s_minfo.mi_nbytes);
}

static void	arena_report_one(t_arena *a, t_awalk *w)
{
	char	*p;
	char	*end;
	t_mhead	*h;
	size_t	bsz;

	p = a->start;
	end = a->start + a->extent;
	while (p < end)
	{
		h = (t_mhead *)p;
		bsz = (size_t)binsize(h->s_minfo.mi_index);
		if (bsz == 0)
			return ;
		if (h->s_minfo.mi_alloc == ISALLOC && h->s_minfo.mi_nbytes > w->min
			&& h->s_minfo.mi_nbytes <= w->max)
		{
			w->bytes += h->s_minfo.mi_nbytes;
			w->count += 1;
			if (w->print)
				arena_print_block(h);
		}
		p += bsz;
	}
}

void	arena_report(t_awalk *w)
{
	t_arena	*a;
	int		n;
	int		i;

	a = arena_table();
	n = arena_count();
	w->bytes = 0;
	w->count = 0;
	i = -1;
	while (++i < n)
		arena_report_one(&a[i], w);
}
