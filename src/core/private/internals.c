/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   internals.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/31 17:56:05 by dlesieur          #+#    #+#             */
/*   Updated: 2026/03/13 23:47:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "private.h"

static void	clear_old_guard(t_addr mem, t_mhead *p)
{
	char	*m;

	m = (char *)mem + p->s_minfo.mi_nbytes;
	m[0] = 0;
	m[1] = 0;
	m[2] = 0;
	m[3] = 0;
}

static void	set_remap_header(t_mhead *np, t_mhead *p, int nu)
{
	if (np != p)
	{
		np->s_minfo.mi_alloc = ISALLOC;
		np->s_minfo.mi_magic2 = MAGIC2;
		malloc_memset((char *)np->s_minfo.mi_magic8,
			MAGIC1, MAGIC8_NUMBYTES);
	}
	np->s_minfo.mi_index = nu;
}

static void	write_remap_guard(t_mhead *np, size_t n)
{
	t_mguard	mg;
	uint8_t		*z;
	char		*m;

	np->s_minfo.mi_nbytes = n;
	mg.i = n;
	z = (uint8_t *)mg.s;
	m = (char *)(np + 1) + n;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
	*m++ = (char)*z++;
}

t_addr	internal_remap(t_addr mem, size_t n, int nu, int flags)
{
	t_mhead	*p;
	t_mhead	*np;
	t_glob	*g;

	(void)flags;
	g = get_glob(GLOB_NONE, NULL);
	if (nu >= NBUCKETS)
		return ((t_addr) NULL);
	p = (t_mhead *)mem - 1;
	clear_old_guard(mem, p);
	g->busy[nu] = 1;
	np = (t_mhead *)MAP_FAILED;
	g->busy[nu] = 0;
	if (np == MAP_FAILED)
		return ((t_addr) NULL);
	set_remap_header(np, p, nu);
	write_remap_guard(np, n);
	return ((t_addr)(np + 1));
}

t_addr	internal_memalign(size_t alignment, size_t size,
				t_val_ctx *v, int flags)
{
	char	*ptr;
	char	*aligned;
	t_mhead	*p;

	(void)flags;
	ptr = (char *)internal_malloc(size + alignment,
			v->file, v->line, MALLOC_INTERNAL);
	if (ptr == 0)
		return (0);
	if (((long)ptr & (alignment - 1)) == 0)
		return ((t_addr)ptr);
	aligned = (char *)(((long)ptr + alignment - 1)
			& (~(long)alignment + 1));
	p = (t_mhead *)aligned - 1;
	p->s_minfo.mi_nbytes = aligned - ptr;
	p->s_minfo.mi_alloc = ISMEMALILGN;
	return ((t_addr)aligned);
}
