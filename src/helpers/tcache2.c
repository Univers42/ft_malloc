/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcache2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/05 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Lock-free realloc fast path. Most real malloc+realloc code (e.g.
 * get_next_line growing a line buffer) reallocs small blocks; routing those
 * through the per-thread magazines avoids the central lock entirely:
 *   - realloc(NULL, n)        -> tls_malloc (a magazine pop)
 *   - shrink / same TLS class -> in place, just rewrite mi_nbytes
 *   - grow within TLS classes -> tls_malloc + fastcopy + tls_free
 * Anything else (large class, hardened build, size 0) sets *handled = 0 and the
 * caller (ft_realloc) takes the locked central path.
 */

#include "alloc.h"

int	calculate_nunits(uint32_t nbytes, t_glob *g);

static t_addr	tls_malloc_or_miss(size_t size, int *handled)
{
	t_addr	np;

	np = tls_malloc(size);
	if (np)
		return (np);
	*handled = 0;
	return ((t_addr)0);
}

static t_addr	tls_realloc_grow(t_addr mem, t_mhead *p, size_t size,
		int *handled)
{
	t_addr	np;

	np = tls_malloc(size);
	if (!np)
	{
		*handled = 0;
		return ((t_addr)0);
	}
	fastcopy(mem, np, p->s_minfo.mi_nbytes);
	tls_free(mem);
	return (np);
}

static t_addr	tls_realloc_resize(t_addr mem, size_t size, int *handled)
{
	t_mhead	*p;
	int		old_nu;
	int		new_nu;

	p = (t_mhead *)mem - 1;
	old_nu = (int)p->s_minfo.mi_index;
	new_nu = calculate_nunits((uint32_t)allocated_bytes(size), (t_glob *)0);
	if (p->s_minfo.mi_alloc != ISALLOC || old_nu < 0 || old_nu > TLS_MAX_BUCKET
		|| new_nu < 0 || new_nu > TLS_MAX_BUCKET)
	{
		*handled = 0;
		return ((t_addr)0);
	}
	if (new_nu <= old_nu)
	{
		p->s_minfo.mi_nbytes = (uint32_t)size;
		return (mem);
	}
	return (tls_realloc_grow(mem, p, size, handled));
}

t_addr	tls_realloc(t_addr mem, size_t size, int *handled)
{
	*handled = 1;
	if (FT_HARDEN || size == 0)
	{
		*handled = 0;
		return ((t_addr)0);
	}
	if (!mem)
		return (tls_malloc_or_miss(size, handled));
	return (tls_realloc_resize(mem, size, handled));
}
