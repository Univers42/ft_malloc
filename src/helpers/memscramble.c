/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memscramble.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:03:37 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:03:47 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MEMSCRAMBLE
static void scramble_memory(void *mem, uint32_t nbytes)
{
	if (nbytes)
		malloc_memset(mem, 0xcf, nbytes);
}

static void scramble_memory_if_enabled(void *mem, uint32_t nbytes)
{
	scramble_memory(mem, nbytes);
}
#else
static void scramble_memory_if_enabled(void *mem, uint32_t nbytes)
{
	(void)mem;
	(void)nbytes;
}
#endif