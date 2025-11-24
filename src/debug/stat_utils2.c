/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stat_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:29:42 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 21:36:25 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_STATS

void	bsplit_stats_inc(int nbuck)
{
	_mstats.tbsplit++;
	_mstats.nsplit[nbuck]++;
}
#else

void	bsplit_stats_inc(int nbuck)
{
	(void)nbuck;
}

#endif
