/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 20:05:42 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 14:29:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

/**
 * could also use (((x) & -(x)) == x);
 */
int	is_powerof2(int x)
{
	return (((x - 1) & x) == 0);
}

/* round up to pages */
size_t	pages_round_up(size_t bytes)
{
	size_t	p;

	p = (size_t)getpagesize();
	return ((bytes + p - 1) & ~(p - 1));
}

/* allocate a new anonymous mapping of new_pages */
void	*alloc_new_mapping(size_t new_pages)
{
	void	*newptr;

	newptr = mmap(NULL, new_pages, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (newptr == MAP_FAILED)
		return (MAP_FAILED);
	return (newptr);
}
