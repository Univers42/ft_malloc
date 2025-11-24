/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal_free.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:58:45 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:13:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

/* Context used by the small helpers so we keep helper signatures short */

/* validate and populate the context (returns 0 on success) */
static void ifree_prepare(t_ifree_ctx *c, const char *file, int line)
{
	c->g = get_glob(GLOB_NONE, NULL);
	c->ap = (char *)c->mem;
	c->p = (t_mhead *)c->ap - 1;
	handle_memalign(&c->mem, &c->ap, &c->p);
	trace_and_watch_setup(c->p, &c->ubytes);
	validate_alloc_status(c->mem, c->p, file, line);
	c->nunits = c->p->s_minfo.mi_index;
	c->nbytes = allocated_bytes(c->p->s_minfo.mi_nbytes);
	if (in_bucket(c->nbytes, c->nunits) == 0)
	{
		set_state_mem(c->mem);
		xbotch(ERR_UNDERFLOW, "free: underflow detected; mh_nbytes out of range", file, line);
	}
	validate_magic8(c->p, c->mem, file, line);
	validate_end_guard(c->ap, c->p, c->mem, file, line);
}

/* perform free path: special cases (munmap/lesscore) or add to free list */
static void ifree_handle_free(t_ifree_ctx *c)
{
	c->freed = handle_special_free_cases(c->p, c->nunits, c->g);
	if (!c->freed)
	{
		scramble_memory_if_enabled(c->mem, c->p->s_minfo.mi_nbytes);
		assert_or_abort(c->nunits < NBUCKETS, "nunits < NBUCKETS", NULL, 0);
		handle_busy_bucket(c->p, c->nunits, c->g);
		if (c->g->busy[c->nunits] != 1)
			add_to_free_list(c->p, c->nunits, c->g);
	}
}

/* public API: thin wrapper that delegates to small helpers */
