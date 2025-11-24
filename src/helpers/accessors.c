/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   accessors.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 21:00:44 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 14:37:44 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

static void	set_glob_binsizes(t_glob *glob, t_binspec *spec)
{
	size_t	i;
	size_t	copy;

	if (spec == NULL || glob == NULL)
		return ;
	copy = spec->len;
	if (copy > (size_t)NBUCKETS)
		copy = (size_t)NBUCKETS;
	i = (size_t)-1;
	while (++i < copy)
		glob->binsizes[i] = (uint64_t)spec->bins[i];
	while (++i < (size_t)NBUCKETS)
		glob->binsizes[i] = 0UL;
	if (copy == 0)
		glob->maxbuck = 0;
	else
		glob->maxbuck = (int)(copy - 1);
}

/* Set a single field on the global struct; kept short to satisfy style */
static void	set_glob_field(t_glob *glob, t_glob_field field, void *value)
{
	if (glob == NULL || value == NULL)
		return ;
	if (field == GLOB_PAGESZ)
		glob->pagesz = *(int *)value;
	else if (field == GLOB_PAGEBUCKET)
		glob->pagebucket = *(int *)value;
	else if (field == GLOB_MAXBUCK)
		glob->maxbuck = *(int *)value;
	else if (field == GLOB_MEMTOP)
		glob->memtop = (char *)value;
	else if (field == GLOB_ERRN)
		glob->errn = *(int *)value;
	else if (field == GLOB_BINSIZE)
		set_glob_binsizes(glob, (t_binspec *)value);
}

t_glob	*get_glob(t_glob_field field, void *value)
{
	static t_glob	glob = {0};

	if (field != GLOB_NONE && value != NULL)
		set_glob_field(&glob, field, value);
	return (&glob);
}
