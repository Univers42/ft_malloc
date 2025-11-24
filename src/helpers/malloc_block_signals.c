/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc_block_signals.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 20:38:43 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:46:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <signal.h>

#ifdef HAVE_POSIX_SIGNALS

void	malloc_block_signals_impl(sigset_t *setp, sigset_t *osetp)
{
	sigfillset(setp);
	sigemptyset(osetp);
	sigprocmask(SIG_BLOCK, setp, osetp);
}

#elif defined(HAVE_BSD_SIGNALS)

void	malloc_block_signals_impl(sigset_t *setp, sigset_t *osetp)
{
	(void)setp;
	*osetp = sigsetmask(-1);
}

#else

void	malloc_block_signals_impl(sigset_t *setp, sigset_t *osetp)
{
	(void)setp;
	(void)osetp;
}

#endif