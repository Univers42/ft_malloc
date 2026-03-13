/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_triggers2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 12:30:00 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 12:30:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

void	handle_malloc_watch_alloc(t_alloc_ctx *ctx)
{
	(void)ctx;
}

void	check_alignment(t_alloc_ctx *ctx)
{
	(void)ctx;
}

int	check_max_alloc_size(size_t n)
{
	if (allocated_bytes(n) > MAXALLOC_SIZE)
		return (0);
	return (1);
}

int	should_trace_or_register_or_watch(void)
{
	return (1);
}
