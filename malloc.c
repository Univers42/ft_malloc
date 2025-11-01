/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 18:56:10 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 14:02:48 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_STATS
	case_reentrant()
	{
		if (busy[nunits])
			mstats.nrecurse++;
	}
#else
	case_reentrant(t_glob *g, int nunits)
	{
		while(g->busy[nunits])
			nunits++;
		g->busy[nunits] = 1;
		if (nunits > g->maxbuck)
			g->maxbuck = nunits;
		if (g->nextf[nunits] == 0)
			morecore(nunits);
		p = g->nextf[nunits];
		if (p == NULL)
		{
			g->busy[nunits] = 0;
			return (NULL);
		}
		g->nextf[nunits] = chain(p);
		g->busy[nunits] = 0;
		if (p->mh_alloc != ISFREE || p->mh_index != nunits)
			xbotch((t_ptr)(p))
	}
#endif
#if SIZEOF_SIZE_T == 8
	t_addr ensure_space_page(void)
	{
		if (allocated_bytes(n) > MAXALLOC_SIZE)
			return (t_addr(NULL));
	}
#else
	t_addr ensure_space_page(uint32_t nbytes, size_t n, t_glob *g, int nunits)
	{
		nbytes = allocated_bytes(n);
		if (nbytes <= (g->pagesz >> 1))
			nunits = STARTBUCK;
		else
			nunits = g->pagebucket;
		while (nunits < NBUCKETS)
			if (nbytes <= binsize(nunits))
				break ;
		if (nunits >= NBUCKETS)
			return ((t_addr)NULL);
		case_reentrant();
	}
#endif
/**
 * Get the system page size and align break pointer so future sbrks will be
 * page-aligned. The page size must be at least 1K -- anything smaller is
 * increased.
 * 
 * Figure out how many bytes are required, rounding up to the nearest multiple
 * of 8, then figure out which nextf[] area to use. Try to be smart about
 * where to start searching -- if the number of butes needed is greater than
 * the page size, we can start at pagebucket.
 */
t_addr  internal_malloc(size_t n, const char *file, int line, int flags)
{
	t_mhead *p;
	int		nunits;
	char	*m;
	char	*z;
	uint32_t	nbytes;
	t_mguard	mg;
	t_glob		*g = get_glob(GLOB_NONE, NULL);

	if (g->pagesz == 0)
		if (pagealign(0) < 0)
			return ((t_addr)NULL);
	ensure_space_page(nbytes, n);
}