/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   use_less_core.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:50:43 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 12:13:43 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef USE_LESSCORE
static int should_lesscore(int nunits, char *p, char *memtop)
{
	if (nunits < LESSCORE_MIN)
		return (0);
	if ((char *)p + binsize(nunits) != memtop)
		return (0);
	return (1);
}

static int can_lesscore(int nunits, t_glob *g)
{
	if (nunits >= LESSCORE_FRC)
		return (1);
	if (g->busy[nunits] || g->nextf[nunits] != 0)
		return (1);
	return (0);
}

static void handle_lesscore_internal(int nunits, t_glob *g)
{
	int o;

	o = errno;
	lesscore(nunits);
	errno = o;
}

/* make this exported (non-static) */
int try_lesscore_free(t_mhead *p, int nunits, t_glob *g)
{
	int freed;

	freed = 0;
	if (should_lesscore(nunits, (char *)p, g->memtop))
	{
		if (can_lesscore(nunits, g))
		{
			handle_lesscore_internal(nunits, g);
			freed = 1;
		}
	}
	return (freed);
}
#else
int try_lesscore_free(t_mhead *p, int nunits, t_glob *g)
{
	(void)p;
	(void)nunits;
	(void)g;
	return (0);
}
#endif