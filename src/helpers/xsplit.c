/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   xsplit.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 15:30:56 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 15:40:58 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

/* helper: find source bucket as original algorithm did (nbuck = nu-1; 
while busy --) */
static int	find_source_bucket(int nu, t_glob *g)
{
	int	nbuck;

	nbuck = nu - 1;
	while (nbuck >= SPLIT_MIN && g->busy[nbuck])
		nbuck--;
	return (nbuck);
}

/* helper: compute number of sub-blocks (preserve original formula) */
static int	compute_nblks(int nu, int nbuck)
{
	return ((int)(binsize(nu) / binsize(nbuck)));
}

/* helper: build chain of free blocks starting at 'start',
returns last block
 * compute block_size internally so caller doesn't need 'siz' */
static t_mhead	*build_free_blocks(t_mhead *start, int nbuck, int nblks)
{
	const uint64_t	block_size = binsize(nbuck);
	t_mhead			*mp;

	mp = start;
	while (1)
	{
		mp->s_minfo.mi_alloc = ISFREE;
		mp->s_minfo.mi_index = (char)nbuck;
		if (--nblks <= 0)
			break ;
		*chain_ptr(mp) = (t_mhead *)((char *)mp + block_size);
		mp = (t_mhead *)((char *)mp + block_size);
	}
	return (mp);
}

/* helper: attach newly created chain (head..last)
to free list of nbuck (preserve original attach) */
static void	attach_free_list_with_head(t_mhead *head, t_mhead *last,
				int nbuck, t_glob *g)
{
	g->busy[nbuck] = 1;
	*chain_ptr(last) = g->nextf[nbuck];
	g->nextf[nbuck] = head;
	g->busy[nbuck] = 0;
}

/* public function: split the provided block (mp)
into smaller blocks and push to free list */
void	xsplit(t_mhead *mp, int nu)
{
	t_mhead	*head;
	t_mhead	*last;
	int		nbuck;
	int		nblks;
	t_glob	*g;

	g = get_glob(GLOB_NONE, NULL);
	nbuck = find_source_bucket(nu, g);
	if (nbuck < SPLIT_MIN)
		return ;
	bsplit_stats_inc(nu);
	head = mp;
	nblks = compute_nblks(nu, nbuck);
	last = build_free_blocks(head, nbuck, nblks);
	attach_free_list_with_head(head, last, nbuck, g);
}
