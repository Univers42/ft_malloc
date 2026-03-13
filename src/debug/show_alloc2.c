/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 13:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include <stdio.h>

size_t	show_category(const char *cat, size_t min, size_t max);

void	show_alloc_mem(void)
{
	size_t	tiny;
	size_t	small;
	size_t	large;
	size_t	total;

	tiny = show_category("TINY", 0, TINY_MAX);
	small = show_category("SMALL", TINY_MAX, SMALL_MAX);
	large = show_category("LARGE", SMALL_MAX, (size_t)-1);
	total = tiny + small + large;
	printf("Total : %zu bytes\n", total);
}

void	leak_report_on_exit(void)
{
	size_t	tiny;
	size_t	small;
	size_t	large;
	size_t	total;

	tiny = show_category("TINY", 0, TINY_MAX);
	small = show_category("SMALL", TINY_MAX, SMALL_MAX);
	large = show_category("LARGE", SMALL_MAX, (size_t)-1);
	total = tiny + small + large;
	if (total > 0)
	{
		fprintf(stderr, "\nMEMORY LEAK DETECTED\n");
		fprintf(stderr, "Total leaked: %zu bytes\n", total);
		show_alloc_mem();
	}
}
