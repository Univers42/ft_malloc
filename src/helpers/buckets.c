/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buckets.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 21:03:02 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 21:03:33 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

bool	in_bucket(size_t nb, int nu)
{
	t_glob	*g;

	g = get_glob(GLOB_NONE, NULL);
	return (nb <= g->binsizes[nu]);
}

bool	right_bucket(size_t nb, int nu)
{
	size_t	lower;
	t_glob	*g;

	g = get_glob(GLOB_NONE, NULL);
	if (nu >= 1)
		lower = g->binsizes[(size_t)nu - 1];
	else
		lower = 0UL;
	return ((nb > lower) && (nb <= g->binsizes[(size_t)nu]));
}

/* Use size_t for bitwise operations to ensure an integral type,
	   then cast the result back to int for the function return. */
size_t	allocated_bytes(size_t n)
{
	return (((size_t)n + (size_t)mover_head() + (size_t)MSLOP + (size_t)MALIGN_MASK) & ~(size_t)MALIGN_MASK);
}
