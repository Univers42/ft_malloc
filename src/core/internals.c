/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internals.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:56:05 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 00:34:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

t_addr	internal_malloc(size_t n, const char *file, int line, int flags)
{

}

t_addr  internal_free(t_addr mem, const char *file, int line, int flags)
{

}

t_addr  internal_realloc(t_addr mem, size_t n, const char *file, int line, int flags)
{

}

/**
 * This function can be omitted at compiled 
 */
#if !defined(NO_VALLOC)
/**
 * This internal implement valloc by calling the allocator memalign routine with
 * the system page size as the alignment, 
 * @param 
 * @return pointer aligned to a page boundary
 * @note internal_memalign does the real work (allocated a block, records, the
 * extra offset so free can recover the original pointer. internal valloc just
 * wraps it and set the MALLOC_INTERNAL flag)
 * 
 * Some issues has been detected cross platform so all the functions will
 * and the other valloc will be be compiled in binary.
 *  * @note This function can be omitted at compile time on some platforms
 */
t_addr  internal_valloc(size_t size, const char *file, int line, int flags)
{
	return (internal_memalign(getpagesize(), size, file, line, flags | MALLOC_INTERNAL));
}
#endif

t_addr  internal_calloc(size_t n, size_t s, const char *file, int line, int flags)
{
	size_t	total;
	t_addr	res;

	total = n * s;
	res = internal_malloc(total, file, line, flags | MALLOC_INTERNAL);
	if (res)
		memset(res, 0, total);
	return (res);
}

void  internal_cfree(t_addr p, const char *file, int line, int flags)
{
	internal_free(p, file, line, flags|MALLOC_INTERNAL);	
}

void    internal_free(t_addr mem, const char *file, int line, int flags)
{
	
}