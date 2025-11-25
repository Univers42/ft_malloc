/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_trace.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:01:39 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:45:17 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_TRACE

void	handle_malloc_trace(t_addr mem, uint32_t ubytes, int nunits,
			const char *file, int line, int flags, t_glob *g)
{
	if (g->malloc_trace && (flags & MALLOC_NOTRACE) == 0)
		mtrace_free(mem, ubytes, file, line);
	else if (g->malloc_trace_buckets[nunits])
		mtrace_free(mem, ubytes, file, line);
}
#else

void	handle_malloc_trace(t_addr mem, uint32_t ubytes, int nunits,
			const char *file, int line, int flags, t_glob *g)
{
	(void)mem;
	(void)ubytes;
	(void)nunits;
	(void)file;
	(void)line;
	(void)flags;
	(void)g;
}
#endif
