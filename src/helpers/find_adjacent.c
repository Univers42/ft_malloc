/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_adjacent.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 14:34:00 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 19:52:49 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

int	try_split_or_coalesce(t_glob *g, int nu)
{
	int	combine_limit;

	if (nu >= SPLIT_MIN && nu <= g->malloc_mmap_threshold)
	{
		bsplit(nu);
		if (g->nextf[nu] != NULL)
			return (1);
	}
	combine_limit = (int)((size_t)g->pagebucket - 1);
	if (nu >= COMBINE_MIN && nu < combine_limit
		&& nu <= g->malloc_mmap_threshold
		&& g->busy[nu - 1] == 0 && g->nextf[nu - 1])
	{
		bcoalesce(nu);
		if (g->nextf[nu] != NULL)
			return (1);
	}
	return (0);
}

static int	find_adjacent_nullcheck(t_mhead *mp1,
				t_mhead **out1, t_mhead **out2)
{
	if (mp1 == NULL)
	{
		if (out1)
			*out1 = NULL;
		if (out2)
			*out2 = NULL;
		return (1);
	}
	return (0);
}

static t_mhead	*find_adjacent_loop(t_mhead **mp1,
		t_mhead **mp2, unsigned long siz)
{
	t_mhead	*mp;

	mp = chain(*mp1);
	while (mp && mp != (t_mhead *)((char *)(*mp1) + siz))
	{
		*mp2 = *mp1;
		*mp1 = mp;
		mp = chain(mp);
	}
	return (mp);
}

t_mhead	*find_adjacent(t_glob *g, int nbuck,
			t_mhead **out_mp1, t_mhead **out_mp2)
{
	unsigned long	siz;
	t_mhead			*mp;
	t_mhead			*mp1;
	t_mhead			*mp2;

	siz = binsize(nbuck);
	mp2 = g->nextf[nbuck];
	mp1 = mp2;
	if (find_adjacent_nullcheck(mp1, out_mp1, out_mp2))
		return (NULL);
	mp = find_adjacent_loop(&mp1, &mp2, siz);
	if (out_mp1)
		*out_mp1 = mp1;
	if (out_mp2)
		*out_mp2 = mp2;
	return (mp);
}
