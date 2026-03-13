/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internal_calloc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/25 11:52:44 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 23:47:33 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

t_addr	internal_calloc(size_t n, size_t s, const char *file, int line)
{
	size_t	total;
	t_addr	res;

	total = n * s;
	res = internal_malloc(total, file, line, MALLOC_INTERNAL);
	if (res)
		memset(res, 0, total);
	return (res);
}

void	internal_cfree(t_addr p, const char *file, int line, int flags)
{
	internal_free(p, file, line, flags | MALLOC_INTERNAL);
}

int	posix_memalign(void **memptr, size_t alignment, size_t size)
{
	void		*mem;
	t_val_ctx	v;

	if ((alignment % sizeof(t_addr) != 0) || alignment == 0)
		return (EINVAL);
	if (is_powerof2(alignment) == 0)
		return (EINVAL);
	v.mem = NULL;
	v.file = NULL;
	v.line = 0;
	mem = (void *)internal_memalign(alignment, size, &v, 0);
	if (mem != 0)
	{
		*memptr = mem;
		return (0);
	}
	return (ENOMEM);
}
