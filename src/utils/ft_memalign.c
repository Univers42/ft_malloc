/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memalign.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:45:25 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 12:03:16 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

void	ensure_init(void)
{
	t_glob *g;

	g = get_glob(GLOB_NONE, NULL);
	if (g->pagesz == 0)
		init_allocator_glob();
}

t_addr ft_memalign(size_t align, size_t size)
{
	ensure_init();
	return (internal_memalign(align, size, (char *)NULL, 0, 0));
}

