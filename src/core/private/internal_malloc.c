/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal_malloc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:29:32 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 23:33:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

int	calculate_nunits(uint32_t nbytes, t_glob *g)
{
	int			nunits;
	uint32_t	half_page;

	half_page = (uint32_t)(g->pagesz >> 1);
	if (nbytes <= half_page)
		nunits = STARTBUCK;
	else
		nunits = g->pagebucket;
	while (nunits < NBUCKETS)
	{
		if (nbytes <= binsize(nunits))
			break ;
		nunits++;
	}
	return (nunits);
}

int	handle_reentrant(int nunits, t_glob *g)
{
	update_recurse_stats(g, nunits);
	while (g->busy[nunits])
		nunits++;
	g->busy[nunits] = 1;
	if (nunits > g->maxbuck)
		g->maxbuck = nunits;
	return (nunits);
}

static void	finalize_alloc(t_alloc_ctx *ctx)
{
	scramble_allocated_memory(ctx->ptr, ctx->n);
	update_alloc_stats(ctx->g, ctx->nunits, ctx->n);
	handle_malloc_trace_alloc(ctx);
	handle_malloc_register_alloc(ctx);
	handle_malloc_watch_alloc(ctx);
	check_alignment(ctx);
	track_allocation(ctx->ptr, ctx->n);
}

static t_addr	do_malloc(t_glob *g, size_t n, t_val_ctx *v, int flags)
{
	t_mhead		*p;
	int			nunits;
	uint32_t	nbytes;
	t_alloc_ctx	ctx;

	nbytes = allocated_bytes(n);
	nunits = calculate_nunits(nbytes, g);
	if (nunits >= NBUCKETS)
		return ((t_addr) NULL);
	nunits = handle_reentrant(nunits, g);
	p = get_block_from_freelist(nunits, g);
	if (p == NULL)
		return (NULL);
	validate_free_block(p, nunits, v);
	setup_block_header(p, n);
	setup_end_guard(p, n);
	ctx = (t_alloc_ctx){(t_addr)(p + 1), p, n, nunits,
		flags, v->file, v->line, g};
	finalize_alloc(&ctx);
	return (ctx.ptr);
}

t_addr	internal_malloc(size_t n, const char *file, int line, int flags)
{
	t_glob		*g;
	t_val_ctx	v;

	g = get_glob(GLOB_NONE, NULL);
	if (g->pagesz == 0)
		if (pagealign() < 0)
			return ((t_addr) NULL);
	if (check_max_alloc_size(n) == 0)
		return ((t_addr) NULL);
	v.mem = NULL;
	v.file = file;
	v.line = line;
	return (do_malloc(g, n, &v, flags));
}
