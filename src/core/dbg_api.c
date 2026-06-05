/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dbg_api.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/04 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "private/private.h"

t_addr	ft_malloc_dbg(size_t size, const char *file, int line)
{
	t_addr	r;

	malloc_lock();
	ensure_init();
	r = internal_malloc(size, file, line, 0);
	malloc_unlock();
	return (r);
}

void	ft_free_dbg(t_addr mem, const char *file, int line)
{
	malloc_lock();
	ensure_init();
	internal_free(mem, file, line, 0);
	malloc_unlock();
}

t_addr	ft_realloc_dbg(t_addr mem, size_t nbytes, const char *file, int line)
{
	t_val_ctx	v;
	t_addr		r;

	malloc_lock();
	ensure_init();
	v.mem = 0;
	v.file = file;
	v.line = line;
	r = internal_realloc(mem, nbytes, &v, 0);
	malloc_unlock();
	return (r);
}

t_addr	ft_calloc_dbg(size_t n, size_t s, const char *file, int line)
{
	t_addr	r;

	malloc_lock();
	ensure_init();
	r = internal_calloc(n, s, file, line);
	malloc_unlock();
	return (r);
}
