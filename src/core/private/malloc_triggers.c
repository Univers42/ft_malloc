/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_triggers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:57:37 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 11:57:37 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

void	scramble_allocated_memory(void *mem, size_t n)
{
	if (FT_HARDEN && n)
		malloc_memset((char *)mem, 0xdf, n);
}

void	update_recurse_stats(t_glob *g, int nunits)
{
	(void)g;
	(void)nunits;
}

void	update_alloc_stats(t_glob *g, int nunits, size_t n)
{
	(void)g;
	(void)nunits;
	(void)n;
}

void	handle_malloc_trace_alloc(t_alloc_ctx *ctx)
{
	(void)ctx;
}

void	handle_malloc_register_alloc(t_alloc_ctx *ctx)
{
	(void)ctx;
}
