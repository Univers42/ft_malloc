/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   accessors.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 21:00:44 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 21:02:17 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

t_glob	*get_glob(t_glob_field field, void *value)
{
	static t_glob	glob = {0};
	size_t			i;
	t_binspec		*spec;
	size_t			copy;

	if (field != GLOB_NONE && value != NULL)
	{
		if (field == GLOB_PAGESZ)
			glob.pagesz = *(int *)value;
		else if (field == GLOB_PAGEBUCKET)
			glob.pagebucket = *(int *)value;
		else if (field == GLOB_MAXBUCK)
			glob.maxbuck = *(int *)value;
		else if (field == GLOB_MEMTOP)
			glob.memtop = (char *)value;
		else if (field == GLOB_ERRN)
			glob.errn = *(int *)value;
		else if (field == GLOB_BINSIZE)
		{
			spec = (t_binspec *)value;
			copy = spec->len;
			if (copy > (size_t)NBUCKETS)
				copy = (size_t)NBUCKETS;
			i = -1;
			while (++i < copy)
				glob.binsizes[i] = (uint64_t)spec->bins[i];
			while (++i < (size_t)NBUCKETS)
				glob.binsizes[i] = 0UL;
			if (copy == 0)
				glob.maxbuck = 0;
			else
				glob.maxbuck = (int)(copy - 1);
		}
	}
	return (&glob);
}
