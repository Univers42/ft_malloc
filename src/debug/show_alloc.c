/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 14:50:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/05 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <stdio.h>

/*
 * One TINY/SMALL/LARGE category for show_alloc_mem, backed by the arena walk
 * (no per-op tracker). First pass totals the live bytes in the size range;
 * if non-zero, print the header then a second pass listing each block.
 * Caller (show_alloc_mem) holds the central lock.
 */
size_t	show_category(const char *cat, size_t min, size_t max)
{
	t_awalk	w;

	w.min = min;
	w.max = max;
	w.print = 0;
	arena_report(&w);
	if (w.bytes > 0)
	{
		printf("%s :\n", cat);
		w.print = 1;
		arena_report(&w);
	}
	return (w.bytes);
}
