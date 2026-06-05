/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pagealign.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 20:11:52 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:18:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "get_page_size.h"
#include <unistd.h>
#include <stdint.h>
#include <string.h>

static int	init_pagesize(t_glob *g)
{
	if (!g)
		return (-1);
	g->pagesz = get_page_size();
	if (g->pagesz < MALLOC_PAGESIZE_MIN)
		g->pagesz = MALLOC_PAGESIZE_MIN;
	return (0);
}

static void	compute_pagebucket(t_glob *g)
{
	int	nunits;

	nunits = 7;
	while (nunits < NBUCKETS)
	{
		if ((uint64_t)g->pagesz <= g->binsizes[(size_t)nunits])
			break ;
		nunits++;
	}
	g->pagebucket = nunits;
}

/* mmap-only allocator: no sbrk prepopulation (concurrent sbrk races with libc's
 * brk and trips ThreadSanitizer). All arenas come from mmap via morecore. */
int	pagealign(void)
{
	t_glob	*g;

	g = get_glob(GLOB_NONE, NULL);
	if (!g)
		return (-1);
	if (init_pagesize(g) == -1)
		return (-1);
	g->memtop = (char *)sbrk(0);
	compute_pagebucket(g);
	return (0);
}
