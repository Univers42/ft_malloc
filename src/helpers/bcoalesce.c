/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bcoalesce.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 18:31:59 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 15:23:45 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "get_page_size.h"

/* forward declarations for helpers implemented in bcoalesce_helpers.c */
void		ensure_chain(t_glob *g, t_mhead *mp1, t_mhead *mp2, int nbuck);
int			bcoalesce_check_adjacent(t_mhead *mp1, t_glob *g,
				int nbuck, unsigned long siz);
void		bcoalesce_stats_inc(int nbuck);
t_mhead		*find_adjacent(t_glob *g, int nbuck,
				t_mhead **out_mp1, t_mhead **out_mp2);

static void	bcoalesce_set_blocks(t_glob *g, t_mhead *mp1, int nbuck, int nu)
{
	g->busy[nbuck] = 0;
	bcoalesce_stats_inc(nbuck);
	mp1->s_minfo.mi_alloc = ISFREE;
	mp1->s_minfo.mi_index = nu;
	*chain_ptr(mp1) = g->nextf[nu];
	g->nextf[nu] = mp1;
}

static void	bcoalesce_update_freelist_struct(t_bcoalesce_ctx *args)
{
	if (args->mp1 == args->g->nextf[args->nbuck])
		args->g->nextf[args->nbuck] = chain(args->mp);
	else
		*chain_ptr(args->mp2) = chain(args->mp);
}

static int	bcoalesce_find_and_check(t_bcoalesce_ctx *ctx)
{
	ctx->mp = find_adjacent(ctx->g, ctx->nbuck, &ctx->mp1, &ctx->mp2);
	if (ctx->mp == NULL)
	{
		ctx->g->busy[ctx->nbuck] = 0;
		return (0);
	}
	ensure_chain(ctx->g, ctx->mp1, ctx->mp2, ctx->nbuck);
	if (!bcoalesce_check_adjacent(ctx->mp1, ctx->g,
			ctx->nbuck, binsize(ctx->nbuck)))
	{
		ctx->g->busy[ctx->nbuck] = 0;
		return (0);
	}
	return (1);
}

static void	bcoalesce_update(t_bcoalesce_ctx *ctx)
{
	t_bcoalesce_ctx	*args;

	args = ctx;
	bcoalesce_update_freelist_struct(args);
	bcoalesce_set_blocks(ctx->g, ctx->mp1, ctx->nbuck, ctx->nu);
}

/*
 * coalesce two adjacent free blocks off the free list for size NU - 1
 * preserves original logic; function kept short to satisfy norminette
 */
void	bcoalesce(int nu)
{
	t_bcoalesce_ctx	ctx;

	ctx.g = get_glob(GLOB_NONE, NULL);
	ctx.nu = nu;
	ctx.nbuck = nu - 1;
	if (ctx.g->nextf[ctx.nbuck] == NULL || ctx.g->busy[ctx.nbuck])
		return ;
	ctx.g->busy[ctx.nbuck] = 1;
	if (!bcoalesce_find_and_check(&ctx))
		return ;
	bcoalesce_update(&ctx);
}
