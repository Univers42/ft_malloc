/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 14:50:00 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:50:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <stdio.h>

#define TINY_MAX 128
#define SMALL_MAX 1024
#define MAX_TRACKED_ALLOCS 10000

typedef struct s_alloc_entry
{
	void *ptr;
	size_t size;
	int active;
} t_alloc_entry;

static t_alloc_entry g_alloc_table[MAX_TRACKED_ALLOCS];
static int g_alloc_count = 0;

void track_allocation(void *ptr, size_t size)
{
	int i;

	if (!ptr)
		return;
	i = 0;
	while (i < MAX_TRACKED_ALLOCS)
	{
		if (g_alloc_table[i].active == 0)
		{
			g_alloc_table[i].ptr = ptr;
			g_alloc_table[i].size = size;
			g_alloc_table[i].active = 1;
			if (i >= g_alloc_count)
				g_alloc_count = i + 1;
			return;
		}
		i++;
	}
}

void untrack_allocation(void *ptr)
{
	int i;

	if (!ptr)
		return;
	i = 0;
	while (i < g_alloc_count)
	{
		if (g_alloc_table[i].active && g_alloc_table[i].ptr == ptr)
		{
			g_alloc_table[i].active = 0;
			return;
		}
		i++;
	}
}

static size_t show_category(const char *category, size_t min, size_t max)
{
	size_t total;
	int printed_header;
	void *start;
	void *end;
	int i;

	total = 0;
	printed_header = 0;
	i = 0;
	while (i < g_alloc_count)
	{
		if (g_alloc_table[i].active && g_alloc_table[i].size > min && g_alloc_table[i].size <= max)
		{
			if (!printed_header)
			{
				printf("%s : %p\n", category, g_alloc_table[i].ptr);
				printed_header = 1;
			}
			start = g_alloc_table[i].ptr;
			end = (void *)((char *)start + g_alloc_table[i].size);
			printf("%p - %p : %zu bytes\n", start, end, g_alloc_table[i].size);
			total += g_alloc_table[i].size;
		}
		i++;
	}
	return (total);
}

void show_alloc_mem(void)
{
	size_t total_tiny;
	size_t total_small;
	size_t total_large;
	size_t grand_total;

	total_tiny = show_category("TINY", 0, TINY_MAX);
	total_small = show_category("SMALL", TINY_MAX, SMALL_MAX);
	total_large = show_category("LARGE", SMALL_MAX, (size_t)-1);
	grand_total = total_tiny + total_small + total_large;
	printf("Total : %zu bytes\n", grand_total);
}
