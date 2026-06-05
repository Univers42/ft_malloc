/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lock.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/05 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * The central allocator lock. Held at every public entry point so the shared
 * t_glob freelists / arenas / tracker are mutated by one thread at a time
 * (concurrent sbrk/mmap and freelist edits would otherwise corrupt the heap).
 * The internal_* helpers run lock-free assuming the caller holds it (this is
 * why internal_realloc can call internal_malloc/internal_free without a
 * recursive lock). The per-thread cache fast path bypasses this lock entirely;
 * it is taken only to refill/flush a magazine or grow an arena.
 */

#include "alloc.h"
#include <pthread.h>

static pthread_mutex_t	*central_mutex(void)
{
	static pthread_mutex_t	lock = PTHREAD_MUTEX_INITIALIZER;

	return (&lock);
}

void	malloc_lock(void)
{
	pthread_mutex_lock(central_mutex());
}

void	malloc_unlock(void)
{
	pthread_mutex_unlock(central_mutex());
}
