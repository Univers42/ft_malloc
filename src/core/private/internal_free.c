/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal_free.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:58:45 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 12:13:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

void	ifree_prepare(t_ifree_ctx *c, t_val_ctx *v)
{
	c->g = get_glob(GLOB_NONE, NULL);
	c->ap = (char *)c->mem;
	c->p = (t_mhead *)c->ap - 1;
	handle_memalign(&c->mem, &c->ap, &c->p);
	trace_and_watch_setup(c->p, &c->ubytes);
	validate_alloc_status(c->mem, c->p, v);
	c->nunits = c->p->s_minfo.mi_index;
	c->nbytes = allocated_bytes(c->p->s_minfo.mi_nbytes);
	if (in_bucket(c->nbytes, c->nunits) == 0)
	{
		set_state_mem(c->mem);
		xbotch(ERR_UNDERFLOW,
			"free: underflow detected", v->file, v->line);
	}
	validate_magic8(c->p, v);
	validate_end_guard(c->ap, c->p, v);
}

void	ifree_handle_free(t_ifree_ctx *c)
{
	c->freed = handle_special_free_cases(c->p, c->nunits, c->g);
	if (!c->freed)
	{
		scramble_allocated_memory(c->mem,
			c->p->s_minfo.mi_nbytes);
		assert_or_abort(c->nunits < NBUCKETS,
			"nunits < NBUCKETS", NULL, 0);
		handle_busy_bucket(c->p, c->nunits, c->g);
		if (c->g->busy[c->nunits] != 1)
			add_to_free_list(c->p, c->nunits, c->g);
	}
}

t_addr	internal_free(t_addr mem, const char *file, int line, int flags)
{
	t_ifree_ctx	ctx;
	t_val_ctx	v;

	(void)flags;
	if (mem == 0)
		return (NULL);
	v.mem = mem;
	v.file = file;
	v.line = line;
	ctx.mem = mem;
	ifree_prepare(&ctx, &v);
	ifree_handle_free(&ctx);
	untrack_allocation(mem);
	return (NULL);
}
