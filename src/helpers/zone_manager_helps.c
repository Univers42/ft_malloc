/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zone_manager_helps.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 20:32:04 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:34:36 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#if USE_SBRK_FOR_TINY_SMALL

void	*algo_for_tiny_small(size_t size)
{
	void	*ptr;

	ptr = sbrk(size);
	if (ptr == (void *)-1)
		return (NULL);
	return (ptr);
}
#else

void	*algo_for_tiny_small(size_t size)
{
	void	*ptr;

	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE | MAP_PRIVATE,
			MAP_ANONYMOUS, -1, 0);
	if (ptr == MAP_FAILED)
		return (NULL);
	return (ptr);
}
#endif

/* initialize zone structure fields */
void	init_zone_struct(t_zone *zone, void *mem,
							size_t zone_size, size_t block_size)
{
	zone->start = (char *)mem + sizeof(t_zone);
	zone->end = (char *)mem + zone_size;
	zone->block_size = block_size;
	zone->next = NULL;
}

/* compute usable bytes and number of blocks that fit in the zone */
size_t	compute_num_blocks(size_t zone_size, size_t block_size)
{
	size_t	usable_size;
	size_t	entry_size;

	usable_size = zone_size - sizeof(t_zone);
	entry_size = block_size + sizeof(t_mhead);
	if (entry_size == 0)
		return (0);
	return (usable_size / entry_size);
}

/* build the free-list chain inside the zone */
void	build_zone_free_list(t_zone *zone,
			size_t block_size, size_t num_blocks)
{
	t_mhead	*block;
	size_t	i;

	if (num_blocks == 0)
	{
		zone->free_list = NULL;
		return ;
	}
	zone->free_list = (t_mhead *)zone->start;
	block = zone->free_list;
	i = 0;
	while (i + 1 < num_blocks)
	{
		block->s_minfo.mi_alloc = ISFREE;
		*chain_ptr(block) = (t_mhead *)((char *)block
			+ block_size + sizeof(t_mhead));
		block = *chain_ptr(block);
		i++;
	}
	block->s_minfo.mi_alloc = ISFREE;
	*chain_ptr(block) = NULL;
}
