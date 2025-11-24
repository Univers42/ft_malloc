/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zone_manager.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 20:20:03 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:33:18 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <sys/mman.h>

void	algo_for_tiny_small(size_t size);
size_t	compute_num_blocks(size_t zone_size, size_t block_size);
void	init_zone_struct(t_zone *zone, void *mem,
			size_t zone_size, size_t block_size);
void	build_zone_free_list(t_zone *zone,
			size_t block_size, size_t num_blocks);

/* Create a new zone for given block size */
t_zone	*create_zone(size_t block_size, size_t zone_size)
{
	t_zone	*zone;
	void	*mem;
	size_t	num_blocks;

	mem = zone_allocate_memory(zone_size);
	if (!mem)
		return (NULL);
	zone = (t_zone *)mem;
	init_zone_struct(zone, mem, zone_size, block_size);
	num_blocks = compute_num_blocks(zone_size, block_size);
	zone->total_blocks = num_blocks;
	zone->used_blocks = 0;
	build_zone_free_list(zone, block_size, num_blocks);
	return (zone);
}

/* Find or create a zone with a free block */
t_zone	*get_zone_with_space(t_zone **zone_list,
							size_t block_size, size_t zone_size)
{
	t_zone	*zone;

	zone = *zone_list;
	while (zone)
	{
		if (zone->free_list != NULL)
			return (zone);
		zone = zone->next;
	}
	zone = create_zone(block_size, zone_size);
	if (!zone)
		return (NULL);
	zone->next = *zone_list;
	*zone_list = zone;
	return (zone);
}

/* Allocate a block from a zone */
void	*allocate_from_zone(t_zone *zone)
{
	t_mhead	*block;
	void	*ptr;

	if (!zone || !zone->free_list)
		return (NULL);
	block = zone->free_list;
	zone->free_list = *chain_ptr(block);
	zone->used_blocks++;
	block->s_minfo.mi_alloc = ISALLOC;
	ptr = (void *)(block + 1);
	return (ptr);
}

/* Free a block back to its zone */
void	free_to_zone(t_zone *zone, t_mhead *block)
{
	if (!zone || !block)
		return ;
	block->s_minfo.mi_alloc = ISFREE;
	*chain_ptr(block) = zone->free_list;
	zone->free_list = block;
	zone->used_blocks--;
}
