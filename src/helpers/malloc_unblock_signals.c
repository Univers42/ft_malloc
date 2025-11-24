/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_unblock_signals.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 20:44:37 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:47:06 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <signal.h>

#ifdef HAVE_POSIX_SIGNALS

void	malloc_unblock_signals_impl(sigset_t *setp, sigset_t *osetp)
{
	sigprocmask(SIG_SETMASK, osetp, (sigset_t *) NULL);
}

#elif defined(HAVE_BSD_SIGNALS)

void	malloc_unblock_signals_impl(sigset_t *setp, sigset_t *osetp)
{
	(void)setp;
	sigsetmask(*osetp);
}
#else

void	malloc_unblock_signals_impl(sigset_t *setp, sigset_t *osetp)
{
	(void)setp;
	(void)osetp;
}
#endif