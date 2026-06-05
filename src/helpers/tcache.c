/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tcache.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 00:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/06/05 00:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * Per-thread magazine cache: the lock-free fast path. malloc pops / free pushes
 * a thread-local freelist with no lock. The central mutex is taken only to
 * refill an empty magazine (a batch from the central heap, growing it via mmap
 * if needed) or flush an overfull one. Large classes (> TLS_MAX_BUCKET) and the
 * hardened build bypass this and use the locked central path directly. The size
 * class lives in the block header (mi_index), so cross-thread frees are correct.
 */

#include "alloc.h"

int		calculate_nunits(uint32_t nbytes, t_glob *g);
t_mhead	*get_block_from_freelist(int nunits, t_glob *g);
void	add_to_free_list(t_mhead *p, int nunits, t_glob *g);

static t_tls	*tls_self(void)
{
	static __thread t_tls	self;

	return (&self);
}

static void	tls_refill(t_tls *t, int nu)
{
	t_glob	*g;
	t_mhead	*p;
	int		i;

	malloc_lock();
	ensure_init();
	g = get_glob(GLOB_NONE, NULL);
	i = -1;
	while (++i < TLS_BATCH)
	{
		p = get_block_from_freelist(nu, g);
		if (!p)
			break ;
		*chain_ptr(p) = t->head[nu];
		t->head[nu] = p;
		t->n[nu]++;
	}
	malloc_unlock();
}

static void	tls_flush(t_tls *t, int nu)
{
	t_glob	*g;
	t_mhead	*p;
	int		i;

	malloc_lock();
	g = get_glob(GLOB_NONE, NULL);
	i = -1;
	while (++i < TLS_BATCH && t->head[nu])
	{
		p = t->head[nu];
		t->head[nu] = *chain_ptr(p);
		t->n[nu]--;
		add_to_free_list(p, nu, g);
	}
	malloc_unlock();
}

t_addr	tls_malloc(size_t size)
{
	t_tls	*t;
	t_mhead	*p;
	int		nu;

	if (FT_HARDEN)
		return ((t_addr)0);
	nu = calculate_nunits((uint32_t)allocated_bytes(size), (t_glob *)0);
	if (nu < 0 || nu > TLS_MAX_BUCKET)
		return ((t_addr)0);
	t = tls_self();
	if (!t->head[nu])
		tls_refill(t, nu);
	p = t->head[nu];
	if (!p)
		return ((t_addr)0);
	t->head[nu] = *chain_ptr(p);
	t->n[nu]--;
	p->s_minfo.mi_alloc = ISALLOC;
	p->s_minfo.mi_nbytes = (uint32_t)size;
	return ((t_addr)(p + 1));
}

int	tls_free(t_addr mem)
{
	t_tls	*t;
	t_mhead	*p;
	int		nu;

	if (!mem)
		return (1);
	if (FT_HARDEN)
		return (0);
	p = (t_mhead *)mem - 1;
	if (p->s_minfo.mi_alloc != ISALLOC)
		return (0);
	nu = (int)p->s_minfo.mi_index;
	if (nu < 0 || nu > TLS_MAX_BUCKET)
		return (0);
	p->s_minfo.mi_alloc = ISFREE;
	t = tls_self();
	*chain_ptr(p) = t->head[nu];
	t->head[nu] = p;
	t->n[nu]++;
	if (t->n[nu] > TLS_HIGH)
		tls_flush(t, nu);
	return (1);
}
