/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pagealign_helpers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 20:12:55 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:15:56 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

long	compute_sbrk_needed(char *curbrk, int pagesz)
{
	long	need;

	need = (long)pagesz - ((long)curbrk & (pagesz - 1));
	if (need < 0)
		need += pagesz;
	return (need);
}

int	perform_sbrk_and_update_memtop(long sbrk_needed,
										char **out_curbrk,
										t_glob *g)
{
	void	*sret;

	if (sbrk_needed == 0)
	{
		*out_curbrk = NULL;
		return (0);
	}
	sret = sbrk(sbrk_needed);
	if (sret == (void *)-1)
		return (-1);
	*out_curbrk = (char *)sret;
	g->memtop += sbrk_needed;
	return (0);
}
