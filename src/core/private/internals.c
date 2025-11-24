/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internals.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:56:05 by dlesieur          #+#    #+#             */
/*   Updated: 2025/11/02 14:34:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "alloc.h"
#include <sys/mman.h>

#if defined(__linux__)
#define _GNU_SOURCE
#include <unistd.h>
#endif

t_addr internal_remap(t_addr mem, size_t n, int nunits, int flags)
{
	t_mhead *p;
	t_mhead *np;
	uint8_t *z;
	char *m;
	t_mguard mg;
	t_glob *g;

	(void)flags;
	g = get_glob(GLOB_NONE, NULL);
	if (nunits >= NBUCKETS)
		return ((t_addr)NULL);
	p = (t_mhead *)mem - 1;
	m = (char *)mem + p->s_minfo.mi_nbytes;
	m[0] = 0;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;
	g->busy[nunits] = 1;
#if defined(HAVE_MREMAP) && defined(MREMAP_MAYMOVE)
	np = (t_mhead *)mremap(p, binsize(p->s_minfo.mi_index), binsize(nunits), MREMAP_MAYMOVE);
#else
	np = MAP_FAILED;
#endif
	g->busy[nunits] = 0;
	if (np == MAP_FAILED)
		return ((t_addr)NULL);
	if (np != p)
	{
		np->s_minfo.mi_alloc = ISALLOC;
		np->s_minfo.mi_magic2 = MAGIC2;
		malloc_memset((char *)np->s_minfo.mi_magic8, MAGIC1, MAGIC8_NUMBYTES);
	}
	np->s_minfo.mi_index = nunits;
	np->s_minfo.mi_nbytes = n;
	mg.i = n;
	z = (uint8_t *)mg.s;
	m = (char *)(np + 1) + n;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	return ((t_addr)(np + 1));
}

/**
 * This function can be omitted at compiled
 */
#if !defined(NO_VALLOC)
/**
 * This internal implement valloc by calling the allocator memalign routine with
 * the system page size as the alignment,
 * @param
 * @return pointer aligned to a page boundary
 * @note internal_memalign does the real work (allocated a block, records, the
 * extra offset so free can recover the original pointer. internal valloc just
 * wraps it and set the MALLOC_INTERNAL flag)
 *
 * Some issues has been detected cross platform so all the functions will
 * and the other valloc will be be compiled in binary.
 *  * @note This function can be omitted at compile time on some platforms
 */
t_addr internal_valloc(size_t size, const char *file, int line, int flags)
{
	return (internal_memalign(getpagesize(), size, file, line, flags | MALLOC_INTERNAL));
}
#endif // !NO_VALLOC

#ifndef NO_CALLOC
t_addr internal_calloc(size_t n, size_t s, const char *file, int line, int flags)
{
	size_t total;
	t_addr res;

	total = n * s;
	res = internal_malloc(total, file, line, flags | MALLOC_INTERNAL);
	if (res)
		memset(res, 0, total);
	return (res);
}

void internal_cfree(t_addr p, const char *file, int line, int flags)
{
	internal_free(p, file, line, flags | MALLOC_INTERNAL);
}
#endif // NO_CALLOC

t_addr internal_memalign(size_t alignment, size_t size, const char *file, int line, int flags)
{
	char *ptr;
	char *aligned;
	t_mhead *p;

	(void)flags;
	ptr = internal_malloc(size + alignment, file, line, MALLOC_INTERNAL);
	if (ptr == 0)
		return (0);
	if (((long)ptr & (alignment - 1)) == 0)
		return (ptr);
	aligned = (char *)(((long)ptr + alignment - 1) & (~alignment + 1));
	p = (t_mhead *)aligned - 1;
	p->s_minfo.mi_nbytes = aligned - ptr;
	p->s_minfo.mi_alloc = ISMEMALILGN;
	return (aligned);
}

int posix_memalign(void **memptr, size_t alignment, size_t size)
{
	void *mem;

	if ((alignment % sizeof(t_addr) != 0) || alignment == 0)
		return (EINVAL);
	else if (is_powerof2(alignment) == 0)
		return (EINVAL);
	mem = internal_memalign(alignment, size, (char *)0, 0, 0);
	if (mem != 0)
	{
		*memptr = mem;
		return (0);
	}
	return (ENOMEM);
}