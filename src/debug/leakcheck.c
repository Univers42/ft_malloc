/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   leakcheck.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/04 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include <stdio.h>

t_alloc_entry	*alloc_table(void);
int				*alloc_count_ptr(void);
unsigned long	*alloc_overflow_ptr(void);

static void	print_leak_entry(t_alloc_entry *e)
{
	const char	*cat;

	if (e->size <= TINY_MAX)
		cat = "tiny";
	else if (e->size <= SMALL_MAX)
		cat = "small";
	else
		cat = "large";
	if (e->file)
		printf("  #%lu  %zuB  %s:%d  (%s)\n",
			e->serial, e->size, e->file, e->line, cat);
	else
		printf("  #%lu  %zuB  <no-debug-site>  (%s)\n",
			e->serial, e->size, cat);
}

size_t	malloc_live_count(void)
{
	t_awalk	w;

	w.min = 0;
	w.max = (size_t)-1;
	w.print = 0;
	arena_report(&w);
	return (w.count);
}

size_t	malloc_live_bytes(void)
{
	t_awalk	w;

	w.min = 0;
	w.max = (size_t)-1;
	w.print = 0;
	arena_report(&w);
	return (w.bytes);
}

size_t	malloc_track_overflow(void)
{
	return ((size_t)(*alloc_overflow_ptr()));
}

void	malloc_leak_map(void)
{
	t_alloc_entry	*e;
	int				i;

	e = alloc_table();
	i = -1;
	printf("=== ft_malloc leak map ===\n");
	while (++i < MAX_TRACKED_ALLOCS)
		if (e[i].active)
			print_leak_entry(&e[i]);
	printf("live: %zu bytes in %zu sites\n",
		malloc_live_bytes(), malloc_live_count());
	if (malloc_track_overflow() > 0)
		printf("WARNING: tracker overflow x%zu (allocs not tracked)\n",
			malloc_track_overflow());
}
