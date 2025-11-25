/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:03:11 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 11:53:10 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

void	trace_and_watch_setup(t_mhead *p, uint32_t *ubytes)
{
	if (should_trace_or_register_or_watch())
		*ubytes = p->s_minfo.mi_nbytes;
}

void	handle_memalign(t_addr *mem, char **ap, t_mhead **p)
{
	(void)mem;
	if ((*p)->s_minfo.mi_alloc == ISMEMALILGN)
	{
		*ap -= (*p)->s_minfo.mi_nbytes;
		*p = (t_mhead *)*ap - 1;
	}
}

void	validate_alloc_status(t_addr mem, t_mhead *p, const char *file, int line)
{
	set_state_mem(mem);
	if (p->s_minfo.mi_alloc != ISALLOC)
	{
		if (p->s_minfo.mi_alloc == ISFREE)
			xbotch(ERR_DUPFREE, "free: called with already freed block argument", file, line);
		else
			xbotch(ERR_UNALLOC, "free: called with unallocated block argument", file, line);
	}
	assert_or_abort(p->s_minfo.mi_magic2 == MAGIC2, "p->s_minfo.mi_magic2 == MAGIC2", file, line);
}

void	validate_magic8(t_mhead *p, t_addr mem, const char *file, int line)
{
	int i;
	uint8_t *z;

	set_state_mem(mem);
	i = 0;
	z = (uint8_t *)p->s_minfo.mi_magic8;
	while (i < MAGIC8_NUMBYTES)
	{
		if (*z++ != MAGIC1)
			xbotch(ERR_UNDERFLOW, "free: underflow detected; magic8 corrupted", file, line);
		i++;
	}
}
