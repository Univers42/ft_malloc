/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsplit_helpers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 15:48:55 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 15:51:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

/* compute split_max (same logic as original) */
int	compute_split_max(t_glob *g)
{
	int	split_max;

	if (g->maxbuck > SPLIT_MAX)
		split_max = g->maxbuck;
	else
		split_max = SPLIT_MAX;
	return (split_max);
}

/* find a bucket to split from (preserve original search strategy) */
int	find_source_bucket_high(t_glob *g, int nu, int split_max)
{
	int	nbuck;

	nbuck = split_max;
	while (nbuck > nu)
	{
		if (g->busy[nbuck] || g->nextf[nbuck] == NULL)
		{
			nbuck--;
			continue ;
		}
		break ;
	}
	return (nbuck);
}

int	find_source_bucket_low(t_glob *g, int nu, int split_max)
{
	int	nbuck;

	nbuck = nu + 1;
	while (nbuck <= split_max)
	{
		if (g->busy[nbuck] || g->nextf[nbuck] == NULL)
		{
			nbuck++;
			continue ;
		}
		break ;
	}
	return (nbuck);
}

int	find_source_bucket(t_glob *g, int nu, int split_max)
{
	if (nu >= SPLIT_MID)
		return (find_source_bucket_high(g, nu, split_max));
	return (find_source_bucket_low(g, nu, split_max));
}

/* detach one block from nextf[nbuck] and return it (preserve busy flags) */
t_mhead	*detach_source_block(t_glob *g, int nbuck)
{
	t_mhead	*mp;

	g->busy[nbuck] = 1;
	mp = g->nextf[nbuck];
	g->nextf[nbuck] = chain(mp);
	g->busy[nbuck] = 0;
	return (mp);
}
