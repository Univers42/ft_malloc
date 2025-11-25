/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_register.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 22:00:59 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/25 13:45:02 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_REGISTER

void	handle_malloc_register(uint32_t ubytes, const char *file,
			int line, int flags, t_glob *g)
{
	if (g->malloc_register && (flags & MALLOC_NOREG) == 0)
		mregister_free(mem, ubytes, file, line);
}
#else

void	handle_malloc_register(uint32_t ubytes, const char *file,
			int line, int flags, t_glob *g)
{
	(void)ubytes;
	(void)file;
	(void)line;
	(void)flags;
	(void)g;
}
#endif
