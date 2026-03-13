/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal_realloc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:30:34 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/14 00:30:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

static void	handle_memalign_realloc(t_addr *mem, t_mhead **p)
{
	char	*ap;

	ap = (char *) *mem;
	*p = (t_mhead *) ap - 1;
	if ((*p)->s_minfo.mi_alloc == ISMEMALILGN)
	{
		ap -= (*p)->s_minfo.mi_nbytes;
		*p = (t_mhead *) ap - 1;
		*mem = (t_addr)(*p + 1);
	}
}

static t_addr	realloc_edge(t_addr mem, size_t n, t_val_ctx *v)
{
	if (n == 0)
	{
		internal_free(mem, v->file, v->line, MALLOC_INTERNAL);
		return ((t_addr) 0);
	}
	return (internal_malloc(n, v->file, v->line,
			MALLOC_INTERNAL));
}

/* same bucket: update size in place and rewrite end guard */
static t_addr	realloc_in_place(t_addr mem, t_mhead *p, size_t n)
{
	p->s_minfo.mi_nbytes = (uint32_t) n;
	setup_end_guard(p, n);
	return (mem);
}

/* standard path: allocate new, copy, free old */
static t_addr	realloc_copy(t_addr mem, size_t n, t_val_ctx *v,
					uint32_t tocopy)
{
	t_addr	m;

	m = internal_malloc(n, v->file, v->line,
			MALLOC_INTERNAL | MALLOC_NOTRACE | MALLOC_NOREG);
	if (m == (t_addr) 0)
		return ((t_addr) 0);
	if ((size_t) tocopy > n)
		tocopy = (uint32_t) n;
	fastcopy((void *) mem, (void *) m, tocopy);
	internal_free(mem, v->file, v->line, MALLOC_INTERNAL);
	return (m);
}

t_addr	internal_realloc(t_addr mem, size_t n, t_val_ctx *v, int flags)
{
	t_mhead	*p;
	t_glob	*g;
	size_t	nbytes;
	int		nunits;

	(void) flags;
	g = get_glob(GLOB_NONE, NULL);
	if (!g)
		return ((t_addr) 0);
	if (n == 0 || mem == (t_addr) 0)
		return (realloc_edge(mem, n, v));
	handle_memalign_realloc(&mem, &p);
	assert_or_abort(p->s_minfo.mi_index < NBUCKETS,
		"nunits < NBUCKETS", v->file, v->line);
	if (n == p->s_minfo.mi_nbytes)
		return (mem);
	nunits = (int) p->s_minfo.mi_index;
	nbytes = allocated_bytes(n);
	if (right_bucket(nbytes, nunits)
		|| (nunits >= 1 && right_bucket(nbytes, nunits - 1)))
		return (realloc_in_place(mem, p, n));
	return (realloc_copy(mem, n, v, p->s_minfo.mi_nbytes));
}
