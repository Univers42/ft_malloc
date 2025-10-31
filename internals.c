/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internals.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:56:05 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/31 18:43:35 by dlesieur         ###   ########.fr       */
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

t_addr  internal_valloc(size_t size, const char *file, int line, int flags)
{

}

t_addr  internal_calloc(size_t n, size_t s, const char *file, int line, int flags)
{
	
}

void  internal_cfree(t_addr p, const char *file, int line, int flags)
{
	internal_free(p, file, line, flags|MALLOC_INTERNAL);	
}

void    internal_free(t_addr mem, const char *file, int line, int flags)
{
	
}