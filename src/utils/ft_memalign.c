/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memalign.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:45:25 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/14 00:30:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "../core/private/private.h"

void	ensure_init(void)
{
	t_glob	*g;

	g = get_glob(GLOB_NONE, NULL);
	if (g->pagesz == 0)
		init_allocator_glob();
}

t_addr	ft_memalign(size_t align, size_t size)
{
	t_val_ctx	v;

	ensure_init();
	v.mem = (t_addr) NULL;
	v.file = NULL;
	v.line = 0;
	return (internal_memalign(align, size, &v, 0));
}
