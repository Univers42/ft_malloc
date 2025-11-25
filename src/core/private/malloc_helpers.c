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

t_mhead *get_block_from_freelist(int nunits, t_glob *g)
{
	t_mhead *p;

	if (g->nextf[nunits] == 0)
		morecore(nunits);
	p = g->nextf[nunits];
	if (p == NULL)
	{
		g->busy[nunits] = 0;
		return (NULL);
	}
	g->nextf[nunits] = *chain_ptr(p);
	g->busy[nunits] = 0;
	return (p);
}

void validate_free_block(t_mhead *p, int nunits, const char *file, int line)
{
	if (p->s_minfo.mi_alloc != ISFREE || p->s_minfo.mi_index != nunits)
	{
		set_state_mem((t_addr)(p + 1));
		xbotch(ERR_ASSERT_FAILED, "malloc: block on free list clobbered", file, line);
	}
}

void setup_block_header(t_mhead *p, size_t n)
{
	p->s_minfo.mi_alloc = ISALLOC;
	p->s_minfo.mi_magic2 = MAGIC2;
	p->s_minfo.mi_nbytes = n;
	malloc_memset((char *)p->s_minfo.mi_magic8, MAGIC1, MAGIC8_NUMBYTES);
}

void setup_end_guard(t_mhead *p, size_t n)
{
	t_mguard mg;
	uint8_t *z;
	char *m;

	mg.i = n;
	z = (uint8_t *)mg.s;
	m = (char *)(p + 1) + n;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
}
