/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dbg_api2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/04 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include "private/private.h"

t_addr	ft_memalign_dbg(size_t align, size_t size, const char *file, int line)
{
	t_val_ctx	v;

	ensure_init();
	v.mem = 0;
	v.file = file;
	v.line = line;
	return (internal_memalign(align, size, &v, 0));
}

t_addr	ft_valloc_dbg(size_t size, const char *file, int line)
{
	t_val_ctx	v;

	ensure_init();
	v.mem = 0;
	v.file = file;
	v.line = line;
	return (internal_valloc(size, &v, 0));
}

void	ft_cfree_dbg(t_addr mem, const char *file, int line)
{
	ensure_init();
	internal_cfree(mem, file, line, 0);
}
