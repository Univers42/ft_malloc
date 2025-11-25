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

void	track_allocation(void *ptr, size_t size)
{
	int				i;
	t_alloc_entry	*tbl;
	int				*countp;
	const char		*logenv = getenv("FTMALLOC_LOG");

	tbl = alloc_table();
	countp = alloc_count_ptr();
	if (!ptr)
		return ;
	i = -1;
	while (++i < MAX_TRACKED_ALLOCS)
	{
		if (tbl[i].active == 0)
		{
			tbl[i].ptr = ptr;
			tbl[i].size = size;
			tbl[i].active = 1;
			if (i >= *countp)
				*countp = i + 1;
			if (logenv)
				fprintf(stderr, "TRACK + %p : %zu\n", ptr, size);
			return ;
		}
	}
	if (logenv)
		fprintf(stderr, "TRACK FAILED (table full) + %p : %zu\n", ptr, size);
}

void	untrack_allocation(void *ptr)
{
	int				i;
	t_alloc_entry	*tbl;
	int				*countp;
	const char		*logenv = NULL;

	tbl = alloc_table();
	countp = alloc_count_ptr();
	if (!ptr)
		return ;
	logenv = getenv("FTMALLOC_LOG");
	i = 0;
	while (i < *countp)
	{
		if (tbl[i].active && tbl[i].ptr == ptr)
		{
			tbl[i].active = 0;
			if (logenv)
				fprintf(stderr, "UNTRACK - %p\n", ptr);
			return ;
		}
		i++;
	}
	if (logenv)
		fprintf(stderr, "UNTRACK MISS - %p\n", ptr);
}

static size_t	show_category(const char *category, size_t min, size_t max)
{
	size_t			total;
	int				printed;
	int				i;
	t_alloc_entry	*tbl;
	int				*countp;

	tbl = alloc_table();
	countp = alloc_count_ptr();
	total = 0;
	printed = 0;
	i = -1;
	while (++i < *countp)
	{
		if (tbl[i].active && tbl[i].size > min && tbl[i].size <= max)
		{
			if (!printed)
			{
				printf("%s : %p\n", category, tbl[i].ptr);
				printed = 1;
			}
			print_entry(tbl[i].ptr, tbl[i].size);
			total += tbl[i].size;
		}
	}
	return (total);
}

void	show_alloc_mem(void)
{
	const size_t	total_tiny = show_category("TINY", 0, TINY_MAX);
	const size_t	total_small = show_category("SMALL", TINY_MAX, SMALL_MAX);
	const size_t	total_large = show_category("LARGE", SMALL_MAX, (size_t)-1);
	const size_t	grand_total = total_tiny + total_small + total_large;

	printf("Total : %zu bytes\n", grand_total);
}

static void		leak_report_on_exit(void) __attribute__((destructor));

static void	leak_report_on_exit(void)
{
	const size_t	total_tiny = show_category("TINY", 0, TINY_MAX);
	const size_t	total_small = show_category("SMALL", TINY_MAX, SMALL_MAX);
	const size_t	total_large = show_category("LARGE", SMALL_MAX, (size_t)-1);
	const size_t	grand_total = total_tiny + total_small + total_large;

	if (grand_total > 0)
	{
		fprintf(stderr, "\n⚠️  MEMORY LEAK DETECTED! ⚠️\n");
		fprintf(stderr, "Total leaked: %zu bytes\n", grand_total);
		show_alloc_mem();
	}
}
