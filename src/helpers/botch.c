/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botch.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 20:10:20 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:37:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"

#ifdef MALLOC_REGISTER
void register_malloc_impl(t_addr mem, int e, const char *s, int line)
{
	if (mem != (t_addr)0 && malloc_register)
		mregister_describe_mem(mem, stderr);
}
#else
void register_malloc_impl(t_addr mem, int e, const char *s, const char *file, int line)
{
	(void)mem;
	(void)e;
	(void)s;
	(void)file;
	(void)line;
	/* No-op when malloc register is not defined. */
}
#endif

/**
 * low-lvl assertion handler. it reports a failed assertion msg
 * and then terminated using the function abort. it is the final
 * fail-stop used by the allocator when an internal invariant is violated
 */
void botch(const char *s, const char *file, int line)
{
	(void)file;
	(void)line;
	dprintf(STDERR_FILENO, "malloc: failed assertion: %s\n", s);
	(void)fflush(stderr);
	abort();
}

/**
 * xbotch is an extended version assertion handler. it prints a more detailed
 * message that includes file and line, optionally describe the offending allocation
 * (via the MALLOC_REGISTER helper), flushes stderr, then delegates to botch
 * which terminate. So we use xbotch when we want extra context about a memory error
 * botch does the actual shutdown/reporting
 */
void xbotch(t_addr mem, int e, const char *s, const char *file, int line)
{
	dprintf(STDERR_FILENO, "\r\nmalloc: %s:%d: assertion botched\r\n",
			file ? file : "unknown", line);
	register_malloc_impl(mem, e, s, file, line);
	(void)fflush(stderr);
	botch(s, file, line);
}