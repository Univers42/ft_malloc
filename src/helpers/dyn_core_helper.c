/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dyn_core_helper.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 15:59:23 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 16:18:17 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

/* forward declarations for functions implemented in dyn_core.c */
int		try_mmap_for_morecore(int nu, long sbrk_amt,
			t_mhead **out_mp, t_glob *g);
void	morecore_stat_nmorecore(int nu, t_glob *g);
void	morecore_stat_nsbrk(long s, t_glob *g);
int		try_split_or_coalesce(t_glob *g, int nu);

void	compute_sbrk_amount_and_nblks(t_glob *g, int nu,
			long *out_sbrk_amt, int *out_nblks)
{
	size_t	siz;
	long	sbrk_amt;
	int		nblks;
	long	rem;

	siz = (size_t)g->binsizes[(size_t)nu];
	if (siz <= (size_t)g->pagesz)
	{
		sbrk_amt = g->pagesz;
		nblks = (int)(sbrk_amt / (long)siz);
	}
	else
	{
		rem = (long)siz & (g->pagesz - 1);
		if (rem == 0)
			sbrk_amt = (long)siz;
		else
			sbrk_amt = (long)siz + g->pagesz - rem;
		nblks = 1;
	}
	*out_sbrk_amt = sbrk_amt;
	*out_nblks = nblks;
}

/* perform sbrk and alignment handling; returns 1 on success, 0 on failure */
int	handle_sbrk(long sbrk_amt, t_mhead **out_mp, int *inout_nblks)
{
	t_glob	*g;
	void	*sret;
	t_mhead	*mp;

	g = get_glob(GLOB_NONE, NULL);
	sret = sbrk(sbrk_amt);
	if (sret == (void *)-1)
		return (0);
	mp = (t_mhead *)sret;
	g->memtop += sbrk_amt;
	if (((uintptr_t)mp & (uintptr_t)MALIGN_MASK) != 0)
	{
		mp = (t_mhead *)((((uintptr_t)mp
						+ (uintptr_t)MALIGN_MASK)
					& ~((uintptr_t)MALIGN_MASK)));
		(*inout_nblks)--;
		if (*inout_nblks <= 0)
			return (0);
	}
	if (out_mp)
		*out_mp = mp;
	return (1);
}

int	request_memory_for_morecore(int nu, long sbrk_amt,
								t_mhead **out_mp, int *inout_nblks)
{
	t_glob	*g;
	t_mhead	*mp;

	g = get_glob(GLOB_NONE, NULL);
	if (try_mmap_for_morecore(nu, sbrk_amt, &mp, g))
	{
		if (out_mp)
			*out_mp = mp;
		return (1);
	}
	return (handle_sbrk(sbrk_amt, out_mp, inout_nblks));
}

void	build_free_list_from_mp(t_glob *g, int nu, t_mhead *mp, int nblks)
{
	size_t	siz;
	t_mhead	*cur;

	siz = binsize(nu);
	g->nextf[nu] = mp;
	cur = mp;
	while (1)
	{
		cur->s_minfo.mi_alloc = ISFREE;
		cur->s_minfo.mi_index = (char)nu;
		if (--nblks <= 0)
			break ;
		*chain_ptr(cur) = (t_mhead *)((char *)cur + siz);
		cur = (t_mhead *)((char *)cur + siz);
	}
	*chain_ptr(cur) = NULL;
}

void	morecore_impl(int nu)
{
	t_mhead	*mp;
	int		nblks;
	size_t	siz;
	long	sbrk_amt;
	t_glob	*g;

	g = get_glob(GLOB_NONE, NULL);
	if (!g)
		return ;
	while (1)
	{
		siz = (size_t)g->binsizes[(size_t)nu];
		if (siz == 0)
			break ;
		morecore_stat_nmorecore(nu, g);
		if (try_split_or_coalesce(g, nu))
			break ;
		compute_sbrk_amount_and_nblks(g, nu, &sbrk_amt, &nblks);
		if (!request_memory_for_morecore(nu, sbrk_amt, &mp, &nblks))
			break ;
		morecore_stat_nsbrk(sbrk_amt, g);
		build_free_list_from_mp(g, nu, mp, nblks);
		break ;
	}
}
