/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   suite_alloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:57:43 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/31 17:58:14 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

t_addr	malloc(size_t size)
{
	return (intern_malloc(size, (char *)NULL, 0, 0));
}

t_addr	realloc (t_addr mem, size_t nbytes)
{
  return internal_realloc (mem, nbytes, (char *)NULL, 0, 0);
}

void	free(t_addr mem)
{
	intern_free(mem, (char *)NULL, 0, 0);
}

t_addr	ft_memalign(size_t	align, size_t size)
{
	return (intern_memalign(align, size, (char *)NULL, 0, 0));
}

t_addr	valloc(size_t size)
{
	return (intern_valloc(size, (char *)NULL), 0, 0);
}
t_addr	ft_calloc(size_t n, size_t s)
{
	return (intern_calloc(n, s, (char *)NULL, 0, 0));
}

void	ft_cfree(t_addr mem)
{
	intern_cfree(mem, (char *)NULL, 0, 0);
}