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
int				*alloc_count_ptr(void);
void			print_entry(void *ptr, size_t size);

static void	track_slot(t_alloc_entry *e, int *cp, void *ptr, size_t sz)
{
	int	i;

	i = -1;
	while (++i < MAX_TRACKED_ALLOCS)
	{
		if (e[i].active == 0)
		{
			e[i].ptr = ptr;
			e[i].size = sz;
			e[i].active = 1;
			if (i >= *cp)
				*cp = i + 1;
			return ;
		}
	}
}

void	track_allocation(void *ptr, size_t size)
{
	if (!ptr)
		return ;
	track_slot(alloc_table(), alloc_count_ptr(), ptr, size);
}

void	untrack_allocation(void *ptr)
{
	int				i;
	t_alloc_entry	*tbl;
	int				*countp;

	if (!ptr)
		return ;
	tbl = alloc_table();
	countp = alloc_count_ptr();
	i = 0;
	while (i < *countp)
	{
		if (tbl[i].active && tbl[i].ptr == ptr)
		{
			tbl[i].active = 0;
			return ;
		}
		i++;
	}
}

static void	show_matching(t_alloc_entry *e, int cnt, size_t mn, size_t mx)
{
	int	i;

	i = -1;
	while (++i < cnt)
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
	int				cnt;

	tbl = alloc_table();
	cnt = *alloc_count_ptr();
	total = 0;
	i = -1;
	while (++i < cnt)
	{
		if (tbl[i].active && tbl[i].size > min
			&& tbl[i].size <= max)
			total += tbl[i].size;
	}
	if (total > 0)
	{
		printf("%s : %p\n", cat, tbl[0].ptr);
		show_matching(tbl, cnt, min, max);
	}
	return (total);
}
