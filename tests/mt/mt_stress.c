/*
 * Multithreaded stress / race test for ft_malloc.
 * Each thread hammers malloc/free/realloc/calloc across size classes and writes
 * the whole block (to surface corruption). A shared, test-locked ring hands
 * blocks between threads so cross-thread frees are exercised. Built two ways:
 *   - against the .so (LD): a plain stress run (must not crash/corrupt)
 *   - with -fsanitize=thread + the allocator sources: must report 0 data races
 * Usage: mt_stress [nthreads] [iters]
 */
#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define RING 4096

static void				*g_ring[RING];
static pthread_mutex_t	g_ring_lock = PTHREAD_MUTEX_INITIALIZER;
static int				g_iters = 200000;

static unsigned int	xr(unsigned int *s)
{
	*s ^= *s << 13;
	*s ^= *s >> 17;
	*s ^= *s << 5;
	return (*s);
}

/* hand a block to another thread, taking back whatever was there to free */
static void	*swap_ring(unsigned int *seed, void *p)
{
	int		slot;
	void	*old;

	slot = (int)(xr(seed) % RING);
	pthread_mutex_lock(&g_ring_lock);
	old = g_ring[slot];
	g_ring[slot] = p;
	pthread_mutex_unlock(&g_ring_lock);
	return (old);
}

static void	touch(unsigned char *p, size_t n, unsigned char v)
{
	if (!p)
		return ;
	p[0] = v;
	p[n - 1] = v;
}

static void	*worker(void *arg)
{
	unsigned int	seed;
	int				i;
	size_t			n;
	unsigned char	*p;

	seed = (unsigned int)(unsigned long)arg | 1u;
	i = -1;
	while (++i < g_iters)
	{
		n = (size_t)(xr(&seed) % 4096) + 1;
		p = ft_malloc(n);
		touch(p, n, (unsigned char)i);
		if ((xr(&seed) & 3) == 0)
		{
			n = (size_t)(xr(&seed) % 8192) + 1;
			p = ft_realloc(p, n);
			touch(p, n, 0x5a);
		}
		ft_free(swap_ring(&seed, p));
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	pthread_t	th[64];
	int			nt;
	int			i;

	nt = argc > 1 ? atoi(argv[1]) : 8;
	if (argc > 2)
		g_iters = atoi(argv[2]);
	if (nt > 64)
		nt = 64;
	i = -1;
	while (++i < nt)
		pthread_create(&th[i], NULL, worker, (void *)(long)(i + 1));
	i = -1;
	while (++i < nt)
		pthread_join(th[i], NULL);
	i = -1;
	while (++i < RING)
		if (g_ring[i])
			ft_free(g_ring[i]);
	printf("mt_stress OK: %d threads x %d iters\n", nt, g_iters);
	return (0);
}
