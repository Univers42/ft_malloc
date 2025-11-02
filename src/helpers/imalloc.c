/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   imalloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 13:05:04 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 13:05:15 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

void	malloc_memset(void *charp, int xch, size_t nbytes)
{
	uint8_t	*mzp;

	if (nbytes == 0)
		return ;
	if (nbytes <= 32)
	{
		mzp = (uint8_t*)charp;
		while (nbytes >= 8)
		{
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			*mzp++ = (uint8_t)xch;
			nbytes -= 8;
		}
		while (nbytes > 0)
		{
			*mzp++ = (uint8_t)xch;
			nbytes--;
		}
	}
	else
		memset(charp, xch, nbytes);
}

size_t	malloc_usable_size(void *mem)
{
	t_mhead	*p;
	char	*ap;

	if ((ap = (char*)mem) == 0)
		return (0);
	p = (t_mhead*)ap - 1;
	if (p->s_minfo.mi_alloc == ISMEMALILGN)
	{
		ap -= p->s_minfo.mi_nbytes;
		p = (t_mhead*)ap - 1;
	}
	if (p->s_minfo.mi_alloc == ISFREE)
		return (0);
	return (p->s_minfo.mi_nbytes);
}
