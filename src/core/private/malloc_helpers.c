/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:56:32 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 11:56:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

t_mhead	*get_block_from_freelist(int nunits, t_glob *g)
{
	t_mhead	*p;
	int		from_cache;

	from_cache = (g->nextf[nunits] != 0);
	if (!from_cache)
		morecore(nunits);
	p = g->nextf[nunits];
	if (p == NULL)
		return (NULL);
	g->nextf[nunits] = *chain_ptr(p);
	if (from_cache && nunits > MMAP_THRESHOLD)
		g->large_cached_bytes -= binsize(nunits);
	return (p);
}

void	validate_free_block(t_mhead *p, int nu, t_val_ctx *v)
{
	if (p->s_minfo.mi_alloc != ISFREE
		|| p->s_minfo.mi_index != nu)
	{
		set_state_mem((t_addr)(p + 1));
		xbotch(ERR_ASSERT_FAILED,
			"malloc: free list clobbered", v->file, v->line);
	}
}

void	setup_block_header(t_mhead *p, size_t n)
{
	p->s_minfo.mi_alloc = ISALLOC;
	p->s_minfo.mi_nbytes = n;
	if (FT_HARDEN)
	{
		p->s_minfo.mi_magic2 = MAGIC2;
		malloc_memset((char *)p->s_minfo.mi_magic8,
			MAGIC1, MAGIC8_NUMBYTES);
	}
}

void	setup_end_guard(t_mhead *p, size_t n)
{
	t_mguard	mg;
	uint8_t		*z;
	char		*m;

	if (!FT_HARDEN)
		return ;
	mg.i = n;
	z = (uint8_t *)mg.s;
	m = (char *)(p + 1) + n;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
}
