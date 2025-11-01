/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sbrk.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 19:17:04 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/01 19:49:33 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SBRK_H
# define SBRK_H

/**
 * brk/sbrk are the classic way to grow the process heap 
 * the allocated asks kernel for more heap space. this is a legacy code
 * we can try to keep this legacy code or maintain it triggering its feature
 * if the other fails first.. we prioritize modernity over standar coding legacy
 * 
 * More modern alternative are used such as 
 * mmap/munmap for all allocations those function have the advantage to be
 * thread-safe, moddern,  avoid interfering with libc heap.
 * 
 * An hybrif approach for efficiency couble to use first sbrk for small/fast
 * allocations and mmap for large blocks.
 */
# include <errno.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdint.h>
# include <sys/syscall.h>

///* Set the end of accessible data space (aka "the break") to ADDR.
//   Returns zero on success and -1 for errors (with errno set).  */
//extern int brk (void *__addr) __THROW __wur;
//
///* Increase or decrease the end of accessible data space by DELTA bytes.
//   If successful, returns the address the previous end of data space
//   (i.e. the beginning of the new space, if DELTA > 0);
//   returns (void *) -1 for errors (with errno set).  */
//extern void *sbrk (intptr_t __delta) __THROW;

// /*
//  * brk state is file-local only (no exported globals).
//  */
static struct s_brk_state
{
	void	*initialbrk;
	void	*curbrk;
}	brk_state {NULL, NULL};

static int	initbrk(void)
{
	void	*b;

	if (brk_state.initialbrk != NULL)
		return (0);
	b = (void *)syscall(SYS_brk, 0);
	if (b == (void *)-1)
		return (-1);
	brk_state.initialbrk = b;
	brk_state.curbrk = b;
	return (0);
}

void	*sbrk(intptr_t incr)
{
	void	*newbrk;
	void	*oldbrk;

	if (brk_state.initialbrk == NULL && initbrk() == -1)
	{
		errno = ENOMEM;
 		return ((void *)-1);
	}
	if (incr == 0)
		return (brk_state.curbrk);
	if ((incr > 0 && (uintptr_t)brk_state.curbrk + (uintptr_t)incr
			< (uintptr_t)brk_state.curbrk)
	|| (incr < 0 && (uintptr_t)brk_state.curbrk + (uintptr_t)incr
			> (uintptr_t)brk_state.curbrk))
	{
		errno = ENOMEM;
 		return ((void *)-1);
	}
	newbrk = (char *)brk_state.curbrk + incr;
	if (newbrk < brk_state.initialbrk)
	{
		errno = EINVAL;
		return ((void *)-1);
	}
	if (brk(newbrk) == -1)
		return ((void *)-1);
	oldbrk = brk_state.curbrk;
	brk_state.curbrk = newbrk;
	return (oldbrk);
}

void	sbrk_get_state(void **initialbrk, void **curbrk)
{
	if (initialbrk != NULL)
		*initialbrk = brk_state.initialbrk;
	if (curbrk != NULL)
		*curbrk = brk_state.curbrk;
}

#endif