/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pagealign.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 20:11:52 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:18:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "get_page_size.h"
#include <unistd.h>
#include <stdint.h>
#include <string.h>

long	compute_sbrk_needed(char *curbrk, int pagesz);
int		perform_sbrk_and_update_memtop(long sbrk_needed,
			char **out_curbrk,
			t_glob *g);

static int	init_pagesize(t_glob *g)
{
	if (!g)
		return (-1);
	g->pagesz = get_page_size();
	if (g->pagesz < MALLOC_PAGESIZE_MIN)
		g->pagesz = MALLOC_PAGESIZE_MIN;
	return (0);
}

static void	populate_prepop_chain(t_glob *g, char *curbrk,
									long sbrk_needed)
{
	int		nunits;
	t_mhead	*mp;
	long	adj;

	adj = sbrk_needed & (PREPOP_SIZE - 1);
	curbrk += adj;
	sbrk_needed -= adj;
	nunits = (int)(sbrk_needed / PREPOP_SIZE);
	if (nunits <= 0)
		return ;
	mp = (t_mhead *)curbrk;
	g->nextf[PREPOP_BIN] = mp;
	while (1)
	{
		mp->s_minfo.mi_alloc = ISFREE;
		mp->s_minfo.mi_index = (char)PREPOP_BIN;
		if (--nunits <= 0)
			break ;
		*chain_ptr(mp) = (t_mhead *)((char *)mp + PREPOP_SIZE);
		mp = (t_mhead *)((char *)mp + PREPOP_SIZE);
	}
	*chain_ptr(mp) = NULL;
}

static int	prepopulate_prepop(t_glob *g, long sbrk_needed)
{
	char	*curbrk;
	int		ret;

	if (sbrk_needed == 0)
		return (0);
	compute_stats_brk(g, sbrk_needed);
	ret = perform_sbrk_and_update_memtop(sbrk_needed, &curbrk, g);
	if (ret == -1)
		return (-1);
	populate_prepop_chain(g, curbrk, sbrk_needed);
	return (0);
}

static void	compute_pagebucket(t_glob *g)
{
	int	nunits;

	nunits = 7;
	while (nunits < NBUCKETS)
	{
		if ((uint64_t)g->pagesz <= g->binsizes[(size_t)nunits])
			break ;
		nunits++;
	}
	g->pagebucket = nunits;
}

int	pagealign(void)
{
	t_glob	*g;
	char	*curbrk;
	long	sbrk_needed;
	int		ret;

	g = get_glob(GLOB_NONE, NULL);
	if (!g)
		return (-1);
	if (init_pagesize(g) == -1)
		return (-1);
	g->memtop = sbrk(0);
	curbrk = sbrk(0);
	sbrk_needed = compute_sbrk_needed(curbrk, g->pagesz);
	ret = prepopulate_prepop(g, sbrk_needed);
	if (ret == -1)
		return (-1);
	compute_pagebucket(g);
	return (0);
}
