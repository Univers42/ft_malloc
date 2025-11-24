/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:05:42 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 21:24:46 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "debug.h"

void	malloc_debug_dummy(void)
{
	ssize_t	ret;

	ret = write(1, "malloc_debug_dummy\n", 19);
	(void)ret;
}

/* Replace file-scope globals with function-local singletons */
t_alloc_entry	*alloc_table(void)
{
	static t_alloc_entry	table[MAX_TRACKED_ALLOCS];

	return (table);
}

int	*alloc_count_ptr(void)
{
	static int	count = 0;

	return (&count);
}

void	print_entry(void *ptr, size_t size)
{
	void	*start;
	void	*end;

	start = ptr;
	end = (void *)((char *)start + size);
	printf("%p - %p : %zu bytes\n", start, end, size);
}
