/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 21:27:49 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:30:34 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include "alloc.h"

/* file-scope implementations selected at compile time */
static void malloc_block_signals_impl(sigset_t *setp, sigset_t *osetp);
static void malloc_unblock_signals_impl(sigset_t *setp, sigset_t *osetp);

#if defined(HAVE_POSIX_SIGNALS)
static void malloc_block_signals_impl(sigset_t *setp, sigset_t *osetp)
{
    sigfillset(setp);
    sigemptyset(osetp);
    sigprocmask(SIG_BLOCK, setp, osetp);
}
static void malloc_unblock_signals_impl(sigset_t *setp, sigset_t *osetp)
{
    sigprocmask(SIG_SETMASK, osetp, (sigset_t *)NULL);
}
#elif defined(HAVE_BSD_SIGNALS)
static void malloc_block_signals_impl(sigset_t *setp, sigset_t *osetp)
{
    (void)setp;
    *osetp = sigsetmask(-1);
}
static void malloc_unblock_signals_impl(sigset_t *setp, sigset_t *osetp)
{
    (void)setp;
    sigsetmask(*osetp);
}
#else
static void malloc_block_signals_impl(sigset_t *setp, sigset_t *osetp)
{
    (void)setp;
    (void)osetp;
}
static void malloc_unblock_signals_impl(sigset_t *setp, sigset_t *osetp)
{
    (void)setp;
    (void)osetp;
}
#endif

/* public wrappers without any #if/#ifdef inside function scope */
void malloc_block_signals(sigset_t *setp, sigset_t *osetp)
{
    malloc_block_signals_impl(setp, osetp);
}

void malloc_unblock_signals(sigset_t *setp, sigset_t *osetp)
{
    malloc_unblock_signals_impl(setp, osetp);
}
