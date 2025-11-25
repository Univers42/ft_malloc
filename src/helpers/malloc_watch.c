/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_watch.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:59:50 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:39:59 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_WATCH

void	handle_malloc_watch(uint32_t ubytes, const char *file,
				int line, t_glob *g)
{
	if (g->_malloc_nwatch > 0)
		_malloc_ckwatch(mem, file, line, W_FREE, ubytes);
}
#else

void	handle_malloc_watch(uint32_t ubytes, const char *file,
				int line, t_glob *g)
{
	(void)ubytes;
	(void)file;
	(void)line;
	(void)g;
}
#endif
