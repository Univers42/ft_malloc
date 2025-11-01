/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   botch.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 20:10:20 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 20:11:20 by dlesieur         ###   ########.fr       */
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
	/* No-op whne malloc register is not defined. */
}
#endif

/**
 * low-lvl assserrtion handler. it repors  a failed assertion msg
 * and then terminated using the function abrt. it is the final
 * fail-stop used by the allocator when an internal invariant is violated
 */
void	botch(const char *s, const char *file, int line)
{
	dprintf(STDERR_FILENO, "malloc: failed assertion: %s\n", s);
	(void)fflush(stderr);
	abort();
}

/**
 * xbotch is an extended version asssertion handler. it prints  a more detailed
 * message that includes  file and line, optionally describe the offending allocation
 * (via the MALLOC_REGISTER helper), flushes stderr, tehn delegates to botch
 *	which termate. So we use xbotch when we want extra context about a memory error
 * botch does the actual shutdown/reporting
 */
void	xbotch(t_addr mem, int e, const char *s, const char *file, int line)
{
	dprintf(STDERR_FILENO, _("\r\nmalloc: %s:%d: assertion botched\r\n"),
			file ? file : _("unknown"), line);
	register_malloc_impl(mem, e, s, file, line);
	(void)fflush(stderr);
	botch(s, file, line);
}