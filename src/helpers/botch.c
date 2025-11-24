/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botch.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 20:10:20 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 15:27:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_REGISTER

void	register_malloc_impl(int e, const char *s, const char *file, int line)
{
	t_addr	mem;

	mem = get_staste_mem();
	if (mem != (t_addr)0)
		mregister_describe_mem(mem, stderr);
	(void)e;
	(void)s;
	(void)file;
	(void)line;
}
#else

void	register_malloc_impl(int e, const char *s, const char *file, int line)
{
	(void)e;
	(void)s;
	(void)file;
	(void)line;
}
#endif

/**
 * low-lvl assertion handler. it reports a failed assertion msg
 * and then terminated using the function abort. it is the final
 * fail-stop used by the allocator when an internal invariant is violated
 */
void	botch(const char *s, const char *file, int line)
{
	(void)file;
	(void)line;
	dprintf(STDERR_FILENO, "malloc: failed assertion: %s\n", s);
	(void)fflush(stderr);
	abort();
}

/* replace previous xbotch signature/impl with the form used by callers:
 * xbotch(error_code, message, file, line)
 */
void	xbotch(int e, const char *s, const char *file, int line)
{
	if (file)
		dprintf(STDERR_FILENO, "\r\nmalloc: %s:%d: assertion botched\r\n",
			file, line);
	else
		dprintf(STDERR_FILENO, "\r\nmalloc: %s:%d: assertion botched\r\n",
			"unknown", line);
	register_malloc_impl(e, s, file, line);
	(void)fflush(stderr);
	botch(s, file, line);
}
