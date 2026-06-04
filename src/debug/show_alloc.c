/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 14:50:00 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 14:59:48 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include <stdio.h>
#include <string.h>

t_alloc_entry	*alloc_table(void);
void			print_entry(void *ptr, size_t size);

static void	show_matching(t_alloc_entry *e, size_t mn, size_t mx)
{
	int	i;

	i = -1;
	while (++i < MAX_TRACKED_ALLOCS)
	{
		if (e[i].active && e[i].size > mn && e[i].size <= mx)
			print_entry(e[i].ptr, e[i].size);
	}
}

size_t	show_category(const char *cat, size_t min, size_t max)
{
	size_t			total;
	int				i;
	t_alloc_entry	*tbl;

	tbl = alloc_table();
	total = 0;
	i = -1;
	while (++i < MAX_TRACKED_ALLOCS)
	{
		if (tbl[i].active && tbl[i].size > min
			&& tbl[i].size <= max)
			total += tbl[i].size;
	}
	if (total > 0)
	{
		printf("%s :\n", cat);
		show_matching(tbl, min, max);
	}
	return (total);
}
