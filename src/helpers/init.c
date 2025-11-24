/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 21:02:22 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:07:14 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

void	init_allocator_glob(void)
{
	static const unsigned long	initial_bins[NBUCKETS] = {
		32UL, 64UL, 128UL, 256UL, 512UL, 1024UL, 2048UL, 4096UL,
		8192UL, 16384UL, 32768UL, 65536UL, 131072UL, 262144UL, 524288UL,
		1048576UL, 2097152UL, 4194304UL, 8388608UL, 16777216UL, 33554432UL,
		67108864UL, 134217728UL, 268435456UL, 536870912UL, 1073741824UL,
		2147483648UL, 4294967295UL};
	t_binspec					spec;

	spec.bins = initial_bins;
	spec.len = (size_t)NBUCKETS;
	get_glob(GLOB_BINSIZE, &spec);
}
