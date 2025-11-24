/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dyn_core.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 21:32:29 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 16:22:17 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <unistd.h>
#include <stdint.h>

#if defined(USE_MMAP)
# include <sys/mman.h>
#endif

void	compute_sbrk_amount_and_nblks(t_glob *g, int nu,
			long *out_sbrk_amt, int *out_nblks);
int		handle_sbrk(long sbrk_amt, t_mhead **out_mp, int *inout_nblks);
int		request_memory_for_morecore(int nu, long sbrk_amt,
			t_mhead **out_mp, int *inout_nblks);
void	build_free_list_from_mp(t_glob *g, int nu, t_mhead *mp, int nblks);
void	morecore_impl(int nu);

#if defined(USE_MMAP)

int	try_mmap_for_morecore(int nu, long sbrk_amt, t_mhead **out_mp, t_glob *g)
{
	t_mhead	*mp;

	if (!g || nu <= g->malloc_mmap_threshold)
		return (0);
	mp = (t_mhead *)mmap(NULL, (size_t)sbrk_amt, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if ((void *)mp == MAP_FAILED)
		return (0);
	g->nextf[nu] = mp;
	mp->s_minfo.mi_alloc = ISFREE;
	mp->s_minfo.mi_index = (char)nu;
	*chain_ptr(mp) = NULL;
	compute_stats_core(g, nu, STAT_MMAP, (size_t)sbrk_amt);
	if (out_mp)
		*out_mp = mp;
	return (1);
}
#else

int	try_mmap_for_morecore(int nu, long sbrk_amt, t_mhead **out_mp, t_glob *g)
{
	(void)nu;
	(void)sbrk_amt;
	(void)out_mp;
	(void)g;
	return (0);
}
#endif

void	lesscore(int nu)
{
	t_glob	*g;
	long	siz;

	g = get_glob(GLOB_NONE, NULL);
	if (!g || (size_t)nu >= NBUCKETS)
		return ;
	siz = (long)g->binsizes[(size_t)nu];
	if (siz <= 0)
		return ;
	if (sbrk(-siz) == (void *)-1)
		return ;
	g->memtop -= siz;
	compute_stats_core(g, nu, STAT_LESS_CORE, (size_t)siz);
}

void	morecore(int nu)
{
	sigset_t	set;
	sigset_t	oset;
	t_glob		*g;

	(void)get_glob;
	g = get_glob(GLOB_NONE, NULL);
	if (!g)
		return ;
	malloc_block_signals(&set, &oset);
	morecore_impl(nu);
	malloc_unblock_signals(&set, &oset);
}
