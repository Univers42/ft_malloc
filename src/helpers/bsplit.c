/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsplit.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 18:38:18 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 15:50:37 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "get_page_size.h"

int		compute_split_max(t_glob *g);
int		find_source_bucket_high(t_glob *g, int nu, int split_max);
int		find_source_bucket_low(t_glob *g, int nu, int split_max);
int		find_source_bucket(t_glob *g, int nu, int split_max);
t_mhead	*detach_source_block(t_glob *g, int nbuck);

/* build a chain of nblks smaller blocks starting at mp; return last block */
static t_mhead	*build_split_chain(t_mhead *mp, int nu, int nblks)
{
	uint64_t	siz;

	siz = binsize(nu);
	while (1)
	{
		mp->s_minfo.mi_alloc = ISFREE;
		mp->s_minfo.mi_index = nu;
		if (--nblks <= 0)
			break ;
		*chain_ptr(mp) = (t_mhead *)((char *)mp + siz);
		mp = (t_mhead *)((char *)mp + siz);
	}
	*chain_ptr(mp) = NULL;
	return (mp);
}

/* public API: split a larger block into nextf[nu] blocks */
void	bsplit(int nu)
{
	t_mhead	*mp;
	int32_t	nbuck;
	int32_t	nblks;
	int32_t	split_max;
	t_glob	*g;

	g = get_glob(GLOB_NONE, NULL);
	split_max = compute_split_max(g);
	nbuck = find_source_bucket(g, nu, split_max);
	if (nbuck > split_max || nbuck <= nu)
		return ;
	mp = detach_source_block(g, nbuck);
	bsplit_stats_inc(nbuck);
	nblks = (int)(binsize((int)nbuck) / binsize(nu));
	g->nextf[nu] = mp;
	(void)build_split_chain(mp, nu, nblks);
}
