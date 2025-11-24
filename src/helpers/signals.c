/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 21:27:49 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/24 20:46:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include "alloc.h"

/* file-scope implementations selected at compile time */
void	malloc_block_signals_impl(sigset_t *setp, sigset_t *osetp);
void	malloc_unblock_signals_impl(sigset_t *setp, sigset_t *osetp);

/* public wrappers without any #if/#ifdef inside function scope */
void	malloc_block_signals(sigset_t *setp, sigset_t *osetp)
{
	malloc_block_signals_impl(setp, osetp);
}

void	malloc_unblock_signals(sigset_t *setp, sigset_t *osetp)
{
	malloc_unblock_signals_impl(setp, osetp);
}
