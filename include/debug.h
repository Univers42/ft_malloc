/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 14:50:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 21:24:50 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEBUG_H
# define DEBUG_H

# include "alloc.h"
# include <stdio.h>
# include <stdlib.h>

# define TINY_MAX 128
# define SMALL_MAX 1024
/* The allocation tracker (powers show_alloc_mem + the audit oracles) is an
 * open-addressing hash table keyed on the user pointer, with backward-shift
 * deletion so churn never accumulates tombstones. Capacity is a power of two;
 * TRACK_SHIFT = 64 - log2(capacity) for the multiplicative hash. */
# define MAX_TRACKED_ALLOCS 16384
# define TRACK_MASK 16383
# define TRACK_SHIFT 50

typedef struct s_alloc_entry
{
	void			*ptr;
	size_t			size;
	int				active;
	const char		*file;
	int				line;
	unsigned long	serial;
}	t_alloc_entry;

#endif
