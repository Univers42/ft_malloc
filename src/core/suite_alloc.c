/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   suite_alloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:57:43 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 22:31:03 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

static void ensure_init(void)
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

#ifndef NO_VALLOC

t_addr ft_valloc(size_t size)
{
	ensure_init();
	return (internal_valloc(size, (char *)NULL, 0, 0));
}
#endif

#ifndef NO_CALLOC

t_addr ft_calloc(size_t n, size_t s)
{
	ensure_init();
	return (internal_calloc(n, s, (char *)NULL, 0, 0));
}

void ft_cfree(t_addr mem)
{
	ensure_init();
	internal_cfree(mem, (char *)NULL, 0, 0);
}

#endif