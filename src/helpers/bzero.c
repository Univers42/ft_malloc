/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bzero.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 13:05:04 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:19:49 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

void	malloc_bzero(void *charp, size_t nbytes)
{
	size_t	*mzp;
	size_t	chunks;
	size_t	i;

	if (nbytes == 0)
		return ;
	if (nbytes <= 32)
	{
		mzp = (size_t *)charp;
		chunks = nbytes >> 3;
		i = 0;
		while (i < chunks)
		{
			*mzp++ = 0;
			i++;
		}
	}
	else
		memset(charp, 0, nbytes);
}

size_t	malloc_usable_size(void *mem)
{
	t_mhead	*p;
	char	*ap;

	ap = (char *)mem;
	if (ap == 0)
		return (0);
	p = (t_mhead *)ap - 1;
	if (p->s_minfo.mi_alloc == ISMEMALILGN)
	{
		ap -= p->s_minfo.mi_nbytes;
		p = (t_mhead *)ap - 1;
	}
	if (p->s_minfo.mi_alloc == ISFREE)
		return (0);
	return (p->s_minfo.mi_nbytes);
}
