/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   track_tbl.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/05 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Open-addressing hash table for the allocation tracker: O(1) insert / lookup /
 * delete keyed on the user pointer. Linear probing with backward-shift deletion
 * (tbl_delete_at) keeps the table free of tombstones, so alloc/free churn does
 * not degrade probe lengths. The public track/untrack API is in track.c.
 */

#include "debug.h"

size_t	tbl_hash(void *ptr)
{
	uintptr_t	h;

	h = (uintptr_t)ptr >> 4;
	h *= 11400714819323198485UL;
	return ((size_t)(h >> TRACK_SHIFT));
}

int	tbl_find(t_alloc_entry *e, void *ptr)
{
	size_t	i;
	size_t	n;

	i = tbl_hash(ptr);
	n = 0;
	while (n < MAX_TRACKED_ALLOCS && e[i].active)
	{
		if (e[i].ptr == ptr)
			return ((int)i);
		i = (i + 1) & TRACK_MASK;
		n++;
	}
	return (-1);
}

int	tbl_insert_slot(t_alloc_entry *e, void *ptr)
{
	size_t	i;
	size_t	n;

	i = tbl_hash(ptr);
	n = 0;
	while (n < MAX_TRACKED_ALLOCS && e[i].active)
	{
		i = (i + 1) & TRACK_MASK;
		n++;
	}
	if (n >= MAX_TRACKED_ALLOCS)
		return (-1);
	return ((int)i);
}

static int	tbl_can_move(size_t i, size_t k, size_t j)
{
	if (j > i)
		return (k <= i || k > j);
	return (k <= i && k > j);
}

void	tbl_delete_at(t_alloc_entry *e, size_t i)
{
	size_t	j;
	size_t	k;

	e[i].active = 0;
	j = i;
	while (1)
	{
		j = (j + 1) & TRACK_MASK;
		if (!e[j].active)
			return ;
		k = tbl_hash(e[j].ptr);
		if (tbl_can_move(i, k, j))
		{
			e[i] = e[j];
			e[j].active = 0;
			i = j;
		}
	}
}
